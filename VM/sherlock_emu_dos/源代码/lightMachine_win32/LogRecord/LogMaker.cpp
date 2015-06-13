#include "LogMaker.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
using namespace std;
const string newLine ="\n";
const string fileName = "log.xml";
string LogMaker::type2String(ErrorType type)
{
	string result_str = "";
	switch(type)
	{
	case MemoryCanNotApply:
		result_str = "MemoryCanNotApply";
		break;
	case FileCanNotRead:
		result_str ="FileCanNotRead";
		break;
	case FileNotFound:
		result_str = "FileNotFound";
		break;
	case Success:
		result_str = "Success";
		break;
	case UnHandle:
		result_str = "UnHandle";
		break;
	default:
		result_str = "Unknow";
	}
	return result_str;
}
bool LogMaker::generateLog(string logSrc, ErrorType type, string comment)
{
	static int countNum = 0;
	char openMode[10];
	time_t now;
	now = time(NULL);
	string str_time( ctime(&now) );
	string str_errorType=type2String(type);
	string log;
	if (0 != countNum++ )
	{
		strcpy(openMode, "a");
	}
	else
	{
		strcpy(openMode, "w");
		log ="<LOG>";
	}
	log +=  "    <Message  type=\"" + str_errorType + "\">" + newLine;
	log += "        <time>" + newLine;
	log += "            " + str_time + newLine;
	log += "        </time>" + newLine;
	log += "        <from>" + newLine;
	log += "            " + logSrc + newLine;
	log += "        </from>" + newLine;
	log += "        <comment>" + newLine;
	log += "            " + comment + newLine;
	log += "        </comment>" + newLine;
	log += "    </Message>" + newLine;
	FILE* pfileLog = NULL;
	if( NULL==(pfileLog = fopen(&fileName.at(0), openMode)) )
		return false;
	fprintf(pfileLog, &log.at(0));
	fclose(pfileLog);
	return true;
}


bool  LogMaker::logEnd()//程序结束时候执行
{
	FILE* pfileLog = NULL;
	if( NULL==(pfileLog = fopen(&fileName.at(0), "a+")) )
		return false;
	fprintf(pfileLog, "</LOG>");
	fclose(pfileLog);
}
