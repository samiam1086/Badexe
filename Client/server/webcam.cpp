#include "stdafx.h"
#include "windows.h"
#include "vfw.h"
#include <string.h>
#include <cstdio>
#include <iostream>

#pragma comment(lib, "Vfw32.lib")
using namespace std;

std::wstring s3ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

string takeSS()
{
	string temp_path = getenv("TEMP");
	string file_path = temp_path + "\\wehr.bmp";

	wstring t = s3ws(file_path);
	wchar_t* ptr = _wcsdup(t.c_str());

	// create the preview window 
	HWND hCam = capCreateCaptureWindow(L"handle", WS_CHILD, 0, 0, 0, 0, ::GetDesktopWindow(), 0);

	if (capDriverConnect(hCam, 0))
	{
		capFileSaveDIB(hCam, ptr);
	}else {
		return "Camera Error";
	}

	DestroyWindow(hCam);

	return file_path;
}