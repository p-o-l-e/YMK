# touchA.py by CWE

from machine import *
from utime import *

led= Pin(25, Pin.OUT)
led.value(0)

send= Pin(16, Pin.OUT) # via 1meg resistor connected to touch pad
send.value(0)

t1= Pin(17,Pin.IN) # directly connected to touch pad
sleep(1)

trigLevel= 0

def getT1(): # Sensor routine gives back a number
    start=0
    end=0
    start= ticks_us()
    send.value(1)
    while t1.value()<1:
        pass
    end= ticks_us()
    send.value(0)
    return(end-start)

def calibT1():
    global trigLevel
    for i in range(0,50):
        trigLevel= max(getT1()*1.3 , trigLevel) # factor 1.3 perhaps needs tuning
        print(".",end="")
        sleep(0.05)

print("Calibrating....")
calibT1()
print("TrigLevel: ", trigLevel)

actT1= False

while True:
    lastT1= actT1
    t1Val= getT1()
    
    actT1= t1Val > trigLevel
    if actT1 and lastT1: # switch only, if two consecutive same levels detected
        led.value(1)
    elif actT1==False and lastT1==False:
        led.value(0)
    print(trigLevel,t1Val) # use with plotter of thonny
    sleep(0.05)
    
