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
	void getTimeTickCount(); //ah=0  从BIOS RAM中获取时钟计数
	void setTimeTickCount();//ah=1  设置BIOS RAM中的时钟计数
	void getTime_CMOS();//ah=2 读CMOS时间
	void setTime_CMOS();//ah=3 设置CMOS时间
	void getDate_CMOS();//ah=4 读CMOS日期
	void setDate_CMOS();//ah=5 设置CMOS日期
	void setAlarmClock();//ah=6 设置闹钟

private:
	//U4 rawTickCount;//程序运行时第一时间获取的tick
	time_t rawTime;//程序运行第一时间获取的时间,主要为日期计算
	//U4 lastReadTickCount;//保持上次ah=0的中断时候的计数值，以确定是否翻转

};

#endif