/*
 *	File: registry_functions.cpp
 *
 *	Created on: Sep 14, 2016
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements all functions connected to posix threading
 *		1) 
 *
 *
 */

#include "stdafx.h"
#include "registry_functions.h"
#include <stdio.h>
#include <tchar.h>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

#ifdef __cplusplus
extern "C"
{
#endif

int OpenEnvironmentRegistry(HKEY* a_phKey, REGSAM a_mode)
{
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		"System\\CurrentControlSet\\Control\\Session Manager\\Environment",
		0,
		a_mode, // KEY_READ | KEY_WRITE
		a_phKey) != ERROR_SUCCESS
		)
	{
		return -1;
	}
	return 0;
}


int ModifyRegistry(HKEY hKey, const char* a_name, DWORD a_dwType, const BYTE* a_data, DWORD a_data_size)
{
	return RegSetValueExA(hKey, a_name, NULL, a_dwType, a_data, a_data_size)== ERROR_SUCCESS ? 0 : -1;
}


int ModifiEnvironmentalVariable(HKEY hKey, const char* a_name, DWORD a_dwType, const char* a_data, DWORD a_data_size, BOOL a_bInform)
{
	int nRet=ModifyRegistry(hKey, a_name, a_dwType, (const BYTE*)a_data, a_data_size);
#ifndef __ARM
	if (a_bInform) { BroadcastSystemMessageA(0, NULL, WM_SETTINGCHANGE, (WPARAM)0, (LPARAM)0); }
#endif
	return nRet;
}


int GetRegistryContent(HKEY hKey, const char* a_cpcName, DWORD* a_pdwType,BYTE* a_pbData, DWORD* a_pdwDataSize)
{
	char		achClass[MAX_PATH] = "";	// buffer for class name 
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

	char		achValue[MAX_VALUE_NAME];
	DWORD		cchValue = MAX_VALUE_NAME;
	DWORD		outDataSize0 = *a_pdwDataSize;

	// Get the class name and the value count. 
	retCode = RegQueryInfoKeyA(
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
		fprintf(stderr,"RegQueryInfoKeyA failed!\n");
		return -1;
	}
	
	// Subkeys are not important
	
	// Enumerate the key values. 
	for (i = 0; i<cValues; i++)
	{
		cchValue = MAX_VALUE_NAME;
		achValue[0] = '\0';
		retCode = RegEnumValueA(hKey, i,
			achValue,
			&cchValue,
			NULL,
			NULL,
			NULL,
			NULL);

		if (retCode == ERROR_SUCCESS)
		{
			*a_pdwDataSize = outDataSize0;
			RegQueryValueExA(hKey, achValue, NULL, a_pdwType, a_pbData, a_pdwDataSize);

			//if (strcmp(achValue, a_cpcName) == 0)
			if (_stricmp(achValue, a_cpcName) == 0)
			{
				return i;
			}
		} // if (retCode == ERROR_SUCCESS)
	} // for (i = 0; i<cValues; i++)
	return 0;
}



int TestRegistry(HKEY hKey)
{
#define DATA_OUT_SIZE	1024
	//char		achKey[MAX_KEY_LENGTH];		// buffer for subkey name
	//DWORD		cbName;						// size of name string 
	char		achClass[MAX_PATH] = "";	// buffer for class name 
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

	char		achValue[MAX_VALUE_NAME];
	DWORD		cchValue = MAX_VALUE_NAME;
	DWORD		dwType;
	BYTE		pbData[DATA_OUT_SIZE];
	DWORD		outDataSize = DATA_OUT_SIZE;

	// Get the class name and the value count. 
	retCode = RegQueryInfoKeyA(
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

	// Subkeys are not important
	
	// Enumerate the key values. 

	for (i = 0; i<cValues; i++)
	{
		cchValue = MAX_VALUE_NAME;
		achValue[0] = '\0';
		retCode = RegEnumValueA(hKey, i,
			achValue,
			&cchValue,
			NULL,
			NULL,
			NULL,
			NULL);

		if (retCode == ERROR_SUCCESS)
		{
			outDataSize = DATA_OUT_SIZE;
			RegQueryValueExA(hKey, achValue, NULL, &dwType, pbData, &outDataSize);

			printf("(%d) %s\t", i + 1, achValue);
			bool bIsString = false;

			switch (dwType)
			{
			case REG_EXPAND_SZ:
				bIsString = true;
				printf("REG_EXPAND_SZ");
				break;
			case REG_SZ:
				bIsString = true;
				printf("REG_SZ");
				break;
			case REG_MULTI_SZ:
				bIsString = true;
				printf("REG_MULTI_SZ");
				break;
			default:
				bIsString = false;
				_tprintf(TEXT("default"));
				break;
			} // switch (dwType)
			_tprintf(TEXT("\t"));
			if (bIsString)
			{
				printf("%s", (char*)pbData);
			}
			_tprintf(TEXT("\n"));

		} // if (retCode == ERROR_SUCCESS)
	} // for (i = 0; i<cValues; i++)

	//
	return 0;
}


#ifdef __cplusplus
}
#endif
