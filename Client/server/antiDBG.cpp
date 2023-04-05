#include "stdafx.h"
#include "windows.h"
#include <tlhelp32.h>
#include <tchar.h>
using namespace std;

bool IsProcessRunning(const TCHAR* const executableName) {
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (!Process32First(snapshot, &entry)) {
		CloseHandle(snapshot);
		return false;
	}
	do {
		if (!_tcsicmp(entry.szExeFile, executableName)) {
			CloseHandle(snapshot);
			return true;
		}
	} while (Process32Next(snapshot, &entry));

	CloseHandle(snapshot);
	return false;
}


bool debug_chk() {
	if (IsDebuggerPresent()) {
		return true;
	}
	
	TCHAR olly[12] = TEXT("ollydbg.exe");
	TCHAR ida[12] = TEXT("ida.exe");
	TCHAR windbg[11] = TEXT("windbg.exe");
	TCHAR x32[11] = TEXT("x32dbg.exe");
	TCHAR x64[11] = TEXT("x64dbg.exe");

	
	if (IsProcessRunning(olly)) {
		return true;
	}else if (IsProcessRunning(windbg)) {
		return true;
	}else if (IsProcessRunning(x32)) {
		return true;
	}else if (IsProcessRunning(x64)) {
		return true;
	}else if (IsProcessRunning(ida)) {
		return true;
	}

	SetLastError(0);
	OutputDebugStringA("Antioch");
	if (GetLastError() != 0){
		return true;
	}

	__try {
		DebugBreak();
		return true;
	}
	__except (GetExceptionCode() == EXCEPTION_BREAKPOINT ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
		
	}


	return false;
}