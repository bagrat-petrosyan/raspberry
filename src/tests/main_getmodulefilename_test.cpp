
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <process.h>

#ifdef __cplusplus
extern "C"
{
#endif
extern void FunctionToExport(void);
#ifdef __cplusplus
}
#endif

int main()
{
	HINSTANCE hLib = (HINSTANCE)0;
	char vcBuffer[1024];
	vcBuffer[0] = 0;
	DWORD dwRet = GetModuleFileNameA(NULL, vcBuffer, 1023);

	printf(("pid=%d, dwRet=%d, FileName=\"%s\"\n"), (int)_getpid(),(int)dwRet, vcBuffer);

	hLib = LoadLibrary(_T("libgetmodulefilename_test.dll"));
	dwRet = GetModuleFileNameA(hLib, vcBuffer, 1023);
	printf(("!!!!!!!!!!!!!! dwRet=%d, FileName=\"%s\"\n"), (int)dwRet, vcBuffer);

	//FunctionToExport();

	goto returnPoint;
returnPoint:
	if (hLib) { FreeLibrary(hLib); }
	printf("++++++++ hLib = %d\n", (int)hLib);
//#ifndef __ARM
#if 1
	printf("Press any key to exit!");
	fflush(stdout);
	_getch();
#endif
	return 0;
}
