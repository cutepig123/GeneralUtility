//#include "stdafx.h"
#include "CentreProcessorUnit.h"
#include"../MotherBoard.h"
#include "OutFile.h"
#include<iostream>
using namespace std;
DeCodeBlock::DeCodeBlock(CentreProcessorUnit* pCPU)
{
	this->hardwareName = "DeCodeBlock";
	this->pCPU=pCPU;
	this->pexeCodeBlock=&(pCPU->exeCodeBlock);
}
static int count =0;
const unsigned int MIN_COUNT=0xffff*38;
const unsigned int MAX_COUNT=MIN_COUNT+0xffff;
void DeCodeBlock::deCodeInstruction()
{
	
	while(true)
	{
	  /*static OutFile outfile("c:\\LMTxt.txt");
	  static Reg_CPU reg;
	  static bool flag =false;
	  
	  if(((0xA0EF==pCPU->ax && pCPU->ip ==0x5B && pCPU->cs == 0x5469 ) || true == flag) )
	  {
		  count++;
		  flag = true;
		  if(count>=MIN_COUNT && count<=MAX_COUNT)
		  {

			  reg.ax = pCPU->ax;
			  reg.bx = pCPU->bx;
			  reg.cx = pCPU->cx;
			  reg.dx = pCPU->dx;
			  reg.sp = pCPU->sp;
			  reg.bp = pCPU->bp;
			  reg.si = pCPU->si;
			  reg.di = pCPU->di;
			  reg.ip = pCPU->ip;
			  reg.ds = pCPU->ds;
			  reg.cs = pCPU->cs;
			  reg.es = pCPU->es;
			  reg.ss = pCPU->ss;
			  reg.of = pCPU->getOF_Flag();
			  reg.sf = pCPU->getSF_Flag();
			  reg.zf = pCPU->getZF_Flag();
			  reg.cf = pCPU->getCF_Flag();
			  reg.af = pCPU->getAF_Flag();
			  reg.pf = pCPU->getPF_Flag();
			  reg.df = pCPU->getDF_Flag();
			  reg.IF = pCPU->getIF_Flag();
			  reg.TF = pCPU->getTF_Flag();
			  outfile.toFile(reg);
		  }
	  }*/
		refresh();
		setExecuteMethod();//�趨ִ�е�ָ��
		if(0x11e4 == pCPU->ip && 0x488a==pCPU->cs)
		{

			pCPU->ip = pCPU->ip;
				//return;
		}
		deCodeInsStuff();//�������������
		((*(this->pexeCodeBlock)).*(this->pexeCodeBlock->needExeMethod))();
		if(true == pCPU->isNeedHandleKeyPress)
		{
			pCPU->isNeedHandleKeyPress = false;
			this->pexeCodeBlock->method_outsideINT(0x9);//�ǳ������벻ͨ�����ִ��09�жϣ�EDIT��QBASIC��������ʾ�˵����ͣס~~����
		}
	}
}
void DeCodeBlock::init()
{
	this->memStartAddr = pCPU->pMB->getMemStartAddr();
	refresh();
	//����spc��tPC
}

void DeCodeBlock::setExecuteMethod()
{
	pexeCodeBlock->setNeedExeMethod( pexeCodeBlock->exeMethodArray[*pOpCode] );
}
void DeCodeBlock::refresh()
{
	this->pOpCode = pCPU->ip + (pCPU->cs<<4) + memStartAddr;
	this->prefix_SegmentOverrideOpCode = 0;//����prefix��Ӱ�죬��ֹӰ����һ��ָ��
	this->prefix_LockOpCode = 0;
	this->prefix_RepeatOpCode = 0;
	//34 λ��Щprefix��ʱ������

}

void DeCodeBlock::deCodeInsStuff()
{
	U1 tmpOpCode ;
LOOP_Decode:	switch(*pOpCode)
	{
	case MASK_00110111://AAA
	case MASK_00111111://AAS
	case MASK_10011000://CBW
	case MASK_11111000://CLC
	case MASK_11111100://CLD
	case MASK_11111010://CLI
	case MASK_11110101://CMC
	case MASK_10100110://CMPSB
	case MASK_10100111://CMPSW
	case MASK_10011001://CWD
	case MASK_00100111://DAA
	case MASK_00101111://DAS
	case MASK_01001000://DEC AX    <DEC>
	case MASK_01001001://DEC CX
	case MASK_01001010://DEC DX
	case MASK_01001011://DEC BX
	case MASK_01001100://DEC SP
	case MASK_01001101://DEC BP
	case MASK_01001110://DEC SI
	case MASK_01001111://DEC DI    </DEC>
	case MASK_11110100://HLT
	case MASK_11101100:// IN ,w=0
	case MASK_11101101:// IN, w=1
	case MASK_01000000:// INC AX
	case MASK_01000001:// INC CX
	case MASK_01000010:// INC DX
	case MASK_01000011:// INC BX
	case MASK_01000100:// INC SP
	case MASK_01000101:// INC BP
	case MASK_01000110:// INC SI
	case MASK_01000111:// INC DI
	case MASK_11001100:// INT ,v=0 û�к�������
	case MASK_11001110://INTO
	case MASK_11001111://IRET
	case MASK_10011111://LAHF
	case MASK_10101100://LODSB ,w=0
	case MASK_10101101://LODSW, w=1
	case MASK_10100100://MOVSB, w=0
	case MASK_10100101://MOVSW, w=1
	case MASK_10010000://NOP
	case MASK_11101110://OUT port=al, w=0
	case MASK_11101111://OUT port=ax, w=1

	case 0x50://PUSH AX
	case 0x51://PUSH CX
	case 0x52://PUSH DX
	case 0x53://PUSH BX
	case 0x54://PUSH SP
	case 0x55://PUSH BP
	case 0x56://PUSH SI
	case 0x57://PUSH DI
	case 0x58://POP AX
	case 0x59://POP CX
	case 0x5A://POP DX
	case 0x5B://POP BX
	case 0x5C://POP SP
	case 0x5D://POP BP
	case 0x5E://POP SI
	case 0x5F://POP DI
	case MASK_00000111://POP ES, û��POP CS��
	case MASK_00010111://POP SS
	case MASK_00011111://POP DS
	case MASK_00000110://PUSH ES
	case MASK_00001110://PUSH CS
	case MASK_00010110://PUSH SS
	case MASK_00011110://PUSH DS

	//case MASK_11110010://REP ,z=0
	//case MASK_11110011://REP ,z=1
	case MASK_10011101://POPF
	case MASK_10011100://PUSHF
	case MASK_11000011://RET
	case MASK_11001011://RET
	case MASK_10011110://SAHF
	case MASK_10101110://SCASB , w=0
	case MASK_10101111://SCASW,  w=1
	case MASK_11111001://STC 
	case MASK_11111101://STD
	case MASK_11111011://STI
	case MASK_10101010://STOSB , w=0
	case MASK_10101011://STOSW , w=1
	case MASK_10011011://WAIT
	//case MASK_10010000://XCHG AX,AX  ������
	case MASK_10010001://XCHG CX
	case MASK_10010010://XCHG DX
	case MASK_10010011://XCHG BX
	case MASK_10010100://XCHG SP
	case MASK_10010101://XCHG BP
	case MASK_10010110://XCHG SI
	case MASK_10010111://XCHG DI
	case MASK_11010111://XLAT
		this->deOpCode_singleDefine();
		break;
	case MASK_11010101://AAD
	case MASK_11010100://AAM
	case MASK_11011000://ESC
	case MASK_11011001:
	case MASK_11011010:
	case MASK_11011011:
	case MASK_11011100:
	case MASK_11011101:
	case MASK_11011110:
	case MASK_11011111:
		this->deOpCode_doubleDefine();
		break;
	case MASK_00010000://ADC
	case MASK_00010001:
	case MASK_00010010:
	case MASK_00010011:
	case MASK_00000000://ADD
	case MASK_00000001:
	case MASK_00000010:
	case MASK_00000011:
	case MASK_00100000://AND
	case MASK_00100001:
	case MASK_00100010:
	case MASK_00100011:
	case MASK_00111000://CMP
	case MASK_00111001:
	case MASK_00111010:
	case MASK_00111011:
	case MASK_11000101://LDS
	case MASK_10001101://LEA
	case MASK_00001000://OR
	case MASK_00001001:
	case MASK_00001010:
	case MASK_00001011:
	case MASK_10001111://POP Ev
	case MASK_00011000://SBB
	case MASK_00011001:
	case MASK_00011010:
	case MASK_00011011:
	case MASK_00101000://SUB
	case MASK_00101001:
	case MASK_00101010:
	case MASK_00101011:
	case MASK_10000100://TEST,��Ϊ�������û�ã�����ֻ������
	case MASK_10000101:
	case MASK_10000110://XCHG,ͬ��
	case MASK_10000111:
	case MASK_00110000://XOR
	case MASK_00110001:
	case MASK_00110010:
	case MASK_00110011:
		this->deOpCode_modRegRm();
		break;
	case MASK_11000100://LES
		tmpOpCode = *pOpCode;
		*pOpCode = MASK_11000101;//����LES��opcode�����wλ��0�����Ա��뻻��1���ĳ�LDS
		this->deOpCode_modRegRm();
		*pOpCode = tmpOpCode;
		break;
	case MASK_00010100://ADC
	case MASK_00010101:
	case MASK_00000100://ADD
	case MASK_00000101:
	case MASK_00100100://AND
	case MASK_00100101:
	case MASK_00111100://CMP
	case MASK_00111101:
	case MASK_00001100://OR
	case MASK_00001101:
	case MASK_00011100://SBB
	case MASK_00011101:
	case MASK_00101100://SUB
	case MASK_00101101:
	case MASK_10101000://TEST
	case MASK_10101001:
	case MASK_00110100://XOR
	case MASK_00110101:
		this->deOpCode_IMM();
	    break;

	case MASK_10000000://Immdiate group
	case MASK_10000001:
	case MASK_10000010:
	case MASK_10000011:
		this->deOpCode_IMMGroup();
		break;
	
	case MASK_11110110://group3
	case MASK_11110111:
		if(0 == (*(pOpCode +1)&MASK_00111000) )//TEST Ib/Iv
			this->deOpCode_TEST_Ib_Iv();
		else
			this->deOpCode_ModRM();
		break;
	case MASK_11010000://group 2
	case MASK_11010001:
	case MASK_11010010:
	case MASK_11010011:
	case MASK_11111110://group4
	case MASK_11111111:
		this->deOpCode_ModRM();
		break;
	case MASK_11100100://IN port w=0
	case MASK_11100101://IN port w=1
	case MASK_11001101://INT type v=1
	case MASK_11101011://JMP ����ֱ��ת�ƶ�
	case 0x70://JO
	case 0x71://JNO
	case 0x72://JB/JNAE/JC
	case 0x73://JNB/JAE/JNC
	case 0x74://JZ
	case 0x75://JNZ
	case 0x76://JBE
	case 0x77://JNBE
	case 0x78://JS
	case 0x79://JNS
	case 0x7A://JP
	case 0x7B://JNP
	case 0x7C://JL
	case 0x7D://JNL
	case 0x7E://JLE
	case 0x7F://JNLE
	case 0xe3://JCXZ
	case MASK_11100001://LOOPE/lOOPZ
	case MASK_11100000://LOOPNE/LOOPNZ
	case MASK_11100010://LOOP
	case MASK_11100110://OUT port w=0
	case MASK_11100111://OUT port w=1
		this->deOpCode_contactData_8bit();
		break;
	case MASK_11101000://CALL---����ֱ�ӵ���
	case MASK_11101001://JMP--����ֱ��ת��
	case MASK_11000010://RET--���������Ķ��ڷ���
	case MASK_11001010://RET--���������Ķμ䷵��
		this->deOpCode_contactData_16bit();
		break;
	case MASK_10011010://CALL--�μ�֮�����
	case MASK_11101010://JMP--�μ�ֱ��ת��
		this->deOpCode_contactData_32bit();
		break;
	case MASK_11000110://MOV--�������� �洢��/�Ĵ���
	case MASK_11000111:
		this->deOpCode_Mov_imm_rm();
		break;
	case MASK_10001110://MOV--�洢�����Ĵ���
	case MASK_10001100:
		this->deOpCode_Mov_rm_seg();
		break;
	case MASK_10001000://MOV--
	case MASK_10001001:
	case MASK_10001010:
	case MASK_10001011:
		this->deOpCode_modRegRm();
		break;
	case MASK_10100000://MOV-- �ۼ������洢��
	case MASK_10100001:
	case MASK_10100010:
	case MASK_10100011:
		this->deOpCode_Mov_mem_AX();
		break;
	case MASK_10110000://MOV--���������Ĵ���
	case MASK_10110001:
	case MASK_10110010:
	case MASK_10110011:
	case MASK_10110100:
	case MASK_10110101:
	case MASK_10110110:
	case MASK_10110111:
	case MASK_10111000:
	case MASK_10111001:
	case MASK_10111010:
	case MASK_10111011:
	case MASK_10111100:
	case MASK_10111101:
	case MASK_10111110:
	case MASK_10111111:
		this->deOpCode_Mov_imm_reg();
		break;


	case 0xf0:
	case 0xf2:
	case 0xf3:
	case 0x2e:
	case 0x36:
	case 0x3e:
	case 0x26:
		((*(this->pexeCodeBlock)).*(this->pexeCodeBlock->needExeMethod))();
		pCPU->ip++;
		pOpCode++;
		setExecuteMethod();
		goto LOOP_Decode;
		break;
	
	default:
		char _str[5];
		sprintf(_str,"%X",*pOpCode);
		this->buildLog(Hardware::ErrorType::UnHandle, "deCodeInsStuff Unhandle a OpCode " + string(_str));
		this->showMessage("deCodeInsStuff Unhandle a OpCode " + string(_str));
		assert(false);
		break;
	



	}
}




void DeCodeBlock::needJIT(const U1 modRegRm, const UnKnown pdispData)
{/*
	U2* point_effectiveMem_first, *point_effectiveMem_second;//����JIT�����ַ�ļĴ���ָ��
	U2* pSegment;//JIT ��Ҫ�õĶμĴ���ָ��
	U4 dispData; //������disp8 ��disp16�� ����JIT�����ַ���ݴ�ƫ����

	switch(modRegRm & MASK_11000000)
	{
	case MASK_00000000:
		dispData = 0;
		break;
	case MASK_01000000:
		dispData = *(U1*)pdispData;
		break;
	case MASK_10000000:
		dispData = *(U2*)pdispData;
		break;
	default: //MOD=11 
		return ; //�����ڴ��ַ���ǼĴ�����������
	}
	switch(modRegRm & MASK_00000111)
	{
	case 0://RM=000
		point_effectiveMem_first = &pCPU->bx;
		point_effectiveMem_second = &pCPU->si;
		pSegment = &pCPU->ds;
		break;
	case 1://RM=001
		point_effectiveMem_first = &pCPU->bx;
		point_effectiveMem_second = &pCPU->di;
		pSegment = &pCPU->ds;
		break;
	case 2://RM=010
		point_effectiveMem_first = &pCPU->bp;
		point_effectiveMem_second = &pCPU->si;
		pSegment = &pCPU->ss;
		break;
	case 3://RM=011
		point_effectiveMem_first = &pCPU->bp;
		point_effectiveMem_second = &pCPU->di;
		pSegment = &pCPU->ss;
		break;
	case 4://RM=100
		point_effectiveMem_first = &pCPU->si;
		point_effectiveMem_second = (U2*)&pCPU->Zero; //����Zero��0,����������ûʲôӰ��,���Ի���U2*
		pSegment = &pCPU->ds;
		break;
	case 5://RM=101
		point_effectiveMem_first = &pCPU->di;
		point_effectiveMem_second = (U2*)&pCPU->Zero;
		pSegment = &pCPU->ds;
		break;
	case 6://RM=110
		if(MASK_00000000 == (modRegRm & MASK_11000000))//MOD=00
		{
			point_effectiveMem_first = (U2*)&pCPU->Zero;
			pSegment = &pCPU->ds;
			dispData = *(U2*)pdispData;
		}
		else//MOD=01,10
		{
			point_effectiveMem_first = &pCPU->bp;
			pSegment = &pCPU->ss;
		}
		point_effectiveMem_second = (U2*)&pCPU->Zero;
		break;
	case 7://RM=111
		point_effectiveMem_first = &pCPU->bx;
		point_effectiveMem_second = (U2*)&pCPU->Zero;
		pSegment = &pCPU->ds;
		break;
	default://˭֪���أ�����������������
		assert(false);
		return ;
	}

	switch( this->prefixOpCode )
	{ //���prefix�ǶԶδ洢���ĸ��ǣ������Ĭ�϶μĴ���
	case 0x2E:// CS segment override prefix
		pSegment = &pCPU->cs;
		break;
	case 0x36://SS segment override prefix
		pSegment = &pCPU->ss;
		break;
	case 0x3E://DS segment override prefix
		pSegment = &pCPU->ds;
		break;
	case 0x26://ES segment override prefix
		pSegment = &pCPU->es;
		break;
	case 0x64://FS segment override prefix
		pSegment = &pCPU->fs;
		break;
	case 0x65://GS segment override prefix
		pSegment = &pCPU->gs;
		break;
	default:
		break;
	}
	this->pexeCodeBlock->setJIT_stuff(point_effectiveMem_first, point_effectiveMem_second, pSegment, dispData);
	*/
}

UnKnown DeCodeBlock::deCodeEffectiveMemAddr(const U1 modRegRm, const UnKnown pdispData, int* pdispLength)
{
	*pdispLength = 0;
	U4 memAddr = 0;
	U1 mod = (modRegRm & MASK_11000000)>>6;
	U2 tmpDS = getDefaultSegment_DS();
	U2 tmpSS = getDefaultSegment_SS();
	switch(modRegRm & MASK_00000111)
	{
	case 0://RM=000
		memAddr =   pCPU->bx + pCPU->si + ((segData = tmpDS) <<4 );
		break;
	case 1://RM=001
		memAddr =   pCPU->bx + pCPU->di + ((segData = tmpDS) <<4 );
		break;
	case 2://RM=010
		memAddr =   pCPU->bp + pCPU->si + ((segData = tmpSS) <<4);
		break;
	case 3://RM=011
		memAddr =   pCPU->bp + pCPU->di + ((segData = tmpSS) <<4);
		break;
	case 4://RM=100
		memAddr =   pCPU->si + ((segData = tmpDS) <<4);
		break;
	case 5://RM=101
		memAddr =   pCPU->di + ((segData = tmpDS) <<4);
		break;
	case 6://RM=110
		if( 0 == mod ) //MOD=00
		{
			memAddr = *(U2*)pdispData + ((segData = tmpDS) <<4);
			*pdispLength=2;
		}
		else
			memAddr = pCPU->bp + ((segData = tmpSS) <<4);
		break;
	case 7://RM=111
		memAddr =   pCPU->bx + ((segData = tmpDS) <<4);
		break;
	default://˭֪���أ�����������������
		this->buildLog(Hardware::ErrorType::UnHandle, "deCodeEffectiveMemAddr UnHandle the rm bits");
		this->showMessage("deCodeEffectiveMemAddr UnHandle the rm bits");
		assert(false);
		return 0;
	}
	

	switch(mod)
	{
	case 0://MOD=00
		break;
	case 1://MOD=01
		memAddr += *(S1*)pdispData;
		*pdispLength = 1;
		break;
	case 2://MOD=10
		memAddr += *(U2*)pdispData;
		*pdispLength = 2;
		break;
	case 3://MOD=11  ȡ�õ���reg�ĵ�ַ��ֱ�ӷ��ؼ���
		return (UnKnown)getRegAddr(this->isWidth, (modRegRm & MASK_00000111) );
	default:
		this->buildLog(Hardware::ErrorType::UnHandle, "deCodeEffectiveMemAddr UnHandle the mod bits");
		this->showMessage("deCodeEffectiveMemAddr UnHandle the mod bits");
		assert(false);
		return 0;
	}
	return (UnKnown)(memStartAddr + memAddr);

}
UnKnown DeCodeBlock::getRegAddr(const bool w, const U1 reg) const
{// ��ʾ�Ƿ�����
	if(false==w)
	{
		switch( reg)
		{
		case 0://REG=000
			return (UnKnown)(&pCPU->al);
		case 1://REG=001
			return (UnKnown)(&pCPU->cl);
		case 2://REG=010
			return (UnKnown)(&pCPU->dl);
		case 3://REG=011
			return (UnKnown)(&pCPU->bl);
		case 4://REG=100
			return (UnKnown)(&pCPU->ah);
		case 5://REG=101
			return (UnKnown)(&pCPU->ch);
		case 6://REG=110
			return (UnKnown)(&pCPU->dh);
		case 7://REG=111
			return (UnKnown)(&pCPU->bh);
		default://˭֪���ء�����
			return NULL;
		}

	}
	else
	{
		switch( reg)
		{
		case 0://REG=000
			return (UnKnown)(&pCPU->ax);
		case 1://REG=001
			return (UnKnown)(&pCPU->cx);
		case 2://REG=010
			return (UnKnown)(&pCPU->dx);
		case 3://REG=011
			return (UnKnown)(&pCPU->bx);
		case 4://REG=100
			return (UnKnown)(&pCPU->sp);
		case 5://REG=101
			return (UnKnown)(&pCPU->bp);
		case 6://REG=110
			return (UnKnown)(&pCPU->si);
		case 7://REG=111
			return (UnKnown)(&pCPU->di);
		default://˭֪���ء�����
			return NULL;
		}

	}
}


U2* DeCodeBlock::getSegAddr(const U1 sreg) const
{
	switch( sreg )
		{//2-Bit sreg2 Field
		case 0://REG=00
			return (U2*)(&pCPU->es);
		case 1://REG=01
			return (U2*)(&pCPU->cs);
		case 2://REG=10
			return (U2*)(&pCPU->ss);
		case 3://REG=11
			return (U2*)(&pCPU->ds);
		default://˭֪���ء�����
			return NULL;
		}
}




void DeCodeBlock::deOpCode_singleDefine()
{
	this->pCPU->ip += sizeof(U1);
}
void DeCodeBlock::deOpCode_doubleDefine()
{
	this->pCPU->ip += sizeof(U2);
}

int DeCodeBlock::deOpCode_modRegRm()
{
	int insDispLength = 0;
	int insLength = OPCODE_WIDTH + MOD_REG_RM_LENGTH;
	U1* pModRegRm = pOpCode+OPCODE_WIDTH;
	U1* pDisp = pModRegRm + MOD_REG_RM_LENGTH;
	this->isWidth = !!(*pOpCode & MASK_00000001);
	prm = deCodeEffectiveMemAddr(*pModRegRm,(UnKnown)pDisp, &insDispLength );
	preg = getRegAddr(isWidth, ((*pModRegRm)&MASK_00111000)>>3 );
	insLength +=  insDispLength;
	this->pCPU->ip +=  insLength ;//����cpu�е�ip������ָ����һ��ָ
	return insLength;
}
void DeCodeBlock::deOpCode_IMM()
{
	int insLength = OPCODE_WIDTH ;
	U1* pimmData = this->pOpCode + OPCODE_WIDTH;
	bool w_width = !!((*pOpCode)&MASK_00000001);
	if(w_width)
	{
		this->immData_16Bit = *(U2*)(pimmData);
		insLength += 2;
	}
	else
	{
		this->immData_8Bit = *(pimmData);
		insLength++;
	}
	this->pCPU->ip += insLength ;//����cpu�е�ip������ָ����һ��ָ��

}
void DeCodeBlock::deOpCode_TEST_Ib_Iv()
{
	int insLength = this->deOpCode_modRegRm();//cpu.ip�Ѿ��仯
	//��deOpCode_IMMGroup�� ���ǲ����sλ
	switch( (*pOpCode)&MASK_00000001 )
	{
	case MASK_00000001:
		this->immData_16Bit =  *(U2*)(pOpCode + insLength);
		this->pCPU->ip += 2;
		break;
	default:
		this->immData_8Bit =  *(pOpCode + insLength);
		this->pCPU->ip++;
	}
	this->nnn = (*(pOpCode + OPCODE_WIDTH) & MASK_00111000)>>3 ; //�ٰ󶨣���ִ�е�ʱ����ȷ�����׸�ִ��ʲô����
	
	return ;
}
void DeCodeBlock::deOpCode_IMMGroup()
{//δ���Ʒ

	int insLength = this->deOpCode_modRegRm();//cpu.ip�Ѿ��仯
	
	switch( (*pOpCode)&MASK_00000011 )
	{//sλ�Ĵ���
	case MASK_00000001:
		this->immData_16Bit =  *(U2*)(pOpCode + insLength);
		this->pCPU->ip += 2;
		break;
	case MASK_00000011:
		this->immData_16Bit =  (S2)(*(S1*)(pOpCode + insLength));
		this->pCPU->ip += 1;
		break;
	default:
		this->immData_8Bit =  *(pOpCode + insLength);
		this->pCPU->ip++;
	}
	this->nnn = (*(pOpCode + OPCODE_WIDTH) & MASK_00111000)>>3 ; //�ٰ󶨣���ִ�е�ʱ����ȷ�����׸�ִ��ʲô����
	
	return ;

}
void DeCodeBlock::deOpCode_contactData_8bit()
{
	int insLength = OPCODE_WIDTH + sizeof(U1);
	this->opContactData_8bit = *(U1*)(pOpCode + OPCODE_WIDTH);
	this->pCPU->ip += insLength;
	return;
}
void DeCodeBlock::deOpCode_contactData_16bit()
{
	int insLength = OPCODE_WIDTH + sizeof(U2);//�Ѿ�ȷ��ָ���
	this->opContactData_16bit = *(U2*)(pOpCode + OPCODE_WIDTH);
	this->pCPU->ip += insLength ;//����cpu�е�ip������ָ����һ��ָ��
	return ;
}

void DeCodeBlock::deOpCode_contactData_32bit()
{
	int insLength = OPCODE_WIDTH + sizeof(U4);//�Ѿ�ȷ��ָ���
	this->opContactData_16bit = *(U2*)(pOpCode + OPCODE_WIDTH);
	this->opContactData_16bitE = *(U2*)(pOpCode + OPCODE_WIDTH +sizeof(U2));
	this->pCPU->ip += insLength ;//����cpu�е�ip������ָ����һ��ָ��
	return;
}

void DeCodeBlock::deOpCode_Mov_mem_AX()
{
	int insLength = OPCODE_WIDTH + sizeof(U2);//�Ѿ�ȷ��ָ���
	int nothing=0;
	this->isWidth = !!(*pOpCode & MASK_00000001);
	this->prm = deCodeEffectiveMemAddr(MASK_00000110, pOpCode+OPCODE_WIDTH, &nothing );
	this->pCPU->ip += insLength ;//����cpu�е�ip������ָ����һ��ָ��
	return ;

}

void DeCodeBlock::deOpCode_Mov_imm_rm()
{

	int insLength = this->deOpCode_modRegRm();//cpu.ip�Ѿ��仯
	
	if(isWidth)
	{
		this->immData_16Bit =  *(U2*)(pOpCode + insLength);
		this->pCPU->ip += 2;
	}
	else
	{
		this->immData_8Bit =  *(pOpCode + insLength);
		this->pCPU->ip++;
	}
	return ;
	
}

void DeCodeBlock::deOpCode_Mov_rm_seg()
{
	int insDispLength = 0;
	int insLength = OPCODE_WIDTH + MOD_REG_RM_LENGTH;
	U1* pModRegRm = pOpCode+OPCODE_WIDTH;
	U1* pDisp = pModRegRm + MOD_REG_RM_LENGTH;
	this->isWidth = true; //һ��Ҫ����
	prm = deCodeEffectiveMemAddr(*pModRegRm,(UnKnown)pDisp, &insDispLength );
	pseg = getSegAddr( ((*pModRegRm)&MASK_00011000)>>3 ); //segment...ֻ��Ҫ����bit�Ϳ���ȷ��																//�������deOpCodeModRegRm������һ
	insLength += insDispLength;
	this->pCPU->ip += insLength ;//����cpu�е�ip������ָ����һ��ָ��
}

void DeCodeBlock::deOpCode_Mov_imm_reg()
{ //reg ���ĸ�����ִ��ʱ��ȷ��
	int insLength = OPCODE_WIDTH + sizeof(U1); //��������ô��
	this->isWidth = !!((*pOpCode) & MASK_00001000);
	if(isWidth)
	{
		this->immData_16Bit = *(U2*)(pOpCode + OPCODE_WIDTH);
		insLength++;
	}
	else
	{
		this->immData_8Bit = *(U1*)(pOpCode + OPCODE_WIDTH);
	}
	this->pCPU->ip += insLength ;//����cpu�е�ip������ָ����һ��ָ��
}





void DeCodeBlock::deOpCode_ModRM()
{
	int insLength = this->deOpCode_modRegRm();//cpu.ip�Ѿ��仯
	this->nnn = (*(pOpCode + OPCODE_WIDTH) & MASK_00111000)>>3 ; //�ٰ󶨣���ִ�е�ʱ����ȷ�����׸�ִ��ʲô����
	return ;
}
U2 DeCodeBlock::getDefaultSegment_DS()
{
	switch(this->prefix_SegmentOverrideOpCode)
	{
	case CS_SEGMENT_OVERRIDE_PREFIX:
		return pCPU->cs;
	case SS_SEGMENT_OVERRIDE_PREFIX:
		return pCPU->ss;
	case ES_SEGMENT_OVERRIDE_PREFIX:
		return pCPU->es;
	default:
		return pCPU->ds;
	}
}

U2 DeCodeBlock::getDefaultSegment_SS()
{
	switch(this->prefix_SegmentOverrideOpCode)
	{
	case CS_SEGMENT_OVERRIDE_PREFIX:
		return pCPU->cs;
	case DS_SEGMENT_OVERRIDE_PREFIX:
		return pCPU->ds;
	case ES_SEGMENT_OVERRIDE_PREFIX:
		return pCPU->es;
	default:
		return pCPU->ss;
	}
}