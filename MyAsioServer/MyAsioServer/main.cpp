#include "stdafx.h"

int client();
int asynServer();
int synServer();

int main(int argc, char **argv)
{
	if (argc < 2)
		return 1;

	int sts = 0;
	if (stricmp(argv[1], "c")==0)
		sts = client();
	else if (stricmp(argv[1], "s") == 0)
		sts = synServer();
	else if (stricmp(argv[1], "as") == 0)
		sts = asynServer();

	return 0;
}