//#include "stdafx.h"
#include"CentreProcessorUnit.h"
#include"../MotherBoard.h"
    

void ExecuteCodeBlock::initExecuteMethod()
{

	this->exeMethodArray[0x00]=&ExecuteCodeBlock::method_ADD_Eb_Gb;
	this->exeMethodArray[0x01]=&ExecuteCodeBlock::method_ADD_Ev_Gv;
	this->exeMethodArray[0x02]=&ExecuteCodeBlock::method_ADD_Gb_Eb;
	this->exeMethodArray[0x03]=&ExecuteCodeBlock::method_ADD_Gv_Ev;
	this->exeMethodArray[0x04]=&ExecuteCodeBlock::method_ADD_AL_Ib;
	this->exeMethodArray[0x05]=&ExecuteCodeBlock::method_ADD_eAX_Iv;

	this->exeMethodArray[0x06]=&ExecuteCodeBlock::method_PUSH_ES;
	this->exeMethodArray[0x07]=&ExecuteCodeBlock::method_POP_ES;


	this->exeMethodArray[0x10]=&ExecuteCodeBlock::method_ADC_Eb_Gb;
	this->exeMethodArray[0x11]=&ExecuteCodeBlock::method_ADC_Ev_Gv;
	this->exeMethodArray[0x12]=&ExecuteCodeBlock::method_ADC_Gb_Eb;
	this->exeMethodArray[0x13]=&ExecuteCodeBlock::method_ADC_Gv_Ev;
	this->exeMethodArray[0x14]=&ExecuteCodeBlock::method_ADC_AL_Ib;
	this->exeMethodArray[0x15]=&ExecuteCodeBlock::method_ADC_eAX_Iv;

	this->exeMethodArray[0x16]=&ExecuteCodeBlock::method_PUSH_SS;
	this->exeMethodArray[0x17]=&ExecuteCodeBlock::method_POP_SS;

	this->exeMethodArray[0x20]=&ExecuteCodeBlock::method_AND_Eb_Gb;
	this->exeMethodArray[0x21]=&ExecuteCodeBlock::method_AND_Ev_Gv;
	this->exeMethodArray[0x22]=&ExecuteCodeBlock::method_AND_Gb_Eb;
	this->exeMethodArray[0x23]=&ExecuteCodeBlock::method_AND_Gv_Ev;
	this->exeMethodArray[0x24]=&ExecuteCodeBlock::method_AND_AL_Ib;
	this->exeMethodArray[0x25]=&ExecuteCodeBlock::method_AND_eAX_Iv;

	this->exeMethodArray[0x26]=&ExecuteCodeBlock::method_SEG_ES;
	this->exeMethodArray[0x27]=&ExecuteCodeBlock::method_DAA;

	this->exeMethodArray[0x30]=&ExecuteCodeBlock::method_XOR_Eb_Gb;
	this->exeMethodArray[0x31]=&ExecuteCodeBlock::method_XOR_Ev_Gv;
	this->exeMethodArray[0x32]=&ExecuteCodeBlock::method_XOR_Gb_Eb;
	this->exeMethodArray[0x33]=&ExecuteCodeBlock::method_XOR_Gv_Ev;
	this->exeMethodArray[0x34]=&ExecuteCodeBlock::method_XOR_AL_Ib;
	this->exeMethodArray[0x35]=&ExecuteCodeBlock::method_XOR_eAX_Iv;

	this->exeMethodArray[0x36]=&ExecuteCodeBlock::method_SEG_SS;
	this->exeMethodArray[0x37]=&ExecuteCodeBlock::method_AAA;

	this->exeMethodArray[0x40]=&ExecuteCodeBlock::method_INC_eAX;
	this->exeMethodArray[0x41]=&ExecuteCodeBlock::method_INC_eCX;
	this->exeMethodArray[0x42]=&ExecuteCodeBlock::method_INC_eDX;
	this->exeMethodArray[0x43]=&ExecuteCodeBlock::method_INC_eBX;
	this->exeMethodArray[0x44]=&ExecuteCodeBlock::method_INC_eSP;
	this->exeMethodArray[0x45]=&ExecuteCodeBlock::method_INC_eBP;
	this->exeMethodArray[0x46]=&ExecuteCodeBlock::method_INC_eSI;
	this->exeMethodArray[0x47]=&ExecuteCodeBlock::method_INC_eDI;

	this->exeMethodArray[0x50]=&ExecuteCodeBlock::method_PUSH_eAX;
	this->exeMethodArray[0x51]=&ExecuteCodeBlock::method_PUSH_eCX;
	this->exeMethodArray[0x52]=&ExecuteCodeBlock::method_PUSH_eDX;
	this->exeMethodArray[0x53]=&ExecuteCodeBlock::method_PUSH_eBX;
	this->exeMethodArray[0x54]=&ExecuteCodeBlock::method_PUSH_eSP;
	this->exeMethodArray[0x55]=&ExecuteCodeBlock::method_PUSH_eBP;
	this->exeMethodArray[0x56]=&ExecuteCodeBlock::method_PUSH_eSI;
	this->exeMethodArray[0x57]=&ExecuteCodeBlock::method_PUSH_eDI;

	this->exeMethodArray[0x60]=&ExecuteCodeBlock::method_PUSHA_PUSHAD;
	this->exeMethodArray[0x61]=&ExecuteCodeBlock::method_POPA_POPAD;
	this->exeMethodArray[0x62]=&ExecuteCodeBlock::method_BOUND_Gv_Ma;
	this->exeMethodArray[0x63]=&ExecuteCodeBlock::method_ARPL_Ew_Gw;
	this->exeMethodArray[0x64]=&ExecuteCodeBlock::method_SEG_FS;
	this->exeMethodArray[0x65]=&ExecuteCodeBlock::method_SEG_GS;
	this->exeMethodArray[0x66]=&ExecuteCodeBlock::method_Operand_Size;
	this->exeMethodArray[0x67]=&ExecuteCodeBlock::method_Address_Size;

	this->exeMethodArray[0x70]=&ExecuteCodeBlock::method_ShortJump_JO;
	this->exeMethodArray[0x71]=&ExecuteCodeBlock::method_ShortJump_JNO;
	this->exeMethodArray[0x72]=&ExecuteCodeBlock::method_ShortJump_JB;
	this->exeMethodArray[0x73]=&ExecuteCodeBlock::method_ShortJump_JNB;
	this->exeMethodArray[0x74]=&ExecuteCodeBlock::method_ShortJump_JZ;
	this->exeMethodArray[0x75]=&ExecuteCodeBlock::method_ShortJump_JNZ;
	this->exeMethodArray[0x76]=&ExecuteCodeBlock::method_ShortJump_JBE;
	this->exeMethodArray[0x77]=&ExecuteCodeBlock::method_ShortJump_JNBE;

	this->exeMethodArray[0x80]=&ExecuteCodeBlock::method_IMMGroup_Eb_Ib;
	this->exeMethodArray[0x81]=&ExecuteCodeBlock::method_IMMGroup_Ev_Iz;
	this->exeMethodArray[0x82]=&ExecuteCodeBlock::method_IMMGroup_Eb_IbX;
	this->exeMethodArray[0x83]=&ExecuteCodeBlock::method_IMMGroup_Ev_Ib;

	this->exeMethodArray[0x84]=&ExecuteCodeBlock::method_TEST_Eb_Gb;
	this->exeMethodArray[0x85]=&ExecuteCodeBlock::method_TEST_Ev_Gv;

	this->exeMethodArray[0x86]=&ExecuteCodeBlock::method_XCHG_Eb_Gb;
	this->exeMethodArray[0x87]=&ExecuteCodeBlock::method_XCHG_Ev_Gv;

	this->exeMethodArray[0x90]=&ExecuteCodeBlock::method_NOP;

	this->exeMethodArray[0x91]=&ExecuteCodeBlock::method_XCHG_eCX;
	this->exeMethodArray[0x92]=&ExecuteCodeBlock::method_XCHG_eDX;
	this->exeMethodArray[0x93]=&ExecuteCodeBlock::method_XCHG_eBX;
	this->exeMethodArray[0x94]=&ExecuteCodeBlock::method_XCHG_eSP;
	this->exeMethodArray[0x95]=&ExecuteCodeBlock::method_XCHG_eBP;
	this->exeMethodArray[0x96]=&ExecuteCodeBlock::method_XCHG_eSI;
	this->exeMethodArray[0x97]=&ExecuteCodeBlock::method_XCHG_eDI;

	this->exeMethodArray[0xA0]=&ExecuteCodeBlock::method_MOV_AL_Ob;
	this->exeMethodArray[0xA1]=&ExecuteCodeBlock::method_MOV_eAX_Ov;
	this->exeMethodArray[0xA2]=&ExecuteCodeBlock::method_MOV_Ob_AL;
	this->exeMethodArray[0xA3]=&ExecuteCodeBlock::method_MOV_Ov_eAX;

	this->exeMethodArray[0xA4]=&ExecuteCodeBlock::method_MOVSB_Xb_Yb;
	this->exeMethodArray[0xA5]=&ExecuteCodeBlock::method_MOVSW_Xv_Yv;
	this->exeMethodArray[0xA6]=&ExecuteCodeBlock::method_CMPSB_Xb_Yb;
	this->exeMethodArray[0xA7]=&ExecuteCodeBlock::method_CMPSW_Xv_Yv;

	this->exeMethodArray[0xB0]=&ExecuteCodeBlock::method_MOV_AL;
	this->exeMethodArray[0xB1]=&ExecuteCodeBlock::method_MOV_CL;
	this->exeMethodArray[0xB2]=&ExecuteCodeBlock::method_MOV_DL;
	this->exeMethodArray[0xB3]=&ExecuteCodeBlock::method_MOV_BL;
	this->exeMethodArray[0xB4]=&ExecuteCodeBlock::method_MOV_AH;
	this->exeMethodArray[0xB5]=&ExecuteCodeBlock::method_MOV_CH;
	this->exeMethodArray[0xB6]=&ExecuteCodeBlock::method_MOV_DH;
	this->exeMethodArray[0xB7]=&ExecuteCodeBlock::method_MOV_BH;

	this->exeMethodArray[0xC0]=&ExecuteCodeBlock::method_ShiftGroup_Eb_Ib;
	this->exeMethodArray[0xC1]=&ExecuteCodeBlock::method_ShiftGroup_Ev_Ib;
	this->exeMethodArray[0xC2]=&ExecuteCodeBlock::method_RET_near_Iw;
	//that would be something
	this->exeMethodArray[0xC3]=&ExecuteCodeBlock::method_RET_near_None;

	this->exeMethodArray[0xC4]=&ExecuteCodeBlock::method_LES_Gv_Mp;
	this->exeMethodArray[0xC5]=&ExecuteCodeBlock::method_LDS_Gv_Mp;

	this->exeMethodArray[0xC6]=&ExecuteCodeBlock::method_MOV_Eb_Ib;
	this->exeMethodArray[0xC7]=&ExecuteCodeBlock::method_MOV_Ev_Iv;

	this->exeMethodArray[0xD0]=&ExecuteCodeBlock::method_ShiftGroup2_Eb_1;
	this->exeMethodArray[0xD1]=&ExecuteCodeBlock::method_ShiftGroup2_Ev_1;
	this->exeMethodArray[0xD2]=&ExecuteCodeBlock::method_ShiftGroup2_Eb_CL;
	this->exeMethodArray[0xD3]=&ExecuteCodeBlock::method_ShiftGroup2_Ev_CL;

	this->exeMethodArray[0xD4]=&ExecuteCodeBlock::method_AMM;
	this->exeMethodArray[0xD5]=&ExecuteCodeBlock::method_AAD;
  //FOUCE ON IT
	this->exeMethodArray[0xD7]=&ExecuteCodeBlock::method_XLAT;

	this->exeMethodArray[0xE0]=&ExecuteCodeBlock::method_LOOPN_Jb;
	this->exeMethodArray[0xE1]=&ExecuteCodeBlock::method_LOOPE_Jb;
	this->exeMethodArray[0xE2]=&ExecuteCodeBlock::method_LOOP_Jb;
	this->exeMethodArray[0xE3]=&ExecuteCodeBlock::method_JCXZ_Jb;

	this->exeMethodArray[0xE4]=&ExecuteCodeBlock::method_IN_AL_Ib;
	this->exeMethodArray[0xE5]=&ExecuteCodeBlock::method_IN_eAX_Ib;
	this->exeMethodArray[0xE6]=&ExecuteCodeBlock::method_OUT_Ib_AL;
	this->exeMethodArray[0xE7]=&ExecuteCodeBlock::method_OUT_Ib_eAX;

	this->exeMethodArray[0xF0]=&ExecuteCodeBlock::method_LOCK;
	//SOME THING...
	this->exeMethodArray[0xF1]=&ExecuteCodeBlock::method_Unknow;
	this->exeMethodArray[0xF2]=&ExecuteCodeBlock::method_REPNE;
	this->exeMethodArray[0xF3]=&ExecuteCodeBlock::method_REP_REPE;
	this->exeMethodArray[0xF4]=&ExecuteCodeBlock::method_HLT;
	this->exeMethodArray[0xF5]=&ExecuteCodeBlock::method_CMC;
	this->exeMethodArray[0xF6]=&ExecuteCodeBlock::method_UnaryGroup_Eb;
	this->exeMethodArray[0xF7]=&ExecuteCodeBlock::method_UnaryGroup_Ev;

	this->exeMethodArray[0x08]=&ExecuteCodeBlock::method_OR_Eb_Gb;
	this->exeMethodArray[0x09]=&ExecuteCodeBlock::method_OR_Ev_Gv;
	this->exeMethodArray[0x0A]=&ExecuteCodeBlock::method_OR_Gb_Eb;
	this->exeMethodArray[0x0B]=&ExecuteCodeBlock::method_OR_Gv_Ev;
	this->exeMethodArray[0x0C]=&ExecuteCodeBlock::method_OR_AL_Ib;
	this->exeMethodArray[0x0D]=&ExecuteCodeBlock::method_OR_eAX_Iv;

	this->exeMethodArray[0x0E]=&ExecuteCodeBlock::method_PUSH_CS;
	this->exeMethodArray[0x0F]=&ExecuteCodeBlock::method_EscapeCode;

	this->exeMethodArray[0x18]=&ExecuteCodeBlock::method_SBB_Eb_Gb;
	this->exeMethodArray[0x19]=&ExecuteCodeBlock::method_SBB_Ev_Gv;
	this->exeMethodArray[0x1A]=&ExecuteCodeBlock::method_SBB_Gb_Eb;
	this->exeMethodArray[0x1B]=&ExecuteCodeBlock::method_SBB_Gv_Ev;
	this->exeMethodArray[0x1C]=&ExecuteCodeBlock::method_SBB_AL_Ib;
	this->exeMethodArray[0x1D]=&ExecuteCodeBlock::method_SBB_eAX_Iv;

	this->exeMethodArray[0x1E]=&ExecuteCodeBlock::method_PUSH_DS;
	this->exeMethodArray[0x1F]=&ExecuteCodeBlock::method_POP_DS;

	this->exeMethodArray[0x28]=&ExecuteCodeBlock::method_SUB_Eb_Gb;
	this->exeMethodArray[0x29]=&ExecuteCodeBlock::method_SUB_Ev_Gv;
	this->exeMethodArray[0x2A]=&ExecuteCodeBlock::method_SUB_Gb_Eb;
	this->exeMethodArray[0x2B]=&ExecuteCodeBlock::method_SUB_Gv_Ev;
	this->exeMethodArray[0x2C]=&ExecuteCodeBlock::method_SUB_AL_Ib;
	this->exeMethodArray[0x2D]=&ExecuteCodeBlock::method_SUB_eAX_Iv;

	this->exeMethodArray[0x2E]=&ExecuteCodeBlock::method_SEG_CS;
	this->exeMethodArray[0x2F]=&ExecuteCodeBlock::method_DAS;

	this->exeMethodArray[0x38]=&ExecuteCodeBlock::method_CMP_Eb_Gb;
	this->exeMethodArray[0x39]=&ExecuteCodeBlock::method_CMP_Ev_Gv;
	this->exeMethodArray[0x3A]=&ExecuteCodeBlock::method_CMP_Gb_Eb;
	this->exeMethodArray[0x3B]=&ExecuteCodeBlock::method_CMP_Gv_Ev;
	this->exeMethodArray[0x3C]=&ExecuteCodeBlock::method_CMP_AL_Ib;
	this->exeMethodArray[0x3D]=&ExecuteCodeBlock::method_CMP_eAX_Iv;

	this->exeMethodArray[0x3E]=&ExecuteCodeBlock::method_SEG_DS;
	this->exeMethodArray[0x3F]=&ExecuteCodeBlock::method_AAS;

	this->exeMethodArray[0x48]=&ExecuteCodeBlock::method_DEC_eAX;
	this->exeMethodArray[0x49]=&ExecuteCodeBlock::method_DEC_eCX;
	this->exeMethodArray[0x4A]=&ExecuteCodeBlock::method_DEC_eDX;
	this->exeMethodArray[0x4B]=&ExecuteCodeBlock::method_DEC_eBX;
	this->exeMethodArray[0x4C]=&ExecuteCodeBlock::method_DEC_eSP;
	this->exeMethodArray[0x4D]=&ExecuteCodeBlock::method_DEC_eBP;
	this->exeMethodArray[0x4E]=&ExecuteCodeBlock::method_DEC_eSI;
	this->exeMethodArray[0x4F]=&ExecuteCodeBlock::method_DEC_eDI;

	this->exeMethodArray[0x58]=&ExecuteCodeBlock::method_POP_eAX;
	this->exeMethodArray[0x59]=&ExecuteCodeBlock::method_POP_eCX;
	this->exeMethodArray[0x5A]=&ExecuteCodeBlock::method_POP_eDX;
	this->exeMethodArray[0x5B]=&ExecuteCodeBlock::method_POP_eBX;
	this->exeMethodArray[0x5C]=&ExecuteCodeBlock::method_POP_eSP;
	this->exeMethodArray[0x5D]=&ExecuteCodeBlock::method_POP_eBP;
	this->exeMethodArray[0x5E]=&ExecuteCodeBlock::method_POP_eSI;
	this->exeMethodArray[0x5F]=&ExecuteCodeBlock::method_POP_eDI;

	this->exeMethodArray[0x68]=&ExecuteCodeBlock::method_PUSH_Iv;
	this->exeMethodArray[0x69]=&ExecuteCodeBlock::method_IMUL_Gv_Ev_Iv;
	this->exeMethodArray[0x6A]=&ExecuteCodeBlock::method_PUSH_Ib;
	this->exeMethodArray[0x6B]=&ExecuteCodeBlock::method_IMUL_Gv_Ev_Ib;
	this->exeMethodArray[0x6C]=&ExecuteCodeBlock::method_INSB_Yb_DX;
	this->exeMethodArray[0x6D]=&ExecuteCodeBlock::method_INSW_Yv_DX;
	this->exeMethodArray[0x6E]=&ExecuteCodeBlock::method_OUTSB_DX_Xb;
	this->exeMethodArray[0x6F]=&ExecuteCodeBlock::method_OUTSW_DX_Xv;

	this->exeMethodArray[0x78]=&ExecuteCodeBlock::method_ShortJumpCondition_JS;
	this->exeMethodArray[0x79]=&ExecuteCodeBlock::method_ShortJumpCondition_JNS;
	this->exeMethodArray[0x7A]=&ExecuteCodeBlock::method_ShortJumpCondition_JP;
	this->exeMethodArray[0x7B]=&ExecuteCodeBlock::method_ShortJumpCondition_JNP;
	this->exeMethodArray[0x7C]=&ExecuteCodeBlock::method_ShortJumpCondition_JL;
	this->exeMethodArray[0x7D]=&ExecuteCodeBlock::method_ShortJumpCondition_JNL;
	this->exeMethodArray[0x7E]=&ExecuteCodeBlock::method_ShortJumpCondition_JLE;
	this->exeMethodArray[0x7F]=&ExecuteCodeBlock::method_ShortJumpCondition_JNLE;

	this->exeMethodArray[0x88]=&ExecuteCodeBlock::method_MOV_Eb_Gb;
	this->exeMethodArray[0x89]=&ExecuteCodeBlock::method_MOV_Ev_Gv;
	this->exeMethodArray[0x8A]=&ExecuteCodeBlock::method_MOV_Gb_Eb;
	this->exeMethodArray[0x8B]=&ExecuteCodeBlock::method_MOV_Gv_Ev;
	this->exeMethodArray[0x8C]=&ExecuteCodeBlock::method_MOV_Ew_Sw;
	this->exeMethodArray[0x8D]=&ExecuteCodeBlock::method_LEA_Gv_M;
	this->exeMethodArray[0x8E]=&ExecuteCodeBlock::method_MOV_Sw_Ew;
	this->exeMethodArray[0x8F]=&ExecuteCodeBlock::method_POP_Ev;

	this->exeMethodArray[0x98]=&ExecuteCodeBlock::method_CBW;
	this->exeMethodArray[0x99]=&ExecuteCodeBlock::method_CWD;
	this->exeMethodArray[0x9A]=&ExecuteCodeBlock::method_CALL_aP;
	this->exeMethodArray[0x9B]=&ExecuteCodeBlock::method_WAIT;
	this->exeMethodArray[0x9C]=&ExecuteCodeBlock::method_PUSHF_Fv;
	this->exeMethodArray[0x9D]=&ExecuteCodeBlock::method_POP_Fv;
	this->exeMethodArray[0x9E]=&ExecuteCodeBlock::method_SAHF;
	this->exeMethodArray[0x9F]=&ExecuteCodeBlock::method_LAHF;

	this->exeMethodArray[0xA8]=&ExecuteCodeBlock::method_TEST_AL_Ib;
	this->exeMethodArray[0xA9]=&ExecuteCodeBlock::method_TEST_eAX_Iv;
	this->exeMethodArray[0xAA]=&ExecuteCodeBlock::method_STOSB_Yb_AL;
	this->exeMethodArray[0xAB]=&ExecuteCodeBlock::method_STOSW_Yv_eAX;
	this->exeMethodArray[0xAC]=&ExecuteCodeBlock::method_LODSB_AL_Xb;
	this->exeMethodArray[0xAD]=&ExecuteCodeBlock::method_LODSW_eAX_Xv;
	this->exeMethodArray[0xAE]=&ExecuteCodeBlock::method_SCASB_AL_Yb;
	this->exeMethodArray[0xAF]=&ExecuteCodeBlock::method_SCASW_eAX_Yv;

	this->exeMethodArray[0xB8]=&ExecuteCodeBlock::method_MOV_eAX;
	this->exeMethodArray[0xB9]=&ExecuteCodeBlock::method_MOV_eCX;
	this->exeMethodArray[0xBA]=&ExecuteCodeBlock::method_MOV_eDX;
	this->exeMethodArray[0xBB]=&ExecuteCodeBlock::method_MOV_eBX;
	this->exeMethodArray[0xBC]=&ExecuteCodeBlock::method_MOV_eSP;
	this->exeMethodArray[0xBD]=&ExecuteCodeBlock::method_MOV_eBP;
	this->exeMethodArray[0xBE]=&ExecuteCodeBlock::method_MOV_eSI;
	this->exeMethodArray[0xBF]=&ExecuteCodeBlock::method_MOV_eDI;

	this->exeMethodArray[0xC8]=&ExecuteCodeBlock::method_ENTER_Iw_Ib;
	this->exeMethodArray[0xC9]=&ExecuteCodeBlock::method_LEAVE;
	this->exeMethodArray[0xCA]=&ExecuteCodeBlock::method_RET_far_Iw;
	this->exeMethodArray[0xCB]=&ExecuteCodeBlock::method_RET_far;
	this->exeMethodArray[0xCC]=&ExecuteCodeBlock::method_INT_3;
	this->exeMethodArray[0xCD]=&ExecuteCodeBlock::method_INT_Ib;
	this->exeMethodArray[0xCE]=&ExecuteCodeBlock::method_INTO;
	this->exeMethodArray[0xCF]=&ExecuteCodeBlock::method_IRET;

	this->exeMethodArray[0xD8]=&ExecuteCodeBlock::method_ESC;
	this->exeMethodArray[0xD9]=&ExecuteCodeBlock::method_ESC;
	this->exeMethodArray[0xDA]=&ExecuteCodeBlock::method_ESC;
	this->exeMethodArray[0xDB]=&ExecuteCodeBlock::method_ESC;
	this->exeMethodArray[0xDC]=&ExecuteCodeBlock::method_ESC;
	this->exeMethodArray[0xDD]=&ExecuteCodeBlock::method_ESC;
	this->exeMethodArray[0xDE]=&ExecuteCodeBlock::method_ESC;
	this->exeMethodArray[0xDF]=&ExecuteCodeBlock::method_ESC;


	this->exeMethodArray[0xE8]=&ExecuteCodeBlock::method_CALL_Jv;
	this->exeMethodArray[0xE9]=&ExecuteCodeBlock::method_JMP_Jv;
	this->exeMethodArray[0xEA]=&ExecuteCodeBlock::method_JMP_Ap;
	this->exeMethodArray[0xEB]=&ExecuteCodeBlock::method_JMP_Jb;
	this->exeMethodArray[0xEC]=&ExecuteCodeBlock::method_IN_AL_DX;
	this->exeMethodArray[0xED]=&ExecuteCodeBlock::method_IN_eAX_DX;
	this->exeMethodArray[0xEE]=&ExecuteCodeBlock::method_OUT_DX_AL;
	this->exeMethodArray[0xEF]=&ExecuteCodeBlock::method_OUT_DX_eAX;

	this->exeMethodArray[0xF8]=&ExecuteCodeBlock::method_CLC;
	this->exeMethodArray[0xF9]=&ExecuteCodeBlock::method_STC;
	this->exeMethodArray[0xFA]=&ExecuteCodeBlock::method_CLI;
	this->exeMethodArray[0xFB]=&ExecuteCodeBlock::method_STI;
	this->exeMethodArray[0xFC]=&ExecuteCodeBlock::method_CLD;
	this->exeMethodArray[0xFD]=&ExecuteCodeBlock::method_STD;
	this->exeMethodArray[0xFE]=&ExecuteCodeBlock::method_INC_Group4;
	this->exeMethodArray[0xFF]=&ExecuteCodeBlock::method_INC_Group5;



}


ExecuteCodeBlock::ExecuteCodeBlock(CentreProcessorUnit* pCPU)
{
	this->hardwareName = "ExecuteCodeBlock";
	this->pCPU=pCPU;
	pdeCodeBlock=&(pCPU->deCodeBlock);
	
}

void ExecuteCodeBlock::setNeedExeMethod(const ExecuteMethod Method)
{
	this->needExeMethod=Method;
}

void ExecuteCodeBlock::init()
{
	this->memStartAddr = this->pCPU->pMB->getMemStartAddr();
	initExecuteMethod();
}
void ExecuteCodeBlock::setJIT_stuff( U2* pfirst,  U2* psecond,  U2* pSegment, const U4 dispData)
{
	this->point_effectiveMem_first = pfirst;
	this->point_effectiveMem_second = psecond;
	this->pSegment = pSegment;
	this->dispData = dispData;
}