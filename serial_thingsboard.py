import serial
import paho.mqtt.client as paho
import os
import json
import time
from datetime import datetime

ser = serial.Serial(port='COM4', baudrate = 115200)

ACCESS_TOKEN='LPWQ3zKTuu9IOsjuGMCz'    
broker="demo.thingsboard.io"   
port=1883 					

def on_publish(client,userdata,result):         
    print("data published to thingsboard \n")
    pass
    
client1= paho.Client("control1")               
client1.on_publish = on_publish                 
client1.username_pw_set(ACCESS_TOKEN)              
client1.connect(broker,port,keepalive=60)    

try:
    ser.isOpen()
    print("serial is open")
except: 
    print("No Serial opened")
    exit()

if(ser.isOpen()):
    try:
        while(1):
            line = ser.readline()
            strline = line.decode("ascii")
            data = strline.split(" ")
            #print(data)
            payload = "{"
            payload += "\"fertilizer\":"
            payload += data[0]
            payload += ","
            payload += "\"light\":"
            payload += data[1]
            payload += ","
            payload += "\"moisture\":"
            payload += data[2]
            payload += ","
            payload += "\"temperature\":"
            payload += data[3]
            payload += "}"
            client1.publish("v1/devices/me/telemetry",payload)                
            print(payload)
            time.sleep(5)
    except Exception:
        print("error")
else:
    print("cant open serial")

