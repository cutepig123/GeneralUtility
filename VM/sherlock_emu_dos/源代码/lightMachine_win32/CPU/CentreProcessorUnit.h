#ifndef _CENTREPROCESSORUNIT_H
#define _CENTREPROCESSORUNIT_H
#include "../common.h"
#include "../Hardware.h"

const int OPCODE_WIDTH = 1;
const int MOD_REG_RM_LENGTH = 1;
class DeCodeBlock;
class ExecuteCodeBlock;
class CentreProcessorUnit;
class MotherBoard;

const U2 MASK_FLAG_OF = 0x800;
const U2 MASK_FLAG_DF = 0x400;
const U2 MASK_FLAG_IF = 0x200;
const U2 MASK_FLAG_TF = 0x100;
const U2 MASK_FLAG_SF = 0x80;
const U2 MASK_FLAG_ZF = 0x40;
const U2 MASK_FLAG_AF = 0x10;
const U2 MASK_FLAG_PF = 0x4;
const U2 MASK_FLAG_CF = 0x1;


const U2 MASK_FLAG_METHOD_ADD = MASK_FLAG_OF | MASK_FLAG_SF | MASK_FLAG_ZF | MASK_FLAG_AF | MASK_FLAG_PF | MASK_FLAG_CF;
const U2 MASK_FLAG_METHOD_INC = MASK_FLAG_OF | MASK_FLAG_SF | MASK_FLAG_ZF | MASK_FLAG_AF | MASK_FLAG_PF;//inc不会影响CF
const U2 MASK_FLAG_METHOD_SUB = MASK_FLAG_METHOD_ADD;
const U2 MASK_FLAG_METHOD_DEC = MASK_FLAG_METHOD_INC;
const U2 MASK_FLAG_METHOD_ADC = MASK_FLAG_METHOD_ADD;
const U2 MASK_FLAG_METHOD_CMP = MASK_FLAG_METHOD_ADD;
const U2 MASK_FLAG_METHOD_CMPS = MASK_FLAG_METHOD_ADD;
const U2 MASK_FLAG_METHOD_NEG = MASK_FLAG_METHOD_ADD;
const U2 MASK_FLAG_METHOD_SBB = MASK_FLAG_METHOD_ADD;
const U2 MASK_FLAG_METHOD_SCAS = MASK_FLAG_METHOD_ADD;

const U2 MASK_FLAG_METHOD_OR = MASK_FLAG_SF | MASK_FLAG_ZF | MASK_FLAG_PF ;
const U2 MASK_FLAG_METHOD_AND = MASK_FLAG_METHOD_OR;
const U2 MASK_FLAG_METHOD_XOR = MASK_FLAG_METHOD_OR;
enum Method{ADD_8bit, ADD_16bit,
			SUB_8bit, SUB_16bit,
			ADC_8bit, ADC_16bit,
			SBB_8bit, SBB_16bit,
			NEG_8bit, NEG_16bit,
			INC_8bit, INC_16bit,
			DEC_8bit, DEC_16bit,
			OR_8bit,  OR_16bit,
			AND_8bit, AND_16bit,
			XOR_8bit, XOR_16bit,
			CMP_8bit, CMP_16bit,
			MOD_8bit,/*为了AAM设的*/};

const U1 CS_SEGMENT_OVERRIDE_PREFIX = 0x2E;
const U1 SS_SEGMENT_OVERRIDE_PREFIX = 0x36;
const U1 DS_SEGMENT_OVERRIDE_PREFIX = 0x3E;
const U1 ES_SEGMENT_OVERRIDE_PREFIX = 0x26;

const U1 LOCK_PREFIX = 0xf0;
const U1 REPNE_PREFIX = 0xf2;
const U1 REP_PREFIX = 0xf3;//和下面这个值相同
const U1 REPE_PREFIX = 0xf3;

class DeCodeBlock:public Hardware
{
public:
	DeCodeBlock(CentreProcessorUnit* pCPU);
	void init();
	void deCodeInstruction();
	
	
	
	CentreProcessorUnit* pCPU;
	ExecuteCodeBlock* pexeCodeBlock;
	U1* memStartAddr;
	//MotherBoard* pMB;

public:
	void *prm;
	void *preg;
	void *pseg;
	U1 immData_8Bit, disp_8Bit;
	U2 immData_16Bit, disp_16Bit;
	U1 opContactData_8bit;//紧跟在opcode后的数据
	U2 opContactData_16bit,opContactData_16bitE;
	U1 nnn; //Group中ModRegRm 中的nnn
	U4 segData;//for LEA..存储了段寄存器的值

	U1 prefix_LockOpCode, prefix_RepeatOpCode; //Lock and repeat prefixes
	U1 prefix_SegmentOverrideOpCode; //更改默认段的指令
	U1 prefix_OperandSize;
	U1 prefix_AddressSize;
	

private:
	void refresh();//获取编译下条指令的必要数据

	
	UnKnown deCodeEffectiveMemAddr(const U1 modRegRm, const UnKnown pdispData, int* pdispLength ) ;
	UnKnown getRegAddr(const bool w, const U1 reg) const;
	U2* getSegAddr(const U1 sreg) const;
	void needJIT(const U1 modRegRm, const UnKnown pdispData);//JIT just in time 需要在执行的时候实时运算得到//virtual memory地址
	void deCodeInsStuff();
	void setExecuteMethod();

	void deOpCode_singleDefine();//done
	void deOpCode_doubleDefine();//done
	int deOpCode_modRegRm();//done
	void deOpCode_IMM();//done
	void deOpCode_IMMGroup();//done
	void deOpCode_ModRM();
	void deOpCode_contactData_8bit();
	void deOpCode_contactData_16bit();//done
	void deOpCode_contactData_32bit();//done
	void deOpCode_Mov_mem_AX();//存储器到累加器，或累加器到存储器(互传)  done
	void deOpCode_Mov_imm_rm();//第七种，立即数到存储器/寄存器 done
	void deOpCode_Mov_rm_seg();//第三种，从寄存器/存储器到段存储器传送（互传）done
	void deOpCode_Mov_imm_reg();//第六种，立即数到寄存器
	void deOpCode_modRm();
	void deOpCode_TEST_Ib_Iv();


private:
	U4 SPC, TPC;
	U1* pOpCode;
	bool isWidth; //当前翻译指令w位是否。。。
	
public:
	U2 getDefaultSegment_DS();
	U2 getDefaultSegment_SS();
	
	


};


typedef void (ExecuteCodeBlock::*ExecuteMethod)();
class ExecuteCodeBlock:public Hardware
{
public:
	ExecuteCodeBlock(CentreProcessorUnit* pCPU);

	void setNeedExeMethod(const ExecuteMethod pMethod);
	void initExecuteMethod();
	void init();
	void setJIT_stuff( U2* pfirst,  U2* psecond,  U2* pSegment, const U4 dispData);//JIT需要用到的寄存器地址和偏移量
public:
	CentreProcessorUnit* pCPU;
	DeCodeBlock* pdeCodeBlock;
	//MotherBoard* pMB;
	
	
	ExecuteMethod exeMethodArray[0xff+1];
private:
	U1* memStartAddr;
	U2  *point_effectiveMem_first, *point_effectiveMem_second;//用于JIT计算地址的寄存器指针
	U2* pSegment;//JIT 需要用的段寄存器指针
	U4 dispData; //可用于disp8 和disp16， 用作JIT计算地址的暂存偏移量
public:
	ExecuteMethod needExeMethod;
private:
	ExecuteMethod repeatMethod;
private:
	
	
	void atomMethod_MOV_8bit(U4 logicAddr_des, const U1 data_src);
	void atomMethod_MOV_16bit(U4 logicAddr_des, const U2 data_src);
	void atomMethod_ADD_8bit(U1& des, const U1 src);
	void atomMethod_ADD_16bit(U2& des, const U2 src);
	void atomMethod_INC_8bit(U1& des);
	void atomMethod_INC_16bit(U2& des);
	void atomMethod_ADC_8bit(U1& des, const U1 src);
	void atomMethod_ADC_16bit(U2& des, const U2 src);
	void atomMethod_SUB_8bit(U1& des, const U1 src);
	void atomMethod_SUB_16bit(U2& des, const U2 src);
	void atomMethod_DEC_8bit(U1& des);
	void atomMethod_DEC_16bit(U2& des);
	void atomMethod_SBB_8bit(U1& des, const U1 src);
	void atomMethod_SBB_16bit(U2& des, const U2 src);
	void atomMethod_OR_8bit(U1& des, const U1 src);
	void atomMethod_OR_16bit(U2& des, const U2 src);
	void atomMethod_AND_8bit(U1& des, const U1 src);
	void atomMethod_AND_16bit(U2& des, const U2 src);
	void atomMethod_XOR_8bit(U1& des, const U1 src);
	void atomMethod_XOR_16bit(U2& des, const U2 src);
	void atomMethod_CMP_8bit(U1 des, const U1 src);
	void atomMethod_CMP_16bit(U2 des, const U2 src);
	void atomMethod_CMPSB(); 
	void atomMethod_CMPSW();
	void atomMethod_MOVSB();
	void atomMethod_MOVSW();
	void atomMethod_STOSB();
	void atomMethod_STOSW();
	void atomMethod_LODSB();
	void atomMethod_LODSW();
	void atomMethod_SCASB();
	void atomMethod_SCASW();
	void atomMethod_INS();
	void atomMethod_OUTS();
	void atomMethod_shortJMP();
	void atomMethod_PUSH(const U2 data);
	U2   atomMethod_POP();
	void atomMethod_INT(const U1 INT_num);


	
	void method_ADD_Eb_Gb();
	void method_ADD_Ev_Gv();
	void method_ADD_Gb_Eb();
	void method_ADD_Gv_Ev();
	void method_ADD_AL_Ib();
	void method_ADD_eAX_Iv();

	void method_PUSH_ES();
	void method_POP_ES();


	void method_ADC_Eb_Gb();
	void method_ADC_Ev_Gv();
	void method_ADC_Gb_Eb();
	void method_ADC_Gv_Ev();
	void method_ADC_AL_Ib();
	void method_ADC_eAX_Iv();

	void method_PUSH_SS();
	void method_POP_SS();

	void method_AND_Eb_Gb();
	void method_AND_Ev_Gv();
	void method_AND_Gb_Eb();
	void method_AND_Gv_Ev();
	void method_AND_AL_Ib();
	void method_AND_eAX_Iv();

	void method_SEG_ES();
	void method_DAA();

	void method_XOR_Eb_Gb();
	void method_XOR_Ev_Gv();
	void method_XOR_Gb_Eb();
	void method_XOR_Gv_Ev();
	void method_XOR_AL_Ib();
	void method_XOR_eAX_Iv();

	void method_SEG_SS();
	void method_AAA();

	void method_INC_eAX();
	void method_INC_eCX();
	void method_INC_eDX();
	void method_INC_eBX();
	void method_INC_eSP();
	void method_INC_eBP();
	void method_INC_eSI();
	void method_INC_eDI();

	void method_PUSH_eAX();
	void method_PUSH_eCX();
	void method_PUSH_eDX();
	void method_PUSH_eBX();
	void method_PUSH_eSP();
	void method_PUSH_eBP();
	void method_PUSH_eSI();
	void method_PUSH_eDI();

	void method_PUSHA_PUSHAD();
	void method_POPA_POPAD();
	void method_BOUND_Gv_Ma();
	void method_ARPL_Ew_Gw();
	void method_SEG_FS();
	void method_SEG_GS();
	void method_Operand_Size();
	void method_Address_Size();

	void method_ShortJump_JO();
	void method_ShortJump_JNO();
	void method_ShortJump_JB();
	void method_ShortJump_JNB();
	void method_ShortJump_JZ();
	void method_ShortJump_JNZ();
	void method_ShortJump_JBE();
	void method_ShortJump_JNBE();

	void method_IMMGroup_Eb_Ib();
	void method_IMMGroup_Ev_Iz();
	void method_IMMGroup_Eb_IbX();
	void method_IMMGroup_Ev_Ib();

	void method_TEST_Eb_Gb();
	void method_TEST_Ev_Gv();

	void method_XCHG_Eb_Gb();
	void method_XCHG_Ev_Gv();

	void method_NOP();

	void method_XCHG_eCX();
	void method_XCHG_eDX();
	void method_XCHG_eBX();
	void method_XCHG_eSP();
	void method_XCHG_eBP();
	void method_XCHG_eSI();
	void method_XCHG_eDI();

	void method_MOV_AL_Ob();
	void method_MOV_eAX_Ov();
	void method_MOV_Ob_AL();
	void method_MOV_Ov_eAX();

	void method_MOVSB_Xb_Yb();
	void method_MOVSW_Xv_Yv();
	void method_CMPSB_Xb_Yb();
	void method_CMPSW_Xv_Yv();

	void method_MOV_AL();
	void method_MOV_CL();
	void method_MOV_DL();
	void method_MOV_BL();
	void method_MOV_AH();
	void method_MOV_CH();
	void method_MOV_DH();
	void method_MOV_BH();

	void method_ShiftGroup_Eb_Ib();
	void method_ShiftGroup_Ev_Ib();
	void method_RET_near_Iw();
	//that would be something
	void method_RET_near_None();

	void method_LES_Gv_Mp();
	void method_LDS_Gv_Mp();

	void method_MOV_Eb_Ib();
	void method_MOV_Ev_Iv();

	void method_ShiftGroup2_Eb_1();
	void method_ShiftGroup2_Ev_1();
	void method_ShiftGroup2_Eb_CL();
	void method_ShiftGroup2_Ev_CL();

	void method_AMM();
	void method_AAD();

	void method_XLAT();

	void method_LOOPN_Jb();
	void method_LOOPE_Jb();
	void method_LOOP_Jb();
	void method_JCXZ_Jb();

	void method_IN_AL_Ib();
	void method_IN_eAX_Ib();
	void method_OUT_Ib_AL();
	void method_OUT_Ib_eAX();

	void method_LOCK();
	void method_REPNE();
	void method_REP_REPE();
	void method_HLT();
	void method_CMC();
	void method_UnaryGroup_Eb();
	void method_UnaryGroup_Ev();

	void method_OR_Eb_Gb();
	void method_OR_Ev_Gv();
	void method_OR_Gb_Eb();
	void method_OR_Gv_Ev();
	void method_OR_AL_Ib();
	void method_OR_eAX_Iv();

	void method_PUSH_CS();
	void method_EscapeCode();

	void method_SBB_Eb_Gb();
	void method_SBB_Ev_Gv();
	void method_SBB_Gb_Eb();
	void method_SBB_Gv_Ev();
	void method_SBB_AL_Ib();
	void method_SBB_eAX_Iv();

	void method_PUSH_DS();
	void method_POP_DS();

	void method_SUB_Eb_Gb();
	void method_SUB_Ev_Gv();
	void method_SUB_Gb_Eb();
	void method_SUB_Gv_Ev();
	void method_SUB_AL_Ib();
	void method_SUB_eAX_Iv();

	void method_SEG_CS();
	void method_DAS();

	void method_CMP_Eb_Gb();
	void method_CMP_Ev_Gv();
	void method_CMP_Gb_Eb();
	void method_CMP_Gv_Ev();
	void method_CMP_AL_Ib();
	void method_CMP_eAX_Iv();

	void method_SEG_DS();
	void method_AAS();

	void method_DEC_eAX();
	void method_DEC_eCX();
	void method_DEC_eDX();
	void method_DEC_eBX();
	void method_DEC_eSP();
	void method_DEC_eBP();
	void method_DEC_eSI();
	void method_DEC_eDI();

	void method_POP_eAX();
	void method_POP_eCX();
	void method_POP_eDX();
	void method_POP_eBX();
	void method_POP_eSP();
	void method_POP_eBP();
	void method_POP_eSI();
	void method_POP_eDI();

	void method_PUSH_Iv();
	void method_IMUL_Gv_Ev_Iv();
	void method_PUSH_Ib();
	void method_IMUL_Gv_Ev_Ib();
	void method_INSB_Yb_DX();
	void method_INSW_Yv_DX();
	void method_OUTSB_DX_Xb();
	void method_OUTSW_DX_Xv();

	void method_ShortJumpCondition_JS();
	void method_ShortJumpCondition_JNS();
	void method_ShortJumpCondition_JP();
	void method_ShortJumpCondition_JNP();
	void method_ShortJumpCondition_JL();
	void method_ShortJumpCondition_JNL();
	void method_ShortJumpCondition_JLE();
	void method_ShortJumpCondition_JNLE();

	void method_MOV_Eb_Gb();
	void method_MOV_Ev_Gv();
	void method_MOV_Gb_Eb();
	void method_MOV_Gv_Ev();
	void method_MOV_Ew_Sw();
	void method_LEA_Gv_M();
	void method_MOV_Sw_Ew();
	void method_POP_Ev();

	void method_CBW();
	void method_CWD();
	void method_CALL_aP();
	void method_WAIT();
	void method_PUSHF_Fv();
	void method_POP_Fv();
	void method_SAHF();
	void method_LAHF();

	void method_TEST_AL_Ib();
	void method_TEST_eAX_Iv();
	void method_STOSB_Yb_AL();
	void method_STOSW_Yv_eAX();
	void method_LODSB_AL_Xb();
	void method_LODSW_eAX_Xv();
	void method_SCASB_AL_Yb();
	void method_SCASW_eAX_Yv();

	void method_MOV_eAX();
	void method_MOV_eCX();
	void method_MOV_eDX();
	void method_MOV_eBX();
	void method_MOV_eSP();
	void method_MOV_eBP();
	void method_MOV_eSI();
	void method_MOV_eDI();

	void method_ENTER_Iw_Ib();
	void method_LEAVE();
	void method_RET_far_Iw();
	void method_RET_far();
	void method_INT_3();
	void method_INT_Ib();
	void method_INTO();
	void method_IRET();

	void method_ESC();

	void method_CALL_Jv();
	void method_JMP_Jv();
	void method_JMP_Ap();
	void method_JMP_Jb();
	void method_IN_AL_DX();
	void method_IN_eAX_DX();
	void method_OUT_DX_AL();
	void method_OUT_DX_eAX();

	void method_CLC();
	void method_STC();
	void method_CLI();
	void method_STI();
	void method_CLD();
	void method_STD();
	void method_INC_Group4();
	void method_INC_Group5();



	
public: //GROUP组都是后绑定的，所以设为public。。便于访问
////////////////////////////////////////////<GROUP1>
	void methodGroup_ADD_Eb_Ib();
	void methodGroup_ADD_Ev_Iv();
	void methodGroup_ADD_Ev_Ib();

	void methodGroup_OR_Eb_Ib();
	void methodGroup_OR_Ev_Iv();
	void methodGroup_OR_Ev_Ib();
	
	void methodGroup_ADC_Eb_Ib();
	void methodGroup_ADC_Ev_Iv();
	void methodGroup_ADC_Ev_Ib();

	void methodGroup_SBB_Eb_Ib();
	void methodGroup_SBB_Ev_Iv();
	void methodGroup_SBB_Ev_Ib();

	void methodGroup_AND_Eb_Ib();
	void methodGroup_AND_Ev_Iv();
	void methodGroup_AND_Ev_Ib();

	void methodGroup_SUB_Eb_Ib();
	void methodGroup_SUB_Ev_Iv();
	void methodGroup_SUB_Ev_Ib();

	void methodGroup_XOR_Eb_Ib();
	void methodGroup_XOR_Ev_Iv();
	void methodGroup_XOR_Ev_Ib();

	void methodGroup_CMP_Eb_Ib();
	void methodGroup_CMP_Ev_Iv();
	void methodGroup_CMP_Ev_Ib();
////////////////////////////////////////////</GROUP1>	
////////////////////////////////////////////<GROUP2>
	void methodGroup_ROL_Eb_Ib();
	void methodGroup_ROL_Ev_Ib();
	void methodGroup_ROL_Eb_1();
	void methodGroup_ROL_Ev_1();
	void methodGroup_ROL_Eb_CL();
	void methodGroup_ROL_Ev_CL();

	void methodGroup_ROR_Eb_Ib();
	void methodGroup_ROR_Ev_Ib();
	void methodGroup_ROR_Eb_1();
	void methodGroup_ROR_Ev_1();
	void methodGroup_ROR_Eb_CL();
	void methodGroup_ROR_Ev_CL();

	void methodGroup_RCL_Eb_Ib();
	void methodGroup_RCL_Ev_Ib();
	void methodGroup_RCL_Eb_1();
	void methodGroup_RCL_Ev_1();
	void methodGroup_RCL_Eb_CL();
	void methodGroup_RCL_Ev_CL();

	void methodGroup_RCR_Eb_Ib();
	void methodGroup_RCR_Ev_Ib();
	void methodGroup_RCR_Eb_1();
	void methodGroup_RCR_Ev_1();
	void methodGroup_RCR_Eb_CL();
	void methodGroup_RCR_Ev_CL();

	void methodGroup_SHL_Eb_Ib();
	void methodGroup_SHL_Ev_Ib();
	void methodGroup_SHL_Eb_1();
	void methodGroup_SHL_Ev_1();
	void methodGroup_SHL_Eb_CL();
	void methodGroup_SHL_Ev_CL();

	void methodGroup_SHR_Eb_Ib();
	void methodGroup_SHR_Ev_Ib();
	void methodGroup_SHR_Eb_1();
	void methodGroup_SHR_Ev_1();
	void methodGroup_SHR_Eb_CL();
	void methodGroup_SHR_Ev_CL();

	void methodGroup_SAR_Eb_Ib();
	void methodGroup_SAR_Ev_Ib();
	void methodGroup_SAR_Eb_1();
	void methodGroup_SAR_Ev_1();
	void methodGroup_SAR_Eb_CL();
	void methodGroup_SAR_Ev_CL();
////////////////////////////////////////////</GROUP2>
////////////////////////////////////////////<GROUP3>
	void methodGroup_TEST_Ib();
	void methodGroup_TEST_Iv();
	void methodGroup_NOT_8bit();
	void methodGroup_NOT_16bit();
	void methodGroup_NEG_8bit();
	void methodGroup_NEG_16bit();
	void methodGroup_MUL_AL();
	void methodGroup_MUL_eAX();
	void methodGroup_IMUL_AL();
	void methodGroup_IMUL_eAX();
	void methodGroup_DIV_AL();
	void methodGroup_DIV_eAX();
	void methodGroup_IDIV_AL();
	void methodGroup_IDIV_eAX();

////////////////////////////////////////////</GROUP3>


//////////////////////////////////////////<Group4--5>
	void methodGroup_INC_Eb();
	void methodGroup_INC_Ev();
	void methodGroup_DEC_Eb();
	void methodGroup_DEC_Ev();
	void methodGroup_CALL_Ev();
	void methodGroup_CALL_Ep();
	void methodGroup_JMP_Ev();
	void methodGroup_JMP_Ep();
	void methodGroup_PUSH_Ev();
	

//////////////////////////////////////////</Group4--5>

	void selfMethod_PUSH(const U1 byteData);
	void selfMethod_PUSH(const U2 wordData);

	void chooseRepeatExeStringMethod();
	void self_REP_StringIns();
	void self_REPE_StringIns();
	void self_REPNE_StringIns();
	void self_string_affect_DI_SI(const DataTypeLength type);
	void self_string_affect_SI(const DataTypeLength type);
	void self_string_affect_DI(const DataTypeLength type);
	void method_Unknow();
	
	void method_outsideINT(U1 INT_num);

};



class CentreProcessorUnit:public Hardware
{
	struct LazyTalbe
	{
		U4 operand1, operand2, result;
		Method opMethod;
		int  bitLength;
		LazyTalbe()
		{
			operand1 = operand2 = result = bitLength =0;
		}

	};
public:
	CentreProcessorUnit():deCodeBlock(this),exeCodeBlock(this),Zero(0)
	{
		
	}
	
	void linkMotherBoard(MotherBoard* pMB);
	void init();
	bool getOF_Flag();
	bool getDF_Flag();
	bool getIF_Flag();
	bool getTF_Flag();
	bool getSF_Flag();
	bool getZF_Flag();
	bool getAF_Flag();
	bool getPF_Flag();
	bool getCF_Flag();
	///
	void setOF_Flag(const bool f);
	void setDF_Flag(const bool f);
	void setIF_Flag(const bool f);
	void setTF_Flag(const bool f);
	void setSF_Flag(const bool f);void setSF_Flag(const U1 data);void setSF_Flag(const U2 data);
	void setZF_Flag(const bool f);void setZF_Flag(const U1 data);void setZF_Flag(const U2 data);
	void setAF_Flag(const bool f);
	void setPF_Flag(const bool f);void setPF_Flag(const U1 data);void setPF_Flag(const U2 data);
	void setCF_Flag(const bool f);
	void setSF_ZF_PF_byResult(const U1 result);
	void setSF_ZF_PF_byResult(const U2 result);
	void storeCaculate(const Method opMethod, const int bitLength,  const U4 operand1, const U4 operand2, const U4 result, const U2 mask_Flag);

	U2 generateFLAG();
	void setFLAG(const U2 flags);
	void set_SF_ZF_AF_PF_CF_Flag(const U2 flags);
	
public:
	MotherBoard* pMB;
	DeCodeBlock deCodeBlock;
	ExecuteCodeBlock exeCodeBlock;
    int Zero;//设置值为0的常量
    unsigned int RWpnt;//好吧。。这个是为了用easyVM测试的
    bool isNeedHandleKeyPress;//用于判断是否执行中断0x9
private:
	LazyTalbe OF_lazyTable, SF_lazyTable, PF_lazyTable, ZF_lazyTable, CF_lazyTable, AF_lazyTable;//DF,IF,TF 不会需要这个，因为相对来说计算它们的值比存储lazytable 要快。
	bool OF_Flag, DF_Flag, IF_Flag, TF_Flag, SF_Flag, ZF_Flag, AF_Flag, PF_Flag, CF_Flag;
	bool isOFCaculated, isDFCaculated, isIFCaculated, isTFCaculated;
	bool isSFCaculated, isZFCaculated, isAFCaculated, isPFCaculated, isCFCaculated;
	U2 needCaculateFlag_mask;
	U2 flag_extern;//存储flag寄存器的14--11位的数据，主要用以解决popf，pushf
	
public:
//各种寄存器
	union
	{
		U4 eax;
		union
		{
			U2 ax;
			struct
			{
				
				U1 al;
				U1 ah;
			};
		};
	};

	union
	{
		U4 ecx;
		union
		{
			U2 cx;
			struct
			{
				
				U1 cl;
				U1 ch;
			};
		};
	};

	union
	{
		U4 edx;
		union
		{
			U2 dx;
			struct
			{
				
				U1 dl;
				U1 dh;
			};
		};
	};

	union
	{
		U4 ebx;
		union
		{
			U2 bx;
			struct
			{
				
				U1 bl;
				U1 bh;
			};
		};
	};

	

	union
	{
		U2 sp;
		U4 esp;
	};	
	union
	{
		U2 bp;
		U4 ebp;
	};	
	union
	{
		U2 si;
		U4 esi;
	};	
	union
	{
		U2 di;
		U4 edi;
	};	
	union
	{
		U2 ip;
		U4 eip;
	};	
	union
	{
		U2 flags;
		U4 eflags;
	};	
	U2 cs;
	U2 ds;
	U2 es;
	U2 ss;
	U2 fs;
	U2 gs;
	
};

#endif