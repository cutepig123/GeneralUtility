// testSystem.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <afx.h>
#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <map>
#include <set>
#include <string>
#include <list>
#include <vector>

char line[10000];
FILE *fpW_Line = 0;							//count line
std::map<float, std::string> file_dates;	//for srch file within some date
std::vector<std::string>	 g_errFdrs;
int hours_before = 0;
std::set<std::string> exts;
FILETIME stNow;

// bug1: file_dates<> to std::map<float, vector<std::string>>
//

#define	FNAME_IN	"linecnt.ini"
#define	FNAME_OU	"linecntOut.ini"

BOOL LogConfig(BOOL isWrite)
{
	if(isWrite)
	{
		FILE *fp=fopen(FNAME_OU,"w");
		std::set<std::string>::iterator it= exts.begin(), end=exts.end();
		for(; it!=end; ++it)
		{
			fprintf( fp, "%s\n", it ->c_str());
		}
		fclose(fp);
	}
	else
	{
		exts.clear();

		FILE *fp=fopen(FNAME_IN,"r");
		char s[300];

		if(fp)
		{
			while(fgets(s,300,fp))
			{
				CString sName(s);
				sName.Trim();
				sName.MakeLower();
				
				exts.insert((const char*)sName);
				
			}
			
			fclose(fp);
		}
		
	}

	return TRUE;
}



void count(const char *file, long &nchar, long &nline)
{
	nchar = 0;
	nline = 0;
	FILE *fp = fopen(file, "r");
	if(fp)
	{
		for(char *p = fgets(line, 10000, fp); p; p = fgets(line, 10000, fp))
		{
			nline++;
			nchar += (long)strlen(p);
		}
		
		fclose(fp);
	}
}

inline bool isExt(const char *sFile)
{
	CString file(sFile);

	file.MakeLower();
	if(exts.empty())
		return true;
	else
	{
		if( exts.find((const char*)file) != exts.end() )
			return true;

		// get file ext
		const char *ext = strrchr(file, '.');
		if(!ext)
			return false;
		ext++;

		//to lower
		char ext_l[100]={0};
		for(size_t i=0, n=strlen(ext); i<=n; i++)
			ext_l[i] = (ext[i] >= 'A' && ext[i] <= 'Z')?(ext[i]-'A'+'a'):(ext[i]);
			
		// srch ext
		return exts.find(ext_l) != exts.end();

	}
}

float seconds_between(const FILETIME & from, const FILETIME & to)
{
    enum {
       TICKS_PER_DAY = 10000000,
    };
   return float((*(__int64 *) & to) - (*(__int64 *) & from)) / TICKS_PER_DAY;
}

void FormatLongPath(CString const &sFull, CString &sFormat)
{
	int NMax=40;
	if(sFull.GetLength()>NMax)
		sFormat.Format("%s...%s  ", sFull.Mid(0, NMax/2), sFull.Mid(NMax-NMax/2, NMax/2));
	else
		sFormat =sFull;
}
// The fnchar, fnline are wrong for folder level counting
//, long &fnchar, long &fnline
void getFileList(const char *rootFolder)
{
	std::list<std::string> stkFolder;
	
	stkFolder.push_back(rootFolder);

	while( !stkFolder.empty() )
	{
		std::string sfolder =stkFolder.front();
		stkFolder.pop_front();
		const char *folder =sfolder.c_str();

		WIN32_FIND_DATA stFindData;
		long nchar, nline;
		CString sFull,sFormat;
		
		sFull.Format( "%s\\*", folder);
		HANDLE Handle = FindFirstFile(sFull, &stFindData);
		
		if(Handle == INVALID_HANDLE_VALUE){
			printf("No File in %s\n", sFull);
			g_errFdrs.push_back((const char*)sFull);
			continue;
		}
		FindNextFile(Handle, &stFindData);
		while (FindNextFile(Handle, &stFindData))
		{
			sFull.Format( "%s\\%s", folder, stFindData.cFileName);
				
			if(stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)	//folder
			{
				FormatLongPath(sFull, sFormat);
				printf("\r[%d] files detected\t %s",file_dates.size(), sFormat);
				stkFolder.push_back( (const char*)sFull );
				
			}
			else	//file
			{
				if(isExt(stFindData.cFileName))
				{
					if(fpW_Line)
					{
						count(sFull, nchar, nline);
						fprintf(fpW_Line, "<tr><td>%s<td>%d<td>%d</tr>\n", sFull, nchar, nline);
						//gnchar += nchar;
						//gnline += nline;
						//fnchar += nchar;
						//fnline += nline;
					}
					
					if(hours_before)
					{
						//FILETIME stNow;
						//GetSystemTimeAsFileTime(&stNow);
						float diff_second = seconds_between(stFindData.ftLastWriteTime,stNow);
						//assert(diff_second>0);
						if(diff_second<hours_before*60.0*60)	//check date 
						{
							file_dates[diff_second] = sFull;
							//fprintf(fpW_Date, "%s\n", sFull);
						}
					}
					
				}
			}
		}
		
		FindClose(Handle);
	}

}

void main(int argc, char *argv[])
{
	bool succeed = false;
	char *src_folder = argv[argc-1];
	char destFolder[MAX_PATH];
	char destfile[300];

	system("pause");
	LogConfig(FALSE);
	LogConfig(TRUE);
	printf("Supported exts:\n");
	system("type " FNAME_OU);

	sprintf( destFolder, src_folder );

	assert(isExt("Winlingo.xml"));

	if(argc>=2)
	{
		//parse arguments
		for(int i=1; i<argc; i++)
		{
			if(argv[i][0]=='-')
			{
				if(argv[i][1]=='t')
				{
					sscanf(argv[i+1], "%d", &hours_before);
				}
				else if(argv[i][1]=='c')
				{
					strcpy(destFolder, argv[i+1]);
				}
				else if(argv[i][1]=='l')
				{
					sprintf(destfile, "%s\\lineCnt.htm", destFolder);
					fpW_Line = fopen(destfile, "w");
				}
			}
		}
		
		printf("parse result, hours_before %d, destFolder %s\n", hours_before, destFolder);
		
		//long fnchar=0, fnline=0;
		
		GetSystemTimeAsFileTime(&stNow);

		if(fpW_Line)
			fprintf(fpW_Line, "<table>\n");
		
		getFileList(src_folder);
		
		if(fpW_Line)
		{
			//fprintf(fpW_Line, "<tr><td>%s<td>%d<td>%d</tr>\n", "Total", gnchar, gnline);
			fprintf(fpW_Line, "</table>\n");
		}
		
		if(fpW_Line)
			fclose(fpW_Line);

		if(hours_before)
		{
			sprintf(destfile, "DateCnt.txt");
			FILE* fpW_Date = fopen(destfile, "w");

			sprintf(destfile, "DateCnt.htm");
			FILE* fpW_Date_htm = fopen(destfile, "w");

			/*if( !fpW_Date || !fpW_Date_htm)
			{
				printf("Create file in folder %s fails!\n", destFolder);
				system("pause");
			}*/

			fprintf(fpW_Date_htm, "<table> \n");
			for(std::map<float, std::string>::iterator it = file_dates.begin(), end = file_dates.end();
				it!=end; ++it)
			{
				fprintf(fpW_Date, "%s\n", it->second.c_str());

				fprintf(fpW_Date_htm, "<tr> <td>%f <td>%s\n", it->first/(60*60), it->second.c_str());
			}
			fprintf(fpW_Date_htm, "</table> \n");

			fclose(fpW_Date);
			fclose(fpW_Date_htm);
		}
	}
	
	if(!succeed)
	{
		printf("Usage:\n");
		printf("lineCnt [-t <hours before>] [-copyto <folder>] [-linecnt] <folder>\n");
	}

	if(!g_errFdrs.empty())
	{
		printf("===Error Files:\n");
		for( size_t i=0, n=g_errFdrs.size(); i<n; i++)
		{
			printf("%s\n",g_errFdrs[i].c_str());
		}
		system("pause");
	}
}

