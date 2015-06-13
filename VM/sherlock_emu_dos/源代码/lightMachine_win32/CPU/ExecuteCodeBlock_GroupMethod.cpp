//#include "stdafx.h"
#include "CentreProcessorUnit.h"

#define rotl(x,n) ((((x)<<(n))&(-(1<<(n))))|(((x)>>(sizeof(x)*8-(n)))&((1<<(n))-1)))

#define ror(x) rotl((x),1)
//循环右移的宏
bool getSignByData(const U1 data)
{
	return !!(data & MASK_10000000);
}
bool getSignByData(const U2 data)
{
	return !!(data & 0x8000);
}
bool getFinalBit(const U1 data)
{
	return !!(data & MASK_00000001);
}
bool getFinalBit(const U2 data)
{
	return !!(data & 1);
}
bool isSignBitChange(const U1 fir, const U1 sec)
{
	if(getSignByData(fir) == getSignByData(sec))
		return false;
	return true;
}
bool isSignBitChange(const U2 fir, const U2 sec)
{
	if(getSignByData(fir) == getSignByData(sec))
		return false;
	return true;
}
////////////////////////////////////////////<GROUP1>
void ExecuteCodeBlock::methodGroup_ADD_Eb_Ib()//tested
{
	this->atomMethod_ADD_8bit(*(U1*)(pdeCodeBlock->prm), pdeCodeBlock->immData_8Bit);
}
void ExecuteCodeBlock::methodGroup_ADD_Ev_Iv()//tested
{
	this->atomMethod_ADD_16bit(*(U2*)(pdeCodeBlock->prm), pdeCodeBlock->immData_16Bit);
}
void ExecuteCodeBlock::methodGroup_ADD_Ev_Ib()//tested
{
	U2 tmp = pdeCodeBlock->immData_8Bit;
	this->atomMethod_ADD_16bit(*(U2*)(pdeCodeBlock->prm), tmp);
}

void ExecuteCodeBlock::methodGroup_OR_Eb_Ib()
{
	this->atomMethod_OR_8bit(*(U1*)(pdeCodeBlock->prm), pdeCodeBlock->immData_8Bit);
}
void ExecuteCodeBlock::methodGroup_OR_Ev_Iv()
{
	this->atomMethod_OR_16bit(*(U2*)(pdeCodeBlock->prm), pdeCodeBlock->immData_16Bit);
}
void ExecuteCodeBlock::methodGroup_OR_Ev_Ib()
{
	//assert(false);//may be some problem
	U2 tmp = pdeCodeBlock->immData_8Bit;
	this->atomMethod_OR_16bit(*(U2*)(pdeCodeBlock->prm), tmp);
}

void ExecuteCodeBlock::methodGroup_ADC_Eb_Ib()//tested
{
	this->atomMethod_ADC_8bit(*(U1*)(pdeCodeBlock->prm), pdeCodeBlock->immData_8Bit);
}
void ExecuteCodeBlock::methodGroup_ADC_Ev_Iv()//tested
{
	this->atomMethod_ADC_16bit(*(U2*)(pdeCodeBlock->prm), pdeCodeBlock->immData_16Bit);
}
void ExecuteCodeBlock::methodGroup_ADC_Ev_Ib()//tested
{
	//assert(false);//may be some problem
	U2 tmp = pdeCodeBlock->immData_8Bit;
	this->atomMethod_ADC_16bit(*(U2*)(pdeCodeBlock->prm), tmp);
}

void ExecuteCodeBlock::methodGroup_SBB_Eb_Ib()
{
	this->atomMethod_SBB_8bit(*(U1*)(pdeCodeBlock->prm), pdeCodeBlock->immData_8Bit);
}
void ExecuteCodeBlock::methodGroup_SBB_Ev_Iv()
{
	this->atomMethod_SBB_16bit(*(U2*)(pdeCodeBlock->prm), pdeCodeBlock->immData_16Bit);
}
void ExecuteCodeBlock::methodGroup_SBB_Ev_Ib()
{
	//assert(false);//may be some problem
	U2 tmp = pdeCodeBlock->immData_8Bit;
	this->atomMethod_SBB_16bit(*(U2*)(pdeCodeBlock->prm), tmp);
}

void ExecuteCodeBlock::methodGroup_AND_Eb_Ib()
{
	this->atomMethod_AND_8bit(*(U1*)(pdeCodeBlock->prm), pdeCodeBlock->immData_8Bit);
}
void ExecuteCodeBlock::methodGroup_AND_Ev_Iv()
{
	this->atomMethod_AND_16bit(*(U2*)(pdeCodeBlock->prm), pdeCodeBlock->immData_16Bit);
}
void ExecuteCodeBlock::methodGroup_AND_Ev_Ib()
{
	//assert(false);//may be some problem
	U2 tmp = pdeCodeBlock->immData_8Bit;
	this->atomMethod_AND_16bit(*(U2*)(pdeCodeBlock->prm), tmp);
}

void ExecuteCodeBlock::methodGroup_SUB_Eb_Ib()//tested
{
	this->atomMethod_SUB_8bit(*(U1*)(pdeCodeBlock->prm), pdeCodeBlock->immData_8Bit);
}
void ExecuteCodeBlock::methodGroup_SUB_Ev_Iv()//tested
{
	this->atomMethod_SUB_16bit(*(U2*)(pdeCodeBlock->prm), pdeCodeBlock->immData_16Bit);
}
void ExecuteCodeBlock::methodGroup_SUB_Ev_Ib()//tested
{
	//assert(false);//may be some problem
	U2 tmp = pdeCodeBlock->immData_8Bit;
	this->atomMethod_SUB_16bit(*(U2*)(pdeCodeBlock->prm), tmp);
}

void ExecuteCodeBlock::methodGroup_XOR_Eb_Ib()
{
	this->atomMethod_XOR_8bit(*(U1*)(pdeCodeBlock->prm), pdeCodeBlock->immData_8Bit);
}
void ExecuteCodeBlock::methodGroup_XOR_Ev_Iv()
{
	this->atomMethod_XOR_16bit(*(U2*)(pdeCodeBlock->prm), pdeCodeBlock->immData_16Bit);
}
void ExecuteCodeBlock::methodGroup_XOR_Ev_Ib()
{
	//assert(false);//may be some problem
	U2 tmp = pdeCodeBlock->immData_8Bit;
	this->atomMethod_XOR_16bit(*(U2*)(pdeCodeBlock->prm), tmp);
}

void ExecuteCodeBlock::methodGroup_CMP_Eb_Ib()
{
	this->atomMethod_CMP_8bit(*(U1*)(pdeCodeBlock->prm), *(S1*)(&pdeCodeBlock->immData_8Bit));
}
void ExecuteCodeBlock::methodGroup_CMP_Ev_Iv()
{
	this->atomMethod_CMP_16bit(*(U2*)(pdeCodeBlock->prm), *(S2*)(&pdeCodeBlock->immData_16Bit));
}
void ExecuteCodeBlock::methodGroup_CMP_Ev_Ib()
{
	//assert(false);//may be some problem
	this->atomMethod_CMP_16bit(*(U2*)(pdeCodeBlock->prm),  (S1)pdeCodeBlock->immData_8Bit);
}
////////////////////////////////////////////</GROUP1>

////////////////////////////////////////////<GROUP2>

void ExecuteCodeBlock::methodGroup_ROL_Eb_Ib(){}
void ExecuteCodeBlock::methodGroup_ROL_Ev_Ib(){}
void ExecuteCodeBlock::methodGroup_ROL_Eb_1()
{
	U1  oldEb  = *(U1*)(pdeCodeBlock->prm);
	bool old_sign = getSignByData( oldEb );
	pCPU->setCF_Flag( old_sign );
	(*(U1*)(pdeCodeBlock->prm)) = ((*(U1*)(pdeCodeBlock->prm))<<1) | ((*(U1*)(pdeCodeBlock->prm))>>7) ;

	pCPU->setOF_Flag(isSignBitChange(oldEb, *(U1*)(pdeCodeBlock->prm)));

	
}
void ExecuteCodeBlock::methodGroup_ROL_Ev_1()
{
	U2  oldEv  = *(U2*)(pdeCodeBlock->prm);
	bool old_sign = getSignByData( oldEv );
	pCPU->setCF_Flag( old_sign );
	(*(U2*)(pdeCodeBlock->prm)) = ((*(U2*)(pdeCodeBlock->prm))<<1) | ((*(U2*)(pdeCodeBlock->prm))>>15) ;

	pCPU->setOF_Flag(isSignBitChange(oldEv, *(U2*)(pdeCodeBlock->prm)));

	
}
void ExecuteCodeBlock::methodGroup_ROL_Eb_CL()
{
	if(0 == pCPU->cl)
		return;
	bool oldOF = pCPU->getOF_Flag();
	U1 tmpCL = pCPU->cl;
	while(tmpCL)
	{
		methodGroup_ROL_Eb_1();
		tmpCL--;
	}
	pCPU->setOF_Flag(oldOF);

	//pCPU->setSF_ZF_PF_byResult(*(U1*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_ROL_Ev_CL()
{
	if(0 == pCPU->cl)
		return;
	bool oldOF = pCPU->getOF_Flag();
	U1 tmpCL = pCPU->cl;
	while(tmpCL)
	{
		methodGroup_ROL_Ev_1();
		tmpCL--;
	}
	pCPU->setOF_Flag(oldOF);

	//pCPU->setSF_ZF_PF_byResult(*(U2*)(pdeCodeBlock->prm));
}

void ExecuteCodeBlock::methodGroup_ROR_Eb_Ib(){}
void ExecuteCodeBlock::methodGroup_ROR_Ev_Ib(){}
void ExecuteCodeBlock::methodGroup_ROR_Eb_1()
{
	U1  oldEb  = *(U1*)(pdeCodeBlock->prm);
	bool finalBit = getFinalBit( oldEb );
	pCPU->setCF_Flag( finalBit );
	(*(U1*)(pdeCodeBlock->prm)) = ((*(U1*)(pdeCodeBlock->prm))>>1) | ((*(U1*)(pdeCodeBlock->prm))<<7) ;

	pCPU->setOF_Flag(isSignBitChange(oldEb, *(U1*)(pdeCodeBlock->prm)));

	//pCPU->setSF_ZF_PF_byResult(*(U1*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_ROR_Ev_1()
{
	U2  oldEv  = *(U2*)(pdeCodeBlock->prm);
	bool finalBit = getFinalBit( oldEv );
	pCPU->setCF_Flag( finalBit );
	(*(U2*)(pdeCodeBlock->prm)) = ((*(U2*)(pdeCodeBlock->prm))>>1) | ((*(U2*)(pdeCodeBlock->prm))<<15) ;

	pCPU->setOF_Flag(isSignBitChange(oldEv, *(U2*)(pdeCodeBlock->prm)));

	//pCPU->setSF_ZF_PF_byResult(*(U2*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_ROR_Eb_CL()
{
	if(0 == pCPU->cl)
		return;
	bool oldOF = pCPU->getOF_Flag();
	U1 tmpCL = pCPU->cl;
	while(tmpCL)
	{
		methodGroup_ROR_Eb_1();
		tmpCL--;
	}
	pCPU->setOF_Flag(oldOF);
	//pCPU->setSF_ZF_PF_byResult(*(U1*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_ROR_Ev_CL()
{
	if(0 == pCPU->cl)
		return;
	bool oldOF = pCPU->getOF_Flag();
	U1 tmpCL = pCPU->cl;
	while(tmpCL)
	{
		methodGroup_ROR_Ev_1();
		tmpCL--;
	}
	pCPU->setOF_Flag(oldOF);
	//pCPU->setSF_ZF_PF_byResult(*(U2*)(pdeCodeBlock->prm));
}

void ExecuteCodeBlock::methodGroup_RCL_Eb_Ib(){}
void ExecuteCodeBlock::methodGroup_RCL_Ev_Ib(){}
void ExecuteCodeBlock::methodGroup_RCL_Eb_1()
{
	U2 tmpFlags = pCPU->generateFLAG();
	methodGroup_SHL_Eb_1();
	bool newCF = pCPU->getCF_Flag();
	bool newOF = pCPU->getOF_Flag();
	pCPU->setFLAG(tmpFlags);
	if(pCPU->getCF_Flag())
	{
		*(U1*)(pdeCodeBlock->prm) |= MASK_00000001;
	}
	pCPU->setCF_Flag(newCF);
	pCPU->setOF_Flag(newOF);
	//pCPU->setSF_ZF_PF_byResult(*(U1*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_RCL_Ev_1()
{
	U2 tmpFlags = pCPU->generateFLAG();
	methodGroup_SHL_Ev_1();
	bool newCF = pCPU->getCF_Flag();
	bool newOF = pCPU->getOF_Flag();
	pCPU->setFLAG(tmpFlags);
	if(pCPU->getCF_Flag())
	{
		*(U2*)(pdeCodeBlock->prm) |= 1;
	}
	pCPU->setCF_Flag(newCF);
	pCPU->setOF_Flag(newOF);
	//pCPU->setSF_ZF_PF_byResult(*(U2*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_RCL_Eb_CL()
{
	if(0 == pCPU->cl)
		return;
	bool oldOF = pCPU->getOF_Flag();
	U1 tmpCL = pCPU->cl;
	while(tmpCL)
	{
		methodGroup_RCL_Eb_1();
		tmpCL--;
	}
	pCPU->setOF_Flag(oldOF);
	//pCPU->setSF_ZF_PF_byResult(*(U1*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_RCL_Ev_CL()
{
	if(0 == pCPU->cl)
		return;
	bool oldOF = pCPU->getOF_Flag();
	U1 tmpCL = pCPU->cl;
	while(tmpCL)
	{
		methodGroup_RCL_Ev_1();
		tmpCL--;
	}
	pCPU->setOF_Flag(oldOF);
	//pCPU->setSF_ZF_PF_byResult(*(U2*)(pdeCodeBlock->prm));
}

void ExecuteCodeBlock::methodGroup_RCR_Eb_Ib(){}
void ExecuteCodeBlock::methodGroup_RCR_Ev_Ib(){}
void ExecuteCodeBlock::methodGroup_RCR_Eb_1()
{
	U1 oldEb = *(U1*)(pdeCodeBlock->prm);
	bool oldCF = pCPU->getCF_Flag();
	U2 tmpFlags = pCPU->generateFLAG();
	methodGroup_SHR_Eb_1();
	bool newCF = pCPU->getCF_Flag();
	pCPU->setFLAG(tmpFlags);//因为methodGroup_SHR_Eb_1函数会改变Flags值，而实际上RCR是不影响那几位的。。
	if(oldCF)
	{
		*(U1*)(pdeCodeBlock->prm) |= MASK_10000000;
	}
	pCPU->setOF_Flag(isSignBitChange(oldEb, *(U1*)(pdeCodeBlock->prm)));
	pCPU->setCF_Flag(newCF);
	//pCPU->setSF_ZF_PF_byResult(*(U1*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_RCR_Ev_1()
{
	U2 oldEb = *(U2*)(pdeCodeBlock->prm);
	U2 tmpFlags = pCPU->generateFLAG();
	bool oldCF = pCPU->getCF_Flag();
	methodGroup_SHR_Ev_1();
	bool newCF = pCPU->getCF_Flag();
	pCPU->setFLAG(tmpFlags);
	if(oldCF)
	{
		*(U2*)(pdeCodeBlock->prm) |= 0x8000;
	}
	pCPU->setOF_Flag(isSignBitChange(oldEb, *(U2*)(pdeCodeBlock->prm)));
	pCPU->setCF_Flag(newCF);
	//pCPU->setSF_ZF_PF_byResult(*(U2*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_RCR_Eb_CL()
{
	if(0 == pCPU->cl)
		return;
	bool oldOF = pCPU->getOF_Flag();
	U1 tmpCL = pCPU->cl;
	while(tmpCL)
	{
		methodGroup_RCR_Eb_1();
		tmpCL--;
	}
	pCPU->setOF_Flag(oldOF);
	//pCPU->setSF_ZF_PF_byResult(*(U1*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_RCR_Ev_CL()
{
	if(0 == pCPU->cl)
		return;
	bool oldOF = pCPU->getOF_Flag();
	U1 tmpCL = pCPU->cl;
	while(tmpCL)
	{
		methodGroup_RCR_Ev_1();
		tmpCL--;
	}
	pCPU->setOF_Flag(oldOF);
	//pCPU->setSF_ZF_PF_byResult(*(U2*)(pdeCodeBlock->prm));
}

void ExecuteCodeBlock::methodGroup_SHL_Eb_Ib(){}
void ExecuteCodeBlock::methodGroup_SHL_Ev_Ib(){}
void ExecuteCodeBlock::methodGroup_SHL_Eb_1()
{	
	U1  oldEb  = *(U1*)(pdeCodeBlock->prm);
	bool old_sign = getSignByData( oldEb );
	pCPU->setCF_Flag( old_sign );
	(*(U1*)(pdeCodeBlock->prm)) <<= 1;

	pCPU->setOF_Flag(isSignBitChange(oldEb, *(U1*)(pdeCodeBlock->prm)));

	pCPU->setSF_ZF_PF_byResult(*(U1*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_SHL_Ev_1()
{
	U2  oldEv  = *(U2*)(pdeCodeBlock->prm);
	bool old_sign = getSignByData( oldEv );
	pCPU->setCF_Flag( old_sign );
	(*(U2*)(pdeCodeBlock->prm)) <<= 1;

	pCPU->setOF_Flag(isSignBitChange(oldEv, *(U2*)(pdeCodeBlock->prm)));

	pCPU->setSF_ZF_PF_byResult(*(U2*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_SHL_Eb_CL()
{
	if(0 == pCPU->cl)
		return;
	bool sign = false;
	U1 tmpCL = pCPU->cl;
	while(tmpCL)
	{
		sign = getSignByData( *(U1*)(pdeCodeBlock->prm) );
		(*(U1*)(pdeCodeBlock->prm)) <<= 1; //低速实现，以后修改
		tmpCL--;
	}
	pCPU->setCF_Flag(sign);
	pCPU->setSF_ZF_PF_byResult(*(U1*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_SHL_Ev_CL()
{
	if(0 == pCPU->cl)
		return;
	bool sign = false;
	U1 tmpCL = pCPU->cl;
	while(tmpCL)
	{
		sign = getSignByData( *(U2*)(pdeCodeBlock->prm) );
		(*(U2*)(pdeCodeBlock->prm)) <<= 1; //低速实现，以后修改
		tmpCL--;
	}
	pCPU->setCF_Flag(sign);
	pCPU->setSF_ZF_PF_byResult(*(U2*)(pdeCodeBlock->prm));
}

void ExecuteCodeBlock::methodGroup_SHR_Eb_Ib(){}
void ExecuteCodeBlock::methodGroup_SHR_Ev_Ib(){}
void ExecuteCodeBlock::methodGroup_SHR_Eb_1()
{
	U1 oldEb = *(U1*)(pdeCodeBlock->prm);
	bool finalBit = getFinalBit( oldEb );
	*(U1*)(pdeCodeBlock->prm) >>= 1;
	pCPU->setCF_Flag(finalBit);
	pCPU->setOF_Flag(isSignBitChange(oldEb, *(U1*)(pdeCodeBlock->prm)));
	pCPU->setSF_ZF_PF_byResult(*(U1*)(pdeCodeBlock->prm));

}
void ExecuteCodeBlock::methodGroup_SHR_Ev_1()
{
	U2 oldEv = *(U2*)(pdeCodeBlock->prm);
	bool finalBit = getFinalBit( oldEv );
	*(U2*)(pdeCodeBlock->prm) >>= 1;
	pCPU->setCF_Flag(finalBit);
	pCPU->setOF_Flag(isSignBitChange(oldEv, *(U2*)(pdeCodeBlock->prm)));
	pCPU->setSF_ZF_PF_byResult(*(U2*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_SHR_Eb_CL()
{
	if(0 == pCPU->cl)
		return;
	bool finalBit = false;
	U1 tmpCL = pCPU->cl;
	while(tmpCL)
	{
		finalBit = getFinalBit(*(U1*)(pdeCodeBlock->prm));
		*(U1*)(pdeCodeBlock->prm) >>= 1;
		tmpCL--;
	}
	pCPU->setCF_Flag(finalBit);
	pCPU->setSF_ZF_PF_byResult(*(U1*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_SHR_Ev_CL()
{
	if(0 == pCPU->cl)
		return;
	bool finalBit = false;
	U1 tmpCL = pCPU->cl;
	while(tmpCL)
	{
		finalBit = getFinalBit(*(U2*)(pdeCodeBlock->prm));
		*(U2*)(pdeCodeBlock->prm) >>= 1;
		tmpCL--;
	}
	pCPU->setCF_Flag(finalBit);
	pCPU->setSF_ZF_PF_byResult(*(U2*)(pdeCodeBlock->prm));
}

void ExecuteCodeBlock::methodGroup_SAR_Eb_Ib(){}
void ExecuteCodeBlock::methodGroup_SAR_Ev_Ib(){}
void ExecuteCodeBlock::methodGroup_SAR_Eb_1()
{
	S1 oldEb = *(S1*)(pdeCodeBlock->prm);
	bool finalBit = getFinalBit( (U1)oldEb );
	*(S1*)(pdeCodeBlock->prm) >>= 1;
	pCPU->setCF_Flag(finalBit);
	pCPU->setOF_Flag(false);//算术右移，部队符号位改变
	pCPU->setSF_ZF_PF_byResult(*(U1*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_SAR_Ev_1()
{
	S2 oldEv = *(S2*)(pdeCodeBlock->prm);
	bool finalBit = getFinalBit( (U1)oldEv );
	*(S2*)(pdeCodeBlock->prm) >>= 1;
	pCPU->setCF_Flag(finalBit);
	pCPU->setOF_Flag(isSignBitChange(oldEv, *(U2*)(pdeCodeBlock->prm)));
	pCPU->setSF_ZF_PF_byResult(*(U2*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_SAR_Eb_CL()
{
	if(0 == pCPU->cl)
		return;
	bool finalBit = false;
	U1 tmpCL = pCPU->cl;
	while(tmpCL)
	{
		finalBit = getFinalBit(*(U1*)(pdeCodeBlock->prm));
		*(S1*)(pdeCodeBlock->prm) >>= 1;
		tmpCL--;
	}
	pCPU->setCF_Flag(finalBit);
	pCPU->setSF_ZF_PF_byResult(*(U1*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_SAR_Ev_CL()
{
	if(0 == pCPU->cl)
		return;
	bool finalBit = false;
	U1 tmpCL = pCPU->cl;
	while(tmpCL)
	{
		finalBit = getFinalBit(*(U2*)(pdeCodeBlock->prm));
		*(S2*)(pdeCodeBlock->prm) >>= 1;
		tmpCL--;
	}
	pCPU->setCF_Flag(finalBit);
	pCPU->setSF_ZF_PF_byResult(*(U2*)(pdeCodeBlock->prm));
}

///////////////////////////////////////</Group2>


////////////////////////////////////<Group3>

void ExecuteCodeBlock::methodGroup_TEST_Ib()
{
	U1 tmp = *(U1*)(pdeCodeBlock->prm);
	this->atomMethod_AND_8bit(tmp, pdeCodeBlock->immData_8Bit);
}
void ExecuteCodeBlock::methodGroup_TEST_Iv()
{
	U2 tmp = *(U2*)(pdeCodeBlock->prm);
	this->atomMethod_AND_16bit(tmp, pdeCodeBlock->immData_16Bit);
}
void ExecuteCodeBlock::methodGroup_NOT_8bit()
{
	*(U1*)(pdeCodeBlock->prm) = ~*(U1*)(pdeCodeBlock->prm);
}
void ExecuteCodeBlock::methodGroup_NOT_16bit()
{
	*(U2*)(pdeCodeBlock->prm) = ~*(U2*)(pdeCodeBlock->prm);
}
void ExecuteCodeBlock::methodGroup_NEG_8bit()
{
	//methodGroup_NOT_8bit();//先求反
	//this->atomMethod_ADD_8bit(*(U1*)(pdeCodeBlock->prm), 1);//再加上一
	U1 zero = 0;
	this->atomMethod_SUB_8bit(zero, *(U1*)(pdeCodeBlock->prm));
	*(U1*)(pdeCodeBlock->prm) = zero;
}
void ExecuteCodeBlock::methodGroup_NEG_16bit()
{
	//methodGroup_NOT_16bit();//先求反
	//this->atomMethod_ADD_16bit(*(U2*)(pdeCodeBlock->prm), 1);//再加上一
	U2 zero = 0;
	this->atomMethod_SUB_16bit(zero, *(U2*)(pdeCodeBlock->prm));
	*(U2*)(pdeCodeBlock->prm) = zero;
}
void ExecuteCodeBlock::methodGroup_MUL_AL()
{
	pCPU->ax = *(U1*)(pdeCodeBlock->prm) * pCPU->al;

	pCPU->setCF_Flag(!!(pCPU->ah));
	pCPU->setOF_Flag(!!(pCPU->ah));

}
void ExecuteCodeBlock::methodGroup_MUL_eAX()
{
	U4 result = *(U2*)(pdeCodeBlock->prm) * pCPU->ax;
	pCPU->dx = U2(result >> 16);
	pCPU->ax = U2(result & 0x0000ffff);

	pCPU->setCF_Flag(!!(pCPU->dx));
	pCPU->setOF_Flag(!!(pCPU->dx));
}
void ExecuteCodeBlock::methodGroup_IMUL_AL()
{
	pCPU->ax = *(S1*)(pdeCodeBlock->prm) * (S1)(pCPU->al);
	if(pCPU->ax == pCPU->al)
	{
		pCPU->setCF_Flag(false);
		pCPU->setOF_Flag(false);
	}
	else
	{
		pCPU->setCF_Flag(true);
		pCPU->setOF_Flag(true);
	}
}
void ExecuteCodeBlock::methodGroup_IMUL_eAX()
{
	S4 result = *(S2*)(pdeCodeBlock->prm) * (S2)(pCPU->ax);
	pCPU->dx = U2(result >> 16);
	pCPU->ax = U2(result & 0x0000ffff);
	if(result == (S4)(pCPU->ax))
	{
		pCPU->setCF_Flag(false);
		pCPU->setOF_Flag(false);
	}
	else
	{
		pCPU->setCF_Flag(true);
		pCPU->setOF_Flag(true);
	}
}
void ExecuteCodeBlock::methodGroup_DIV_AL()
{
	if(0 == (*(U1*)(pdeCodeBlock->prm)) )
	{//如果除数是零，产生零号中断
		this->atomMethod_INT(0);
		return ;
	}
	U2 tmpAX_unsigned = pCPU->ax;
	pCPU->al = tmpAX_unsigned / (*(U1*)(pdeCodeBlock->prm)); //商
	pCPU->ah = tmpAX_unsigned % (*(U1*)(pdeCodeBlock->prm));//余数
}
void ExecuteCodeBlock::methodGroup_DIV_eAX()
{
	if(0 == (*(U2*)(pdeCodeBlock->prm)) )
	{//如果除数是零，产生零号中断
		this->atomMethod_INT(0);
		return ;
	}

	U4 data = (((U4)(pCPU->dx))<<16) + pCPU->ax;
	pCPU->ax = data / (*(U2*)(pdeCodeBlock->prm)); //商
	pCPU->dx = data % (*(U2*)(pdeCodeBlock->prm));//余数

}
void ExecuteCodeBlock::methodGroup_IDIV_AL()
{
	if(0 == (*(S1*)(pdeCodeBlock->prm)) )
	{//如果除数是零，产生零号中断
		this->atomMethod_INT(0);
		return ;
	}
	S2 tmpAX_signed = pCPU->ax;
	pCPU->al = tmpAX_signed / (*(S1*)(pdeCodeBlock->prm)); //商
	pCPU->ah = tmpAX_signed % (*(S1*)(pdeCodeBlock->prm));//余数
}
void ExecuteCodeBlock::methodGroup_IDIV_eAX()
{
	if(0 == (*(S2*)(pdeCodeBlock->prm)) )
	{//如果除数是零，产生零号中断
		this->atomMethod_INT(0);
		return ;
	}

	S4 data = (((U4)(pCPU->dx))<<16) + pCPU->ax;
	pCPU->ax = data / (*(S2*)(pdeCodeBlock->prm)); //商
	pCPU->dx = data % (*(S2*)(pdeCodeBlock->prm));//余数
}



//////////////////////////////////////</Group3>
//////////////////////////////////////////<Group4--5>
void ExecuteCodeBlock::methodGroup_INC_Eb()
{
	this->atomMethod_INC_8bit(*(U1*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_INC_Ev()
{
	this->atomMethod_INC_16bit(*(U2*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_DEC_Eb()
{
	this->atomMethod_DEC_8bit(*(U1*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_DEC_Ev()
{
	this->atomMethod_DEC_16bit(*(U2*)(pdeCodeBlock->prm));
}
void ExecuteCodeBlock::methodGroup_CALL_Ev()//nnn=010 段内间接调用
{
	this->atomMethod_PUSH(pCPU->ip); // 没有PUSH IP 。。所以用这个来代替
	pCPU->ip = *(U2*)(pdeCodeBlock->prm);//near call absolute
}
void ExecuteCodeBlock::methodGroup_CALL_Ep()//nnn=011 段间间接调用
{
	this->atomMethod_PUSH(pCPU->cs);
	this->atomMethod_PUSH(pCPU->ip); // 没有PUSH IP 。。所以用这个来代替

	this->pCPU->cs = *(((U2*)pdeCodeBlock->prm) + 1);
	this->pCPU->ip = *(U2*)(pdeCodeBlock->prm);
}
void ExecuteCodeBlock::methodGroup_JMP_Ev()//段内间接转移
{
	pCPU->ip = *(U2*)(pdeCodeBlock->prm);
}
void ExecuteCodeBlock::methodGroup_JMP_Ep()//段间间接转移
{
	this->pCPU->cs = *(((U2*)pdeCodeBlock->prm) + 1);
	this->pCPU->ip = *(U2*)(pdeCodeBlock->prm);

}
void ExecuteCodeBlock::methodGroup_PUSH_Ev()
{
	this->atomMethod_PUSH(*(U2*)(pdeCodeBlock->prm));
}
	

//////////////////////////////////////////</Group4--5>