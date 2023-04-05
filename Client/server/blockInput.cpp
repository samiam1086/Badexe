#include "stdafx.h"
#include "windows.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <winuser.h>

#pragma warning(disable:4996) 
using namespace std;

string dis_inp(bool tof) {
	string color_GRE = "\033[92m";
	string color_reset = "\033[0m";
	string GRE_plus = color_GRE + "[+] " + color_reset;

	if (tof) {
		BlockInput(true);
		return GRE_plus + "Input is disabled!";
	}else {
		BlockInput(false);
		return GRE_plus + "Input is enabled!";
	}
}
