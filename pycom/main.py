#https://docs.pycom.io/tutorials/networks/wlan/#connecting-to-a-router
#https://docs.pycom.io/firmwareapi/micropython/usocket/
import os
import _thread, time
from network import WLAN
import utime, sys, pycom
import pycom_mcp230xx as mcp230xx
#from micropython import const
import irc, oauth
import machine
from machine import I2C, Pin, UART

serial = UART(1, 19200)
i2c = I2C(0, I2C.MASTER)
addresses = mcp230xx.MCP23017(i2c, 0x20)
data = mcp230xx.MCP23008(i2c, 0x21)
cntr = mcp230xx.MCP23008(i2c, 0x22)

def clockpulsed(arg):
    if serial.any():
        print("Serial Ready.")

def checkSerial():
    print(serial)
    while True:
        if serial.any():
            print("Serial Ready.")
        time.sleep(1)

def setupdebugger():
    clockIrq = Pin('P22', mode=Pin.IN, pull=Pin.PULL_DOWN)
    clockIrq.callback(Pin.IRQ_RISING, handler=clockpulsed)
    i2c.init(I2C.MASTER, baudrate=100000)
    if len(i2c.scan()) is not 3:
        print("i2cbus is not 3"+i2c.scan())
    else:
        print("I2C init.")
    addresses.iodir = 0xffff #set both banks to input
    data.iodir = 0xff #set all 8 bits to input
    cntr.iodir = 0x0f #set bit 0-3 to input for the clock, rwb, vda, vpa
    cntr.gpinten = 0xff
    cntr.intcon = 0xff
    print("GPIO", addresses.iodir, data.iodir, cntr.iodir, cntr.intcon, cntr.gpinten)
    print("Initing the 65xx serial.")
    serial.init(19200, bits=8, parity=None, stop=1)
    #_thread.start_new_thread(checkSerial(), ("Thread No:2", 3))

def connectWifi(retries):
    for attempt in range(retries):
        if wifi.isconnected(): #skip if 
            continue
        available_networks = wifi.scan(show_hidden=True)
        current_nets = frozenset([e.ssid for e in available_networks])
        know_net_names = frozenset([key for key in oauth.KNOWN_NETWORKS])
        network_to_use = list(current_nets & know_net_names)
        if not network_to_use:
            print("No known Network found, retrying:")
            continue
        try:
            network_to_use = network_to_use[0]
            net_props = oauth.KNOWN_NETWORKS[network_to_use]
            password = net_props['pwd']
            security = [e.sec for e in available_networks if e.ssid == network_to_use][0]
            wifi.connect(network_to_use, (security, password), timeout=10000)
            while not wifi.isconnected():
                machine.idle()
            print("connected " + network_to_use+" w/ IP:"+wifi.ifconfig()[0] + "in " + str(attempt))
            break
        except Exception as exp:
            print(exp)
    else:
        print("Wifi AP not found in alloted retries.")
        sys.exit()

connectWifi(10)
#setupdebugger()
irc.connectBot()