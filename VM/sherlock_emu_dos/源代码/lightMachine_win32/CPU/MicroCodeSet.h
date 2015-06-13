#define ADD_Eb_Gb
#define ADD_Ev_Gv
#define ADD_Gb_Eb
#define ADD_Gv_Ev
#define ADD_AL_Ib
#define ADD_eAX_Iv

#define PUSH_ES
#define PUSH_POP_ES


#define ADC_Eb_Gb
#define ADC_Ev_Gv
#define ADC_Gb_Eb
#define ADC_Gv_Ev
#define ADC_AL_Ib
#define ADC_eAX_Iv

#define PUSH_SS
#define POP_SS

#define AND_Eb_Gb
#define AND_Ev_Gv
#define AND_Gb_Eb
#define AND_Gv_Ev
#define AND_AL_Ib
#define AND_eAX_Iv

#define SEG_ES
#define DAA

#define XOR_Eb_Gb
#define XOR_Ev_Gv
#define XOR_Gb_Eb
#define XOR_Gv_Ev
#define XOR_AL_Ib
#define XOR_eAX_Iv

#define SEG_SS
#define AAA

#define INC_eAX
#define INC_eCX
#define INC_eDX
#define INC_eBX
#define INC_eSP
#define INC_eBP
#define INC_eSI
#define INC_eDI

#define PUSH_eAX
#define PUSH_eCX
#define PUSH_eDX
#define PUSH_eBX
#define PUSH_eSP
#define PUSH_eBP
#define PUSH_eSI
#define PUSH_eDI

#define PUSHA_PUSHAD
#define POPA_POPAD
#define BOUND_Gv_Ma
#define ARPL_Ew_Gw
#define SEG_FS
#define SEG_GS
#define Operand_Size
#define Address_Size

#define ShortJump_JO
#define ShortJump_JNO
#define ShortJump_JB
#define ShortJump_JNB
#define ShortJump_JZ
#define ShortJump_JNZ
#define ShortJump_JBE
#define ShortJump_JNBE

#define IMMGroup_Eb_Ib
#define IMMGroup_Ev_Iv
#define IMMGroup_Ev_Ib
#define IMMGroup_Eb_Ib

#define TEST_Eb_Gb
#define TEST_Ev_Gv

#define XCHG_Eb_Gb
#define XCHG_Ev_Gv

#define NOP

#define XCHG_eCX
#define XCHG_eDX
#define XCHG_eBX
#define XCHG_eSP
#define XCHG_eBP
#define XCHG_eSI
#define XCHG_eDI

#define MOV_AL_Ob
#define MOV_eAX_Ov
#define MOV_Ob_AL
#define MOV_Ov_eAX

#define MOVSB_Xb_Yb
#define MOVSW_Xv_Yv
#define CMPSB_Xb_Yb
#define CMPSW_Xv_Yv

#define MOV_AL
#define MOV_CL
#define MOV_DL
#define MOV_BL
#define MOV_AH
#define MOV_CH
#define MOV_DH
#define MOV_BH

#define ShiftGroup_Eb_Ib
#define ShiftGroup_Ev_Ib
#define RET_near_Iw
//that would be something
#define RET_near_None  

#define LES_Gv_Mp
#define LDS_Gv_Mp

#define MOV_Eb_Ib
#define MOV_Ev_Iv

#define ShiftGroup2_Eb_1
#define ShiftGroup2_Ev_1
#define ShiftGroup2_Eb_CL
#define ShiftGroup2_Ev_CL

#define AMM
#define ADD

#define XLAT

#define LOOPN_Jb
#define LOOPE_Jb
#define LOOP_Jb
#define JCXZ_Jb

#define IN_AL_Ib
#define IN_eAX_Ib
#define OUT_Ib_AL
#define OUT_Ib_eAX

#define LOCK
#define REPNE
#define REP_REPE
#define HLT
#define CMC
#define UnaryGroup_Eb
#define UnaryGroup_Ev

#define OR_Eb_Gb
#define OR_Ev_Gv
#define OR_Gb_Eb
#define OR_Gv_Ev
#define OR_AL_Ib
#define OR_eAX_Iv

#define PUSH_CS
#define EscapeCode

#define SBB_Eb_Gb
#define SBB_Ev_Gv
#define SBB_Gb_Eb
#define SBB_Gv_Ev
#define SBB_AL_Ib
#define SBB_eAX_Iv

#define PUSH_DS
#define POP_DS

#define SUB_Eb_Gb
#define SUB_Ev_Gv
#define SUB_Gb_Eb
#define SUB_Gv_Ev
#define SUB_AL_Ib
#define SUB_eAX_Iv

#define SEG_CS
#define DAS

#define CMP_Eb_Gb
#define CMP_Ev_Gv
#define CMP_Gb_Eb
#define CMP_Gv_Ev
#define CMP_AL_Ib
#define CMP_eAX_Iv

#define SEG_DS
#define AAS

#define DEC_eAX
#define DEC_eCX
#define DEC_eDX
#define DEC_eBX
#define DEC_eSP
#define DEC_eBP
#define DEC_eSI
#define DEC_eDI

#define POP_eAX
#define POP_eCX
#define POP_eDX
#define POP_eBX
#define POP_eSP
#define POP_eBP
#define POP_eSI
#define POP_eDI

#define PUSH_Iv
#define IMUL_Gv_Ev_Iv
#define PUSH_Ib
#define IMUL_Gv_Ev_Ib
#define INSB_Yb_DX
#define INSW_Yv_DX
#define OUTSB_DX_Xb
#define OUTSW_DX_Xv

#define ShortJumpCondition_JS
#define ShortJumpCondition_JNS
#define ShortJumpCondition_JP
#define ShortJumpCondition_JNP
#define ShortJumpCondition_JL
#define ShortJumpCondition_JNL
#define ShortJumpCondition_JLE
#define ShortJumpCondition_JNLE

#define MOV_Eb_Gb
#define MOV_Ev_Gv
#define MOV_Gb_Eb
#define MOV_Gv_Ev
#define MOV_Ew_Sw
#define LEA_Gv_M
#define MOV_Sw_Ew
#define POP_Ev

#define CBW
#define CWD
#define CALL_aP
#define WAIT
#define PUSHF_Fv
#define POP_Fv
#define SAHF
#define LAHF

#define TEST_AL_Ib
#define TEST_eAX_Iv
#define STOSB_Yb_AL
#define STOSW_Yv_eAX
#define LODSB_AL_Xb
#define LODSW_eAX_Xv
#define SCASB_AL_Yb
#define SCASW_eAX_Yv

#define MOV_eAX
#define MOV_eCX
#define MOV_eDX
#define MOV_eBX
#define MOV_eSP
#define MOV_eBP
#define MOV_eSI
#define MOV_eDI

#define ENTER_Iw_Ib
#define LEAVE
#define RET_far_Iw
#define RET_far
#define INT_3
#define INT_Ib
#define INTO
#define IRET

#define ESC

#define CALL_Jv
#define JMP_Jv
#define JMP_Ap
#define JMP_Jb
#define IN_AL_Dx
#define IN_eAX_DX
#define OUT_DX_AL
#define OUT_dX_eAX

#define CLC
#define STC
#define CLI
#define STI
#define CLD
#define STD
#define INC_Group4
#define INC_Group5









