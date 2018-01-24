/*
 *	File: main_restart.cpp
 *
 *	Created on: Sep 15, 2016
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements all functions connected to posix threading
 *		1) 
 *
 *
 */
#include <windows.h>
#include <stdio.h>

static BOOL MySystemShutdown(UINT a_uFlags);

int main()
{
	printf("trying to restart the system!\n");
	MySystemShutdown(EWX_REBOOT);
	return 0;
}


static BOOL MySystemShutdown(UINT a_uFlags)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	// Get a token for this process. 
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return(FALSE);

	// Get the LUID for the shutdown privilege. 
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
		&tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get the shutdown privilege for this process. 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
		(PTOKEN_PRIVILEGES)NULL, 0);

	if (GetLastError() != ERROR_SUCCESS)
		return FALSE;

	// Shut down the system and force all applications to close. 

	//if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 
	/*if (!ExitWindowsEx(EWX_REBOOT ,
	SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
	SHTDN_REASON_MINOR_UPGRADE |
	SHTDN_REASON_FLAG_PLANNED)) */
	if (!ExitWindowsEx(a_uFlags, EWX_FORCE))
		return FALSE;

	return TRUE;
}
