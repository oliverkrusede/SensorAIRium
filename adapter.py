#Version 1.0
import json
import socket
import paho.mqtt.client as mqtt
import asyncio
import serial
import serial.tools.list_ports
import time
import mySerial
import atexit
#values
CO2_GOOD_UNDER = 950
CO2_MODERATE_UNDER = 1500


#net receive
addr="localhost"
port = 3000

#mqtt
mqttAddr = "airquality.local"
topic = "airquality/#"

#socket
socketConnected = False
s = None

#chordType
chordType = 0
chordNum = 0

ard = None
def arduinoFade(led, r,g,b,size,time):
    print("FADE")
    print("LED: "+str(led)+" rgb: "+str(r)+" "+str(g)+" "+str(b)+" size: "+str(size)+" time: "+str(time))
    if(ard==None):
        return
    mySerial.fade(ard,led,r,g,b,size,time)

def arduinoFadeBack(led, size, time):
    print("FADE BACK")
    print("LED: "+str(led)+" size: "+str(size)+" time: "+str(time))
    if(ard==None):
        return
    mySerial.fadeBack(ard,led,size,time)

def arduinoBreath(r,g,b,time):
    if(ard==None):
        return
    mySerial.breath(ard,r,g,b,time)

def onMsg(client, data, msg):
    global socketConnected
    global chordType
    if(not socketConnected):
        return
    data = json.loads(msg.payload.decode("utf-8"))
    try:
        s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((addr,port))
        if("thv" in data):
            s.send(("temp "+str(data["thv"]["temperature"])+";").encode())
            s.send(("hum "+str(data["thv"]["humidity"])+";").encode())
        if("co2" in data):
            coData = data["co2"]["co2"]
            s.send(("co "+str(coData)+";").encode())
            #determine chordType
            if(coData<=CO2_GOOD_UNDER):
                chordType = 0
                s.send("chordType 0;".encode())
            elif(coData<=CO2_MODERATE_UNDER):
                chordType = 1
                s.send("chordType 1;".encode())
            else:
                chordType = 2
                s.send("chordType 2;".encode())

        if("pm" in data):
            s.send(("pOne "+str(data["pm"]["pm1.0"])+";").encode())
            s.send(("pTwo "+str(data["pm"]["pm2.5"])+";").encode())
            s.send(("pThree "+str(data["pm"]["pm4.0"])+";").encode())
            s.send(("pFour "+str(data["pm"]["pm4.0"])+";").encode())
    except socket.error:
        s = None
        socketConnected = False
    except:
        pass

mqttClient = mqtt.Client("Rasp")
mqttClient.connect(mqttAddr)
mqttClient.loop_start()
mqttClient.subscribe(topic)
mqttClient.on_message=onMsg

#connect to Arduino (only if available)
for p in serial.tools.list_ports.comports():
    print(p.manufacturer)
    if("Arduino" in p.manufacturer):
        ard = serial.Serial(port="/dev/"+p.name, baudrate=9600)
        print("Arduino connected!")
        break
debug = 0
while True:
    if(socketConnected):
        try:
            msg = s.recv(1024)
            if(msg==b""):
                socketConnected=False
                s.close()
                time.sleep(5)
            for data in msg.decode().split(";\n"):
                if(data==""):
                    continue
                print("MSG:"+data)
                if(data=="beat"):
                    pass
                if(data.startswith("noteON ")):
                    values = data.split(" ")
                    arduinoFade(int(values[1])%24, 0, 0, 255, 1, int(values[2]))
                if(data.startswith("noteOFF ")):
                    values = data.split(" ")
                    arduinoFadeBack(int(values[1])%24, 1, int(values[2]))
                if(data.startswith("breath ")):
                    values = data.split(" ");
                    if(chordType==0):
                        if(chordNum==0):
                            chordNum=1
                            arduinoBreath(0, 102, 0, int(values[1]))
                        else:
                            chordNum=0
                            arduinoBreath(0, 102, 20, int(values[1]))
                    elif(chordType==1):
                        if(chordNum==0):
                            chordNum=1
                            arduinoBreath(136, 34, 34, int(values[1]))
                        else:
                            chordNum=0
                            arduinoBreath(102, 102, 0, int(values[1]))
                    elif(chordType==2):
                        if(chordNum==0):
                            chordNum=1
                            arduinoBreath(102, 0, 0, int(values[1]))
                        else:
                            chordNum=0
                            arduinoBreath(102, 0, 153, int(values[1]))



        except socket.error:
            print("connection lost... Trying again in 5 sec")
            socketConnected=False
            s=None
            time.sleep(5)
    else:
        try:
            s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((addr,port))
            socketConnected=True
            print("connected!")
        except socket.error:
            print("trying to connect in 5 sec")
            time.sleep(5)
#loop = asyncio.get_event_loop()
#try:
#    loop.run_forever()
#finally:
#    loop.close()
