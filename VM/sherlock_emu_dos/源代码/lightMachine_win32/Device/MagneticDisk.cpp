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
	if(NULL==(fExe = fopen("DOS.bin","rb")))//��ʱ������//��ȡ��Ҫִ�е��ļ�
	{
		return ;
	}
	if(!(sizeReaded=fread(memFloppy, sizeof(U1), totalByte, fExe)))//���ļ�����Ĵ�����
		return ;

	fclose(fExe);
	return ;
}
vector<int> MagneticDisk::get_INT_shouldHandle()
{
	vector<int> INTarray;
	INTarray.push_back(0x13);//DISK��Ҫ����INT Ϊ10 ���ж�

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
	//��ʱ�ò���INT_num
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
	//else if( 0x11 ==INT_num)//ȡ�豸��Ϣ
	//{
	//	pCPU->ax = 0x61;
	//}
	//else if(0x12 == INT_num)//�ڴ�����
	//{
	//	pCPU->ax = 0x27f;
	//}
	//else if(0x1a == INT_num)//ʱ��
	//{
	//	switch(pCPU->ah)
	//	{
	//	case 0://��������
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
	//	//		pCPU->ah = 1;//��ʱ
	//	//	else
	//	//		pCPU->ah = 0x10;//��ӡ��ѡ��
	//	//}
	//	//else if(0 == pCPU->ah)
	//	//{
	//	//	//pCPU->ah = 0x10;//��ӡ��ѡ��
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
{//�ο�easyVM
	if(0x80 <= pCPU->dl)
	{
		pCPU->ah=0x1;
		pCPU->setCF_Flag(true);
		storeCondition_AH();
		pCPU->setPF_Flag(false);
		pCPU->setIF_Flag(false);//DOS�ǵĻ�
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
{//�ο�easyVM
		if (pCPU->dl<0x02)		//��ʱֻ֧����������
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
	{//Ŀǰ�����ֻ��һ����������
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
	if(drive/*��������*/ != 0 || magneticHead/*��ͷ��*/ >=2 || sector/*������*/ > 18 ||
		magneticTrack/*�ŵ���*/ >= 80)
	{
		pCPU->ah = 0x4; /*û���ҵ���Ҫ������*/
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
	if(drive/*��������*/ != 0 || magneticHead/*��ͷ��*/ >=2 || sector/*������*/ > 18 ||
		magneticTrack/*�ŵ���*/ >= 80)
	{
		pCPU->ah = 0x4; /*û���ҵ���Ҫ������*/
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
{//�洢״̬�ֽ�
	*(U1*)(this->memStartAddr + 0x400 + 0x41) = pCPU->ah;
}
U1 MagneticDisk::getLastCondition()
{//��ȡ�ϴδ洢��״̬�ֽ�
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