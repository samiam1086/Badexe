// https://mohitdabas.wordpress.com/2017/05/29/finding-installed-antivirus-using-visual-c/
#include "stdafx.h"
#include "atlstr.h"
#include <string>
#include <atlbase.h> 
#include <wbemidl.h> 
#include <iostream>
#pragma comment(lib, "wbemuuid.lib")

using namespace std;


string GetAntivirusName()
{
	HRESULT hr = ::CoInitializeSecurity(NULL, -1, NULL, NULL,
		RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL, EOAC_NONE, NULL);

	CComPtr<IWbemLocator> pWbemLocator;
	hr = pWbemLocator.CoCreateInstance(CLSID_WbemLocator);

	CComPtr<IWbemServices> pWbemServices;
	//Security Center for WMI prior to Windows Vista is: "\root\SecurityCenter"
	//Security Center for WMI for Windows Vista and above is: "\root\SecurityCenter2"

	hr = pWbemLocator->ConnectServer(CComBSTR(L"root\\SecurityCenter2"), NULL, NULL, 0, NULL, 0, NULL, &pWbemServices);

	CComPtr<IEnumWbemClassObject> pEnum;
	CComBSTR cbsQuery = L"Select * From AntivirusProduct";
	hr = pWbemServices->ExecQuery(CComBSTR("WQL"), cbsQuery, WBEM_FLAG_FORWARD_ONLY, NULL, &pEnum);

	ULONG uObjectCount = 0;
	CComPtr<IWbemClassObject> pWmiObject;
	hr = pEnum->Next(WBEM_INFINITE, 1, &pWmiObject, &uObjectCount);

	CComVariant cvtVersion;
	hr = pWmiObject->Get(L"displayName", 0, &cvtVersion, 0, 0);
	wstring ws(cvtVersion.bstrVal, SysStringLen(cvtVersion.bstrVal));

	string antivirus_name(ws.begin(), ws.end());

	return antivirus_name;
}

string av_check()
{
	HRESULT hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	string antivirus_name = GetAntivirusName();
	::CoUninitialize();

	return antivirus_name;
}