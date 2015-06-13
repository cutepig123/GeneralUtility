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
	if(NULL==(fExe = fopen("DOS.bin","rb")))//��ʱ������//��ȡ��Ҫִ�е��ļ�
	{
		this->buildLog(Hardware::ErrorType::FileNotFound, "Can not find the execution file!");
		this->showMessage( "Can not find the execution file!");
		return notFoundError;
	}
	this->buildLog(Hardware::ErrorType::Success, "Found the execution file!");

	U1* memStartAddr = pMB->getMemStartAddr();
	if(/*SIZE_SECTOR !=*/ !(sizeReaded=fread(memStartAddr+0x7c00, sizeof(U1), SIZE_SECTOR, fExe)))//���ļ������������ڴ���
	{
		this->buildLog(Hardware::ErrorType::FileCanNotRead, "Can not read the execution file!");
		this->showMessage( "Can not read the execution file!");
		return readError;
	}
	this->buildLog(Hardware::ErrorType::Success, "Read the execution file!");
	fclose(fExe);
	//<����BIOS����>
	if(NULL==(fBiosINT = fopen("BIOSData/VirtualBIOS_INT.bin", "rb")))
	{
		this->buildLog(Hardware::ErrorType::FileNotFound, "Can not find the BIOS_INT file!");
		this->showMessage( "Can not find the BIOS_INT file!");
		return notFoundError;
	}
	this->buildLog(Hardware::ErrorType::Success, "Found the BIOS_INT file!");
	if(!(sizeReaded=fread(memStartAddr, sizeof(U1), 0x500, fBiosINT)))//���ļ�����������bios�������ݵ��ڴ���
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
	if(!(sizeReaded=fread(memStartAddr+0xf0000, sizeof(U1), 1024, fBiosStuff)))//���ļ�����������bios�������ݵ��ڴ���
	{
		this->buildLog(Hardware::ErrorType::FileCanNotRead, "Can not read the BIOS_Stuff file!");
		this->showMessage( "Can not read the BIOS_Stuff file!");
		return readError;
	}
	this->buildLog(Hardware::ErrorType::Success, "Read the BIOS_Stuff file!");
	fclose(fBiosStuff);
	//fclose (fBiosINT);
	//</����BIOS����>

	*(U2*)(memStartAddr+0x41A) = 0x41E;
	*(U2*)(memStartAddr+0x41C) = 0x41E;
	

	//{//����ָ�� Disk parameters ��ָ��ֵ
	//	*(memStartAddr+0x78 + 0x0) = 0xc3;
	//	*(memStartAddr+0x78 + 0x1) = 0x01;
	//	*(memStartAddr+0x78 + 0x2) = 0x00;
	//	*(memStartAddr+0x78 + 0x3) = 0xf0;

	//	*(memStartAddr+0x1C3 + 0x0) = 0xaf;//��1.44M��������
	//	*(memStartAddr+0x1C3 + 0x1) = 0x02;//��2.88M�������ϵĴ�ͷ����ʱ�䡣��������Bochs��
	//	*(memStartAddr+0x1C3 + 0x2) = 0x25;//���������ر���ʱΪ����
	//	*(memStartAddr+0x1C3 + 0x3) = 0x02;//ÿ������512���ֽ�
	//	*(memStartAddr+0x1C3 + 0x4) = 0x12;//ÿ�ŵ�������18��
	//	*(memStartAddr+0x1C3 + 0x5) = 0x1b;//����֮���϶���ȣ�1bh��1.2M��1.44M��2.88M����
	//	*(memStartAddr+0x1C3 + 0x6) = 0xff;//���ݳ���
	//	*(memStartAddr+0x1C3 + 0x7) = 0x6c;//��ʽ����϶���ȣ���1.44M����
	//	*(memStartAddr+0x1C3 + 0x8) = 0xf6;//��ʽ������ֽ�
	//	*(memStartAddr+0x1C3 + 0x9) = 0x0f;//��ͷ��λʱ�䣬Ϊ15ms
	//	*(memStartAddr+0x1C3 + 0xa) = 0x08;//�ȴ���￪��ʱ�䣬��8������һ�����ʱ
	//	*(memStartAddr+0x1C3 + 0x10) = 0x04;//һ�¶��Ǹ���easyVM���õ�
	//	*(memStartAddr+0x1C3 + 0x11) = 0x0;
	//	*(memStartAddr+0x1C3 + 0x12) = 0x04;
	//	*(memStartAddr+0x1C3 + 0x13) = 0x0;
	//	*(memStartAddr+0x1C3 + 0x14) = 0x0;

	//	*(U2*)(memStartAddr+0x413)= 0x27f;
	//	*(U2*)(memStartAddr+0x400+0x100)= 0x0;//�����������
	//	*(U2*)(memStartAddr+0x410) &= 0x0010;//��Ƶ
	//	*(U2*)(memStartAddr+0x410) |= 0x0020;//��Ƶ
	//	*(U2*)(memStartAddr+0x410) |= 0x0060;//��������
	//	*(U2*)(memStartAddr+0x410) |= 0x0001;//װ�д���
	//	*(U4*)(memStartAddr+0x46C) = 0xB3937;//ʱ��
	//	*(U1*)(memStartAddr+0x470) = 0x0;//ʱ��
	//	//*(U1*)(memStartAddr+0x474) = 0x0;//Ӳ��
	//	//*(U1*)(memStartAddr+0x475) = 0x1;
	
	return OK;

}