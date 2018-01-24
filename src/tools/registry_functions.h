/*
 *	File: registry_functions.h
 *
 *	Created on: Sep 14, 2016
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements all functions connected to posix threading
 *		1) 
 *
 *
 */
#ifndef __registry_functions_h__
#define __registry_functions_h__

#include <windows.h>


#ifdef __cplusplus
extern "C"
{
#endif

int OpenEnvironmentRegistry(HKEY* phKey, REGSAM mode);
#define	EnvironmentRegistryKeyClose(__hKey) RegCloseKey((__hKey))
int GetRegistryContent(HKEY hKey, const char* a_cpcName, DWORD* pType,BYTE* a_pbData, DWORD* a_pdwDataSize);
int ModifyRegistry(HKEY hKey, const char* name, DWORD type, const BYTE* data, DWORD data_size);
int ModifiEnvironmentalVariable(HKEY hKey, const char* name, DWORD type, const char* data, DWORD data_size, BOOL bInform);
int TestRegistry(HKEY hKey);

#ifdef __cplusplus
}
#endif


#endif  // #ifndef __registry_functions_h__
