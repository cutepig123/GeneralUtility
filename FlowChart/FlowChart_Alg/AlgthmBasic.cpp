#include "AlgthmBasic.h"
#include <FlowChartBase/Flow.h>

class AlgthmMyFlow :public AlgthmCommImpl<AlgthmMyFlow>
{
public:
	Flow flow;

	AlgthmMyFlow()
	{
		int mid_add, mid_start, mid_end;

		//设置开始结束pin的数量和类型
		m_vInPins.resize(2);
		m_vInPins[0].m_type = "double";
		m_vInPins[1].m_type = "double";

		m_vOutPins.resize(1);
		m_vOutPins[0].m_type = "double";

		flow.AddModule("ALG_NAME_START", &mid_start, &m_vInPins);
		flow.AddModule("ALG_NAME_END", &mid_end, &m_vOutPins);

		flow.AddModule("Alg_Add", &mid_add);

		flow.ConnectModule(mid_start, 0, mid_add, 0);
		flow.ConnectModule(mid_start, 1, mid_add, 1);
		flow.ConnectModule(mid_add, 0, mid_end, 0);
	}

	short run(MyAlgEnv *flowx)
	{
		flow.run(&flowx->vInPin, &flowx->vOutPin);

		return 0;
	}


};


// User codes
void Flow_Test_Basic()
{
	RegisterAlgthm("MyFlowAlg", std::shared_ptr<AlgthmBase>(new AlgthmMyFlow));

	if (1)
	{
		Flow flow;

		int mid_add, mid_start, mid_start2, mid_end;

		flow.AddModule("AlgthmOutDbl", &mid_start);
		flow.AddModule("Alg_Add", &mid_add);
		//flow.AddModule("AlgthmInDbl", &mid_end, 0);

		flow.ConnectModule(mid_start, 0, mid_add, 0);
		flow.ConnectModule(mid_start, 0, mid_add, 1);
		//flow.ConnectModule(mid_add, 0, mid_end, 0);

		std::vector<std::shared_ptr<PinTypeBase> >    vOutPins;

		flow.run(0, &vOutPins);

		assert(fabs(2 * OUT_DBL_VAL - dynamic_cast<PinDouble&>(*vOutPins[0]).m_d) < 0.0001);
	}

	if (1)
	{
		Flow flow;

		int mid_add, mid_start, mid_end;

		//设置开始结束pin的数量和类型
		std::vector<AlgthmBase::Pin>    vInPins;
		vInPins.resize(2);
		vInPins[0].m_type = "double";
		vInPins[1].m_type = "double";

		std::vector<AlgthmBase::Pin>    vOutPins;
		vOutPins.resize(1);
		vOutPins[0].m_type = "double";

		flow.AddModule("ALG_NAME_START", &mid_start, &vInPins);
		flow.AddModule("ALG_NAME_END", &mid_end, &vOutPins);

		flow.AddModule("Alg_Add", &mid_add);

		flow.ConnectModule(mid_start, 0, mid_add, 0);
		flow.ConnectModule(mid_start, 1, mid_add, 1);
		flow.ConnectModule(mid_add, 0, mid_end, 0);

		std::vector<std::shared_ptr<PinTypeBase> > vPinIn(2);
		vPinIn[0].reset(new PinDouble(1));
		vPinIn[1].reset(new PinDouble(2));

		std::vector<std::shared_ptr<PinTypeBase> > vPinOut(1);
		vPinOut[0].reset(new PinDouble);

		flow.run(&vPinIn, &vPinOut);

		printf("Output %f\n", dynamic_cast<PinDouble&>(*vPinOut[0]).m_d);
		assert(fabs(dynamic_cast<PinDouble&>(*vPinOut[0]).m_d - 3) < 0.00001);
	}

	if (1)
	{
		Flow flow;
		int mid_add, mid_start, mid_end;
		flow.AddModule("AlgthmOutDbl", &mid_start);
		flow.AddModule("MyFlowAlg", &mid_add);
		//flow.AddModule("AlgthmInDbl", &mid_end);

		flow.ConnectModule(mid_start, 0, mid_add, 0);
		flow.ConnectModule(mid_start, 0, mid_add, 1);
		//flow.ConnectModule(mid_add, 0, mid_end, 0);

		std::vector<std::shared_ptr<PinTypeBase> > vPinOut(1);

		flow.run(0, &vPinOut);

		assert(fabs(2 * OUT_DBL_VAL - dynamic_cast<PinDouble&>(*vPinOut[0]).m_d) < 0.0001);

		flow.io("filename", true);
	}

	{
		Flow flow;

		flow.io("filename", false);

		//MyAlgEnv end;

		flow.run();	//how to get the output

		//assert(fabs(2 * OUT_DBL_VAL - dynamic_cast<PinDouble&>(*end.vInPin[0]).m_d) < 0.0001);
	}

}
