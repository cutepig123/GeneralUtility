#ifndef _LOG_MAKER_H
#define _LOG_MAKER_H
#include<string>
#include<stdio.h>
using namespace std;

enum LMErrorType{MemoryCanNotApply, FileCanNotRead, FileNotFound, UnHandle,Success };
class LogMaker
{
public:
	typedef LMErrorType ErrorType;

	static bool  generateLog(string logSrc, ErrorType type, string comment);
	static bool  logEnd();//�������ʱ��ִ��
private://��ֹ����LogMakerʵ��
	LogMaker(){}
	static string type2String(ErrorType type); 
};



#endif