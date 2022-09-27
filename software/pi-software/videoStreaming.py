# python3
# based on https://raspberrypi.stackexchange.com/questions/58871/pi-camera-v2-fast-full-sensor-capture-mode-with-downsampling/58941#58941
import time
import picamera
import numpy as np
from PIL import Image
import paho.mqtt.client as mqtt

ip_address = "0.0.0.0"

RECORD_TIME = 3000 # number of seconds to record

WRITE_IMAGES = True

CAPTURE_EVERY_X_FRAME = 1 # 2

# SENSOR_MODE = 4  # 1640x1232, full FoV, binning 2x2
# RESOLUTION = (1640, 1232)
# RESOLUTION = (820, 616)

#SENSOR_MODE = 6  # 1280x720, partial FoV, binning 2x2
#RESOLUTION = (1280, 720)

SENSOR_MODE = 7  # 640x480, partial FoV, binning 2x2
RESOLUTION = (640, 480)

#FRAME_RATE = 90
FRAME_RATE = 120
#FRAME_RATE = 60

FPS_MODE_OFF = 0
FPS_MODE_T0 = 1
FPS_MODE_FBF = 2
FPS_MODE = FPS_MODE_OFF

# Calculate the actual image size in the stream (accounting for rounding
# of the resolution)
# Capturing yuv will round horizontal resolution to 16 multiple and vertical to 32 multiple
# see: https://picamera.readthedocs.io/en/release-1.12/recipes2.html#unencoded-image-capture-yuv-format
fwidth = (RESOLUTION[0] + 31) // 32 * 32
fheight = (RESOLUTION[1] + 15) // 16 * 16
print(f'frame size {fwidth}x{fheight}')

frame_cnt = 0
t0 = None
t_prev = None


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("device/get")


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    if msg.topic == "device/get":
        print("#")
        jpg_image = msg.payload


class RecordingOutput(object):
    """
    Object mimicking file-like object so start_recording will write each frame to it.
    See: https://picamera.readthedocs.io/en/release-1.12/api_camera.html#picamera.PiCamera.start_recording
    """

    def write(self, buf):
        global frame_cnt, t_prev
        # write will be called once for each frame of output. buf is a bytes
        # object containing the frame data in YUV420 format; we can construct a
        # numpy array on top of the Y plane of this data quite easily:
        y_data = np.frombuffer(buf, dtype=np.uint8, count=fwidth * fheight).reshape((fheight, fwidth))

        # actual processing
        if WRITE_IMAGES and frame_cnt % CAPTURE_EVERY_X_FRAME == 0:
            client.publish("device/feed", payload=y_data.tobytes())
            #pass

        frame_cnt += 1

        if FPS_MODE is not FPS_MODE_OFF:
            if FPS_MODE == FPS_MODE_FBF:
                # frame by frame difference
                dt = time.time() - t_prev  # dt
                t_prev = time.time()
                fps = 1.0 / dt
            else:
                # calculation based on time elapsed since capture start
                dt = time.time() - t0  # dt
                fps = frame_cnt / dt

            if frame_cnt % 10 == 0:
                print(f'fps: {round(fps, 2)}')

    def flush(self):
        pass  # called at end of recording


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(ip_address, 1883, 60)
client.loop_start()

with picamera.PiCamera(
        sensor_mode=SENSOR_MODE,
        resolution=RESOLUTION,
        framerate=FRAME_RATE
) as camera:
    print('camera setup')
    camera.rotation = 180

    time.sleep(2)  # let the camera warm up and set gain/white balance

    print('starting recording')
    output = RecordingOutput()

    t0 = time.time()  # seconds
    t_prev = t0

    camera.start_recording(output, 'yuv')
    camera.wait_recording(RECORD_TIME)
    camera.stop_recording()

    dt = time.time() - t_prev
    print(f'total frames: {frame_cnt}')
    print(f'time recording: {round(dt, 2)}s')
    fps = frame_cnt / dt
    print(f'fps: {round(fps, 2)}s')

print('done')
