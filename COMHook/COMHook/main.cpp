#include "stdafx.h"

int TestCOMHook();
void TestVirtFunAddr();

int _tmain(int argc, _TCHAR* argv[])
{
	switch (1)
	{
		case 0:
		{
			int nRet = TestCOMHook();
			printf("Return %d\n", nRet);
		}
		break;
		case 1:
		{
			TestVirtFunAddr();
		}
		break;
	}

	return 0;
}
