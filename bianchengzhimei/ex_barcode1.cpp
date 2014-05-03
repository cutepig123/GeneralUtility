#include "inc.h"
#include "assert.h"

int checksum(const char *s)
{
	int n = strlen(s);
	int a = 0, b = 0;
	for (int i = n - 1; i >= 0; i-=2)
	{
		int t = s[i] - '0';
		assert(t >= 0 && t <= 9);
		a += t;
	}

	for (int i = n - 2; i >= 0; i -= 2)
	{
		int t = s[i] - '0';
		assert(t >= 0 && t <= 9);
		b += t;
	}

	return 10 - (a * 3 + b) % 10;
}
void ex()
{
	int c = checksum("692481080200");
	assert(c == 2);
	std::cout << c;
}