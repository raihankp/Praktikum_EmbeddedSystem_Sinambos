import serial
import time
import json
import paho.mqtt.client as mqtt

username = '2ytAQQbIJSWS1kYvTYCH'
password = ''
broker_address = 'demo.thingsboard.io'

def on_message(client, userdata, message):
    print("message received " ,str(message.payload.decode("utf-8")))
    print("message topic=",message.topic)
    print("message qos=",message.qos)
    print("message retain flag=",message.retain)


# print("creating new instance")
# client = mqtt.Client("P1")
# client.on_message=on_message
# print("connecting to broker")

# client.username_pw_set(username, password)
# client.connect(broker_address)

ser = serial.Serial(
    port='COM5',    # Ganti sama serial port nya (misal COM1 atau /dev/usbTTY0)
    baudrate=9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

buffer = ser.read(ser.in_waiting)
# client.loop_start()
while True:
    try:
        str = ser.readline().decode().strip()
        print(str)
        # jobj = json.loads(str)
        # if(jobj["gpio"] == 54):
        #     data = {"potentiometer" : jobj["value"]}
        #     client.publish("v1/devices/me/telemetry", json.dumps(data))
    except KeyboardInterrupt:
        break
    except:
        pass