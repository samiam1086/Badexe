#include <stdio.h>
#include <string>
#include "stdafx.h"

using namespace std;

//Only issue is if you give an invalid command it breaks program
string cmdExec(const char* cmd) {
	char buffer[128];
	string result = "";
	FILE* pipe = _popen(cmd, "r");
	if (!pipe) throw runtime_error("_popen() failed!");
	try {
		while (fgets(buffer, sizeof buffer, pipe) != NULL) {
			result += buffer;
		}
	}catch (...) {
		_pclose(pipe);
		return "Error";
	}
	_pclose(pipe);
	return result;
}


string cmd(string command) {
	stringstream ss(cmdExec(command.data()).data());
	string out = ss.str();
	return out;
}