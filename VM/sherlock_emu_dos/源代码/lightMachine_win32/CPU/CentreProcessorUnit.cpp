//#include "stdafx.h"
#include "CentreProcessorUnit.h"
#include "../MotherBoard.h"
void CentreProcessorUnit::linkMotherBoard(MotherBoard* pMB)
{//����MotherBoard
	this->pMB=pMB;
	init();
	this->deCodeBlock.init();
	this->exeCodeBlock.init();
}
void CentreProcessorUnit::init()
{
	this->hardwareName ="CentreProcessorUnit";
	isNeedHandleKeyPress = false;
	OF_Flag=DF_Flag=IF_Flag=TF_Flag=SF_Flag=ZF_Flag=AF_Flag= PF_Flag= CF_Flag=false;
	SF_Flag=CF_Flag=AF_Flag=true;
	isOFCaculated= isDFCaculated= isIFCaculated= isTFCaculated=isSFCaculated=isZFCaculated=isAFCaculated= isPFCaculated= isCFCaculated = false;
	this->flag_extern = 0;
	this->needCaculateFlag_mask = 0;

	this->eax = 0xaa55;
	this->ebx = 0;
	this->ecx = 1;
	this->edx = 0;
	this->esp = 0xfffe;
	this->ebp = 0;
	this->edi = 0x400;
	this->esi = 0x293;

	this->ds = 0;
	this->es = 0;
	this->ss = 0;


	this->cs=0x0;
	this->ip=0x7c00;
}

void CentreProcessorUnit::storeCaculate(const Method opMethod, const int bitLength, const U4 operand1, const U4 operand2, const U4 result, const U2 mask_Flag)
{	
	if(mask_Flag & MASK_FLAG_SF)
	{
		this->SF_lazyTable.bitLength = bitLength;
		this->SF_lazyTable.result = result;
	}
	if(mask_Flag & MASK_FLAG_ZF)
	{
		this->ZF_lazyTable.result = result;
	}
	if(mask_Flag & MASK_FLAG_PF)
	{
		this->PF_lazyTable.opMethod = opMethod;
		this->PF_lazyTable.bitLength = bitLength;
		this->PF_lazyTable.operand1 = operand1;
		this->PF_lazyTable.operand2 = operand2;
		this->PF_lazyTable.result = result;
	}
	if(mask_Flag & MASK_FLAG_CF)
	{
		this->CF_lazyTable.opMethod = opMethod;
		this->CF_lazyTable.bitLength = bitLength;
		this->CF_lazyTable.operand1 = operand1;
		this->CF_lazyTable.operand2 = operand2;
		this->CF_lazyTable.result = result;
	}
	if(mask_Flag & MASK_FLAG_OF)
	{
		this->OF_lazyTable.opMethod = opMethod;
		this->OF_lazyTable.bitLength = bitLength;
		this->OF_lazyTable.operand1 = operand1;
		this->OF_lazyTable.operand2 = operand2;
		this->OF_lazyTable.result = result;
	}
	if(mask_Flag & MASK_FLAG_AF)
	{
		this->AF_lazyTable.opMethod = opMethod;
		this->AF_lazyTable.bitLength = bitLength;
		this->AF_lazyTable.operand1 = operand1;
		this->AF_lazyTable.operand2 = operand2;
		this->AF_lazyTable.result = result;
	}

	this->needCaculateFlag_mask |= mask_Flag;
}

bool CentreProcessorUnit::getOF_Flag()
{//����������
	if( !(needCaculateFlag_mask & MASK_FLAG_OF) )
		return this->OF_Flag;
	needCaculateFlag_mask &= ~(MASK_FLAG_OF) ;
	
	switch(OF_lazyTable.opMethod)
	{
	case ADD_8bit:
	case ADC_8bit:
		if( ((OF_lazyTable.operand1&MASK_10000000) == (OF_lazyTable.operand2&MASK_10000000)) &&
			((OF_lazyTable.operand1&MASK_10000000) !=(OF_lazyTable.result&MASK_10000000)))
			return OF_Flag = true;
		else
			return OF_Flag = false;
	case ADD_16bit:
	case ADC_16bit:
		if( ((OF_lazyTable.operand1&0x8000) == (OF_lazyTable.operand2&0x8000)) &&
			((OF_lazyTable.operand1&0x8000) !=(OF_lazyTable.result&0x8000)))
			return OF_Flag = true;
		else
			return OF_Flag = false;
	case SUB_8bit:
	case SBB_8bit:
	case CMP_8bit:
		if( ((OF_lazyTable.operand1&MASK_10000000) != (OF_lazyTable.operand2&MASK_10000000)) && 
			((OF_lazyTable.operand2&MASK_10000000) ==(OF_lazyTable.result&MASK_10000000)))
			return OF_Flag = true;
		else
			return OF_Flag = false;
	case SUB_16bit:
	case SBB_16bit:
	case CMP_16bit:
		if( ((OF_lazyTable.operand1&0x8000) != (OF_lazyTable.operand2&0x8000)) &&
			((OF_lazyTable.operand2&0x8000) ==(OF_lazyTable.result&0x8000)))
			return OF_Flag = true;
		else
			return OF_Flag = false;
	default:
		this->buildLog(Hardware::ErrorType::UnHandle, "OF flags lazy comput wrong");
		this->showMessage("OF flags lazy comput wrong");
		assert(false);
		return false;

	}

}
bool CentreProcessorUnit::getDF_Flag()
{
	//assert(false);�����㣬����û��ʲô��������Ϊ����~~
	if( !(needCaculateFlag_mask & MASK_FLAG_DF) )
		return this->DF_Flag;
	needCaculateFlag_mask &= ~(MASK_FLAG_DF) ;
}
bool CentreProcessorUnit::getIF_Flag()
{
	//assert(false);�����㣬����û��ʲô��������Ϊ����~~
	if( !(needCaculateFlag_mask & MASK_FLAG_IF) )
		return this->IF_Flag;
	needCaculateFlag_mask &= ~(MASK_FLAG_IF) ;
}
bool CentreProcessorUnit::getTF_Flag()
{
	//assert(false);�����㣬����û��ʲô��������Ϊ����~~
	if( !(needCaculateFlag_mask & MASK_FLAG_TF) )
		return this->TF_Flag;
	needCaculateFlag_mask &= ~(MASK_FLAG_TF) ;
}
bool CentreProcessorUnit::getSF_Flag()
{
	if( !(needCaculateFlag_mask & MASK_FLAG_SF) )
		return this->SF_Flag;
	needCaculateFlag_mask &= ~(MASK_FLAG_SF) ;
	switch(SF_lazyTable.bitLength)
	{
	case 8:
		return SF_Flag = !!(SF_lazyTable.result & MASK_10000000);
	case 16:
		return SF_Flag = !!(SF_lazyTable.result & 0x8000);
	default:
		this->buildLog(Hardware::ErrorType::UnHandle,"when comput SF flags , found the bitLength out of handle ");
		this->showMessage("when comput SF flags , found the bitLength out of handle ");
		assert(false);
	}

}
bool CentreProcessorUnit::getZF_Flag()
{
	if( !(needCaculateFlag_mask & MASK_FLAG_ZF) )
		return this->ZF_Flag;
	needCaculateFlag_mask &= ~(MASK_FLAG_ZF) ;
	return ZF_Flag = !ZF_lazyTable.result;
}
bool CentreProcessorUnit::getAF_Flag()
{
	if( !(needCaculateFlag_mask & MASK_FLAG_AF) )
		return this->AF_Flag;
	needCaculateFlag_mask &= ~(MASK_FLAG_AF) ;
	bool operand1_thirdBit, operand2_thirdBit, result_thirdBit;
	operand1_thirdBit = !!(AF_lazyTable.operand1 & MASK_00001000);
	operand2_thirdBit = !!(AF_lazyTable.operand2 & MASK_00001000);
	result_thirdBit   = !!(AF_lazyTable.result   & MASK_00001000);
	switch(AF_lazyTable.opMethod)
	{
	case ADD_8bit:
	case ADD_16bit:
	case ADC_8bit:
	case ADC_16bit:
	case SUB_8bit:
	case SUB_16bit:
	case SBB_8bit:
	case SBB_16bit:
	case CMP_8bit:
	case CMP_16bit:
		return AF_Flag = ((AF_lazyTable.operand1 ^ AF_lazyTable.operand2) ^ AF_lazyTable.result) & 0x10;
	default:
		this->buildLog(Hardware::ErrorType::UnHandle, "when comput AF flags , found the bitLength out of handle");
		this->showMessage("when comput AF flags , found the bitLength out of handle");
		assert(false);
	}
}
bool CentreProcessorUnit::getPF_Flag()
{
	if( !(needCaculateFlag_mask & MASK_FLAG_PF) )
		return this->PF_Flag;
	needCaculateFlag_mask &= ~(MASK_FLAG_PF) ;
	int oneNum = 0;
	U1 result_8bit = (U1)(PF_lazyTable.result);

	while(result_8bit)
	{
		result_8bit &= (result_8bit-1); 
		oneNum++;

	}
	return PF_Flag=(0==oneNum%2);//�������еĵ�8λ��ż����1��PFΪtrue�� ������1��PFΪfalse
}
bool CentreProcessorUnit::getCF_Flag()
{
	if( !(needCaculateFlag_mask & MASK_FLAG_CF) )
		return this->CF_Flag;
	needCaculateFlag_mask &= ~(MASK_FLAG_CF) ;
	switch(CF_lazyTable.opMethod)
	{//����������
	case ADD_8bit:
	case ADD_16bit:
		if( CF_lazyTable.result < CF_lazyTable.operand1 || CF_lazyTable.result < CF_lazyTable.operand2)
			return CF_Flag = true;
		else
			return CF_Flag = false;
		break;
	case ADC_8bit://������ADCʱ�򣬶��Ǳ�ʾCFΪ�棬���CFΪ�٣�ADC����ִ�еĽ���ADD������SBBͬ��~~
	case ADC_16bit:
		return CF_Flag =(CF_lazyTable.result <= CF_lazyTable.operand1);
		break;
	case SUB_8bit:
	case SUB_16bit:
	case CMP_8bit:
	case CMP_16bit:
		return CF_Flag = (CF_lazyTable.operand1 < CF_lazyTable.operand2);
		break;
	case SBB_8bit:
		return CF_Flag = (CF_lazyTable.operand1 < CF_lazyTable.result) || (0xff == CF_lazyTable.operand2);
		break;
	case SBB_16bit:
		return CF_Flag = (CF_lazyTable.operand1 < CF_lazyTable.result) || (0xffff == CF_lazyTable.operand2); 
		break;
	default:
		this->buildLog(Hardware::ErrorType::UnHandle, "when comput CF flags , found the bitLength out of handle");
		this->showMessage("when comput CF flags , found the bitLength out of handle");
		assert(false);
	}
}



void CentreProcessorUnit::setOF_Flag(const bool f)
{
	this->OF_Flag = f;
	this->needCaculateFlag_mask &= ~MASK_FLAG_OF;
}
void CentreProcessorUnit::setDF_Flag(const bool f)
{
	this->DF_Flag = f;
	this->needCaculateFlag_mask &= ~MASK_FLAG_DF;
}
void CentreProcessorUnit::setIF_Flag(const bool f)
{
	this->IF_Flag = f;
	this->needCaculateFlag_mask &= ~MASK_FLAG_IF;
}
void CentreProcessorUnit::setTF_Flag(const bool f)
{
	this->TF_Flag = f;
	this->needCaculateFlag_mask &= ~MASK_FLAG_TF;
}


void CentreProcessorUnit::setSF_Flag(const bool f)
{
	this->SF_Flag = f;
	this->needCaculateFlag_mask &= ~MASK_FLAG_SF;
}
void CentreProcessorUnit::setSF_Flag(const U1 data)
{
	 SF_Flag = !!(data & MASK_10000000);
	this->needCaculateFlag_mask &= ~MASK_FLAG_SF;
}
void CentreProcessorUnit::setSF_Flag(const U2 data)
{
	 SF_Flag = !!(data & 0x8000);
	this->needCaculateFlag_mask &= ~MASK_FLAG_SF;
}


void CentreProcessorUnit::setZF_Flag(const bool f)
{
	this->ZF_Flag = f;
	this->needCaculateFlag_mask &= ~MASK_FLAG_ZF;
}
void CentreProcessorUnit::setZF_Flag(const U1 data)
{
	this->ZF_Flag = !data;
	this->needCaculateFlag_mask &= ~MASK_FLAG_ZF;
}
void CentreProcessorUnit::setZF_Flag(const U2 data)
{
	this->ZF_Flag = !data;
	this->needCaculateFlag_mask &= ~MASK_FLAG_ZF;
}



void CentreProcessorUnit::setAF_Flag(const bool f)
{
	this->AF_Flag = f;
	this->needCaculateFlag_mask &= ~MASK_FLAG_AF;
}
void CentreProcessorUnit::setPF_Flag(const bool f)
{
	this->PF_Flag = f;
	this->needCaculateFlag_mask &= ~MASK_FLAG_PF;
}
void CentreProcessorUnit::setPF_Flag(const U1 data)
{
	int oneNum = 0;
	U1 result_8bit = (U1)(data);

	while(result_8bit)
	{
		result_8bit &= (result_8bit-1); 
		oneNum++;

	}
	PF_Flag=(0==oneNum%2);//�������еĵ�8λ��ż����1��PFΪtrue�� ������1��PFΪfalse
	this->needCaculateFlag_mask &= ~MASK_FLAG_PF;
}
void CentreProcessorUnit::setPF_Flag(const U2 data)
{
	setPF_Flag(U1(data));
}


void CentreProcessorUnit::setCF_Flag(const bool f)
{
	this->CF_Flag = f;
	this->needCaculateFlag_mask &= ~MASK_FLAG_CF;
}

void CentreProcessorUnit::setSF_ZF_PF_byResult(const U1 result)
{
	this->setZF_Flag(result);	
	this->setSF_Flag(result);
	this->setPF_Flag(result);

}
void CentreProcessorUnit::setSF_ZF_PF_byResult(const U2 result)
{
	this->setZF_Flag(result);	
	this->setSF_Flag(result);
	this->setPF_Flag(result);
}

U2 CentreProcessorUnit::generateFLAG()
{
	//U2 flags=0x7000 + 2;//����bochs�������У�flags�ĵڶ�λ����1�����Լ���2
	U2 flags = flag_extern&0xf000;
	flags|=0x2;//�ڶ�λҪΪ1
	flags&=0xffd7;//����λҪΪ0,����λΪ0
	if(this->getOF_Flag())
		flags |= MASK_FLAG_OF;
	if(this->getDF_Flag())
		flags |= MASK_FLAG_DF;
	if(this->getIF_Flag())
		flags |= MASK_FLAG_IF;
	if(this->getTF_Flag())
		flags |= MASK_FLAG_TF;
	if(this->getSF_Flag())
		flags |= MASK_FLAG_SF;
	if(this->getZF_Flag())
		flags |= MASK_FLAG_ZF;
	if(this->getAF_Flag())
		flags |= MASK_FLAG_AF;
	if(this->getPF_Flag())
		flags |= MASK_FLAG_PF;
	if(this->getCF_Flag())
		flags |= MASK_FLAG_CF;
	
	return flags;

}
void CentreProcessorUnit::setFLAG(const U2 flags)
{
	this->setOF_Flag(!!(flags&MASK_FLAG_OF));
	this->setDF_Flag(!!(flags&MASK_FLAG_DF));
	this->setIF_Flag(!!(flags&MASK_FLAG_IF));
	this->setTF_Flag(!!(flags&MASK_FLAG_TF));
	this->setSF_Flag(!!(flags&MASK_FLAG_SF));
	this->setZF_Flag(!!(flags&MASK_FLAG_ZF));
	this->setAF_Flag(!!(flags&MASK_FLAG_AF));
	this->setPF_Flag(!!(flags&MASK_FLAG_PF));
	this->setCF_Flag(!!(flags&MASK_FLAG_CF));
	this->needCaculateFlag_mask = 0;//ȥ������Ҫ�ٴμ���
	flag_extern = flags;
}
void CentreProcessorUnit::set_SF_ZF_AF_PF_CF_Flag(const U2 flags)
{
	
	this->setSF_Flag(!!(flags&MASK_FLAG_SF));
	this->setZF_Flag(!!(flags&MASK_FLAG_ZF));
	this->setAF_Flag(!!(flags&MASK_FLAG_AF));
	this->setPF_Flag(!!(flags&MASK_FLAG_PF));
	this->setCF_Flag(!!(flags&MASK_FLAG_CF));

	this->needCaculateFlag_mask &= ~ (MASK_FLAG_SF|MASK_FLAG_ZF|MASK_FLAG_AF|MASK_FLAG_PF|MASK_FLAG_CF);
}