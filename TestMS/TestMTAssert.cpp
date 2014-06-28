#include "stdio.h"
#include "windows.h"
#include "assert.h"
#include "stdio.h"
#include <crtdbg.h>

#define CHK_STS		if(sts<0) {DispLastError();printf("Error line %d\n", __LINE__); goto Exit;}
#define VERIFY(x)	if(!(x)) {sts=-1;CHK_STS;}

DWORD (WINAPI Thread)(
    LPVOID lpThreadParameter
    )
{
	_ASSERT(0);

	return 0;
}

int main_mt(int argc, char* argv[])
{
	for(int i=0; i<10; i++)
   {
	   HANDLE h=CreateThread(0,0,Thread,0,0,0);
   }

	system("pause");
	return 0;
}
