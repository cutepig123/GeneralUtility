#include "RealTimeClock.h"
#include "../CPU/CentreProcessorUnit.h"
const float TICK_TIME = 54.9254;
const int TickCountAddr = 0x46c; //时钟滴答数保证在这个位置，为两个字的长度！
const int trunBackSignAddr = 0x470;//翻转标志，单位为字节！
struct LightTimer
{
	int hour;
	int min;
	int sec;
	LightTimer()
	{
		hour = 0;
		min = 0;
		sec = 0;
	}
};
U1 toBCD(int m )
{
	if(m>99 || 0==m)
	{
		return 0;
	}
	U1 result=0;
	result = m%10;//取个位
	result |= (m/10)<<4;//取出十位
	return result;
}
int BCD2dex(U1 m)
{
	int result = m&0x0f;
	result += ((m&0xf0)>>4)*10;
	return result;
}
LightTimer tickConver2Time(const long tick)
{
	LightTimer time;
	double sec = (tick*TICK_TIME)/1000;
	time.hour = sec/3600;
	time.min = (sec - time.hour*3600)/60;
	time.sec = (sec - time.hour*3600 - time.min*60);
	return time;
}
RealTimeClock::RealTimeClock()
{
	this->hardwareName = "RealTimeClock";
}
RealTimeClock::~RealTimeClock(){}
void RealTimeClock::init()
{
	rawTime = time(NULL);//获取现在时间
	rawTime += 8*3600;//非常奇怪。。就因为中国是东八区，所以就要加8个小时。。悲剧
	tm* pstruct_time = gmtime(&rawTime);
	unsigned int tickCount = ((pstruct_time->tm_hour*3600 + pstruct_time->tm_min*60 + pstruct_time->tm_sec)*1000)/TICK_TIME;
	*(U4*)(memStartAddr+TickCountAddr) = tickCount;
	*(U1*)(memStartAddr+trunBackSignAddr) = 0;
}
vector<int> RealTimeClock::get_INT_shouldHandle()
{
	vector<int> INTarray;
	INTarray.push_back(0x1A);
	return INTarray;
}
void RealTimeClock::HandleINT(U1 INT_num)
{
	switch(pCPU->ah)
	{
	case 0:
		this->getTimeTickCount();
		break;
	case 0x1:
		this->setTimeTickCount();
		break;
	case 0x2:
		this->getTime_CMOS();
		break;
	case 0x3:
		this->setTime_CMOS();
		break;
	case 0x4:
		this->getDate_CMOS();
		break;
	case 0x5:
		this->setDate_CMOS();
		break;
	case 0x6:
		this->setAlarmClock();
		break;
	default:
		this->buildLog(Hardware::ErrorType::UnHandle, "RealTimeClock Unhandle some INTs");
		//this->showMessage("RealTimeClock Unhandle some INTs");
		assert(false);
		break;
	}
}

void RealTimeClock::getTimeTickCount()
{//ah=0  从BIOS RAM中获取时钟计数
	pCPU->cx = (*(U4*)(memStartAddr+TickCountAddr))>>16; //计时器的高位字
	pCPU->dx = *(U4*)(memStartAddr+TickCountAddr) & 0x0000ffff;//计时器的低位字
	if(0==*(U1*)(memStartAddr+trunBackSignAddr))
		pCPU->al = 0;//从上次读算起，计数没有超过24小时
	else 
		pCPU->al = 1;//从上次读算起，计数超过24小时
	*(U1*)(memStartAddr+trunBackSignAddr) = 0;//翻转标志质0；
}
void RealTimeClock::setTimeTickCount()
{//ah=1  设置BIOS RAM中的时钟计数
	U4 tick = ((pCPU->cx)<<16)+pCPU->dx;
	*(U4*)(memStartAddr+TickCountAddr) = tick;
	*(U1*)(memStartAddr+trunBackSignAddr) = 0;//翻转标志质0；
}
void RealTimeClock::getTime_CMOS()
{//ah=2 读CMOS时间
	LightTimer time = tickConver2Time(*(U4*)(memStartAddr+TickCountAddr));
	pCPU->ch = toBCD(time.hour);
	pCPU->cl = toBCD(time.min);
	pCPU->dh = toBCD(time.sec);
	pCPU->dl = 0;//禁止夏令制时间
	pCPU->setCF_Flag(false);//读取时间成功

}
void RealTimeClock::setTime_CMOS()
{//ah=3 设置CMOS时间
	
	int hour = BCD2dex(pCPU->ch);
	int min = BCD2dex(pCPU->cl);
	int sec = BCD2dex(pCPU->dh);
	*(U4*)(memStartAddr+TickCountAddr) = ((hour*3600 + min*60 + sec)*1000)/TICK_TIME;
	pCPU->setCF_Flag(false);
}
void RealTimeClock::getDate_CMOS()
{//ah=4 读CMOS日期
	tm* pstruct_time = gmtime(&this->rawTime);
	if(pstruct_time->tm_year>=100)//year 从1900年算起，
	{
		pCPU->ch = 0x20;// BCD表示的20世纪
		pCPU->cl = toBCD(pstruct_time->tm_year - 100);
	}
	else
	{
		pCPU->ch = 0x19;//BCD表示的19世纪
		pCPU->cl = toBCD(pstruct_time->tm_year);
	}
	pCPU->dh = toBCD(pstruct_time->tm_mon + 1);//因为1月时，tm_mon是0
	pCPU->dl = toBCD(pstruct_time->tm_mday);//一个月的第几天
	pCPU->setCF_Flag(false);//读取日期成功
}
void RealTimeClock::setDate_CMOS()
{//ah=5 设置CMOS日期
	
	tm* pstruct_time = gmtime(&this->rawTime);
	if(0x20 == pCPU->ch )
	{
		pstruct_time->tm_year = 100;//表示2000年
		pstruct_time->tm_year += BCD2dex(pCPU->cl);
	}
	else
	{//pCPU->ch == 0x19
		pstruct_time->tm_year = BCD2dex(pCPU->cl);
	}

}
void RealTimeClock::setAlarmClock()
{//ah=6 设置闹钟
	pCPU->setCF_Flag(true);//调用不成功
}

void RealTimeClock::updateTime()
{
	if((*(U4*)(memStartAddr+TickCountAddr))++ >= 0x1800b2)
	{//满24小时
		*(U4*)(memStartAddr+TickCountAddr) = 0;
		*(U1*)(memStartAddr+trunBackSignAddr) = 1;
	}
}
