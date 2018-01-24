
// 2017 Dec 15
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <conio.h>

#ifndef AllocMemOnRemoteProcess
#define		AllocMemOnRemoteProcess(__hProcess, __size) \
					VirtualAllocEx((__hProcess),0,(__size),MEM_COMMIT,PAGE_READWRITE)
#endif // #ifndef AllocMemOnRemoteProcess
#ifndef DeleteMemOnRemoteProcess
#define		DeleteMemOnRemoteProcess(__hProcess,__mem) \
					VirtualFreeEx((__hProcess),(LPVOID)((size_t)(__mem)), 0, MEM_RELEASE)
#endif  // #ifndef DeleteMemOnRemoteProcess

static DWORD WINAPI PTHREAD_START_ROUTINE_local(LPVOID a_lpThreadParameter);
typedef DWORD (WINAPI *TypeRemoteFunction)(LPVOID a_lpThreadParameter);
extern __declspec(dllimport)  int64_t g_unOffsetToFunction;

int main(int argc, char* argv[])
{

#if 1
	printf("Hello world");
	return 0;
#else
	const char* cpcLibraryName;
	char vcLibraryName[MAX_PATH];
	TypeRemoteFunction fpRemoteFunction = &PTHREAD_START_ROUTINE_local;
	HINSTANCE thisInstance ;
	int64_t unFncRelativeAddr;
	void* pRemoteMem = NULL;
	HANDLE hProcess = (HANDLE)0;
	HANDLE hThread = (HANDLE)0;
	size_t unDllNameLenPlus1;
	SIZE_T szWritten;
	DWORD dwThreadId, dwHinstanse2, dwReturn;
	int nPid, nIndexPid;

	if(argc<2){
		fprintf(stderr, "Provide dll to load and victim process pid!\n");
		goto returnPoint;
	}
	else
	{
		if(argc==2){
			GetModuleFileNameA(NULL, vcLibraryName, MAX_PATH - 1);
			cpcLibraryName = vcLibraryName;
			nIndexPid = 1;
		}
		else{
			cpcLibraryName = argv[ 1];
			nIndexPid = 2;
		}
		
	}

#ifdef _DEBUG
	printf("Press any key t exit!"); fflush(stdout);
	_getch();
	printf("\n");
#endif

	nPid = atoi(argv[nIndexPid]);
	unDllNameLenPlus1 = strlen(cpcLibraryName)+1;
	printf("pid=%d, dllNameLen=%d, ldd=%s\n",nPid,(int)unDllNameLenPlus1, cpcLibraryName);

	hProcess = OpenProcess(`);
	printf("hProcess=%d\n",(int)((size_t)hProcess));
	if(!hProcess){
		goto returnPoint;
	}

	pRemoteMem = AllocMemOnRemoteProcess(hProcess, unDllNameLenPlus1);
	printf("pRemoteMem=%d\n", (int)((size_t)pRemoteMem));
	if (!pRemoteMem) {
		goto returnPoint;
	}

	WriteProcessMemory(hProcess, pRemoteMem, cpcLibraryName,unDllNameLenPlus1, &szWritten);

	hThread = CreateRemoteThread(hProcess, NULL, 0,
		(LPTHREAD_START_ROUTINE)LoadLibraryA,
		pRemoteMem, 0, &dwThreadId);

	printf("hThread=%d\n", (int)((size_t)hThread));
	if (!hThread) {
		goto returnPoint;
	}

	WaitForSingleObject(hThread, INFINITE);
	GetExitCodeThread(hThread, &dwHinstanse2);
	CloseHandle(hThread);
	hThread = (HANDLE)0;

	printf("dwHinstanse=%d\n", (int)((size_t)dwHinstanse2));

	if(!dwHinstanse2){goto returnPoint;}

	if(argc==2){
		thisInstance = LoadLibraryA(cpcLibraryName);
		printf("thisInstance=%d\n", (int)((size_t)thisInstance));
		if(!thisInstance){goto returnPoint;}
		unFncRelativeAddr = (char*)(*((void**)&fpRemoteFunction)) - (char*)thisInstance;
		//pFncRelativeAddr = (char*)(*((void**)&thisInstance));
		//pFncRelativeAddr = (char*)(thisInstance);

		hThread = CreateRemoteThread(hProcess, NULL, 0,
			(LPTHREAD_START_ROUTINE)(unFncRelativeAddr+((int64_t)dwHinstanse2)),
			(void*)((size_t)0x1000), 0, &dwThreadId);

		FreeLibrary(thisInstance);

		printf("hThread=%d\n", (int)((size_t)hThread));
		if (!hThread) {
			goto returnPoint;
		}

		WaitForSingleObject(hThread, INFINITE);
		GetExitCodeThread(hThread, &dwReturn);
		CloseHandle(hThread);
		hThread = (HANDLE)0;
	}
	else{
		if(strncmp("trojan_dll",argv[1],10)==0){
			hThread = CreateRemoteThread(hProcess, NULL, 0,
				(LPTHREAD_START_ROUTINE)(g_unOffsetToFunction +((int64_t)dwHinstanse2)),
				(void*)((size_t)0x2000), 0, &dwThreadId);

			printf("hThread=%d\n", (int)((size_t)hThread));
			if (!hThread) {
				goto returnPoint;
			}

			WaitForSingleObject(hThread, INFINITE);
			GetExitCodeThread(hThread, &dwReturn);
			CloseHandle(hThread);
			hThread = (HANDLE)0;
		}
	}

	hThread = CreateRemoteThread(hProcess, NULL, 0,
		(LPTHREAD_START_ROUTINE)FreeLibrary,
		(void*)((size_t)dwHinstanse2), 0, &dwThreadId);

	printf("hThread=%d\n", (int)((size_t)hThread));
	if (!hThread) {
		goto returnPoint;
	}

	WaitForSingleObject(hThread, INFINITE);
	GetExitCodeThread(hThread, &dwReturn);
	CloseHandle(hThread);
	hThread = (HANDLE)0;

	printf("FreeLibrary_return=%d\n", (int)((size_t)dwReturn));

returnPoint:
	if(pRemoteMem){
		DeleteMemOnRemoteProcess(hProcess, pRemoteMem);
	}
	if(hProcess){
		CloseHandle(hProcess);
	}
	return 0;
#endif
}


static DWORD WINAPI PTHREAD_START_ROUTINE_local(LPVOID a_lpThreadParameter)
{
	printf("%s(%p)\n",__FUNCTION__, a_lpThreadParameter);

	return 0;
}

#if 1
BOOL WINAPI DllMain(_In_ HINSTANCE a_hinstDLL, _In_ DWORD a_fdwReason, _In_ LPVOID a_lpvReserved)
{
	printf("%s\n", __FUNCTION__);
	return TRUE;
}
#endif
