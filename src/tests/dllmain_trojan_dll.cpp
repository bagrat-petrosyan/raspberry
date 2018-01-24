
// 2017 Dec 15

#include <Windows.h>
#include <stdio.h>
#include <stdint.h>

static DWORD WINAPI PTHREAD_START_ROUTINE_static(LPVOID a_lpThreadParameter);
__declspec(dllexport)  int64_t g_unOffsetToFunction = 0;
typedef DWORD(WINAPI *TypeRemoteFunction)(LPVOID a_lpThreadParameter);


#ifdef __cplusplus
extern "C" {
#endif


BOOL WINAPI DllMain(_In_ HINSTANCE a_hinstDLL,_In_ DWORD a_fdwReason,_In_ LPVOID a_lpvReserved)
{
	TypeRemoteFunction fpRemoteFunction = &PTHREAD_START_ROUTINE_static;
	printf("hInstance=%d, reserved=%p, reason=%d(",
		(int)((size_t)a_hinstDLL),a_lpvReserved, (int)a_fdwReason);

	//FreeLibrary
	switch (a_fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_unOffsetToFunction = (char*)(*((void**)&fpRemoteFunction)) - (char*)a_hinstDLL;
		printf("DLL_PROCESS_ATTACH, g_unOffsetToFunction=%d", (int)g_unOffsetToFunction);
		break;
	case DLL_THREAD_ATTACH:
		printf("DLL_THREAD_ATTACH");
		break;
	case DLL_PROCESS_DETACH:
		printf("DLL_PROCESS_DETACH");
		break;
	case DLL_THREAD_DETACH:
		printf("DLL_THREAD_DETACH");
		break;
	default:
		printf("default");
		break;
	}

	printf(")\n");

	return TRUE;
}


__declspec(dllexport) DWORD WINAPI PTHREAD_START_ROUTINE_exported(LPVOID a_lpThreadParameter)
{
	printf("%s(%p)\n", __FUNCTION__, a_lpThreadParameter);

	return 0;
}


#ifdef __cplusplus
}
#endif


static DWORD WINAPI PTHREAD_START_ROUTINE_static(LPVOID a_lpThreadParameter)
{
	printf("%s(%p)\n", __FUNCTION__, a_lpThreadParameter);

	return 0;
}

