import socket, _thread, time
import oauth
from pycom import rgbled as led
from machine import rng as random

#https://dev.twitch.tv/docs/client/guide
#ws://irc-ws.chat.twitch.tv:80
SERVER = "irc.chat.twitch.tv"
PORT = 6667
BOTOWNER = 'bahamutkotd'
NICK = 'BahamutCodeBot'
CHANNEL =  '#bahamutkotd'

def connectBot():
    print("connecting to "+ SERVER)
    try:
        global client 
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        serverip = socket.getaddrinfo(SERVER, PORT)
        #client.setblocking(False)
        client.connect(serverip[0][-1])
        send_Data('PASS '+ oauth.TOKEN)
        send_Data('NICK ' + NICK)
        send_Data('JOIN '+ CHANNEL)      
    except Exception as exp:
        raise exp
    finally:
        print("connectBot finished")
        _thread.start_new_thread(Listener, ())

def send_Data(command):
    client.sendall(bytes(command + '\n', "UTF-8"))

def sendchat(command):
    send_Data("PRIVMSG " + CHANNEL +" :"+command)

def Listener():
    while True:
        buffer = client.recv(1024).decode("utf-8")
        if buffer:
            print(str(buffer))
            user = str(buffer[1:buffer.find('!', 1, 26)])
            msg = str(buffer).split()
            if "PING" in msg[0]:
                send_Data("PONG %s" % msg[1]) #todo: add a randomized heartbeat message? or skip a few?
                sendchat("Bot: Hey thanks for hanging out! Like what you see why not drop a follow.")
                led(random())
            elif "!seen" in buffer:
                sendchat("Bot: I see, I respond.")
            elif "!rgb" in buffer:
                sendchat("Led command Recieved")
                try:
                    if msg[4] == 'help':
                        sendchat("Bot: Led command takes values from 0 to 0xffffff as second value,"
                                 "for a list of colors try !rgb colors.")
                    elif msg[4] == 'colors':
                        sendchat("Bot: For defined RGB values check "
                                "https://github.com/FastLED/FastLED/wiki/Pixel-reference#colors.")
                    elif 0xffffff >= int(msg[4]) >= 0:
                        led(int(msg[4]))
                except Exception as exp:
                    print(exp)
                    sendchat("Incorrect input; please try again, !rgb help, for info.")
            elif "!lurk" in buffer:
                sendchat("Bot: Enjoy your lurk %s, Thanks for hanging out." % user)
            elif "!cmd" in buffer:
                msg[4:]
            
        time.sleep(.5)
