//#include "stdafx.h"
#include "CentreProcessorUnit.h"
#include "../MotherBoard.h"
void ExecuteCodeBlock::atomMethod_MOV_8bit(U4 logicAddr_des, const U1 data_src)
{
	*(U1*)(memStartAddr + logicAddr_des) = data_src;
}
void ExecuteCodeBlock::atomMethod_MOV_16bit(U4 logicAddr_des, const U2 data_src)
{
	*(U2*)(memStartAddr + logicAddr_des) = data_src;
}


void ExecuteCodeBlock::atomMethod_ADD_8bit(U1& des, const U1 src)
{
	U1 operand1 = des;
	des += src;
	pCPU->storeCaculate(ADD_8bit, 8, 
							operand1,  src, des,
							MASK_FLAG_METHOD_ADD);
}
void ExecuteCodeBlock::atomMethod_ADD_16bit(U2& des, const U2 src)
{
	U2 operand1 = des;
	des += src;
	pCPU->storeCaculate(ADD_16bit, 16, 
							operand1,  src, des,
							MASK_FLAG_METHOD_ADD);
}
void ExecuteCodeBlock::atomMethod_INC_8bit(U1& des)
{
	U1 operand1 = des;
	des++;
	pCPU->storeCaculate(ADD_8bit, 8,
							operand1, 1, des,
							MASK_FLAG_METHOD_INC);
}
void ExecuteCodeBlock::atomMethod_INC_16bit(U2& des)
{
	U2 operand1 = des;
	des++;
	pCPU->storeCaculate(ADD_16bit, 16,
							operand1, 1, des,
							MASK_FLAG_METHOD_INC);
}
void ExecuteCodeBlock::atomMethod_ADC_8bit(U1& des, const U1 src)
{
	U1 operand1 = des;
	if(pCPU->getCF_Flag())
	{
		des += src +1;
		pCPU->storeCaculate(ADC_8bit, 8, 
							operand1,  src, des,//�ο�bochs����..�����ADC
							MASK_FLAG_METHOD_ADD);
	}
	else
	{
		des += src;
		pCPU->storeCaculate(ADD_8bit, 8, 
							operand1,  src, des,//���CFΪ��ʱ�����൱��ִ��ADD����
							MASK_FLAG_METHOD_ADD);
	}
}
void ExecuteCodeBlock::atomMethod_ADC_16bit(U2& des, const U2 src)
{
	U2 operand1 = des;
	if(pCPU->getCF_Flag())
	{
		des += src +1;
		pCPU->storeCaculate(ADC_16bit, 16, 
							operand1,  src, des,//�ο�bochs����..�����ADC
							MASK_FLAG_METHOD_ADD);
	}
	else
	{
		des += src;
		pCPU->storeCaculate(ADD_16bit, 16, 
							operand1,  src, des,//���CFΪ��ʱ�����൱��ִ��ADD����
							MASK_FLAG_METHOD_ADD);
	}
}
void ExecuteCodeBlock::atomMethod_SUB_8bit(U1& des, const U1 src)
{
	U1 operand1 = des;
	des -= src;
	pCPU->storeCaculate(SUB_8bit, 8, 
							operand1,  src, des,
							MASK_FLAG_METHOD_SUB);
}
void ExecuteCodeBlock::atomMethod_SUB_16bit(U2& des, const U2 src)
{
	U2 operand1 = des;
	des -= src;
	pCPU->storeCaculate(SUB_16bit, 16, 
							operand1,  src, des,
							MASK_FLAG_METHOD_SUB);
}
void ExecuteCodeBlock::atomMethod_DEC_8bit(U1& des)
{
	U1 operand1 = des;
	des--;
	pCPU->storeCaculate(SUB_8bit, 8,
							operand1, 1, des,
							MASK_FLAG_METHOD_DEC);
}
void ExecuteCodeBlock::atomMethod_DEC_16bit(U2& des)
{
	U2 operand1 = des;
	des--;
	pCPU->storeCaculate(SUB_16bit, 16,
							operand1, 1, des,
							MASK_FLAG_METHOD_DEC);
}
void ExecuteCodeBlock::atomMethod_SBB_8bit(U1& des, const U1 src)
{
	U1 operand1 = des;
	if(pCPU->getCF_Flag())
	{
		des -= src +1;
		pCPU->storeCaculate(SBB_8bit, 8, 
							operand1,  src, des,//�ο�bochs����..�����SBB
							MASK_FLAG_METHOD_ADD);
	}
	else
	{
		des -= src;
		pCPU->storeCaculate(SUB_8bit, 8, 
							operand1,  src, des,//���CFΪ��ʱ�����൱��ִ��SUB����
							MASK_FLAG_METHOD_ADD);
	}
}
void ExecuteCodeBlock::atomMethod_SBB_16bit(U2& des, const U2 src)
{
	U2 operand1 = des;
	if(pCPU->getCF_Flag())
	{
		des -= src +1;
		pCPU->storeCaculate(SBB_16bit, 16, 
							operand1,  src, des,//�ο�bochs����..�����SBB
							MASK_FLAG_METHOD_ADD);
	}
	else
	{
		des -= src;
		pCPU->storeCaculate(SUB_16bit, 16, 
							operand1,  src, des,//���CFΪ��ʱ�����൱��ִ��SUB����
							MASK_FLAG_METHOD_ADD);
	}
}
void ExecuteCodeBlock::atomMethod_OR_8bit(U1& des, const U1 src)
{
	U1 operand1 = des;
	des |= src;
	pCPU->storeCaculate(OR_8bit, 8, 
							operand1,  src, des,
							MASK_FLAG_METHOD_OR);
	pCPU->setOF_Flag(false);
	pCPU->setCF_Flag(false);
	pCPU->setAF_Flag(false);//Intel��˵�ǲ�Ӱ�죬����debug�л�Ӱ��
}
void ExecuteCodeBlock::atomMethod_OR_16bit(U2& des, const U2 src)
{
	U2 operand1 = des;
	des |= src;
	pCPU->storeCaculate(OR_16bit, 16, 
							operand1,  src, des,
							MASK_FLAG_METHOD_OR);
	pCPU->setOF_Flag(false);
	pCPU->setCF_Flag(false);
	pCPU->setAF_Flag(false);//Intel��˵�ǲ�Ӱ�죬����debug�л�Ӱ��
}

void ExecuteCodeBlock::atomMethod_AND_8bit(U1& des, const U1 src)
{
	U1 operand1 = des;
	des &= src;
	pCPU->storeCaculate(AND_8bit, 8, 
							operand1,  src, des,
							MASK_FLAG_METHOD_AND);
	pCPU->setOF_Flag(false);
	pCPU->setCF_Flag(false);
	pCPU->setAF_Flag(false);//Intel��˵�ǲ�Ӱ�죬����debug�л�Ӱ��

}
void ExecuteCodeBlock::atomMethod_AND_16bit(U2& des, const U2 src)
{
	U2 operand1 = des;
	des &= src;
	pCPU->storeCaculate(AND_16bit, 16, 
							operand1,  src, des,
							MASK_FLAG_METHOD_AND);
	pCPU->setOF_Flag(false);
	pCPU->setCF_Flag(false);
	pCPU->setAF_Flag(false);//Intel��˵�ǲ�Ӱ�죬����debug�л�Ӱ��
}

void ExecuteCodeBlock::atomMethod_XOR_8bit(U1& des, const U1 src)
{
	U1 operand1 = des;
	des ^= src;
	pCPU->storeCaculate(XOR_8bit, 8, 
							operand1,  src, des,
							MASK_FLAG_METHOD_XOR);
	pCPU->setOF_Flag(false);
	pCPU->setCF_Flag(false);
	pCPU->setAF_Flag(false);//Intel��˵�ǲ�Ӱ�죬����debug�л�Ӱ��
}
void ExecuteCodeBlock::atomMethod_XOR_16bit(U2& des, const U2 src)
{
	U2 operand1 = des;
	des ^= src;
	pCPU->storeCaculate(XOR_16bit, 16, 
							operand1,  src, des,
							MASK_FLAG_METHOD_XOR);
	pCPU->setOF_Flag(false);
	pCPU->setCF_Flag(false);
	pCPU->setAF_Flag(false);//����debug�ó��ģ���ȻIntel���ĵ�˵��undefine

}
void ExecuteCodeBlock::atomMethod_CMP_8bit(U1 des, const U1 src)
{
	U1 result = des - src;
	pCPU->storeCaculate(CMP_8bit, 8, 
							des,  src, result,
							MASK_FLAG_METHOD_CMP);
}
void ExecuteCodeBlock::atomMethod_CMP_16bit(U2 des, const U2 src)
{
	U2 result = des - src;
	pCPU->storeCaculate(CMP_16bit, 16, 
							des,  src, result,
							MASK_FLAG_METHOD_CMP);
}
void ExecuteCodeBlock::atomMethod_CMPSB()
{
	U1 des, src;
	des = pCPU->pMB->getMemData_byte(pCPU->di + (pCPU->es<<4));
	src = pCPU->pMB->getMemData_byte(pCPU->si + (pdeCodeBlock->getDefaultSegment_DS() <<4));
	atomMethod_CMP_8bit(src, des);
	self_string_affect_DI_SI(dataType_U1);
}
void ExecuteCodeBlock::atomMethod_CMPSW()
{
	U2 des, src;
	des = pCPU->pMB->getMemData_word(pCPU->di + (pCPU->es<<4) );
	src = pCPU->pMB->getMemData_word(pCPU->si + (pdeCodeBlock->getDefaultSegment_DS() <<4));
	atomMethod_CMP_16bit(src, des);
	self_string_affect_DI_SI(dataType_U2);
}
void ExecuteCodeBlock::atomMethod_MOVSB()
{
	U1 data = this->pCPU->pMB->getMemData_byte(pCPU->si + (pdeCodeBlock->getDefaultSegment_DS() <<4));
	this->pCPU->pMB->writeMem_byte(pCPU->di + (pCPU->es<<4), data);
	self_string_affect_DI_SI(dataType_U1); //��������DI�� SI�޸�
}
void ExecuteCodeBlock::atomMethod_MOVSW()
{
	U2 data = this->pCPU->pMB->getMemData_word(pCPU->si + (pdeCodeBlock->getDefaultSegment_DS() <<4));
	this->pCPU->pMB->writeMem_word(pCPU->di + (pCPU->es<<4), data);
	self_string_affect_DI_SI(dataType_U2);//��������DI�� SI�޸�
}

void ExecuteCodeBlock::atomMethod_STOSB()
{
	this->pCPU->pMB->writeMem_byte(pCPU->di + (pCPU->es<<4), pCPU->al);
	self_string_affect_DI(dataType_U1);//��������DI�޸�
}
void ExecuteCodeBlock::atomMethod_STOSW()
{
	this->pCPU->pMB->writeMem_word(pCPU->di + (pCPU->es<<4), pCPU->ax);
	self_string_affect_DI(dataType_U2);//��������DI�޸�
}
void ExecuteCodeBlock::atomMethod_LODSB()
{
	pCPU->al = this->pCPU->pMB->getMemData_byte(pCPU->si + (pdeCodeBlock->getDefaultSegment_DS()<<4));
	self_string_affect_SI(dataType_U1);//�������� SI�޸�
}
void ExecuteCodeBlock::atomMethod_LODSW()
{
	pCPU->ax = this->pCPU->pMB->getMemData_word(pCPU->si + (pdeCodeBlock->getDefaultSegment_DS()<<4));
	self_string_affect_SI(dataType_U2);//�������� SI�޸�
}
void ExecuteCodeBlock::atomMethod_SCASB()
{
	atomMethod_CMP_8bit(pCPU->al, pCPU->pMB->getMemData_byte(pCPU->di + (pCPU->es<<4)));
	self_string_affect_DI(dataType_U1);//��������DI�� SI�޸�
}
void ExecuteCodeBlock::atomMethod_SCASW()
{
	atomMethod_CMP_16bit(pCPU->ax, pCPU->pMB->getMemData_word(pCPU->di + (pCPU->es<<4)));
	self_string_affect_DI(dataType_U2);//��������DI�� SI�޸�
}
void ExecuteCodeBlock::atomMethod_INS() //80x86û�����ָ��
{
	//need finish
}
void ExecuteCodeBlock::atomMethod_OUTS()//80x86û�����ָ��
{
	//need finish
}





void ExecuteCodeBlock::atomMethod_shortJMP()
{
	
		pCPU->ip += (S1)(pdeCodeBlock->opContactData_8bit);
}

void ExecuteCodeBlock::atomMethod_PUSH(const U2 data)
{
	pCPU->sp -= 2;
	this->pCPU->pMB->writeMem_word((pCPU->sp + ((pCPU->ss)<<4)), data);
}

U2   ExecuteCodeBlock::atomMethod_POP()
{
	U2 data = this->pCPU->pMB->getMemData_word((pCPU->sp + ((pCPU->ss)<<4)));
	pCPU->sp += 2;
	return data;
}



void ExecuteCodeBlock::atomMethod_INT(const U1 INT_num)
{
	this->atomMethod_PUSH(pCPU->generateFLAG());//FLAGS��ջ	
	this->atomMethod_PUSH(pCPU->cs);
	this->atomMethod_PUSH(pCPU->ip);
	pCPU->setIF_Flag(false);
	pCPU->setTF_Flag(false); //��ֹӲ���жϵĵ����ж�
	pCPU->ip = this->pCPU->pMB->getMemData_word(INT_num*4);
	pCPU->cs = this->pCPU->pMB->getMemData_word(INT_num*4 + 2);
}



void ExecuteCodeBlock::self_string_affect_DI_SI(const DataTypeLength type)
{//ִ�д�������� di��si���иı�
	unsigned step =0;
	switch(type)
	{
	case dataType_U1:
		step = 1;
		break;
	case dataType_U2:
		step = 2;
		break;
	}
	if(pCPU->getDF_Flag())
	{
		pCPU->si-=step;
		pCPU->di-=step;
	}
	else
	{
		pCPU->si+=step;
		pCPU->di+=step;
	}
}
void ExecuteCodeBlock::self_string_affect_SI(const DataTypeLength type) 
{
	unsigned step =0;
	switch(type)
	{
	case dataType_U1:
		step = 1;
		break;
	case dataType_U2:
		step = 2;
		break;
	}
	if(pCPU->getDF_Flag())
		pCPU->si-=step;
	else
		pCPU->si+=step;
}
void ExecuteCodeBlock::self_string_affect_DI(const DataTypeLength type) 
{
	unsigned step =0;
	switch(type)
	{
	case dataType_U1:
		step = 1;
		break;
	case dataType_U2:
		step = 2;
		break;
	}
	if(pCPU->getDF_Flag())
		pCPU->di-=step;
	else
		pCPU->di+=step;

}