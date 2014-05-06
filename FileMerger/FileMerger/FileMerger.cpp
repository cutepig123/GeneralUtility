// FileMerger.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "vector"
#include "string"
#include <algorithm>

#define	MY_ASSERT(x)	if(!(x)) {printf("Error assert(" #x ") Line %d\n", __LINE__);wSts=-1;goto Exit;}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
	return std::find(begin, end, option) != end;
}

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
	char ** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
	{
		return *itr;
	}
	return 0;
}
// 功能：讲一堆文件放到一个大文件里面
// append stream to end of file
// get list of files
// get one of the files
// 不支持删除文件

struct ItemHdr
{
	char name[_MAX_PATH];
	size_t size;
	char p[0];
};
struct Header
{
	char sig[5];
	size_t nFile;
};

#define	MY_ERR_OPEN_FILE	-1
#define	MY_ASSERT_FAIL		-2
#define	MY_ERR_FILE_INDEX	-3

#define	MY_SIG	"HJSFI"

enum Oper
{
	GETLIST, APPEND, GETFILE
};

struct In
{
	Oper oper;

	
	struct Append{
		char *data;
		size_t nLen;
		char *name;
	}app;

	struct GetFile
	{
		int index;
		char *dest_file;
	}getf;
};

struct Out
{
	struct GetList
	{
		std::vector<std::string>  v;
	}getl;

	struct GetFile
	{
		char name[_MAX_PATH];
		std::vector<char>  d;
	}getf;
};

short MainOp(In const &in, Out &out, const char *file)
{
	short wSts = 0;

	FILE *fp = NULL;

	switch (in.oper)
	{
	case APPEND:
		printf("Open %s for append\n", file);
		fp = fopen(file, "rb+");
		if (!fp){
			fp = fopen(file, "wb+");
		}
		break;
	case GETFILE:
	case GETLIST:
		printf("Open %s for read\n", file);
		fp = fopen(file, "rb");
		break;
	}
	if (!fp){ wSts = MY_ERR_OPEN_FILE; goto Exit; }

	fseek(fp, 0, SEEK_END);
	long n = ftell(fp);
	printf("File Sz %d\n", n);

	Header	h;
	memset(&h, 0, sizeof(h));
	fseek(fp, 0, SEEK_SET);
	if (n > sizeof(Header))
	{
		fread(&h, sizeof(h), 1, fp);
		if (0 != memcmp(h.sig, MY_SIG, sizeof(h.sig))){ wSts = MY_ASSERT_FAIL; goto Exit; }
	}
	else
	{
		strncpy(h.sig, MY_SIG, sizeof(h.sig));
		h.nFile = 0;
		fwrite(&h, sizeof(h), 1, fp);
	}
	printf("Read header Num File %d\n", h.nFile);

	switch (in.oper)
	{
	case APPEND:
	
		break;
	case GETFILE:
		if (in.getf.index < 0 || in.getf.index >= h.nFile)
		{
			wSts = MY_ERR_FILE_INDEX; goto Exit;
		}
		break;
	case GETLIST:
		out.getl.v.reserve(h.nFile);
		break;
	}

	size_t nOff = sizeof(h);
	for (size_t i = 0; i < h.nFile; i++)
	{
		fseek(fp, nOff, SEEK_SET);

		ItemHdr item;
		memset(&item, 0, sizeof(item));
		fread(&item, sizeof(item), 1, fp);
		if (item.size == 0)
			break;

		printf("File [%d] Header Offset %d Name %s Size %d\n", i, nOff, item.name, item.size);

		switch (in.oper)
		{
		case APPEND:

			break;
		case GETFILE:
			if (in.getf.index ==i)
			{
				printf("Extract to Dest file %s\n", in.getf.dest_file);

				_snprintf(out.getf.name, _MAX_PATH, "%s", item.name);
				out.getf.d.resize(item.size);

				fread(&out.getf.d[0], item.size, 1, fp);
				FILE *fpw = fopen(in.getf.dest_file, "wb");
				fwrite(&out.getf.d[0], item.size, 1, fpw);
				fclose(fpw);
			}
			break;
		case GETLIST:
			out.getl.v.push_back(item.name);
			break;
		}
		nOff += sizeof(item)+item.size;
	}
	
	switch (in.oper)
	{
	case APPEND:
	{
		ItemHdr item;
		memset(&item, 0, sizeof(item));
		item.size = in.app.nLen;
		sprintf(item.name, in.app.name);
		printf("Append File [%d] Header Offset %d Name %s Size %d\n", h.nFile, nOff, item.name, item.size);
		fseek(fp, nOff, SEEK_SET);
		fwrite(&item, sizeof(item), 1, fp);
		fwrite(in.app.data, in.app.nLen, 1, fp);

		fseek(fp, 0, SEEK_SET);
		h.nFile++;
		fwrite(&h, sizeof(h), 1, fp);

	}
		break;
	case GETFILE:
		
		break;
	case GETLIST:
		
		break;
	}
	
Exit:
	if (fp)fclose(fp);
	return wSts;
}

int _tmain(int argc, _TCHAR* argv[])
{
	short wSts = 0;

	bool isDbg =cmdOptionExists(argv, argv + argc, "-debug");
	if (isDbg) system("pause");

	// -mode append -src name -file file
	// -mode getfile -idx id -des name -file file
	// -mode getlist -file file
	In in;
	Out out;

	char * file = getCmdOption(argv, argv + argc, "-file");
	MY_ASSERT(file);
	
	char * mode = getCmdOption(argv, argv + argc, "-mode");
	MY_ASSERT(mode);
	if (stricmp(mode, "append") == 0)
	{
		// -mode append -src name -file file
		in.oper = APPEND;

		std::vector<char> buf;
		char * name = getCmdOption(argv, argv + argc, "-src");
		MY_ASSERT(name);
		FILE *fp = fopen(name, "rb");
		MY_ASSERT(fp);
		fseek(fp, 0, SEEK_END);
		long n = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		buf.resize(n);
		fread(&buf[0], n, 1, fp);
		fclose(fp);

		in.app.name = name;
		in.app.nLen = n;
		in.app.data = &buf[0];
		MainOp(in, out, file);
	}
	else if (stricmp(mode, "getfile") == 0)
	{
		// -mode getfile -idx id -des name -file file
		in.oper = GETFILE;

		char * idx = getCmdOption(argv, argv + argc, "-idx");
		MY_ASSERT(idx);

		char * des = getCmdOption(argv, argv + argc, "-des");
		MY_ASSERT(des);

		in.getf.index = atoi(idx);
		in.getf.dest_file = des;

		MainOp(in, out, file);
	}
	else if (stricmp(mode, "getlist") == 0)
	{
		// -mode getlist -file file
		in.oper = GETLIST;
		MainOp(in, out, file);
	}
	else MY_ASSERT(0);

Exit:
	return 0;
}

