/**********************************************************************
 * 
 *  Toby Opferman
 *
 *  Example Communicating with a driver
 *
 *  This example is for educational purposes only.  I license this source
 *  out for use in learning how to write a device driver.
 *
 *    
 **********************************************************************/


#include <windows.h>
#include <stdio.h>

/*********************************************************
 *   Main Function Entry
 *
 *********************************************************/
int _cdecl main(void)
{
    HANDLE hFile;
    DWORD dwReturn;
	const char *file ="\\\\.\\Example";
	BOOL bRet;
    hFile = CreateFile(file, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	printf("Create File %s return %x\n", file, hFile);
    //if(hFile)
    //{
        bRet =WriteFile(hFile, "Hello from user mode!", sizeof("Hello from user mode!"), &dwReturn, NULL); 
		printf("WriteFile return %d (%s)\n", bRet, bRet?"OK":"Fail");
		
        CloseHandle(hFile);
		printf("CloseHandle\n");
    //}
    
    return 0;
}


