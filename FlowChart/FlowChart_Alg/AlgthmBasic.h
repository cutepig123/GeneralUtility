// header file of basic algorithms
// Used by user
#pragma once
#include <FlowChartBase/AlgthmBase.h>

// plugin codes
template <class T>
class AlgthmCommImpl:public AlgthmBase
{
public:
	virtual AlgthmBase *Create()
	{
		AlgthmBase *p=new T;
		p->m_name =m_name;
		p->m_vInPins =m_vInPins;
		p->m_vOutPins =m_vOutPins;
		return p;
	}
};

template <class T>
class PinCommImpl :public PinTypeBase
{
public:
	virtual PinTypeBase *Create()
	{
		PinTypeBase *p = new T;
		return p;
	}
};

class PinDouble : public PinCommImpl<PinDouble>
{
public:
	explicit PinDouble(double d=0)
	{
		m_type = "double";
		m_d = d;
	}
	
	double m_d;
};
class AlgthmAdd :public AlgthmCommImpl<AlgthmAdd>
{
public:
	AlgthmAdd()
	{
		m_vInPins.resize(2);
		m_vOutPins.resize(1);
		m_vInPins[0].m_type = "double";
		m_vInPins[1].m_type = "double";
		m_vOutPins[0].m_type = "double";

		//m_name = "Alg_Add";
	}

	short run(MyAlgEnv *flow)
	{
		PinDouble* in0 = flow->findInPin<PinDouble>( 0);
		PinDouble* in1 = flow->findInPin<PinDouble>( 1);
		PinDouble* out = flow->findOutPin<PinDouble>( 0);
		out->m_d = in0->m_d + in1->m_d;
		printf("AlgthmAdd run %f %f->%f\n", in0->m_d, in1->m_d, out->m_d);
		return 0;
	}

};

#define	OUT_DBL_VAL	123
class AlgthmOutDbl :public AlgthmCommImpl<AlgthmOutDbl>
{
public:
	AlgthmOutDbl()
	{
		m_vOutPins.resize(1);
		m_vOutPins[0].m_type = "double";
		
		m_name = "AlgthmOutDbl";
	}

	short run(MyAlgEnv *flow)
	{
		PinDouble* out = flow->findOutPin<PinDouble>( 0);
		out->m_d = OUT_DBL_VAL;
		printf("AlgthmOutDbl run ->%f\n", out->m_d);
		return 0;
	}

};

class AlgthmInDbl :public AlgthmCommImpl<AlgthmInDbl>
{
public:
	AlgthmInDbl()
	{
		m_vInPins.resize(1);
		m_vInPins[0].m_type = "double";

		//m_name = "AlgthmInDbl";
	}

	short run(MyAlgEnv *flow)
	{
		PinDouble* in = flow->findInPin<PinDouble>( 0);
		printf("AlgthmInDbl run %f->\n", in->m_d);
		return 0;
	}

	
};

class AlgthmStartEnd:public AlgthmCommImpl<AlgthmStartEnd>
{
public:
	short run(MyAlgEnv *flow)
	{
		flow->vOutPin = flow->vInPin;
		
		return 0;
	}

	virtual void config(void *p){
		std::vector<Pin>    *pvPins = (std::vector<Pin>*)p;
		m_vOutPins = m_vInPins = *pvPins;
	}
};

