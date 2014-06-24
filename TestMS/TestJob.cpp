// TestJob.cpp : Defines the entry point for the console application.
//
#include "stdio.h"
#include "windows.h"
#include "assert.h"

struct MY_PROCESS
{
	HANDLE hProcess;
	HANDLE hThread;
};
int MySystem( LPSTR path, MY_PROCESS *pProc )
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    if( !CreateProcess( NULL,   // No module name (use command line)
        path,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        printf( "CreateProcess %s failed (%d).\n", path, GetLastError() );
        return -1;
    }

	pProc->hProcess =pi.hProcess;
	pProc->hThread =pi.hThread;

   
	return 0;
}

int MyWait(MY_PROCESS const *pProc)
{
	 // Wait until child process exits.
    WaitForSingleObject(pProc->hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pProc->hProcess );
    CloseHandle( pProc->hThread );

	return 0;
}




void DispLastError() 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    char lpDisplayBuf[1000]; 
    printf(TEXT("\tfailed with error %d: %s\n"), 
        dw, lpMsgBuf); 
    
    LocalFree(lpMsgBuf);
    
}

#define CHK_STS		if(sts<0) {DispLastError();printf("Error line %d\n", __LINE__); goto Exit;}
#define VERIFY(x)	if(!(x)) {sts=-1;CHK_STS;}

int main(int argc, char* argv[])
{
	int sts =0;
	HANDLE hJob= CreateJobObject(0,"MyJob");
	VERIFY(hJob);

	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobInfo;
	memset(&jobInfo,0,sizeof(jobInfo));
	jobInfo.BasicLimitInformation.LimitFlags =JOB_OBJECT_LIMIT_PROCESS_MEMORY;
	jobInfo.PeakProcessMemoryUsed =30e6;

	//VERIFY( SetInformationJobObject(hJob,JobObjectExtendedLimitInformation,&jobInfo,sizeof(jobInfo)));

	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobInfoQ;
	DWORD lReturnLengthQ;
	 VERIFY( QueryInformationJobObject (hJob, JobObjectExtendedLimitInformation,
     &jobInfoQ, sizeof(jobInfoQ),&lReturnLengthQ));

	MY_PROCESS proc;
	char *p="notepad";

	if(argc>1)
		p=argv[1];

	sts =MySystem(p,&proc);
	CHK_STS;

	VERIFY( AssignProcessToJobObject(hJob,proc.hProcess));
	
	system("pause");
	VERIFY(TerminateJobObject(hJob,0));
	system("pause");

	MyWait(&proc);

Exit:

	return sts;
}

