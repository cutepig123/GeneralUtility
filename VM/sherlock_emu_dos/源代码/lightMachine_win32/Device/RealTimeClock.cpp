#include "RealTimeClock.h"
#include "../CPU/CentreProcessorUnit.h"
const float TICK_TIME = 54.9254;
const int TickCountAddr = 0x46c; //ʱ�ӵδ�����֤�����λ�ã�Ϊ�����ֵĳ��ȣ�
const int trunBackSignAddr = 0x470;//��ת��־����λΪ�ֽڣ�
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
	result = m%10;//ȡ��λ
	result |= (m/10)<<4;//ȡ��ʮλ
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
	rawTime = time(NULL);//��ȡ����ʱ��
	rawTime += 8*3600;//�ǳ���֡�������Ϊ�й��Ƕ����������Ծ�Ҫ��8��Сʱ��������
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
{//ah=0  ��BIOS RAM�л�ȡʱ�Ӽ���
	pCPU->cx = (*(U4*)(memStartAddr+TickCountAddr))>>16; //��ʱ���ĸ�λ��
	pCPU->dx = *(U4*)(memStartAddr+TickCountAddr) & 0x0000ffff;//��ʱ���ĵ�λ��
	if(0==*(U1*)(memStartAddr+trunBackSignAddr))
		pCPU->al = 0;//���ϴζ����𣬼���û�г���24Сʱ
	else 
		pCPU->al = 1;//���ϴζ����𣬼�������24Сʱ
	*(U1*)(memStartAddr+trunBackSignAddr) = 0;//��ת��־��0��
}
void RealTimeClock::setTimeTickCount()
{//ah=1  ����BIOS RAM�е�ʱ�Ӽ���
	U4 tick = ((pCPU->cx)<<16)+pCPU->dx;
	*(U4*)(memStartAddr+TickCountAddr) = tick;
	*(U1*)(memStartAddr+trunBackSignAddr) = 0;//��ת��־��0��
}
void RealTimeClock::getTime_CMOS()
{//ah=2 ��CMOSʱ��
	LightTimer time = tickConver2Time(*(U4*)(memStartAddr+TickCountAddr));
	pCPU->ch = toBCD(time.hour);
	pCPU->cl = toBCD(time.min);
	pCPU->dh = toBCD(time.sec);
	pCPU->dl = 0;//��ֹ������ʱ��
	pCPU->setCF_Flag(false);//��ȡʱ��ɹ�

}
void RealTimeClock::setTime_CMOS()
{//ah=3 ����CMOSʱ��
	
	int hour = BCD2dex(pCPU->ch);
	int min = BCD2dex(pCPU->cl);
	int sec = BCD2dex(pCPU->dh);
	*(U4*)(memStartAddr+TickCountAddr) = ((hour*3600 + min*60 + sec)*1000)/TICK_TIME;
	pCPU->setCF_Flag(false);
}
void RealTimeClock::getDate_CMOS()
{//ah=4 ��CMOS����
	tm* pstruct_time = gmtime(&this->rawTime);
	if(pstruct_time->tm_year>=100)//year ��1900������
	{
		pCPU->ch = 0x20;// BCD��ʾ��20����
		pCPU->cl = toBCD(pstruct_time->tm_year - 100);
	}
	else
	{
		pCPU->ch = 0x19;//BCD��ʾ��19����
		pCPU->cl = toBCD(pstruct_time->tm_year);
	}
	pCPU->dh = toBCD(pstruct_time->tm_mon + 1);//��Ϊ1��ʱ��tm_mon��0
	pCPU->dl = toBCD(pstruct_time->tm_mday);//һ���µĵڼ���
	pCPU->setCF_Flag(false);//��ȡ���ڳɹ�
}
void RealTimeClock::setDate_CMOS()
{//ah=5 ����CMOS����
	
	tm* pstruct_time = gmtime(&this->rawTime);
	if(0x20 == pCPU->ch )
	{
		pstruct_time->tm_year = 100;//��ʾ2000��
		pstruct_time->tm_year += BCD2dex(pCPU->cl);
	}
	else
	{//pCPU->ch == 0x19
		pstruct_time->tm_year = BCD2dex(pCPU->cl);
	}

}
void RealTimeClock::setAlarmClock()
{//ah=6 ��������
	pCPU->setCF_Flag(true);//���ò��ɹ�
}

void RealTimeClock::updateTime()
{
	if((*(U4*)(memStartAddr+TickCountAddr))++ >= 0x1800b2)
	{//��24Сʱ
		*(U4*)(memStartAddr+TickCountAddr) = 0;
		*(U1*)(memStartAddr+trunBackSignAddr) = 1;
	}
}
