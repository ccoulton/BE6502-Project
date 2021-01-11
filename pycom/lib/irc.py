import socket, _thread
import oauth
import pycom
import time

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
        _thread.start_new_thread(Listener(),
            ("Thread No:1",2))

def send_Data(command):
    client.sendall(bytes(command + '\n', "UTF-8"))

def sendchat(command):
    send_Data("PRIVMSG " + CHANNEL +" :"+command)

def Listener():
    while True:
        buffer = client.recv(1024).decode("utf-8")
        if buffer:
            print(str(buffer))
            msg = str(buffer).split()
            if "PING" in msg[0]:
                send_Data("PONG %s" % msg[1]) #todo: add a randomized heartbeat message? or skip a few?
                sendchat("Bot: Hey thanks for hanging out! Like what you see why not drop a follow.")
            elif "!seen" in buffer:
                sendchat("Bot: I see, I respond.")
            elif "!rgb" in buffer:
                sendchat("Led command Recieved")
                try:
                    if 0xffffff >= int(msg[4]) >= 0:
                        pycom.rgbled(int(msg[4]))
                except IndexError:
                    sendchat("Led command takes values from 0 to 0xffffff as second value")
                except Exception as exp:
                    print(exp)
                    sendchat("Incorrect input; please try again type the command with out arguments for help")
            elif "!lurk" in buffer:
                print(msg)
                sendchat("Bot: Enjoy your lurk, Thanks for hanging out.")
            
        time.sleep(.5)
