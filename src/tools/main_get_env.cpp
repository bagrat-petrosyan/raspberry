/*
 *	File: main_get_env.cpp
 *
 *	Created on: Sep 14, 2016
 *	Author    : Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements all functions connected to posix threading
 *		1) 
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>

#ifdef _MSC_VER
#if(_MSC_VER >= 1400)
 //#define		_CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)
#endif
#endif


int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		fprintf(stderr,"The name of environmental variable is not provided!\n");
		return 1;
	}
	char* pEnv = getenv(argv[1]);

	if (pEnv == NULL)
	{
		printf("Environmental variable with name %s does not exist!\n", argv[1]);
		return 0;
	}

	printf("%s=%s\n", argv[1], pEnv);

	return 0;
}
