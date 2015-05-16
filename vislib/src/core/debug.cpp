#include <Windows.h>
#include <stdio.h>
#include <vis/debug.h>

void TRACE(const char *fmt, ...)
{
	char s[1000];
	va_list l;

	va_start(l, fmt);
	vsprintf_s(s,1000, fmt, l);
	va_end(l);

	OutputDebugString(s);
}

void DBG_CrtMemCheckpoint(
	_CrtMemState * _State
	)
{
	_CrtMemCheckpoint(_State);
}