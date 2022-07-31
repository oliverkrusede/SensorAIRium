import serial
import serial.tools.list_ports
def _clip(n, h, l):
    return max(l, min(h, n))
def fade(ard, led, r, g, b, size, time):
    out=bytearray() #type 00 -> fadeIn
    out.append(_clip(led, 31, 0))
    #print(str(r)+" "+str(g)+" "+str(b))    
    out.append(_clip(r, 255, 0))
    out.append(_clip(g, 255, 0))
    out.append(_clip(b, 255, 0))
    
    lastPack=_clip(size, 7,0)+(_clip(round(time/100), 31,1)*8)
    print(lastPack)
    out.append(lastPack)
    print("--MySerial--")
    print(out)
    print(list(out))
    ard.write(out)

def fadeBack(ard, led,size,time):
    out=bytearray() 
    out.append(_clip(led, 31, 0)+64) #128 for the type
    lastPack=_clip(size, 7,0)+(_clip(round(time/100), 31,1)*8)
    out.append(lastPack)
    ard.write(out)

def breath(ard, r, g, b, time):
    out=bytearray()
    out.append(128+_clip(round(time/500), 63,1))
    out.append(_clip(r, 255, 0))
    out.append(_clip(g, 255, 0))
    out.append(_clip(b, 255, 0))
    print(out)
    print(list(out))
    ard.write(out)

def connect():
    for p in serial.tools.list_ports.comports():
        print(p.manufacturer)
        if("Arduino" in p.manufacturer):
            return serial.Serial(port="/dev/"+p.name, baudrate=9600)
        #print("Arduino connected!")
        #break
