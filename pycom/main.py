#https://docs.pycom.io/tutorials/networks/wlan/#connecting-to-a-router
#https://docs.pycom.io/firmwareapi/micropython/usocket/
import os
import _thread, time
from network import WLAN
import utime, sys
#from micropython import const
import irc, oauth
import machine
from machine import UART

serial = UART(1, 19200)
def setupSerial():
    serial.init(19200, bits=8, parity=None, stop=1)
    _thread.start_new_thread(checkSerial, ())

def checkSerial():
    while 1:
        if serial.any():
            print(serial.read())
        time.sleep(.5)

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
setupSerial()
irc.connectBot()