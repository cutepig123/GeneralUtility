#include <windows.h>
#include <stdio.h>
//使用CTL_CODE必须加入winioctl.h
#include <winioctl.h>
#include "..\NT_Driver\Ioctls.h"

int __cdecl main()
{
	HANDLE hDevice = 
		CreateFile("\\\\.\\HelloDDK",
					GENERIC_READ | GENERIC_WRITE,
					0,		// share mode none
					NULL,	// no security
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL );		// no template

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("Failed to obtain file handle to device: "
			"%s with Win32 error code: %d\n",
			"MyWDMDevice", GetLastError() );
		return 1;
	}

	
	UCHAR InputBuffer[10];
	UCHAR OutputBuffer[10];
	//将输入缓冲区全部置成0XBB
	memset(InputBuffer,0xBB,10);
	DWORD dwOutput;
	//输入缓冲区作为输入，输出缓冲区作为输出

	BOOL bRet;
	bRet = DeviceIoControl(hDevice, IOCTL_TEST1, InputBuffer, 10, &OutputBuffer, 10, &dwOutput, NULL);
	if (bRet)
	{
		printf("Output buffer:%d bytes\n",dwOutput);
		for (int i=0;i<(int)dwOutput;i++)
		{
			printf("%02X ",OutputBuffer[i]);
		}
		printf("\n");
	}

	bRet = DeviceIoControl(hDevice, IOCTL_TEST2, InputBuffer, 10, &OutputBuffer, 10, &dwOutput, NULL);
	if (bRet)
	{
		printf("Output buffer:%d bytes\n",dwOutput);
		for (int i=0;i<(int)dwOutput;i++)
		{
			printf("%02X ",OutputBuffer[i]);
		}
		printf("\n");
	}

 	bRet = DeviceIoControl(hDevice, IOCTL_TEST3, InputBuffer, 10, &OutputBuffer, 10, &dwOutput, NULL);
	if (bRet)
	{
		printf("Output buffer:%d bytes\n",dwOutput);
		for (int i=0;i<(int)dwOutput;i++)
		{
			printf("%02X ",OutputBuffer[i]);
		}
		printf("\n");
	}

	{
		struct CreateProcessPar stPar={0};
		void *pWinExec =GetProcAddress(LoadLibrary("kernel32"), "WinExec");
		sprintf(stPar.acExePath,"c:\\WINDOWS\\system32\\notepad.exe");
		stPar.pWinExec =pWinExec;
		printf("pWinExec %x\n", pWinExec);
		bRet = DeviceIoControl(hDevice, IOCTL_CREATEPROCESS, &stPar, sizeof(stPar), &OutputBuffer, 10, &dwOutput, NULL);
		printf("DeviceIoControl IOCTL_CREATEPROCESS return %d, %s\n", bRet, bRet?"OK":"FAIL");
	}
	
	CloseHandle(hDevice);

	return 0;
}