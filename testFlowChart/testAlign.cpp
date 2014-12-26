#include "AlgthmBase.h"
#include "basicAlgthm.h"
#include "Flow.h"
#include <cpp_wrapper\align.h>
#include <string>

class PinRPoint : public PinTypeBase
{
public:
	PinRPoint()
	{
		m_type = "RPoint";
	}


	virtual PinTypeBase* Create()
	{
		return new PinRPoint();
	}

	RPoint m_d;
};

class PinString: public PinTypeBase
{
public:
	explicit PinString(const char *s=NULL)
	{
		m_type = "PinString";
		if(s) m_d = s;
	}


	virtual PinTypeBase* Create()
	{
		return new PinString();
	}

	std::string m_d;
};

class PinAlignRec : public PinTypeBase
{
public:
	PinAlignRec()
	{
		m_type = "PinAlignRec";
	}

	virtual PinTypeBase* Create()
	{
		return new PinAlignRec();
	}

	align_rec m_d;
};

class AlgthmAlignLearn :public AlgthmCommImpl<AlgthmAlignLearn>
{
public:
	AlgthmAlignLearn()
	{
		m_vInPins.resize(1);
		m_vOutPins.resize(1);
		m_vInPins[0].m_type = "PinString";
		
		m_vOutPins[0].m_type = "PinAlignRec";

	}

	short run(MyAlgEnv *flow)
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
		Point pt = { 10, 10 };
		Size sz = { buf.GetBuf().size.x - pt.x * 2, buf.GetBuf().size.y - pt.y * 2 };
		par.setRoi(&pt, &sz);

		Point pt2 = { 0, 0 };
		Size sz2 = { buf.GetBuf().size.x - pt.x * 2, buf.GetBuf().size.y - pt.y * 2 };
		par.setSelfSrchRoi(&pt2, &sz2);

		wSts = align_learn(&buf.GetImage(), &msk.GetImage(), &par, &out->m_d);
		assert(wSts == 0);
		
		return 0;
	}

};

class AlgthmAlignSrch :public AlgthmCommImpl<AlgthmAlignSrch>
{
public:
	AlgthmAlignSrch()
	{
		m_vInPins.resize(1);
		m_vOutPins.resize(1);
		m_vInPins[0].m_type = "PinString";

		m_vOutPins[0].m_type = "PinRPoint";

	}

	short run(MyAlgEnv *flow)
	{
		PinString* in0 = flow->findInPin<PinString>(0);

		PinRPoint* out = flow->findOutPin<PinRPoint>(0);

		/*Buffer buf;
		short wSts = 0;
		wSts = buf.Load(in0->m_d.c_str());
		assert(wSts == 0);

		Buffer msk;
		msk.Resize(buf.GetBuf().size);
		std::fill(msk.Data(), msk.Data() + buf.GetBuf().size.x *buf.GetBuf().size.y, 1);

		align_lrn_par par;
		Point pt = { 10, 10 };
		Size sz = { buf.GetBuf().size.x - pt.x * 2, buf.GetBuf().size.y - pt.y * 2 };
		par.setRoi(&pt, &sz);

		Point pt2 = { 0, 0 };
		Size sz2 = { buf.GetBuf().size.x - pt.x * 2, buf.GetBuf().size.y - pt.y * 2 };
		par.setSelfSrchRoi(&pt2, &sz2);

		wSts = align_learn(&buf.GetImage(), &msk.GetImage(), &par, &out->m_d);
		assert(wSts == 0);*/

		return 0;
	}

};

void testAlign()
{
	RegisterAlgthm(ALG_NAME_START, std::shared_ptr<AlgthmBase>(new AlgthmStartEnd));
	RegisterAlgthm(ALG_NAME_END, std::shared_ptr<AlgthmBase>(new AlgthmStartEnd));
	RegisterAlgthm("AlgthmAlignLearn", std::shared_ptr<AlgthmBase>(new AlgthmAlignLearn));
	RegisterAlgthm("AlgthmAlignSrch", std::shared_ptr<AlgthmBase>(new AlgthmAlignSrch));

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

		flow.AddModule(ALG_NAME_START, &mid_start, &vInPins);
		flow.AddModule(ALG_NAME_END, &mid_end, &vOutPins);
		flow.AddModule("AlgthmAlignLearn", &mid_learn);

		flow.ConnectModule(mid_start, 0, mid_learn, 0);
		flow.ConnectModule(mid_learn, 0, mid_end, 0);

		std::vector<std::shared_ptr<PinTypeBase> > vPinIn(1);
		vPinIn[0].reset(new PinString("1.bmp"));
		
		std::vector<std::shared_ptr<PinTypeBase> > vPinOut(1);
		vPinOut[0].reset(new PinAlignRec);

		flow.run(&vPinIn, &vPinOut);

	}
}