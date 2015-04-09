// MyCmdDebugger.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <process.h> 
#include <vector> 

enum TOKEN_TYPE
{
	ENTER = '\n',
	IF,

};

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2)
		return 1;

	FILE *fp = fopen(argv[1], "r");
	fseek(fp, SEEK_END, 0);
	long length = ftell(fp);
	fseek(fp, SEEK_SET, 0);
	std::vector<char> content(length);
	fread(&content[0], 1, length, fp);
	fclose(fp);

	return 0;
}

