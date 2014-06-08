#include "stdafx.h"
#include "windows.h"

int client();
int asynServer();
int synServer();
int httpServer();
int httpClient();

int main(int argc, char **argv)
{
	system("pause");

	if (argc < 2)
		return 1;

	int sts = 0;
	if (_stricmp(argv[1], "c") == 0)
		sts = client();
	else if (_stricmp(argv[1], "s") == 0)
		sts = synServer();
	else if (_stricmp(argv[1], "as") == 0)
		sts = asynServer();
	else if (_stricmp(argv[1], "hs") == 0)
		sts = httpServer();
	else if (_stricmp(argv[1], "hc") == 0)
		sts = httpClient();

	return 0;
}