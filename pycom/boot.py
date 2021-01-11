import pycom
import sys
from machine import Pin
from network import WLAN
ThreadKill = dict()

def button_Pushed(arg):
    print("button Pushed")
    machine.reset()

pycom.heartbeat(False)
Button = Pin('P12', mode=Pin.IN, pull=Pin.PULL_DOWN)
Button.callback(Pin.IRQ_RISING, handler=button_Pushed)
wifi = WLAN(mode=WLAN.STA)