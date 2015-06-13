//#include "StdAfx.h"
#include "MagneticDisk.h"
#include"../CPU/CentreProcessorUnit.h"
MagneticDisk::MagneticDisk(void)
{
	this->hardwareName ="MagneticDisk";
}

MagneticDisk::~MagneticDisk(void)
{
	delete[] memFloppy;
}
void MagneticDisk::init()
{
	unsigned int totalByte = bytePerSector*sectorPerTrack*trackPerHead*numHeads;
	memFloppy = new U1[totalByte];
	FILE* fExe;
	unsigned int sizeReaded=0;
	if(NULL==(fExe = fopen("DOS.bin","rb")))//暂时这样用//读取需要执行的文件
	{
		return ;
	}
	if(!(sizeReaded=fread(memFloppy, sizeof(U1), totalByte, fExe)))//把文件读入的磁盘中
		return ;

	fclose(fExe);
	return ;
}
vector<int> MagneticDisk::get_INT_shouldHandle()
{
	vector<int> INTarray;
	INTarray.push_back(0x13);//DISK需要处理INT 为10 的中断

	//INTarray.push_back(0x11);
	//INTarray.push_back(0x12);
	//INTarray.push_back(0x1a);
	////INTarray.push_back(0x15);
	//INTarray.push_back(0x14);
	//INTarray.push_back(0x17);
	//INTarray.push_back(0x16);
	return INTarray;
}
void MagneticDisk::HandleINT(U1 INT_num)
{
	//暂时用不到INT_num
	if(0x13 ==INT_num)
	{
		switch(pCPU->ah)
		{
		case 0:
			this->diskReset();
			break;
		case 1:
			this->readCondition();
			break;
		case 2:
			this->readDisk();
			break;
		case 3:
			this->writeDisk();
		case 0x8:
			this->getPara();
			break;
		case 0x15:
			this->getTypeOfFloppy();
			break;
		default:
			//assert(false);
			return;
		}
	}
	//else if( 0x11 ==INT_num)//取设备信息
	//{
	//	pCPU->ax = 0x61;
	//}
	//else if(0x12 == INT_num)//内存容量
	//{
	//	pCPU->ax = 0x27f;
	//}
	//else if(0x1a == INT_num)//时钟
	//{
	//	switch(pCPU->ah)
	//	{
	//	case 0://读和设置
	//		pCPU->dx = 0xd6f7;
	//		pCPU->cx = 0xf;
	//		pCPU->al = 0;
	//		break;
	//	case 1:
	//	case 2:
	//		pCPU->cx = 0x1550;
	//		pCPU->dx = 0x1600;
	//		break;
	//	case 3:
	//		pCPU->cx =0xA;
	//		pCPU->dx =0x7755;
	//		break;
	//	case 4:
	//		pCPU->dx = 0x713;
	//		pCPU->cx = 0x2010;
	//		pCPU->setCF_Flag(false);
	//		break;
	//	case 5:
	//		pCPU->ch= 0x20;
	//		pCPU->cl = 0x10;
	//		pCPU->dh =1;
	//		pCPU->dl =1;
	//		break;
	//	case 6:
	//		pCPU->setCF_Flag(true);
	//		break;
	//	
	//	}
	//}
	//else if(0x15== INT_num)
	//{
	//	switch(pCPU->ah)
	//	{
	//	case 0x87:
	//		pCPU->ah=0;
	//		break;
	//	case 0x88:
	//		pCPU->setCF_Flag(false);
	//		pCPU->ax = 0x7c00;
	//		break;
	//	}
	//}
	//else if(0x14 == INT_num)
	//{
	//	/*if(0==pCPU->dx)
	//		pCPU->ax = 0x6030;
	//	else
	//		pCPU->ax = 0xa3;*/
	//}
	//else if(0x17 == INT_num)
	//{
	//	//if(1 == pCPU->ah )
	//	//{
	//	//	if(2==pCPU->dx || 1==pCPU->dx)
	//	//		pCPU->ah = 1;//超时
	//	//	else
	//	//		pCPU->ah = 0x10;//打印机选择
	//	//}
	//	//else if(0 == pCPU->ah)
	//	//{
	//	//	//pCPU->ah = 0x10;//打印机选择
	//	//}
	//}
	//else if(0x16 == INT_num)
	//{
	//	switch(pCPU->ah)
	//	{
	//	case 0x1:
	//		pCPU->setZF_Flag(false);
	//		break;
	//	case 0x2:
	//		pCPU->al = 0x20;
	//		break;
	//	}
	//}
}

void MagneticDisk::getPara()
{//参考easyVM
	if(0x80 <= pCPU->dl)
	{
		pCPU->ah=0x1;
		pCPU->setCF_Flag(true);
		storeCondition_AH();
		pCPU->setPF_Flag(false);
		pCPU->setIF_Flag(false);//DOS惹的祸
	}
	else 
	{
		pCPU->ah = 0;
		pCPU->bl = 0x4;
		pCPU->cx = 0x4f12;
		pCPU->dx = 0x102;
		pCPU->di = *(U2*)(memStartAddr + 0x1e*4);
		pCPU->es = *(U2*)(memStartAddr + 0x1e*4+2);
		pCPU->setCF_Flag(false);
		storeCondition_AH();
	}
}
void MagneticDisk::getTypeOfFloppy()
{//参考easyVM
		if (pCPU->dl<0x02)		//暂时只支持两个软盘
		{
			pCPU->setCF_Flag(false);
			storeCondition_AH();
			pCPU->ah=1;
		}
		else
		{
			pCPU->setCF_Flag(false);
			storeCondition_AH();
			pCPU->ah=0;
		}
}
void MagneticDisk::diskReset()
{
	if(pCPU->dl != 0)
	{//目前虚拟机只有一个磁盘驱动
		pCPU->ah = 0xc;
		pCPU->setCF_Flag(true);
	}
	else
	{
		pCPU->ah = 0;
		pCPU->setCF_Flag(false);
	}
	storeCondition_AH();
}
void MagneticDisk::readCondition()
{
	pCPU->ah = getLastCondition();
}
void MagneticDisk::readDisk()
{
	unsigned drive = pCPU->dl;
	unsigned magneticHead = pCPU->dh;
	unsigned magneticTrack = pCPU->ch;
	unsigned sector = pCPU->cl;
	if(drive/*驱动器号*/ != 0 || magneticHead/*磁头号*/ >=2 || sector/*扇区号*/ > 18 ||
		magneticTrack/*磁道号*/ >= 80)
	{
		pCPU->ah = 0x4; /*没有找到需要的扇区*/
		pCPU->setCF_Flag(true);
		storeCondition_AH();
		return;
	}
	else
	{
		U4 dataBufferLogicAddr = pCPU->bx +(pCPU->es<<4);
		readSector(drive, magneticHead, magneticTrack, sector, memStartAddr + dataBufferLogicAddr, pCPU->al);
		pCPU->ah = 0;
		pCPU->setCF_Flag(false);
	}
	storeCondition_AH();
	
}
void MagneticDisk::writeDisk()
{
	unsigned drive = pCPU->dl;
	unsigned magneticHead = pCPU->dh;
	unsigned magneticTrack = pCPU->ch;
	unsigned sector = pCPU->cl;
	if(drive/*驱动器号*/ != 0 || magneticHead/*磁头号*/ >=2 || sector/*扇区号*/ > 18 ||
		magneticTrack/*磁道号*/ >= 80)
	{
		pCPU->ah = 0x4; /*没有找到需要的扇区*/
		pCPU->setCF_Flag(true);
		storeCondition_AH();
		return;
	}
	else
	{
		U4 dataBufferLogicAddr = pCPU->bx +(pCPU->es<<4);
		writeSector(drive, magneticHead, magneticTrack, sector, memStartAddr + dataBufferLogicAddr, pCPU->al);
		pCPU->ah = 0;
		pCPU->setCF_Flag(false);
	}
	storeCondition_AH();
}



void MagneticDisk::storeCondition_AH()
{//存储状态字节
	*(U1*)(this->memStartAddr + 0x400 + 0x41) = pCPU->ah;
}
U1 MagneticDisk::getLastCondition()
{//读取上次存储的状态字节
	return *(U1*)(this->memStartAddr + 0x400 + 0x41);
}


void MagneticDisk::readSector(unsigned int drive, unsigned int magneticHead, unsigned int magneticTrack,
				unsigned int sector, U1* des, unsigned int count)
{
	if(drive!=0)
	{
		this->buildLog(Hardware::ErrorType::UnHandle, "the drive is out of range");
		this->showMessage("the drive is out of range");
		assert(drive==0);
	}
	memcpy(des,  memFloppy + ((magneticTrack*numHeads + magneticHead)*sectorPerTrack + sector-1)*bytePerSector, bytePerSector*count);
}
void MagneticDisk::writeSector(unsigned int drive, unsigned int magneticHead, unsigned int magneticTrack,
				unsigned int sector, U1* des, unsigned int count)
{
	if(drive!=0)
	{
		this->buildLog(Hardware::ErrorType::UnHandle, "the drive is out of range");
		this->showMessage("the drive is out of range");
		assert(drive==0);
	}
	memcpy(memFloppy + ((magneticTrack*numHeads + magneticHead)*sectorPerTrack + sector-1)*bytePerSector, des, bytePerSector*count);
}