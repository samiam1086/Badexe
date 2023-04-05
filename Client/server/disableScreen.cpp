using namespace std;

string blankMonitor(bool tof) {

	string color_GRE = "\033[92m";
	string color_reset = "\033[0m";
	string GRE_plus = color_GRE + "[+] " + color_reset;

	if (tof){
		SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
		return GRE_plus + "Monitor is now turned off";
	}else {
		SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, -1);
		return GRE_plus + "Monitor is now turned on";
	}
}