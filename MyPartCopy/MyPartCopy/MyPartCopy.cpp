// MyPartCopy.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

#define	MY_ASSERT(x)	if(!(x)) {printf("Error " #x " Line %d\n", __LINE__);sts=-1;goto Exit;}

int create(char const * file, unsigned long nSize, char data)
{
	int sts =0;
	FILE *fp =0;

	fp =fopen(file,"wb");
	MY_ASSERT(fp);

	for(unsigned long i=0; i<nSize; i++)
		fwrite( &data, 1, 1, fp);

Exit:
	if(fp )
		fclose(fp);

    return sts;	
}

int copy(const char* file_s, unsigned long offset_s, const char* file_d, unsigned long offset_d, unsigned long nSize)
{
	int sts =0;
	FILE *fp_s =0;
	FILE *fp_d =0;

	struct _stat stat_s;
	MY_ASSERT(0==_stat(file_s, &stat_s));
	MY_ASSERT(offset_s +nSize <=stat_s.st_size );
	
	struct _stat stat_d;
	MY_ASSERT(0==_stat(file_d, &stat_d));
	MY_ASSERT(offset_d +nSize <=stat_d.st_size );
	
	fp_s =fopen(file_s,"rb");
	MY_ASSERT(fp_s);

	/* That's the difference. r+ doesn't truncate the file, it just opens it
and allows to read and write. fopen with w+ always returns a handle to
empty file with read and write privileges. */
	fp_d =fopen(file_d,"rb+");
	MY_ASSERT(fp_d);

	MY_ASSERT(0 == fseek( fp_s, offset_s, SEEK_SET));
	MY_ASSERT(0 == fseek( fp_d, offset_d, SEEK_SET));
   
	for(unsigned long i=0; i<nSize; i++)
	{
		char c=0;
		MY_ASSERT(1 ==fread(&c,1,1,fp_s));
		MY_ASSERT(1 ==fwrite( &c, 1, 1, fp_d));
	}

Exit:
	if(fp_s )
		fclose(fp_s);

	if(fp_d )
		fclose(fp_d);

    return sts;	
}

// Example:
// C:\JSHe\codes\MyPartCopy\Debug>MyPartCopy.exe -debug -mode create -size 1234 -file 1.dat
// C:\JSHe\codes\MyPartCopy\Debug>MyPartCopy.exe -debug -mode copy -file_s MyPartCopy.x -file_d 1.dat -offset_s 0 -offset_d 1 -size 10
int main(int argc, char * argv[])
{
	int sts =0;

	bool debug= cmdOptionExists(argv, argv + argc, "-debug");
	if(debug)
	{
		system("pause");
	}

    char * mode= getCmdOption(argv, argv + argc, "-mode");

	MY_ASSERT(mode);
    if (stricmp(mode,"create")==0)
    {
		char * file= getCmdOption(argv, argv + argc, "-file");
		MY_ASSERT(file);
		
		char * size= getCmdOption(argv, argv + argc, "-size");
        MY_ASSERT(size);
		unsigned long nSize = atol(size);
		MY_ASSERT(nSize>0);
		
		sts =create(file,nSize, 0);
		MY_ASSERT(sts>=0);
    }
	else if (stricmp(mode,"copy")==0)
	{
		char * file_s= getCmdOption(argv, argv + argc, "-file_s");
		MY_ASSERT(file_s);

		char * file_d= getCmdOption(argv, argv + argc, "-file_d");
		MY_ASSERT(file_d);

		char * offset_s= getCmdOption(argv, argv + argc, "-offset_s");
        MY_ASSERT(offset_s);
		unsigned long noffset_s = atol(offset_s);
		MY_ASSERT(noffset_s>=0);

		char * offset_d= getCmdOption(argv, argv + argc, "-offset_d");
        MY_ASSERT(offset_d);
		unsigned long noffset_d = atol(offset_d);
		MY_ASSERT(noffset_d>=0);

		char * size= getCmdOption(argv, argv + argc, "-size");
        MY_ASSERT(size);
		unsigned long nSize = atol(size);
		MY_ASSERT(nSize>0);

		sts =copy(file_s, noffset_s, file_d, noffset_d, nSize);
		MY_ASSERT(sts>=0);
	}
	else
		MY_ASSERT(0);
Exit:
    return sts;
}

