#include <FlowChartBase/AlgthmBase.h>
#include "AlgthmAlign.h"
//#include "Algthmbasic.h"
#include <FlowChartBase/Flow.h>
//#include <cpp_wrapper\align.h>
#include <string>
#include <windows.h>

short AlgthmAlignLearn::run(MyAlgEnv *flow)
{
	PinString* in0 = flow->findInPin<PinString>(0);
	
	PinAlignRec* out = flow->findOutPin<PinAlignRec>(0);
	
	Buffer buf;
	short wSts = 0;
	wSts =buf.Load(in0->m_d.c_str());
	assert(wSts == 0);

	Buffer msk;
	msk.Resize(buf.GetBuf().size);
	std::fill(msk.Data(), msk.Data() + buf.GetBuf().size.x *buf.GetBuf().size.y, 1);

	align_lrn_par par;
	Point pt(10, 10 );
	Size sz ( buf.GetBuf().size.x - pt.x * 2, buf.GetBuf().size.y - pt.y * 2 );
	par.setRoi(&pt, &sz);

	Point pt2 ( 0, 0 );
	Size sz2 ( buf.GetBuf().size.x - pt2.x * 2, buf.GetBuf().size.y - pt2.y * 2 );
	par.setSelfSrchRoi(&pt2, &sz2);

	wSts = align_learn(&buf.GetImage(), &msk.GetImage(), &par, &out->m_d);
	assert(wSts == 0);
	
	return 0;
}

short AlgthmAlignSrch::run(MyAlgEnv *flow)
{
	PinString* in0 = flow->findInPin<PinString>(0);
	PinAlignRec* in1 = flow->findInPin<PinAlignRec>(1);

	PinRPoint* out = flow->findOutPin<PinRPoint>(0);

	Buffer buf;
	short wSts = 0;
	wSts = buf.Load(in0->m_d.c_str());
	assert(wSts == 0);

	Buffer msk;
	msk.Resize(buf.GetBuf().size);
	std::fill(msk.Data(), msk.Data() + buf.GetBuf().size.x *buf.GetBuf().size.y, 1);

	align_srch_par par;
	par.setRoi(&Point(0, 0), &Size(buf.GetBuf().size.x, buf.GetBuf().size.y));
	/*Point pt = { 10, 10 };
	Size sz = { buf.GetBuf().size.x - pt.x * 2, buf.GetBuf().size.y - pt.y * 2 };
	par.setRoi(&pt, &sz);

	Point pt2 = { 0, 0 };
	Size sz2 = { buf.GetBuf().size.x - pt.x * 2, buf.GetBuf().size.y - pt.y * 2 };
	par.setSelfSrchRoi(&pt2, &sz2);*/

	align_srch_resut res;

	wSts = align_search(&buf.GetImage(), &msk.GetImage(), &par, &in1->m_d, &res);
	assert(wSts == 0);

	out->m_d = res.vObj[0].pt;

	return 0;
}


void Flow_Test_align()
{
	std::shared_ptr<PinTypeBase> pinRec;
	const char *sbuffer = "s.bmp";
	const char *dbuffer = "d.bmp";

	{
		Flow flow;

		int mid_learn, mid_start, mid_end;

		//设置开始结束pin的数量和类型
		std::vector<AlgthmBase::Pin>    vInPins;
		vInPins.resize(1);
		vInPins[0].m_type = "PinString";

		std::vector<AlgthmBase::Pin>    vOutPins;
		vOutPins.resize(1);
		vOutPins[0].m_type = "PinAlignRec";

		flow.AddModule("ALG_NAME_START", &mid_start, &vInPins);
		flow.AddModule("ALG_NAME_END", &mid_end, &vOutPins);
		flow.AddModule("AlgthmAlignLearn", &mid_learn);

		flow.ConnectModule(mid_start, 0, mid_learn, 0);
		flow.ConnectModule(mid_learn, 0, mid_end, 0);

		std::vector<std::shared_ptr<PinTypeBase> > vPinIn(1);
		vPinIn[0].reset(new PinString(sbuffer));
		
		std::vector<std::shared_ptr<PinTypeBase> > vPinOut(1);
		
		flow.run(&vPinIn, &vPinOut);
		pinRec =vPinOut[0];
	}

	std::shared_ptr<PinRPoint> res;
	{
		Flow flow;

		int mid_learn, mid_start, mid_end;

		//设置开始结束pin的数量和类型
		std::vector<AlgthmBase::Pin>    vInPins;
		vInPins.resize(2);
		vInPins[0].m_type = "PinString";
		vInPins[1].m_type = "PinAlignRec";

		std::vector<AlgthmBase::Pin>    vOutPins;
		vOutPins.resize(1);
		vOutPins[0].m_type = "PinRPoint";

		flow.AddModule("ALG_NAME_START", &mid_start, &vInPins);
		flow.AddModule("AlgthmAlignSrch", &mid_learn);
		flow.AddModule("ALG_NAME_END", &mid_end, &vOutPins);

		flow.ConnectModule(mid_start, 0, mid_learn, 0);
		flow.ConnectModule(mid_start, 1, mid_learn, 1);
		flow.ConnectModule(mid_learn, 0, mid_end, 0);

		std::vector<std::shared_ptr<PinTypeBase> > vPinIn(2);
		vPinIn[0].reset(new PinString(dbuffer));
		vPinIn[1] = pinRec;

		std::vector<std::shared_ptr<PinTypeBase> > vPinOut(1);
		
		flow.run(&vPinIn, &vPinOut);
		res = std::dynamic_pointer_cast<PinRPoint>(vPinOut[0]);

		printf("Result %f %f\n", res->m_d.x, res->m_d.y);
	}

	//允许用户指定输入端和输出端，这样可以取消start，和end节点
	res.reset();
	{
		Flow flow;

		int mid_learn, mid_start, mid_end;

		flow.AddModule("AlgthmAlignSrch", &mid_learn);
		
		//flow.ConnectModule(mid_start, 1, mid_learn, 1);
		
		std::vector<std::shared_ptr<PinTypeBase> > vPinIn(2);
		vPinIn[0].reset(new PinString(dbuffer));
		vPinIn[1] = pinRec;

		std::vector<std::shared_ptr<PinTypeBase> > vPinOut;

		flow.run(&vPinIn, &vPinOut, 0, 0);
		res = std::dynamic_pointer_cast<PinRPoint>(vPinOut[0]);

		printf("Result %f %f\n", res->m_d.x, res->m_d.y);
	}
}

