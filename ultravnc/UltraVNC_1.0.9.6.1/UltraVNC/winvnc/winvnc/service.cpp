//  Copyright (C) 2002 RealVNC Ltd. All Rights Reserved.
//  Copyright (C) 1999 AT&T Laboratories Cambridge. All Rights Reserved.
//
//  This file is part of the VNC system.
//
//  The VNC system is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
//  USA.
//
// If the source code for the VNC system is not available from the place 
// whence you received this file, check http://www.uk.research.att.com/vnc or contact
// the authors on vnc@uk.research.att.com for information on obtaining it.


// vncService

// Implementation of service-oriented functionality of WinVNC
#include <windows.h>
#include <userenv.h>
#include <wtsapi32.h>
#include <stdio.h>
#include <tlhelp32.h>
#include "inifile.h"
#include "common/win32_helpers.h"

HANDLE hEvent=NULL;
HANDLE hEventcad=NULL;
extern HANDLE stopServiceEvent;
static char app_path[MAX_PATH];
typedef DWORD (*WTSGETACTIVECONSOLESESSIONID)();
typedef bool (*WTSQUERYUSERTOKEN)(ULONG,PHANDLE);
helper::DynamicFn<WTSGETACTIVECONSOLESESSIONID> lpfnWTSGetActiveConsoleSessionId("kernel32", "WTSGetActiveConsoleSessionId");
helper::DynamicFn<WTSQUERYUSERTOKEN> lpfnWTSQueryUserToken("Wtsapi32.dll", "WTSQueryUserToken");
PROCESS_INFORMATION  ProcessInfo;
int counter=0;
extern char cmdtext[256];
int kickrdp=0;
int clear_console=0;
bool W2K=0;
//////////////////////////////////////////////////////////////////////////////
#define MAXSTRLENGTH    255
BOOL Char2Wchar(WCHAR* pDest, char* pSrc, int nDestStrLen)
{
     int nSrcStrLen = 0;
     int nOutputBuffLen = 0;
     int retcode = 0;

     if(pDest == NULL || pSrc == NULL)
     {
          return FALSE;
     }

     nSrcStrLen = strlen(pSrc);
     if(nSrcStrLen == 0)
     {  
          return FALSE;
     }

     nDestStrLen = nSrcStrLen;

     if (nDestStrLen > MAXSTRLENGTH - 1)
     {
          return FALSE;
     }
     memset(pDest,0,sizeof(TCHAR)*nDestStrLen);
     nOutputBuffLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pSrc,nSrcStrLen, pDest, nDestStrLen);
 
     if (nOutputBuffLen == 0)
     {
          retcode = GetLastError();
          return FALSE;
     }

     pDest[nOutputBuffLen] = '\0';
     return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
typedef BOOLEAN (WINAPI* pWinStationQueryInformationW)(
  IN   HANDLE hServer,
  IN   ULONG LogonId,
  IN   DWORD /*WINSTATIONINFOCLASS*/ WinStationInformationClass,
  OUT  PVOID pWinStationInformation,
  IN   ULONG WinStationInformationLength,
  OUT  PULONG pReturnLength
);
DWORD MarshallString(LPCWSTR    pszText, LPVOID, DWORD  dwMaxSize,LPBYTE*
ppNextBuf, DWORD* pdwUsedBytes)
{
        DWORD   dwOffset = *pdwUsedBytes;
        if(!pszText)
                return 0;
        DWORD   dwLen = (wcslen(pszText)+1)*sizeof(WCHAR);
        if(*pdwUsedBytes + dwLen> dwMaxSize)
                return 0;
        memmove(*ppNextBuf, pszText , dwLen);
        *pdwUsedBytes += dwLen;
        *ppNextBuf += dwLen;
        return dwOffset;

}

typedef struct _CPAU_PARAM{
        DWORD   cbSize;
        DWORD   dwProcessId;
        BOOL    bUseDefaultToken;
        HANDLE  hToken;
        LPWSTR  lpApplicationName;
        LPWSTR  lpCommandLine;
        SECURITY_ATTRIBUTES     ProcessAttributes;
        SECURITY_ATTRIBUTES ThreadAttributes;
        BOOL bInheritHandles;
        DWORD dwCreationFlags;
        LPVOID lpEnvironment;
        LPWSTR lpCurrentDirectory;
        STARTUPINFOW StartupInfo;
        PROCESS_INFORMATION     ProcessInformation;

}CPAU_PARAM;

typedef struct _CPAU_RET_PARAM{
        DWORD   cbSize;
        BOOL    bRetValue;
        DWORD   dwLastErr;
        PROCESS_INFORMATION     ProcInfo;

}CPAU_RET_PARAM;

BOOL CreateRemoteSessionProcess(
        IN DWORD        dwSessionId,
        IN BOOL         bUseDefaultToken,
        IN HANDLE       hToken,
        IN LPCWSTR      lpApplicationName,
        IN LPSTR       A_lpCommandLine,
        IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
        IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
        IN BOOL bInheritHandles,
        IN DWORD dwCreationFlags,
        IN LPVOID lpEnvironment,
        IN LPCWSTR lpCurrentDirectory,
        IN LPSTARTUPINFO A_lpStartupInfo,
        OUT LPPROCESS_INFORMATION lpProcessInformation)
{

		WCHAR       lpCommandLine[255];
		STARTUPINFOW StartupInfo;
		Char2Wchar(lpCommandLine, A_lpCommandLine, 255);
		ZeroMemory(&StartupInfo,sizeof(STARTUPINFOW));
		StartupInfo.wShowWindow = SW_SHOW;
		StartupInfo.lpDesktop = L"Winsta0\\Winlogon";
		StartupInfo.cb = sizeof(STARTUPINFOW);

        WCHAR           szWinStaPath[MAX_PATH];
        BOOL            bGetNPName=FALSE;
        WCHAR           szNamedPipeName[MAX_PATH]=L"";
        DWORD           dwNameLen;
        HINSTANCE       hInstWinSta;
        HANDLE          hNamedPipe;
        LPVOID          pData=NULL;
        BOOL            bRet = FALSE;
        DWORD           cbReadBytes,cbWriteBytes;
        DWORD           dwEnvLen = 0;
        union{
                CPAU_PARAM      cpauData;
                BYTE            bDump[0x2000];
        };
        CPAU_RET_PARAM  cpauRetData;
        DWORD                   dwUsedBytes = sizeof(cpauData);
        LPBYTE                  pBuffer = (LPBYTE)(&cpauData+1);
        GetSystemDirectoryW(szWinStaPath, MAX_PATH);
        lstrcatW(szWinStaPath,L"\\winsta.dll");
        hInstWinSta = LoadLibraryW(szWinStaPath);

        if(hInstWinSta)
        {
                pWinStationQueryInformationW pfWinStationQueryInformationW=(pWinStationQueryInformationW)GetProcAddress(hInstWinSta,"WinStationQueryInformationW");
                if(pfWinStationQueryInformationW)
                {
                        bGetNPName = pfWinStationQueryInformationW(0, dwSessionId, 0x21,szNamedPipeName, sizeof(szNamedPipeName), &dwNameLen);
                }
                FreeLibrary(hInstWinSta);
        }
        if(!bGetNPName || szNamedPipeName[0] == '\0')
        {
                swprintf(szNamedPipeName,260,L"\\\\.\\Pipe\\TerminalServer\\SystemExecSrvr\\%d", dwSessionId);
        }

        do{
                hNamedPipe = CreateFileW(szNamedPipeName, GENERIC_READ|GENERIC_WRITE,0, NULL, OPEN_EXISTING, 0, 0);
                if(hNamedPipe == INVALID_HANDLE_VALUE)
                {
                        if(GetLastError() == ERROR_PIPE_BUSY)
                        {
                                if(!WaitNamedPipeW(szNamedPipeName, 30000))
                                        return FALSE;
                        }
                        else
                        {
                                return FALSE;
                        }
                }
        }while(hNamedPipe == INVALID_HANDLE_VALUE);

        memset(&cpauData, 0, sizeof(cpauData));
        cpauData.bInheritHandles        = bInheritHandles;
        cpauData.bUseDefaultToken       = bUseDefaultToken;
        cpauData.dwCreationFlags        = dwCreationFlags;
        cpauData.dwProcessId            = GetCurrentProcessId();
        cpauData.hToken                         = hToken;
        cpauData.lpApplicationName      =(LPWSTR)MarshallString(lpApplicationName, &cpauData, sizeof(bDump),&pBuffer, &dwUsedBytes);
        cpauData.lpCommandLine          = (LPWSTR)MarshallString(lpCommandLine,&cpauData, sizeof(bDump), &pBuffer, &dwUsedBytes);
        cpauData.StartupInfo            = StartupInfo;
        cpauData.StartupInfo.lpDesktop  =(LPWSTR)MarshallString(cpauData.StartupInfo.lpDesktop, &cpauData,sizeof(bDump), &pBuffer, &dwUsedBytes);
        cpauData.StartupInfo.lpTitle    =(LPWSTR)MarshallString(cpauData.StartupInfo.lpTitle, &cpauData,sizeof(bDump), &pBuffer, &dwUsedBytes);

        if(lpEnvironment)
        {
                if(dwCreationFlags & CREATE_UNICODE_ENVIRONMENT)
                {
                        while((dwEnvLen+dwUsedBytes <= sizeof(bDump)))
                        {
                                if(((LPWSTR)lpEnvironment)[dwEnvLen/2]=='\0' &&((LPWSTR)lpEnvironment)[dwEnvLen/2+1] == '\0')
                                {
                                        dwEnvLen+=2*sizeof(WCHAR);
                                        break;
                                }
                                dwEnvLen+=sizeof(WCHAR);
                        }
                }
                else
                {
                        while(dwEnvLen+dwUsedBytes <= sizeof(bDump))
                        {
                                if(((LPSTR)lpEnvironment)[dwEnvLen]=='\0' && ((LPSTR)lpEnvironment)[dwEnvLen+1]=='\0')
                                {
                                        dwEnvLen+=2;
                                        break;
                                }
                                dwEnvLen++;
                        }
                }
                if(dwEnvLen+dwUsedBytes <= sizeof(bDump))
                {
                        memmove(pBuffer, lpEnvironment, dwEnvLen);
                        cpauData.lpEnvironment = (LPVOID)dwUsedBytes;
                        pBuffer += dwEnvLen;
                        dwUsedBytes += dwEnvLen;
                }
                else
                {
                        cpauData.lpEnvironment = NULL;
                }
        }
        else
        {
                cpauData.lpEnvironment  = NULL;
        }
        cpauData.cbSize  = dwUsedBytes;

		HANDLE hProcess=NULL;
        if(WriteFile(hNamedPipe, &cpauData, cpauData.cbSize, &cbWriteBytes,NULL))
		{
			Sleep(250);
			if (ReadFile(hNamedPipe, & cpauRetData, sizeof(cpauRetData),&cbReadBytes, NULL))
			{
				hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, cpauRetData.ProcInfo.dwProcessId);
	#ifdef _DEBUG
						char			szText[256];
						sprintf(szText," ++++++cpau  %i  %i %i %i %i %i\n",cpauRetData.bRetValue,cpauRetData.ProcInfo.hProcess,cpauRetData.ProcInfo.dwProcessId,cpauRetData.ProcInfo.dwThreadId,cpauRetData.ProcInfo.hThread,hProcess);
						OutputDebugString(szText);						
	#endif
					bRet = cpauRetData.bRetValue;
					if(bRet)
					{
							*lpProcessInformation = cpauRetData.ProcInfo;
					}
					else
							SetLastError(cpauRetData.dwLastErr);
			}
		}
        else
                bRet = FALSE;
		// function sometimes fail, the use processid to get hprocess... bug MS
		if (lpProcessInformation->hProcess==0) lpProcessInformation->hProcess=hProcess;
		//this should never happen, looping connections
		if (lpProcessInformation->hProcess==0) 
			Sleep(5000);
        CloseHandle(hNamedPipe);
        return bRet;

}
//////////////////////////////////////////////////////////////////////////////

static int pad2()
{

	OSVERSIONINFO OSversion;
	
	OSversion.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);

	GetVersionEx(&OSversion);
	W2K=0;
	switch(OSversion.dwPlatformId)
	{
		case VER_PLATFORM_WIN32_NT:
						  if(OSversion.dwMajorVersion==5 && OSversion.dwMinorVersion==0)
									 W2K=1;							    
								  
	}
	char exe_file_name[MAX_PATH];
	char cmdline[MAX_PATH];
    GetModuleFileName(0, exe_file_name, MAX_PATH);

    strcpy(app_path, exe_file_name);
	strcat(app_path, " ");
	strcat(app_path,cmdtext);
    strcat(app_path, "_run");
	IniFile myIniFile;
	kickrdp=myIniFile.ReadInt("admin", "kickrdp", kickrdp);
	clear_console=myIniFile.ReadInt("admin", "clearconsole", clear_console);
	myIniFile.ReadString("admin", "service_commandline",cmdline,256);
	if (strlen(cmdline)!=0)
	{
		strcpy(app_path, exe_file_name);
		strcat(app_path, " ");
		strcat(app_path,cmdline);
		strcat(app_path, " -service_run");
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////

BOOL SetTBCPrivileges(VOID) {
  DWORD dwPID;
  HANDLE hProcess;
  HANDLE hToken;
  LUID Luid;
  TOKEN_PRIVILEGES tpDebug;
  dwPID = GetCurrentProcessId();
  if ((hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)) == NULL) return FALSE;
  if (OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken) == 0) return FALSE;
  if ((LookupPrivilegeValue(NULL, SE_TCB_NAME, &Luid)) == 0) return FALSE;
  tpDebug.PrivilegeCount = 1;
  tpDebug.Privileges[0].Luid = Luid;
  tpDebug.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  if ((AdjustTokenPrivileges(hToken, FALSE, &tpDebug, sizeof(tpDebug), NULL, NULL)) == 0) return FALSE;
  if (GetLastError() != ERROR_SUCCESS) return FALSE;
  CloseHandle(hToken);
  CloseHandle(hProcess);
  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
#include <tlhelp32.h>

DWORD GetwinlogonPid()
{
	//DWORD dwSessionId;
	DWORD dwExplorerLogonPid=0;
	PROCESSENTRY32 procEntry;

	//dwSessionId=0;

	

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE)
    {
        return 0 ;
    }

    procEntry.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnap, &procEntry))
    {
		CloseHandle(hSnap);
        return 0 ;
    }

    do
    {
        if (_stricmp(procEntry.szExeFile, "winlogon.exe") == 0)
        {
			dwExplorerLogonPid = procEntry.th32ProcessID;
        }

    } while (Process32Next(hSnap, &procEntry));
	CloseHandle(hSnap);
	return dwExplorerLogonPid;
}
//////////////////////////////////////////////////////////////////////////////
DWORD
Find_winlogon(DWORD SessionId)
{

  PWTS_PROCESS_INFO pProcessInfo = NULL;
  DWORD         ProcessCount = 0;
//  char         szUserName[255];
  DWORD         Id = -1;

  typedef BOOL (WINAPI *pfnWTSEnumerateProcesses)(HANDLE,DWORD,DWORD,PWTS_PROCESS_INFO*,DWORD*);
  typedef VOID (WINAPI *pfnWTSFreeMemory)(PVOID);

  helper::DynamicFn<pfnWTSEnumerateProcesses> pWTSEnumerateProcesses("wtsapi32","WTSEnumerateProcessesA");
  helper::DynamicFn<pfnWTSFreeMemory> pWTSFreeMemory("wtsapi32", "WTSFreeMemory");

  if (pWTSEnumerateProcesses.isValid() && pWTSFreeMemory.isValid())
  {
    if ((*pWTSEnumerateProcesses)(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pProcessInfo, &ProcessCount))
  {
    // dump each process description
    for (DWORD CurrentProcess = 0; CurrentProcess < ProcessCount; CurrentProcess++)
    {

        if( _stricmp(pProcessInfo[CurrentProcess].pProcessName, "winlogon.exe") == 0 )
        {    
			if (SessionId==pProcessInfo[CurrentProcess].SessionId)
			{
            Id = pProcessInfo[CurrentProcess].ProcessId;
            break;
			}
        }
    }

    (*pWTSFreeMemory)(pProcessInfo);
    }
  }

  return Id;
}

//////////////////////////////////////////////////////////////////////////////
BOOL
get_winlogon_handle(OUT LPHANDLE  lphUserToken)
{
	BOOL   bResult = FALSE;
	HANDLE hProcess;
	HANDLE hAccessToken = NULL;
	HANDLE hTokenThis = NULL;
	DWORD ID_session=0;
	if (lpfnWTSGetActiveConsoleSessionId.isValid()) ID_session=(*lpfnWTSGetActiveConsoleSessionId)();
	DWORD Id=0;
	if (W2K==0) Id=Find_winlogon(ID_session);
	else Id=GetwinlogonPid();

    // fall back to old method if Terminal services is disabled
    if (W2K == 0 && Id == -1)
        Id=GetwinlogonPid();

	#ifdef _DEBUG
					char			szText[256];
					DWORD error=GetLastError();
					sprintf(szText," ++++++ Find_winlogon %i %i %d\n",ID_session,Id,error);
					SetLastError(0);
					OutputDebugString(szText);		
	#endif


	hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, Id );
	if (hProcess)
	{
		#ifdef _DEBUG
					char			szText[256];
					DWORD error=GetLastError();
					sprintf(szText," ++++++ OpenProcess %i \n",hProcess);
					SetLastError(0);
					OutputDebugString(szText);		
		#endif

		OpenProcessToken(hProcess, TOKEN_ASSIGN_PRIMARY|TOKEN_ALL_ACCESS, &hTokenThis);

		#ifdef _DEBUG
					error=GetLastError();
					sprintf(szText," ++++++ OpenProcessToken %i %i\n",hTokenThis,error);
					SetLastError(0);
					OutputDebugString(szText);		
		#endif
		{
		bResult = DuplicateTokenEx(hTokenThis, TOKEN_ASSIGN_PRIMARY|TOKEN_ALL_ACCESS,NULL, SecurityImpersonation, TokenPrimary, lphUserToken);
		#ifdef _DEBUG
					error=GetLastError();
					sprintf(szText," ++++++ DuplicateTokenEx %i %i %i %i\n",hTokenThis,&lphUserToken,error,bResult);
					SetLastError(0);
					OutputDebugString(szText);		
		#endif
		SetTokenInformation(*lphUserToken, TokenSessionId, &ID_session, sizeof(DWORD));
		#ifdef _DEBUG
					error=GetLastError();
					sprintf(szText," ++++++ SetTokenInformation( %i %i %i\n",hTokenThis,&lphUserToken,error);
					SetLastError(0);
					OutputDebugString(szText);		
		#endif
		CloseHandle(hTokenThis);
		}
		CloseHandle(hProcess);
	}
	return bResult;
}
//////////////////////////////////////////////////////////////////////////////

BOOL
GetSessionUserTokenWin(OUT LPHANDLE  lphUserToken)
{
  BOOL   bResult = FALSE;
  HANDLE hImpersonationToken = INVALID_HANDLE_VALUE;
  DWORD ID=0;
  if (lpfnWTSGetActiveConsoleSessionId.isValid()) ID=(*lpfnWTSGetActiveConsoleSessionId)();
  HANDLE hTokenThis = NULL;
  
  if (lphUserToken != NULL) {   
		  bResult = get_winlogon_handle(lphUserToken);
  }
  return bResult;
}
//////////////////////////////////////////////////////////////////////////////
BOOL
GetSessionUserTokenDefault(OUT LPHANDLE  lphUserToken)
{
  BOOL   bResult = FALSE;
  HANDLE hImpersonationToken = INVALID_HANDLE_VALUE;
  DWORD ID=0;
  if (lpfnWTSGetActiveConsoleSessionId.isValid()) ID=(*lpfnWTSGetActiveConsoleSessionId)();

  HANDLE hTokenThis = NULL;
  
  if (lphUserToken != NULL) {   
      if ((*lpfnWTSQueryUserToken)(ID, &hImpersonationToken)) 
      {
        bResult = DuplicateTokenEx(hImpersonationToken,
                                   0,
                                   NULL,
                                   SecurityImpersonation,
                                   TokenPrimary,
                                   lphUserToken);
        CloseHandle(hImpersonationToken);
      }     
  }
  return bResult;

}
//////////////////////////////////////////////////////////////////////////////
// START the app as system 
BOOL
LaunchProcessWin(DWORD dwSessionId)
{
  BOOL                 bReturn = FALSE;
  HANDLE               hToken;
  STARTUPINFO          StartUPInfo;
  PVOID                lpEnvironment = NULL;

  ZeroMemory(&StartUPInfo,sizeof(STARTUPINFO));
  ZeroMemory(&ProcessInfo,sizeof(PROCESS_INFORMATION));
  StartUPInfo.wShowWindow = SW_SHOW;
  StartUPInfo.lpDesktop = "Winsta0\\Winlogon";
  //StartUPInfo.lpDesktop = "Winsta0\\Default";
  StartUPInfo.cb = sizeof(STARTUPINFO);
  SetTBCPrivileges();
  pad2();

  if ( GetSessionUserTokenWin(&hToken) )
  {
      if ( CreateEnvironmentBlock(&lpEnvironment, hToken, FALSE) ) 
      {
      
		 SetLastError(0);
         if (CreateProcessAsUser(hToken,NULL,app_path,NULL,NULL,FALSE,CREATE_UNICODE_ENVIRONMENT |DETACHED_PROCESS,lpEnvironment,NULL,&StartUPInfo,&ProcessInfo))
			{
				counter=0;
				bReturn = TRUE;
				DWORD error=GetLastError();
				#ifdef _DEBUG
					char			szText[256];
					sprintf(szText," ++++++ CreateProcessAsUser winlogon %d\n",error);
					OutputDebugString(szText);		
				#endif
			}
		 else
		 {
			 DWORD error=GetLastError();
			 #ifdef _DEBUG
					char			szText[256];
					sprintf(szText," ++++++ CreateProcessAsUser failed %d %d %d\n",error,kickrdp,counter);
					OutputDebugString(szText);		
			#endif
			if (error==233 && kickrdp==1)
					{
						counter++;
						if (counter>3)
							{
								#ifdef _DEBUG
								DWORD error=GetLastError();
								sprintf(szText," ++++++ error==233 win\n");
								SetLastError(0);
								OutputDebugString(szText);		
								#endif
								typedef BOOLEAN (WINAPI * pWinStationConnect) (HANDLE,ULONG,ULONG,PCWSTR,ULONG);
								typedef BOOL (WINAPI * pLockWorkStation)();
								HMODULE  hlibwinsta = LoadLibrary("winsta.dll"); 
								HMODULE  hlibuser32 = LoadLibrary("user32.dll");
								pWinStationConnect WinStationConnectF=NULL;
								pLockWorkStation LockWorkStationF=NULL;

								if (hlibwinsta)
								   {
									   WinStationConnectF=(pWinStationConnect)GetProcAddress(hlibwinsta, "WinStationConnectW"); 
								   }
								if (hlibuser32)
								   {
									   LockWorkStationF=(pLockWorkStation)GetProcAddress(hlibuser32, "LockWorkStation"); 
								   }
								if (WinStationConnectF!=NULL && WinStationConnectF!=NULL)
									{
											DWORD ID=0;
											if (lpfnWTSGetActiveConsoleSessionId.isValid()) ID=(*lpfnWTSGetActiveConsoleSessionId)();
											WinStationConnectF(0, 0, ID, L"", 0);
											LockWorkStationF();
									}
								Sleep(3000);
						}
					}
			else if (error==233)
			{
				CreateRemoteSessionProcess(dwSessionId,true,hToken,NULL,app_path,NULL,NULL,FALSE,CREATE_UNICODE_ENVIRONMENT |DETACHED_PROCESS,lpEnvironment,NULL,&StartUPInfo,&ProcessInfo);
				counter=0;
				bReturn = TRUE;
			}
		 }

         if (lpEnvironment) 
         {
            DestroyEnvironmentBlock(lpEnvironment);
         }

	  }//createenv
	  else
	  {
		  SetLastError(0);
         if (CreateProcessAsUser(hToken,NULL,app_path,NULL,NULL,FALSE,DETACHED_PROCESS,NULL,NULL,&StartUPInfo,&ProcessInfo))
			{
				counter=0;
				bReturn = TRUE;
				DWORD error=GetLastError();
				#ifdef _DEBUG
					char			szText[256];
					sprintf(szText," ++++++ CreateProcessAsUser winlogon %d\n",error);
					OutputDebugString(szText);		
				#endif
			}
		 else
		 {
			 DWORD error=GetLastError();
			 #ifdef _DEBUG
					char			szText[256];
					sprintf(szText," ++++++ CreateProcessAsUser no env failed %d\n",error);
					OutputDebugString(szText);		
			#endif
			//Little trick needed, FUS sometimes has an unreachable logon session.
			 //Switch to USER B, logout user B
			 //The logon session is then unreachable
			 //We force the logon session on the console
			if (error==233 && kickrdp==1)
					{
						counter++;
						if (counter>3)
							{
								#ifdef _DEBUG
								DWORD error=GetLastError();
								sprintf(szText," ++++++ error==233 win\n");
								SetLastError(0);
								OutputDebugString(szText);		
								#endif
								typedef BOOLEAN (WINAPI * pWinStationConnect) (HANDLE,ULONG,ULONG,PCWSTR,ULONG);
								typedef BOOL (WINAPI * pLockWorkStation)();
								HMODULE  hlibwinsta = LoadLibrary("winsta.dll"); 
								HMODULE  hlibuser32 = LoadLibrary("user32.dll");
								pWinStationConnect WinStationConnectF=NULL;
								pLockWorkStation LockWorkStationF=NULL;

								if (hlibwinsta)
								   {
									   WinStationConnectF=(pWinStationConnect)GetProcAddress(hlibwinsta, "WinStationConnectW"); 
								   }
								if (hlibuser32)
								   {
									   LockWorkStationF=(pLockWorkStation)GetProcAddress(hlibuser32, "LockWorkStation"); 
								   }
								if (WinStationConnectF!=NULL && WinStationConnectF!=NULL)
									{
											DWORD ID=0;
											if (lpfnWTSGetActiveConsoleSessionId.isValid()) ID=(*lpfnWTSGetActiveConsoleSessionId)();
											WinStationConnectF(0, 0, ID, L"", 0);
											LockWorkStationF();
									}
								Sleep(3000);
						}
			}
			else if (error==233)
			{
				CreateRemoteSessionProcess(dwSessionId,true,hToken,NULL,app_path,NULL,NULL,FALSE,DETACHED_PROCESS,NULL,NULL,&StartUPInfo,&ProcessInfo);
				counter=0;
				bReturn = TRUE;
			}
	  }
        
	}  //getsession
	CloseHandle(hToken);
	}
  else
  {
	 #ifdef _DEBUG
	char			szText[256];
	DWORD error=GetLastError();
	sprintf(szText," ++++++ Getsessionusertokenwin failed %d\n",error);
	OutputDebugString(szText);		
	#endif

  }
    
    return bReturn;
}
//////////////////////////////////////////////////////////////////////////////

void wait_for_existing_process()
{
#ifdef _DEBUG
        OutputDebugString("Checking for preexisting tray icon\n");
#endif
    while ((hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Global\\SessionEventUltra")) != NULL) {
    	SetEvent(hEvent); // signal tray icon to shut down 
        CloseHandle(hEvent);

#ifdef _DEBUG
        OutputDebugString("Waiting for existing tray icon to exit\n");
#endif
        Sleep(1000);
    }
}

#include <sddl.h>
SECURITY_ATTRIBUTES secAttr;
extern SERVICE_STATUS serviceStatus;

char old_buffer_s[512];
int old_buflen_s=0;
char buffer_s[512];
int buflen_s;

bool CheckIPAddrString() {
    char namebuf[256];

    if (gethostname(namebuf, 256) != 0) {
		strncpy(buffer_s, "Host name unavailable", buflen_s);
		return false;
    };

    HOSTENT *ph = gethostbyname(namebuf);
    if (!ph) {
		strncpy(buffer_s, "IP address unavailable", buflen_s);
		return false;
    };

    *buffer_s = '\0';
    char digtxt[5];
    for (int i = 0; ph->h_addr_list[i]; i++) {
    	for (int j = 0; j < ph->h_length; j++) {
			sprintf(digtxt, "%d.", (unsigned char) ph->h_addr_list[i][j]);
			strncat(buffer_s, digtxt, (buflen_s-1)-strlen(buffer_s));
		}	
		buffer_s[strlen(buffer_s)-1] = '\0';
		if (ph->h_addr_list[i+1] != 0)
			strncat(buffer_s, ", ", (buflen_s-1)-strlen(buffer_s));
    }
	if (strlen(buffer_s)<512) // just in case it would be bigger then our buffer
	{
	if (old_buflen_s!=0)
	{
		if (strncmp(old_buffer_s,buffer_s,old_buflen_s)!=NULL)	
		{
			old_buflen_s=strlen(buffer_s);
			strncpy(old_buffer_s,buffer_s,strlen(buffer_s));
			return false;
		}
	}
	old_buflen_s=strlen(buffer_s);
	strncpy(old_buffer_s,buffer_s,strlen(buffer_s));
	}
	return true;
}

void monitor_sessions()
{
	pad2();
	HANDLE hTokenNew = NULL, hTokenDup = NULL;

	int counter_ipcheck=0;
	DWORD dwSessionId=0;
	DWORD OlddwSessionId=99;
	ProcessInfo.hProcess=0;
	bool win=false;
	bool Slow_connect=false;
	bool last_con=false;
	//We use this event to notify the program that the session has changed
	//The program need to end so the service can restart the program in the correct session
    wait_for_existing_process();
	hEvent = CreateEvent(NULL, FALSE, FALSE, "Global\\SessionEventUltra");	
	hEventcad = CreateEvent(NULL, FALSE, FALSE, "Global\\SessionEventUltraCad");
	Sleep(3000);
	HANDLE testevent[2];
	testevent[0]=stopServiceEvent;
	testevent[1]=hEventcad;
	bool ToCont=true;
	while(ToCont && serviceStatus.dwCurrentState==SERVICE_RUNNING)
	{
	DWORD dwEvent;
	dwEvent=WaitForMultipleObjects(2,testevent,FALSE, 1000);
	switch (dwEvent) 
    { 
		//stopServiceEvent, exit while loop
		case WAIT_OBJECT_0 + 0: 
			ToCont=false;
            break; 

        //cad request
        case WAIT_OBJECT_0 + 1: 
			{
			typedef VOID (WINAPI *SendSas)(BOOL asUser);
			HINSTANCE Inst = LoadLibrary("sas.dll");
			SendSas sendSas = (SendSas) GetProcAddress(Inst, "SendSAS");
			if (sendSas) sendSas(FALSE);
			else
			{
				char WORKDIR[MAX_PATH];
				char mycommand[MAX_PATH];
				if (GetModuleFileName(NULL, WORKDIR, MAX_PATH))
					{
					char* p = strrchr(WORKDIR, '\\');
					if (p == NULL) return;
					*p = '\0';
					}
				strcpy(mycommand,"");
				strcat(mycommand,WORKDIR);//set the directory
				strcat(mycommand,"\\");
				strcat(mycommand,"cad.exe");
				(void)ShellExecute(GetDesktopWindow(), "open", mycommand, "", 0, SW_SHOWNORMAL);
			}
			if (Inst) FreeLibrary(Inst);
			}
            break; 

        case WAIT_TIMEOUT:
			{
				counter_ipcheck++;
				if (counter_ipcheck==30)
				{
					counter_ipcheck=0;
					if (!CheckIPAddrString()) OlddwSessionId=99;  //force restart
				}

									if (lpfnWTSGetActiveConsoleSessionId.isValid())
										dwSessionId = (*lpfnWTSGetActiveConsoleSessionId)();
									if (OlddwSessionId!=dwSessionId)
									{
										#ifdef _DEBUG
												char			szText[256];
												sprintf(szText," ++++++SetEvent Session change: signal tray icon to shut down\n");
												OutputDebugString(szText);		
										#endif
										SetEvent(hEvent);
									}

		
			

									if (dwSessionId!=0xFFFFFFFF)
										{
													DWORD dwCode=0;
													if (ProcessInfo.hProcess==NULL)
													{
																Sleep(1000);
																if (Slow_connect) Sleep(2000);
							#ifdef _DEBUG
														OutputDebugString("No Tray icon existed, starting first process\n");
							#endif
																LaunchProcessWin(dwSessionId);
																win=false;
																Slow_connect=false;
													}
													else if (GetExitCodeProcess(ProcessInfo.hProcess,&dwCode))
													{
														if(dwCode != STILL_ACTIVE)
															{
																if (last_con==true)
																{
																	//problems, we move from win-->default-->win
																	// Put a long timeout to give system time to start or logout
																	Sleep(2000);
																}
							//#if 0
																Sleep(1000);
																if (Slow_connect) Sleep(4000);
							//#endif
							#ifdef _DEBUG
																OutputDebugString("Waiting up to 15 seconds for tray icon process to exit\n");
							#endif

																WaitForSingleObject(ProcessInfo.hProcess, 15000);
																CloseHandle(ProcessInfo.hProcess);
																CloseHandle(ProcessInfo.hThread);
																LaunchProcessWin(dwSessionId);
																win=false;
																Slow_connect=false;
															}
														else
															{
																if (Slow_connect==false)
																{
																	//This is the first time, so createprocess worked
																	//last_con=false-->defaultdesk
																	//last_con=true-->windesk
																	last_con=false;
																}
																Slow_connect=true;
															}
													}
													else
													{
														if (last_con==true)
																{
																	//problems, we move from win-->default-->win
																	// Put a long timeout to give system time to start or logout
																	Sleep(2000);
																}
														Sleep(1000);
														if (Slow_connect) Sleep(4000);
														if (ProcessInfo.hProcess) CloseHandle(ProcessInfo.hProcess);
														if (ProcessInfo.hThread) CloseHandle(ProcessInfo.hThread);
							#ifdef _DEBUG
														OutputDebugString("Tray icon exited, starting new process\n");
							#endif
														LaunchProcessWin(dwSessionId);
														win=false;
														Slow_connect=false;
													}
												#ifdef _DEBUG
												char			szText[256];
												sprintf(szText," ++++++1 %i %i %i %i\n",OlddwSessionId,dwSessionId,dwCode,ProcessInfo.hProcess);
												OutputDebugString(szText);		
												#endif
												OlddwSessionId=dwSessionId;
										}
			}//timeout
		}//switch
	}//while
	#ifdef _DEBUG
					char			szText[256];
					sprintf(szText," ++++++SetEvent Service stopping: signal tray icon to shut down\n");
					OutputDebugString(szText);		
	#endif
	if (hEvent) SetEvent(hEvent);

    if (ProcessInfo.hProcess)
    {
#ifdef _DEBUG
    OutputDebugString("Waiting up to 15 seconds for tray icon process to exit\n");
#endif
        WaitForSingleObject(ProcessInfo.hProcess, 15000);
	    if (ProcessInfo.hProcess) CloseHandle(ProcessInfo.hProcess);
	    if (ProcessInfo.hThread) CloseHandle(ProcessInfo.hThread);
    }

//	EndProcess();

	if (hEvent) CloseHandle(hEvent);
	if (hEventcad) CloseHandle(hEventcad);
}

// 20 April 2008 jdp paquette@atnetsend.net

bool IsAnyRDPSessionActive()
{
    WTS_SESSION_INFO *pSessions = 0;
    DWORD   nSessions(0);
    DWORD   rdpSessionExists = false;

    typedef BOOL (WINAPI *pfnWTSEnumerateSessions)(HANDLE,DWORD,DWORD,PWTS_SESSION_INFO*,DWORD*);
    typedef VOID (WINAPI *pfnWTSFreeMemory)(PVOID);

    helper::DynamicFn<pfnWTSEnumerateSessions> pWTSEnumerateSessions("wtsapi32","WTSEnumerateSessionsA");
    helper::DynamicFn<pfnWTSFreeMemory> pWTSFreeMemory("wtsapi32", "WTSFreeMemory");

    if (pWTSEnumerateSessions.isValid() && pWTSFreeMemory.isValid())


    if ((*pWTSEnumerateSessions)(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessions, &nSessions)) 
    {
        for (DWORD i(0); i < nSessions && !rdpSessionExists; ++i)
        {
            if ((_stricmp(pSessions[i].pWinStationName, "Console") != 0) &&
                (pSessions[i].State == WTSActive        || 
                 pSessions[i].State == WTSShadow        ||
                 pSessions[i].State == WTSConnectQuery
                ))
            {
                rdpSessionExists = true;
            }
        }

        (*pWTSFreeMemory)(pSessions);
    }

    return rdpSessionExists ? true : false;
}

// 20 April 2008 jdp paquette@atnetsend.net
void disconnect_remote_sessions()
{
	typedef BOOLEAN (WINAPI * pWinStationConnect) (HANDLE,ULONG,ULONG,PCWSTR,ULONG);
	
	typedef BOOL (WINAPI * pLockWorkStation)();

	HMODULE  hlibwinsta = LoadLibrary("winsta.dll"); 
	HMODULE  hlibuser32 = LoadLibrary("user32.dll");
	pWinStationConnect WinStationConnectF=NULL;
	pLockWorkStation LockWorkStationF=NULL;


    // don't kick rdp off if there's still an active session
    if (IsAnyRDPSessionActive())
        return;

	if (hlibwinsta)
	   {
		   WinStationConnectF=(pWinStationConnect)GetProcAddress(hlibwinsta, "WinStationConnectW"); 
	   }
	
	if (hlibuser32)
	   {
		   LockWorkStationF=(pLockWorkStation)GetProcAddress(hlibuser32, "LockWorkStation"); 
	   }
	if (WinStationConnectF!=NULL && LockWorkStationF!=NULL)
		{
				DWORD ID=0;
				if (lpfnWTSGetActiveConsoleSessionId.isValid()) ID=(*lpfnWTSGetActiveConsoleSessionId)();
				WinStationConnectF(0, 0, ID, L"", 0);
				// sleep to allow the system to finish the connect/disconnect process. If we don't
				// then the workstation won't get locked every time.
            	Sleep(3000);
				if (!LockWorkStationF())
                {
                    char msg[1024];
                    sprintf(msg, "LockWorkstation failed with error 0x%0X", GetLastError());
                    ::OutputDebugString(msg);
                }

		}
	Sleep(3000);

	if (hlibwinsta)
        FreeLibrary(hlibwinsta);
	if (hlibuser32)
        FreeLibrary(hlibuser32);
}

