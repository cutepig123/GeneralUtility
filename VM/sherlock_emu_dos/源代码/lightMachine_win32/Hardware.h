#ifndef _HARDWARE_H
#define _HARDWARE_H
#include "LogRecord/LogMaker.h"
#include <string>
using namespace std;
class Hardware
{
public:
	typedef LogMaker::ErrorType ErrorType;
	Hardware();
	~Hardware();
	virtual void buildLog(ErrorType type, string comment);
	virtual string toString();
	virtual void showMessage(string msg);

	string hardwareName;
};


#endif