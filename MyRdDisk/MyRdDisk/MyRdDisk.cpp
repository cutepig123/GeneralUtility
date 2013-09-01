// MyRdDisk.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <assert.h>

#pragma pack(push) 
#pragma pack(1)
struct BootSector
{
	char jmp[3];

	char bpbOEM[8];
	short bpbBytesPerSector;
	char bpbSectorsPerCluster;
	short bpbReservedSectors;
	char bpbNumberOfFATs;
	short bpbRootEntries;
	short bpbTotalSectors;
	char bpbMedia;
	short bpbSectorsPerFAT;
	short bpbSectorsPerTrack;
	short bpbHeadsPerCylinder;
	int bpbHiddenSectors;
	int bpbTotalSectorsBig;
	char bsDriveNumber;
	char bsUnused;
	char bsExtBootSignature;
	int bsSerialNumber;
	char bsVolumeLabel[11];
	char bsFileSystem[8];
	char dummy[448];
	short signature;
};
struct FATTbl
{
	char d[512*9];
};

#define	ENTRY_STS_UNALLOCED	0x00
#define	ENTRY_STS_DELETED	0xE5

struct RootDirEntry
{
	char nameOrSts[11];	//[0] sts
	char attr;
	char reserved;
	char create_s;
	short create_hms;
	short create_date;
	short access_date;
	short addr_hi;	//20-21
	short modify_hms;
	short modify_date;
	short addr_low;
	unsigned int size;
};

struct FAT
{
	BootSector bs;
	FATTbl fat[2];
	RootDirEntry dir[1];
};

#pragma pack(pop) 

#define MY_ASSERT(x) if(!(x)) {assert(0);sts =-1; goto Exit;}
#define CHK_STS(x) sts =(x); if(sts<0) { assert(0);goto Exit;}

short ReadSect
	(const char *_dsk,    // disk to access
	void *_buff,         // buffer where sector will be stored
	unsigned int _nsect_id,   // sector number, starting with 0
	unsigned int _nsect   // sector number, starting with 0
	)
{
	short sts =0;
	DWORD dwRead; 
	HANDLE hDisk =0;
	

	hDisk=CreateFile(_dsk,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_RANDOM_ACCESS,0);
	MY_ASSERT(hDisk!=INVALID_HANDLE_VALUE);
	
	SetFilePointer(hDisk,_nsect_id*512,0,FILE_BEGIN); // which sector to read

	MY_ASSERT( ReadFile(hDisk,_buff,_nsect*512,&dwRead,0));  // read sector
	MY_ASSERT( dwRead==_nsect*512 );
	
Exit:

	CloseHandle(hDisk);

	return sts;
}

int main()
{
	short sts =0;
	char *dsk="\\\\.\\A:"; 
	RootDirEntry *dir =0;

	BootSector bootSec;
	MY_ASSERT( sizeof(bootSec)==512 );
	CHK_STS( ReadSect(dsk,&bootSec,0, 1) );

	FATTbl fat[2];
	MY_ASSERT( sizeof(fat)==512*9*2 );
	CHK_STS( ReadSect(dsk,fat,1, 9*2) );

	
	int nDir =14*512/sizeof(RootDirEntry);
	MY_ASSERT( nDir*sizeof(RootDirEntry) ==14*512 );
	dir =new RootDirEntry[nDir];
	CHK_STS( ReadSect(dsk,dir,19, 14) );

	//for(int i=0; i<nDir; i++)
	//{
	//	if( 
	//}
Exit:
	if(dir) delete []dir;
}