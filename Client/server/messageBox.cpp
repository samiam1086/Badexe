#include <string>
using namespace std;

void mBox(string message) {
	string command = ("mshta vbscript:Execute(\"msgbox \"\"" + message +"\"\":close\")");
	system(command.c_str());
}