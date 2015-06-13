#include "stdafx.h"
#include "VirtualBIOS.h"
#include "MotherBoard.h"
#include <stdio.h>

VirtualBIOS::VirtualBIOS(MotherBoard *pMB)
{
	this->hardwareName ="VirtualBIOS";
	this->pMB = pMB;
	init();
}

void VirtualBIOS::init()
{
	//some thing
}
FileError VirtualBIOS::loadExecuteFile()
{
	FILE* fExe;
	FILE* fBiosStuff=NULL, *fBiosINT=NULL;
	unsigned int sizeReaded=0;
	if(NULL==(fExe = fopen("DOS.bin","rb")))//暂时这样用//读取需要执行的文件
	{
		this->buildLog(Hardware::ErrorType::FileNotFound, "Can not find the execution file!");
		this->showMessage( "Can not find the execution file!");
		return notFoundError;
	}
	this->buildLog(Hardware::ErrorType::Success, "Found the execution file!");

	U1* memStartAddr = pMB->getMemStartAddr();
	if(/*SIZE_SECTOR !=*/ !(sizeReaded=fread(memStartAddr+0x7c00, sizeof(U1), SIZE_SECTOR, fExe)))//把文件读入的虚拟机内存中
	{
		this->buildLog(Hardware::ErrorType::FileCanNotRead, "Can not read the execution file!");
		this->showMessage( "Can not read the execution file!");
		return readError;
	}
	this->buildLog(Hardware::ErrorType::Success, "Read the execution file!");
	fclose(fExe);
	//<读入BIOS数据>
	if(NULL==(fBiosINT = fopen("BIOSData/VirtualBIOS_INT.bin", "rb")))
	{
		this->buildLog(Hardware::ErrorType::FileNotFound, "Can not find the BIOS_INT file!");
		this->showMessage( "Can not find the BIOS_INT file!");
		return notFoundError;
	}
	this->buildLog(Hardware::ErrorType::Success, "Found the BIOS_INT file!");
	if(!(sizeReaded=fread(memStartAddr, sizeof(U1), 0x500, fBiosINT)))//把文件读入的虚拟机bios保存数据的内存中
	{
		this->buildLog(Hardware::ErrorType::FileCanNotRead, "Can not read the BIOS_INT file!");
		this->showMessage( "Can not read the BIOS_INT file!");
		return readError;
	}
	this->buildLog(Hardware::ErrorType::Success, "Read the BIOS_INT file!");
	if(NULL==(fBiosStuff = fopen("BIOSData/VirtualBios_0xf0000.bin", "rb")))
	{
		this->buildLog(Hardware::ErrorType::FileNotFound, "Can not find the BIOS_Stuff file!");
		this->showMessage( "Can not find the BIOS_Stuff file!");
		return notFoundError;
	}
	this->buildLog(Hardware::ErrorType::Success, "Found the BIOS_Stuff file!");
	if(!(sizeReaded=fread(memStartAddr+0xf0000, sizeof(U1), 1024, fBiosStuff)))//把文件读入的虚拟机bios保存数据的内存中
	{
		this->buildLog(Hardware::ErrorType::FileCanNotRead, "Can not read the BIOS_Stuff file!");
		this->showMessage( "Can not read the BIOS_Stuff file!");
		return readError;
	}
	this->buildLog(Hardware::ErrorType::Success, "Read the BIOS_Stuff file!");
	fclose(fBiosStuff);
	//fclose (fBiosINT);
	//</读入BIOS数据>

	*(U2*)(memStartAddr+0x41A) = 0x41E;
	*(U2*)(memStartAddr+0x41C) = 0x41E;
	

	//{//设置指向 Disk parameters 的指针值
	//	*(memStartAddr+0x78 + 0x0) = 0xc3;
	//	*(memStartAddr+0x78 + 0x1) = 0x01;
	//	*(memStartAddr+0x78 + 0x2) = 0x00;
	//	*(memStartAddr+0x78 + 0x3) = 0xf0;

	//	*(memStartAddr+0x1C3 + 0x0) = 0xaf;//在1.44M驱动器上
	//	*(memStartAddr+0x1C3 + 0x1) = 0x02;//在2.88M驱动器上的磁头载入时间。。（参照Bochs）
	//	*(memStartAddr+0x1C3 + 0x2) = 0x25;//驱动器马达关闭延时为两秒
	//	*(memStartAddr+0x1C3 + 0x3) = 0x02;//每个扇区512个字节
	//	*(memStartAddr+0x1C3 + 0x4) = 0x12;//每磁道扇区有18个
	//	*(memStartAddr+0x1C3 + 0x5) = 0x1b;//扇区之间间隙长度，1bh供1.2M，1.44M，2.88M软盘
	//	*(memStartAddr+0x1C3 + 0x6) = 0xff;//数据长度
	//	*(memStartAddr+0x1C3 + 0x7) = 0x6c;//格式化间隙长度，供1.44M软盘
	//	*(memStartAddr+0x1C3 + 0x8) = 0xf6;//格式化填充字节
	//	*(memStartAddr+0x1C3 + 0x9) = 0x0f;//磁头定位时间，为15ms
	//	*(memStartAddr+0x1C3 + 0xa) = 0x08;//等待马达开启时间，用8来设置一秒的延时
	//	*(memStartAddr+0x1C3 + 0x10) = 0x04;//一下都是根据easyVM设置的
	//	*(memStartAddr+0x1C3 + 0x11) = 0x0;
	//	*(memStartAddr+0x1C3 + 0x12) = 0x04;
	//	*(memStartAddr+0x1C3 + 0x13) = 0x0;
	//	*(memStartAddr+0x1C3 + 0x14) = 0x0;

	//	*(U2*)(memStartAddr+0x413)= 0x27f;
	//	*(U2*)(memStartAddr+0x400+0x100)= 0x0;//设成软盘启动
	//	*(U2*)(memStartAddr+0x410) &= 0x0010;//视频
	//	*(U2*)(memStartAddr+0x410) |= 0x0020;//视频
	//	*(U2*)(memStartAddr+0x410) |= 0x0060;//两个软驱
	//	*(U2*)(memStartAddr+0x410) |= 0x0001;//装有磁盘
	//	*(U4*)(memStartAddr+0x46C) = 0xB3937;//时钟
	//	*(U1*)(memStartAddr+0x470) = 0x0;//时钟
	//	//*(U1*)(memStartAddr+0x474) = 0x0;//硬盘
	//	//*(U1*)(memStartAddr+0x475) = 0x1;
	
	return OK;

}