from flask import Flask, render_template
from flask_socketio import SocketIO, emit
from random import choice
from string import digits, ascii_letters
import paho.mqtt.client as mqtt
from time import time
import numpy as np
import pybase64
import cv2

ip_address = "10.42.0.198"
# ip_address = "0.0.0.0"
jpg_image = ""
tp = 0
N = 30
count = 0
mov_avg = [0 for _ in range(N)]
avg = 0

RESOLUTION = (640, 480)
fwidth = (RESOLUTION[0] + 31) // 32 * 32
fheight = (RESOLUTION[1] + 15) // 16 * 16

def on_connect(client, userdata, flags, rc):
    client.subscribe("device/feed")
    client.subscribe("device/voltage_measurements")


def on_message(client, userdata, msg):
    global jpg_image, tp, N, mov_avg, avg, count

    if msg.topic == "device/feed":
        jpg_image = msg.payload

        data = np.frombuffer(jpg_image, dtype=np.uint8).reshape((fheight, fwidth))
        retval, jpg_image = cv2.imencode('.jpg', data)

        t = time()
        dt = t - tp

        avg = (dt - mov_avg[count % N]) / N + avg
        mov_avg[count % N] = dt
        count += 1
        tp = time()

        # process
        # jpg_image = f(jpg_image)
        # print(f"fps : {1 / avg} Got data")

        # encode to base64
        encoded = pybase64.b64encode(jpg_image)
        socketio.emit("data", {"feed":  str(encoded, "utf-8")})


    if msg.topic == "device/voltage_measurements":
        measurements = [float(voltage) for voltage in msg.payload.decode().split()]
        socketio.emit("voltage", {"voltage_measurements" : measurements})


def MQTT_run():
    global tp
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(ip_address, 1883, 60)
    tp = time()
    client.loop_start()


s = ascii_letters + digits + "+_)(*&^%$#@!"

app = Flask(__name__)
app.config['SECRET_KEY'] = "".join([choice(s) for _ in range(16)])
socketio = SocketIO(app)


@app.route('/')
def hello_world():
    return render_template("index.html")


@socketio.on('get_data')
def handle_json(json):
    # emit("data", {"feed": jpg_image})
    pass

if __name__ == '__main__':
    MQTT_run()
    socketio.run(app)
