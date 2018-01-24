
// 2017 Dec 14

#include <Windows.h>
#include <signal.h>
#include <stdio.h>
#include <process.h>

static void signal_handler(int a_sigNum) 
{
	printf("sigNum=%d\n",a_sigNum);
}

int main()
{

	_crt_signal_t retVal;

	printf("!!!!!!! NSIG=%d, pid=%d\n", NSIG,(int)_getpid());

	for(int i(0);i<NSIG;++i){
		retVal = signal(i, signal_handler);
		if(retVal==SIG_ERR){
			printf("sigNum=%d, error to register!\n",i);
		}
	}
	

	while(1){
		Sleep(10);
	}

	return 0;
}
