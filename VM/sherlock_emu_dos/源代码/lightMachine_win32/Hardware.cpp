#include "Hardware.h"
#include "GUI.h"
Hardware::Hardware()
{
}
Hardware::~Hardware(){}
void Hardware::buildLog(ErrorType type, string comment)
{
	if( false==LogMaker::generateLog(hardwareName, type, comment) )
	{
		MessageMaker::displayMessage("A Unknow Error Emerge When Write Log");
	}
}
string Hardware::toString()
{
	return hardwareName;
}
void Hardware::showMessage(string msg)
{
	MessageMaker::displayMessage(msg);
}