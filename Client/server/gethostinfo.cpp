#include <iostream>
#include <winsock.h>
#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <Lmcons.h>

using namespace std;

string get_hostname() {

	WSADATA wsa_data;
	/* Load Winsock 2.0 DLL */
	if (WSAStartup(MAKEWORD(2, 0), &wsa_data) != 0)
	{
		return "Error";
	}
	char myhostname[256];
	int rc = gethostname(myhostname, sizeof myhostname);
	WSACleanup(); /* Cleanup Winsock */

	return myhostname;
	
}

string get_username() {
	TCHAR username[UNLEN + 1];
	DWORD size = UNLEN + 1;
	GetUserName((TCHAR*)username, &size);
	wstring temp_wstring(username);
	string username_string(temp_wstring.begin(), temp_wstring.end());
	return username_string;
}

string get_cwd() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	wstring::size_type pos = wstring(buffer).find_last_of(L"\\/");
	wstring temp = wstring(buffer).substr(0, pos);
	string path_string(temp.begin(), temp.end());
	return path_string;
}