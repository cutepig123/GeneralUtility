#pragma once
#include <vis/export.h>
#include <crtdbg.h>

VIS_XXPORT void TRACE(const char *fmt, ...);

VIS_XXPORT void DBG_CrtMemCheckpoint(
	_CrtMemState * _State
	);
