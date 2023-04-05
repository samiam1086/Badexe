#include <string>
#include "stdafx.h"
#include "windows.h"
#include <iostream>
#include <stdio.h>
using namespace std;


BOOL IsProgramElevated() {
	BOOL fRet = FALSE;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
			fRet = Elevation.TokenIsElevated;
		}
	}
	if (hToken) {
		CloseHandle(hToken);
	}
	return fRet;
}

string perms() {
	BOOL priv = IsProgramElevated();
	if (priv == TRUE) {
		return "Admin";
	}
	else {
		return "User";
	}
	return "User";
}