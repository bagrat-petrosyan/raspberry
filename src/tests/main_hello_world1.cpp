
#include <stdio.h>
#include <iostream>
#include <conio.h>

int main()
{
	char vcBuffer[1024];

	_getch();
	std::cin >> vcBuffer;
	printf("Hello world !\nbuf=%s\n",vcBuffer);
	return 0;
}
