
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#ifdef __cplusplus
extern "C"
{
#endif


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	printf("fdwReason=");
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		printf("DLL_PROCESS_ATTACH, ");
		break;
	case DLL_PROCESS_DETACH:
		printf("DLL_PROCESS_DETACH, ");
		break;
	case DLL_THREAD_ATTACH:
		printf("DLL_THREAD_ATTACH, ");
		break;
	case DLL_THREAD_DETACH:
		printf("DLL_THREAD_DETACH, ");
		break;
	default:
		printf("default, ");
		break;
	}

	char vcBuffer[1024];
	vcBuffer[0] = 0;
	DWORD dwRet = GetModuleFileNameA(hinstDLL, vcBuffer, 1023);
	printf(("dwRet=%d, FileName=\"%s\"\n"), (int)dwRet, vcBuffer);

	return TRUE;
}

_declspec(dllexport) void FunctionToExport(void)
{
	printf("FunctionToExport\n");
}

#ifdef __cplusplus
}
#endif
