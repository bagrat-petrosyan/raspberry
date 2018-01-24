/*
 *	File: main_set_env.cpp
 *
 *	Created on: Sep 14, 2016
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements all functions connected to posix threading
 *		1) 
 *
 *
 */

#ifndef __ARM
#define  _DEBUG_APP_
#endif

#include "registry_functions.h"
#include <conio.h>
#include <stdio.h>

#ifdef _MSC_VER
#if(_MSC_VER >= 1400)
 //#define		_CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)
#endif
#endif

int main(int argc, char* argv[])
{
	int nReturn = 0;
	char vcData[2048];
	DWORD dwDataSize = 2048;
	HKEY hKey = (HKEY)0;
	int nIndex = 1;
	const char* cpcEnvVar;
	int nAdd = 0;
	DWORD dwType = REG_SZ;

	if (argc < 3)
	{
		fprintf(stderr, "name and value should be specified!\n");
		goto returnPoint;
	}

	cpcEnvVar = argv[2];

	if ((strcmp(argv[1], "add") == 0) || (strcmp(argv[1], "--add") == 0))
	{
		nIndex = 2;
		nAdd = 1;
		if (argc < 4)
		{
			fprintf(stderr, "name and value should be specified!\n");
			goto returnPoint;
		}
	}

	if (OpenEnvironmentRegistry(&hKey, KEY_READ | KEY_WRITE)){goto returnPoint;}

	if (nAdd)
	{
		vcData[0] = 0;
		if (GetRegistryContent(hKey, argv[nIndex], &dwType, (BYTE*)vcData, &dwDataSize)<0)
		{
			goto returnPoint;
		}
		strncat(vcData, ";", 2047);
		strncat(vcData, argv[3],2047);
		cpcEnvVar = vcData;
	}
	printf("Value to set: \"%s\"\n", cpcEnvVar);
	nReturn = ModifiEnvironmentalVariable(hKey,argv[nIndex], dwType, cpcEnvVar,(DWORD)strlen(cpcEnvVar),1);

	printf("return=%d\n", nReturn);
#ifdef __ARM
	printf("Open new shell to apply changes!\n");
#endif

returnPoint:
	if (hKey) { RegCloseKey(hKey); }

#ifdef _DEBUG_APP_
	_getch();
#endif

	return nReturn;
}
