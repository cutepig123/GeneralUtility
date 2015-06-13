#ifndef _REAL_TIME_CLOCK_H
#define _REAL_TIME_CLOCK_H
#include "../common.h"
#include "../Device.h"
#include <time.h>
#include <stdlib.h>
class RealTimeClock:public Device
{
public:
	RealTimeClock();
	~RealTimeClock();
	void init();
	vector<int> get_INT_shouldHandle();
	void HandleINT(U1 INT_num);
	void updateTime();
private:
	void getTimeTickCount(); //ah=0  ��BIOS RAM�л�ȡʱ�Ӽ���
	void setTimeTickCount();//ah=1  ����BIOS RAM�е�ʱ�Ӽ���
	void getTime_CMOS();//ah=2 ��CMOSʱ��
	void setTime_CMOS();//ah=3 ����CMOSʱ��
	void getDate_CMOS();//ah=4 ��CMOS����
	void setDate_CMOS();//ah=5 ����CMOS����
	void setAlarmClock();//ah=6 ��������

private:
	//U4 rawTickCount;//��������ʱ��һʱ���ȡ��tick
	time_t rawTime;//�������е�һʱ���ȡ��ʱ��,��ҪΪ���ڼ���
	//U4 lastReadTickCount;//�����ϴ�ah=0���ж�ʱ��ļ���ֵ����ȷ���Ƿ�ת

};

#endif