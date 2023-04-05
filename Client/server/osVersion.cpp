#include "stdafx.h"
#include "windows.h"
#include <stdio.h>
#include <iostream>
#include <string>
#pragma warning(disable:4996) 
using namespace std;

string os_info()
{

	int os_ver = 0.0;

	NTSTATUS(WINAPI *RtlGetVersion)(LPOSVERSIONINFOEXW);

	OSVERSIONINFOEXW os_info;

	*(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");

	if (NULL != RtlGetVersion)
	{
		os_info.dwOSVersionInfoSize = sizeof(os_info);
		RtlGetVersion(&os_info);
		os_ver = os_info.dwMajorVersion;
	}

	if (os_ver == 6){
		if (os_info.dwMinorVersion == 0){
			if (os_info.wProductType == VER_NT_WORKSTATION) {
				return "Windows Vista";
			}
			else {
				return "Windows Server 2008";
			}
		}else if (os_info.dwMinorVersion == 1){
			if (os_info.wProductType == VER_NT_WORKSTATION) {
				return "Windows 7";
			}
			else {
				return "Windows Server 2008 R2";
			}
		}else if (os_info.dwMinorVersion == 2) {
			if (os_info.wProductType == VER_NT_WORKSTATION) {
				return "Windows 8";
			}
			else {
				return "Windows Server 2012";
			}
		}else if (os_info.dwMinorVersion == 3) {
			if (os_info.wProductType == VER_NT_WORKSTATION) {
				return "Windows 8.1";
			}
			else {
				return "Windows Server 2012 R2";
			}
		}
	}else if(os_ver == 10){
		if (os_info.dwMinorVersion == 0) {
			if (os_info.wProductType == VER_NT_WORKSTATION) {
				return "Windows 10";
			}
			else {
				return "Windows Server";
			}
		}
	}else {
		return "Other OS";
	}

}