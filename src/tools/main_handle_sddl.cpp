/*
 *	File: main_handle_sddl.cpp
 *
 *	Created on: 13 March, 2017
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *		https://developer.microsoft.com/en-us/windows/iot/Samples/CustomDeviceAccessor 
 *
 */




#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include "Setupapi.h"
#include <tchar.h>

#ifdef _MSC_VER
#if(_MSC_VER >= 1400)
#pragma warning(disable : 4996)
#endif
#endif

static int MakeErrorReport(void);

#define DEBUG_APP(...) do{\
	printf("ln:%d, fnc:%s\n",__LINE__,__FUNCTION__);}while(0)

#include <Sddl.h>

int GetRegistryContentW(HKEY hKey, const wchar_t* a_cpcName, DWORD* a_pdwType, BYTE* a_pbData, DWORD* a_pdwDataSize);

#define SDL_STRING	"D:P(A;;GA;;;SY)(A;;GA;;;BA)(A;;GA;;;AU)(A;;GA;;;AC)"
//#define SDL_STRING	"O:AOG:DAD:(A;;RPWPCCDCLCSWRCWDWOGA;;;S-1-0-0)"

int main()
{

	printf("version=7\n");
	DEBUG_APP();
	//EnumWDMDriver();

	ULONG ulDescrLen = 0;
	BOOL bRet = FALSE;

#if 1

	PSECURITY_DESCRIPTOR aDescr = (PSECURITY_DESCRIPTOR)0;
	unsigned char* pcSecDescr;

	bRet = ConvertStringSecurityDescriptorToSecurityDescriptor(
		_T(SDL_STRING),
		SDDL_REVISION_1, &aDescr, &ulDescrLen);

	printf("bRet=%d\n", (int)bRet);
	if (bRet) {
		_tprintf(_T("descrLen=%d\n"), (int)ulDescrLen);

		pcSecDescr = (unsigned char*)aDescr;
		for (ULONG i(0); i < ulDescrLen; ++i) { printf("%.2x", (int)pcSecDescr[i]); }
		printf("\n");
	}

	HKEY hKey = (HKEY)0;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		_T("SYSTEM\\ControlSet001\\Enum\\ACPI\\BCM2836\\0"),
		0,
		KEY_READ | KEY_WRITE, // KEY_READ | KEY_WRITE
		&hKey) != ERROR_SUCCESS
		)
	{
		fprintf(stderr, "Unable to open access to the registry!\n");
		goto returnPoint;
	}

	DWORD dwType;
	BYTE vData[2048];
	DWORD dwDataSize(2048);

	if (GetRegistryContentW(hKey, L"Security", &dwType, vData, &dwDataSize) < 0) { goto returnPoint; }
#if 0
	if (ERROR_SUCCESS != RegSetValueEx(hKey, _T("Security"), NULL, dwType, (const BYTE*)aDescr, ulDescrLen))
	{
		MakeErrorReport();
		goto returnPoint;
	}
#endif

	printf("type=");
	switch (dwType)
	{
	case REG_BINARY:
		printf("REG_BINARY");
		break;
	default:
		printf("unknown");
		break;
	}
	printf("\n");

	printf("len=%d\n", (int)dwDataSize);
	pcSecDescr = (unsigned char*)vData;
	for (ULONG i(0); i < dwDataSize; ++i) { printf("%.2x", (int)pcSecDescr[i]); }
	printf("\n");

returnPoint:
	LocalFree(aDescr);
	if (hKey) { RegCloseKey(hKey); }

#else  // #if 0

	const char* cpcSecurityDescriptor =
		"0100049000"
		"0000000000"
		"0000000000"
		"0014000000"
		"02004C0003"
		"0000000000"
		"1400000000"
		"1001010000"
		"0000000512"
		"0000000000"
		"1800000000"
		"1001020000"
		"0000000520"
		"0000002002"
		"0000000018"
		"00000000C0"
		"0102000000"
		"0000052000"
		"0000210200"
		"00";

	TCHAR* pChar = NULL;

	bRet = ConvertSecurityDescriptorToStringSecurityDescriptor(
		const_cast<void*>(((const void*)cpcSecurityDescriptor)),
		SDDL_REVISION_1, SYNCHRONIZE | READ_CONTROL, &pChar, &ulLen);

	printf("bRet=%d\n", (int)bRet);
	if (bRet) { _tprintf(_T("pChar=%s\n"), pChar); }

	LocalFree(pChar);

#endif  // // #if 0

	if (!bRet) { MakeErrorReport(); }

	return 0;

#if 0
	int nReturn(0);
	//const char* cpcComPortName = "\\\\.\\UART0";// Failed
	//const char* cpcComPortName = "\\\\.\\COM0"; // Failed
	const char* cpcComPortName = "UART0";
	const char* cpcError = "";
	const char* cpcCommand = "-0.250000 1 nr";

	HANDLE hComm = CreateFileA(
		cpcComPortName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		0,
		0);

	printf("port:\"%s\", handle:%x\n", cpcComPortName, (unsigned int)hComm);

	if (hComm == INVALID_HANDLE_VALUE)
	{
		nReturn = -1;
		cpcError = "invalid handle";
		goto retiurnPoint;
	}

	DWORD dwWritten;
	char vcBuffer[128];
	memcpy(vcBuffer, cpcCommand, strlen(cpcCommand));
	vcBuffer[strlen(cpcCommand)] = 10;
	vcBuffer[strlen(cpcCommand) + 1] = 13;
	vcBuffer[strlen(cpcCommand) + 2] = 13;
	WriteFile(hComm, vcBuffer, (DWORD)(strlen(cpcCommand) + 3), &dwWritten, NULL);

	printf("handle=%d, written=%d\n", (int)hComm, (int)dwWritten);


retiurnPoint:
	if (hComm && (hComm != INVALID_HANDLE_VALUE)) { CloseHandle(hComm); }
	if (nReturn) { fprintf(stderr, "errCode=%d, err_str=\"%s\"\n", nReturn, cpcError); }

	printf("Press any key to exit!"); fflush(stdout);
	_getch();

#endif

	return 0;
}

#define MAX_VALUE_NAME 16383


int GetRegistryContentW(HKEY hKey, const wchar_t* a_cpcName, DWORD* a_pdwType, BYTE* a_pbData, DWORD* a_pdwDataSize)
{
	wchar_t		achClass[MAX_PATH] = L"";	// buffer for class name 
	DWORD		cchClassName = MAX_PATH;	// size of class string 
	DWORD		cSubKeys = 0;				// number of subkeys 
	DWORD		cbMaxSubKey;				// longest subkey size 
	DWORD		cchMaxClass;				// longest class string 
	DWORD		cValues;					// number of values for key 
	DWORD		cchMaxValue;				// longest value name 
	DWORD		cbMaxValueData;				// longest value data 
	DWORD		cbSecurityDescriptor;		// size of security descriptor 
	FILETIME	ftLastWriteTime;			// last write time 
	DWORD		i, retCode;

	wchar_t		achValue[MAX_VALUE_NAME];
	DWORD		cchValue = MAX_VALUE_NAME;
	DWORD		outDataSize0 = *a_pdwDataSize;

	// Get the class name and the value count. 
	retCode = RegQueryInfoKeyW(
		hKey,                    // key handle 
		achClass,                // buffer for class name 
		&cchClassName,           // size of class string 
		NULL,                    // reserved 
		&cSubKeys,               // number of subkeys 
		&cbMaxSubKey,            // longest subkey size 
		&cchMaxClass,            // longest class string 
		&cValues,                // number of values for this key 
		&cchMaxValue,            // longest value name 
		&cbMaxValueData,         // longest value data 
		&cbSecurityDescriptor,   // security descriptor 
		&ftLastWriteTime);       // last write time 


	if (retCode != ERROR_SUCCESS)
	{
		fprintf(stderr, "RegQueryInfoKeyA failed!\n");
		return -1;
	}

	// Subkeys are not important

	// Enumerate the key values. 
	for (i = 0; i<cValues; i++)
	{
		cchValue = MAX_VALUE_NAME;
		achValue[0] = '\0';
		retCode = RegEnumValueW(hKey, i,
			achValue,
			&cchValue,
			NULL,
			NULL,
			NULL,
			NULL);

		if (retCode == ERROR_SUCCESS)
		{
			*a_pdwDataSize = outDataSize0;
			RegQueryValueExW(hKey, achValue, NULL, a_pdwType, a_pbData, a_pdwDataSize);

			//if (strcmp(achValue, a_cpcName) == 0)
			if (wcsicmp(achValue, a_cpcName) == 0)
			{
				return i;
			}
		} // if (retCode == ERROR_SUCCESS)
	} // for (i = 0; i<cValues; i++)
	return 0;
}


static SP_CLASSIMAGELIST_DATA _spImageData = { 0 };
static short FindDeviceOrder(const TCHAR* szName);
static TCHAR AddNewDeviceOrderNode(const TCHAR* szDevName);
static void GetDeviceInstanceID(HDEVINFO hDevInfo,
	SP_DEVINFO_DATA* pspDevInfoData,
	TCHAR *szInstanceID);
static void GetDeviceInterfaceInfo(HDEVINFO hDevInfo, SP_DEVINFO_DATA spDevInfoData,
	TCHAR *szPath);
static void DisplayDriverDetailInfo(const TCHAR *szBuf, int iImageIdx, int iSelectImage);
static char AddNewDeviceNode(const GUID guid,
	const TCHAR* szName,
	const TCHAR* szInstallID,
	const TCHAR* szPath,
	int wIndex,
	short wOrder);

static char EnumWDMDriver(void)
{
	DEBUG_APP();
	HDEVINFO        hDevInfo = 0L;
	SP_DEVINFO_DATA spDevInfoData = { 0 };
	short           wIndex = 0;

	//
	hDevInfo = SetupDiGetClassDevs(0L, 0L, (HWND)0, DIGCF_PRESENT |
		DIGCF_ALLCLASSES | DIGCF_PROFILE);

	if (hDevInfo == INVALID_HANDLE_VALUE) { return MakeErrorReport(); }

	//
	wIndex = 0;
	spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	//
	while (1)
	{
		if (SetupDiEnumDeviceInfo(hDevInfo,
			wIndex,
			&spDevInfoData))
		{
			TCHAR  szBuf[MAX_PATH] = { 0 };
			short wImageIdx = 0;
			short wItem = 0;
			//
			if (!SetupDiGetDeviceRegistryProperty(hDevInfo,
				&spDevInfoData,
				SPDRP_CLASS, //SPDRP_DEVICEDESC,
				0L,
				(PBYTE)szBuf,
				2048,
				0))
			{
				//DEBUG_APP();
				MakeErrorReport();
				wIndex++;
				continue;
			}

			//
			/*if (SetupDiGetClassImageListEx(&_spImageData,
			NULL,NULL))*/
			{
#if 1
				TCHAR                   szName[64] = { 0 };
				TCHAR                   szID[LINE_LEN] = { 0 };
				TCHAR                   szPath[MAX_PATH] = { 0 };
				DWORD                  dwRequireSize;
				short                  wOrder;
				//
				if (!SetupDiGetClassDescription(&spDevInfoData.ClassGuid,
					szBuf,
					MAX_PATH,
					&dwRequireSize))
				{
					DEBUG_APP();
					wIndex++;
					continue;
				}
				wOrder = FindDeviceOrder(szBuf);
				if (!AddNewDeviceOrderNode(szBuf))
				{
					wIndex++;
					continue;
				}
				GetDeviceInstanceID(hDevInfo, &spDevInfoData, szID);
				GetDeviceInterfaceInfo(hDevInfo, spDevInfoData, szPath);
				//
				if (SetupDiGetDeviceRegistryProperty(hDevInfo,
					&spDevInfoData,
					SPDRP_FRIENDLYNAME,
					0L,
					(PBYTE)szName,
					63,
					0))
				{
					DisplayDriverDetailInfo(szName, wImageIdx, wImageIdx);
					AddNewDeviceNode(spDevInfoData.ClassGuid, szName, szID, szPath, wIndex, wOrder);
				}
				else if (SetupDiGetDeviceRegistryProperty(hDevInfo,
					&spDevInfoData,
					SPDRP_DEVICEDESC,
					0L,
					(PBYTE)szName,
					63,
					0))
				{
					DisplayDriverDetailInfo(szName, wImageIdx, wImageIdx);
					AddNewDeviceNode(spDevInfoData.ClassGuid, szName, szID, szPath, wIndex, wOrder);
					//                    if (!GetFirmwareEnvironmentVariable(szName, (LPCSTR)&spDevInfoData.ClassGuid, szBuf, 127))
					//                        ShowErrorMsg(_hDlg, GetLastError(), "GetFirmwareEnvironmentVariable");
				}
#endif
			}
			//            SetupDiDestroyDriverInfoList(hDevInfo, &spDevInfoData, SPDIT_COMPATDRIVER);
		}
		else
			break;
		wIndex++;
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);
	return 1;
}


static void GetDeviceInstanceID(HDEVINFO hDevInfo,
	SP_DEVINFO_DATA* pspDevInfoData,
	TCHAR *szInstanceID)
{
	DEBUG_APP();
	if (!SetupDiGetDeviceInstanceId(hDevInfo,
		pspDevInfoData,
		szInstanceID,
		LINE_LEN,
		0))
		MakeErrorReport();

	printf("dev_inst=%d\n", (int)pspDevInfoData->DevInst);
}



static int MakeErrorReport(void)
{
	DWORD dwError = GetLastError();

	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		0, // Default language
		(LPSTR)&lpMsgBuf,
		0,
		NULL
	);

	fprintf(stderr, "err_code=%d, text=%s", (int)dwError, (LPSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
	return (int)dwError;
}


struct DEVICE_ORDER
{
	DEVICE_ORDER* pNext;
	TCHAR szDevName[256];
};

static DEVICE_ORDER *_pOrderHead = 0L;


static short FindDeviceOrder(const TCHAR* szName)
{
	DEVICE_ORDER *pList = _pOrderHead->pNext;
	short        wOrder = 0;
	//
	while (pList)
	{
		if (!_tcscmp(pList->szDevName, szName))
			wOrder++;
		pList = pList->pNext;
	};
	return wOrder;
}



static DEVICE_ORDER* AllocNewDeviceOrderNode(void)
{
	DEVICE_ORDER* pNew = (DEVICE_ORDER*)LocalAlloc(LPTR, sizeof(DEVICE_ORDER));
	//
	if (!pNew)
	{
		MakeErrorReport();
		return NULL;
	};
	//RtlZeroMemory(pNew->szDevName, sizeof(char)*LINE_LEN);
	pNew->pNext = 0L;
	return pNew;
}


static TCHAR AddNewDeviceOrderNode(const TCHAR* szDevName)
{
	DEVICE_ORDER* pAdd = AllocNewDeviceOrderNode();
	//
	if (!pAdd)
		return 0;
	memcpy(pAdd->szDevName, szDevName, _tcslen(szDevName));
	pAdd->pNext = _pOrderHead->pNext;
	_pOrderHead->pNext = pAdd;
	return 1;
}



static void GetDeviceInterfaceInfo(HDEVINFO hDevInfo, SP_DEVINFO_DATA spDevInfoData,
	TCHAR *szPath)
{
	SP_DEVICE_INTERFACE_DATA spDevInterfaceData = { 0 };
	//
	spDevInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	if (!SetupDiCreateDeviceInterface(hDevInfo,
		&spDevInfoData,
		&spDevInfoData.ClassGuid,
		0L,
		0L,
		&spDevInterfaceData))
		MakeErrorReport();
	else
	{
		SP_DEVICE_INTERFACE_DETAIL_DATA *pspDevInterfaceDetail = 0L;
		DWORD                           dwRequire = 0L;
		//
		if (!SetupDiGetDeviceInterfaceDetail(hDevInfo,
			&spDevInterfaceData,
			0L,
			0,
			&dwRequire,
			0L))
		{
			DWORD dwError = GetLastError();
			//
			if (dwError != ERROR_INSUFFICIENT_BUFFER)
			{
				MakeErrorReport();
				return;
			};
		}
		//
		pspDevInterfaceDetail = (SP_DEVICE_INTERFACE_DETAIL_DATA*)LocalAlloc(LPTR,
			sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA)*dwRequire);
		pspDevInterfaceDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		if (!SetupDiGetDeviceInterfaceDetail(hDevInfo,
			&spDevInterfaceData,
			pspDevInterfaceDetail,
			dwRequire,
			&dwRequire,
			0L))
		{
			DWORD dwError = GetLastError();
			//
			if (dwError != ERROR_INSUFFICIENT_BUFFER)
				MakeErrorReport();
		}
		else
		{
			memcpy(szPath, pspDevInterfaceDetail->DevicePath,
				sizeof(TCHAR)*_tcslen(pspDevInterfaceDetail->DevicePath));
			//            switch(spDevInterfaceData.                    
		}
		//
		if (pspDevInterfaceDetail)
			LocalFree(pspDevInterfaceDetail);
	}
}


static void DisplayDriverDetailInfo(const TCHAR *szBuf, int iImageIdx, int iSelectImage)
{
	_tprintf(_T("szBuf=\"%s\", iImageIdx=%d, iSelectImage=%d\n"), szBuf, iImageIdx, iSelectImage);
}


struct DEVICE_LIST
{
	GUID guid;
	TCHAR szInstallID[256];
	TCHAR	szName[256];
	TCHAR	szPath[256];
	int		wIndex;
	short  wOrder;
	DEVICE_LIST* pNext;
};

static DEVICE_LIST* AllocNewDeviceNode(void)
{
	DEVICE_LIST* pNew = (DEVICE_LIST*)LocalAlloc(LPTR, sizeof(DEVICE_LIST));
	//
	if (!pNew)
	{
		MakeErrorReport();
		return 0;
	};
	//RtlZeroMemory(&pNew->guid, sizeof(GUID));
	//RtlZeroMemory(pNew->szInstallID, sizeof(TCHAR)*LINE_LEN);
	//RtlZeroMemory(pNew->szName, sizeof(char)*MAX_PATH);
	//RtlZeroMemory(pNew->szPath, sizeof(char)*MAX_PATH);
	pNew->wOrder = -1;
	pNew->wIndex = -1;
	pNew->pNext = 0L;
	return pNew;
}

static DEVICE_LIST  *_pHead = 0L;

#if 1
static char AddNewDeviceNode(const GUID guid,
	const TCHAR* szName,
	const TCHAR* szInstallID,
	const TCHAR* szPath,
	int wIndex,
	short wOrder)
{
	DEVICE_LIST* pAdd = AllocNewDeviceNode();
	//
	if (!pAdd)
		return 0;
	memcpy(&pAdd->guid, &guid, sizeof(GUID));
	memcpy(pAdd->szInstallID, szInstallID, sizeof(TCHAR)*_tcslen(szInstallID));
	memcpy(pAdd->szName, szName, sizeof(TCHAR)*_tcslen(szName));
	memcpy(pAdd->szPath, szPath, sizeof(TCHAR)*_tcslen(szPath));
	pAdd->wIndex = wIndex;
	pAdd->wOrder = wOrder;
	pAdd->pNext = _pHead->pNext;
	_pHead->pNext = pAdd;
	return 1;
}
#endif

