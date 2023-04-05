#include "stdafx.h"
#include <windows.h>
#include <atlimage.h>
#include <iostream>
#include <string.h>


using namespace std;

std::wstring s2ws(const std::string& s)
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

string screenshot() {
	string temp_path = getenv("TEMP");
	string file_path = temp_path + "\\fewgy.jpg"; // jees that was hell just to get that to work
	wstring t = s2ws(file_path);
	wchar_t* ptr = _wcsdup(t.c_str());

	int x1, x2, y1, y2, w, h;

	x1 = GetSystemMetrics(SM_XVIRTUALSCREEN);
	y1 = GetSystemMetrics(SM_YVIRTUALSCREEN);
	x2 = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	y2 = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	
	w = x2 - x1;
	h = y2 - y1;
	HDC screen = GetDC(NULL);
	HDC de_hdc = CreateCompatibleDC(screen);
	HBITMAP bitmap_data = CreateCompatibleBitmap(screen, w, h);
	SelectObject(de_hdc, bitmap_data);
	BitBlt(de_hdc, 0, 0, w, h, screen, x1, y1, SRCCOPY);

	CImage image;
	image.Attach(bitmap_data);
	image.Save(ptr);

	return file_path;
}