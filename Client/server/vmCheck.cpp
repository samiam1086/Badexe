// https://github.com/lunatic0x0/VMdetection
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <psapi.h>
#include <stdio.h>
#include <iphlpapi.h>
#include "stdafx.h"
#include <stdlib.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>
#include <string>
#pragma warning(disable:4996) 
#pragma comment(lib, "IPHLPAPI.lib")
using namespace std;
#define ARRAY_SIZE 1024

void GetErrorMessage(int errorcode) {

	LPWSTR text;
	DWORD error = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, errorcode, NULL, (LPWSTR)&text, 0, nullptr);

	if (error > 0) {

		printf("[-] Error: %ws", text);
		::LocalFree(text);
	}
}

bool CheckMacAdress(const char *maddress) {

	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;

	DWORD dwBufLen = sizeof(PIP_ADAPTER_INFO);

	char* mac_addr_buffer = (char*)malloc(18);
	pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(PIP_ADAPTER_INFO));

	if (pAdapterInfo == NULL) {

		free(mac_addr_buffer);
		return false;
	}


	if (::GetAdaptersInfo(pAdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO*)malloc(dwBufLen);
		if (pAdapterInfo == NULL) {
			free(mac_addr_buffer);
			return false;
		}
	}

	if (::GetAdaptersInfo(pAdapterInfo, &dwBufLen) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		do {
			sprintf(mac_addr_buffer, "%02X:%02X:%02X:%02X:%02X:%02X",
				pAdapter->Address[0], pAdapter->Address[1],
				pAdapter->Address[2], pAdapter->Address[3],
				pAdapter->Address[4], pAdapter->Address[5]);

			
			char* result = strstr(mac_addr_buffer, maddress);
			if (result) {
				return true;
			}

			pAdapter = pAdapter->Next;
		} while (pAdapter);
	}

	free(pAdapterInfo);
	return false;
}


bool CheckVMwareTools() {

	HKEY hVMToolKey;
	char* vmtool1_buffer;
	DWORD size = 256;
	DWORD type;
	vmtool1_buffer = (char*)malloc(sizeof(char) * size);

	LSTATUS RegOpenStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\ControlSet001\\Services\\VMTools", 0, KEY_READ, &hVMToolKey);
	if (RegOpenStatus != ERROR_SUCCESS) {
		
		free(vmtool1_buffer);
		return false;
	}

	DWORD RegQueryStatus = RegQueryValueExA(hVMToolKey, "ImagePath", NULL, &type, (LPBYTE)vmtool1_buffer, &size);
	while (RegQueryStatus == ERROR_MORE_DATA) {
		size += 256;
		vmtool1_buffer = (char*)realloc(vmtool1_buffer, size);
		RegQueryStatus = RegQueryValueExA(hVMToolKey, "0", NULL, &type, (LPBYTE)vmtool1_buffer, &size);
	}

	if (RegQueryStatus == ERROR_SUCCESS) {
		char* lower_case_value = CharLowerA((char*)vmtool1_buffer);
		char* result = strstr(lower_case_value, "vmtoolsd.exe");
		if (result) {
			return true;
		}
		else {
			return false;
		}
	}
	RegCloseKey(hVMToolKey);
	free(vmtool1_buffer);
	return false;
}

bool CheckRegistry(const char *val) {

	HKEY hKey;
	char* disk_enum_buffer;
	DWORD size = 256;
	DWORD type;
	disk_enum_buffer = (char*)malloc(sizeof(char)*size);

	LSTATUS RegOpenStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\ControlSet001\\Services\\disk\\Enum", 0, KEY_READ, &hKey);
	if (RegOpenStatus != ERROR_SUCCESS) {
		free(disk_enum_buffer);
		return false;
	}

	DWORD RegQueryStatus = RegQueryValueExA(hKey, "0", NULL, &type, (LPBYTE)disk_enum_buffer, &size);
	while (RegQueryStatus == ERROR_MORE_DATA) {
		size += 256;
		disk_enum_buffer = (char*)realloc(disk_enum_buffer, size);
		RegQueryStatus = RegQueryValueExA(hKey, "0", NULL, &type, (LPBYTE)disk_enum_buffer, &size);
	}

	if (RegQueryStatus == ERROR_SUCCESS) {
		char* lower_case_value = CharLowerA((char*)disk_enum_buffer);
		char* result = strstr(lower_case_value, val);
		if (result) {
			return true;
		}
		else {
			return false;
		}
	}

	RegCloseKey(hKey);
	free(disk_enum_buffer);
}

DWORD CheckRunningVMProcess(const wchar_t *process) {

	DWORD pid = 0;
	PROCESSENTRY32 pe;
	HANDLE hSnap;

	pe.dwSize = sizeof(PROCESSENTRY32);

	hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (::Process32First(hSnap, &pe)) {
		while (::Process32Next(hSnap, &pe)) {
			//printf("%ls\n", pe.szExeFile);

			if ((_tcsicmp(pe.szExeFile, process) == 0)) {
				CloseHandle(hSnap);
				return pe.th32ProcessID;
			}
		}
	}
	CloseHandle(hSnap);
	return pid;

}


bool CheckVMServices(const wchar_t * val)
{
	const wchar_t *srvcs = _T("SYSTEM\\ControlSet001\\Services\\") + *val;
	const wchar_t *micro = _T("SOFTWARE\\Microsoft\\") + *val;
	const wchar_t *dsdt = _T("HARDWARE\\ACPI\\DSDT") + *val;
	const wchar_t *fadt = _T("HARDWARE\\ACPI\\FADT") + *val;
	HKEY hTestKey;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, (LPCWSTR)srvcs, 0, KEY_READ, &hTestKey) == ERROR_SUCCESS)
	{
		RegCloseKey(hTestKey);
		return true;
	}else if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, (LPCWSTR)micro, 0, KEY_READ, &hTestKey) == ERROR_SUCCESS)
	{
		RegCloseKey(hTestKey);
		return true;
	}
	else if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, (LPCWSTR)dsdt, 0, KEY_READ, &hTestKey) == ERROR_SUCCESS)
	{
		RegCloseKey(hTestKey);
		return true;
	}
	else if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, (LPCWSTR)fadt, 0, KEY_READ, &hTestKey) == ERROR_SUCCESS)
	{
		RegCloseKey(hTestKey);
		return true;
	}
	
	RegCloseKey(hTestKey);
	return false;

}


bool CheckVMDrivers() {

	LPVOID drivers[ARRAY_SIZE];
	DWORD pcbNeeded;
	int cDrivers;

	if (::EnumDeviceDrivers(drivers, sizeof(drivers), &pcbNeeded) && pcbNeeded < sizeof(drivers)) {

		CHAR szDriver[ARRAY_SIZE];
		cDrivers = pcbNeeded / sizeof(drivers[0]);
		for (int i = 0; i < cDrivers; i++) {

			GetDeviceDriverBaseNameA(drivers[i], szDriver, sizeof(szDriver) / sizeof(szDriver[0]));
			if (strcmp(szDriver, "vmmouse.sys") == 0 || strcmp(szDriver, "vmmemctl.sys") == 0) {
				return true;
			}
		}
	}
	else {
		return false;
	}

	return false;
}

string vmware_check() {
	int pid;
	if (CheckMacAdress("00:0C:29")) {return "VMware";}
	if (CheckRegistry("vmware")) {return "VMware";}

	pid = (int)CheckRunningVMProcess(_T("vmacthlp.exe"));
	if (pid != NULL) {return "VMware";}
	pid = (int)CheckRunningVMProcess(_T("vmtoolsd.exe"));
	if (pid != NULL) { return "VMware"; }
	pid = (int)CheckRunningVMProcess(_T("vmwareuser.exe"));
	if (pid != NULL) { return "VMware"; }
	pid = (int)CheckRunningVMProcess(_T("vmwaretray.exe"));
	if (pid != NULL) { return "VMware"; }
	const wchar_t *vmware_svc[] = {_T("vmdebug"), _T("vmmouse"), _T("VMTools"), _T("VMMEMCTL")};
	for (int i = 0; i < 4; i++) {
		if (CheckVMServices(vmware_svc[i])) { return "VMware"; }
	}
	
	if (CheckVMwareTools()) {return "VMware";}
	if (CheckVMDrivers()) {return "VMware";}
	return "";
}

string vbox_check() {
	
	int pid;
	if (CheckMacAdress("08:00:27")) {return "Virtualbox";}
	if (CheckRegistry("vbox")) { return "Virtualbox"; }
	if (CheckRegistry("virtualbox")) { return "Virtualbox"; }

	pid = (int)CheckRunningVMProcess(_T("vboxservice.exe"));
	if (pid != NULL) { return "Virtualbox"; }
	pid = (int)CheckRunningVMProcess(_T("vboxtray.exe"));
	if (pid != NULL) { return "Virtualbox"; }
	
	const wchar_t *virtbox_svc[] = {_T("VBoxMouse"), _T("VBoxGuest"), _T("VBoxService"), _T("VBoxSF"), _T("VBOX__"), _T("vbox")};
	for (int i = 0; i < 6; i++) {
		if (CheckVMServices(virtbox_svc[i])) {return "Virtualbox"; }
	}

	return "";
}

string vpc_check() {
	int pid;

	pid = (int)CheckRunningVMProcess(_T("vmsrvc.exe"));
	if (pid != NULL) { return "VirtualPC"; }
	pid = (int)CheckRunningVMProcess(_T("vmusrvc.exe"));
	if (pid != NULL) { return "VirtualPC"; }

	const wchar_t *virtpc_svc[] = { _T("vpcbus"), _T("vpc-s3"), _T("vpcuhub"), _T("msvmmouf") };
	for (int i = 0; i < 4; i++) {
		if (CheckVMServices(virtpc_svc[i])) { return "VirtualPC"; }
	}
	return "";
}

string xen_check() {

	const wchar_t *xen_svc[] = { _T("xenevtchn"), _T("xennet"), _T("xennet6"), _T("xensvc"), _T("xenvdb"), _T("Xen")};
	for (int i = 0; i < 6; i++) {
		if (CheckVMServices(xen_svc[i])) { return "Xen"; }
	}
	return "";
}

string hyperv_check() {
	if (CheckMacAdress("00:15:5d")) {return "Hyper-V";};
	const wchar_t *hyperv_svc[] = {_T("Hyper-V"), _T("VirtualMachine")};
	return "";
}

string vm_check()
{
	string vm = "";
	vm += vmware_check();
	vm += vbox_check();
	vm += vpc_check();
	vm += xen_check();
	vm += hyperv_check();

	if (vm.length() < 2) {
		vm = "Not a vm";
	}
	return vm;
}