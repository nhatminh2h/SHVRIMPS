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


def run():
    import serial
    import paho.mqtt.client as mqtt

    ip_address = "0.0.0.0"
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(ip_address, 1883, 60)
    client.loop_start()
    


    while True:
        with serial.Serial('/dev/ttyUSB0', 115200, timeout=1) as ser:
            voltages = ser.readline()
            client.publish("device/voltage_measurements", payload=voltages)



if __name__ == "__main__":
    run()
