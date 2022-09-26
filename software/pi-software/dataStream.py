# python3
# based on https://raspberrypi.stackexchange.com/questions/58871/pi-camera-v2-fast-full-sensor-capture-mode-with-downsampling/58941#58941
import time
import serial
import numpy as np
from PIL import Image
import paho.mqtt.client as mqtt

ip_address = "0.0.0.0"

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


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(ip_address, 1883, 60)
client.loop_start()


with serial.Serial('/dev/ttyACM0', 9600, timeout=1) as ser:
    while True:
        data = ser.readline()
        print(data)
        client.publish("device/voltage_measurements", payload=data)