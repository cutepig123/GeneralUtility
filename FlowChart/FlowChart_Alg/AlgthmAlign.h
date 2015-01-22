#pragma once
#include <FlowChartBase/AlgthmBase.h>
#include "AlgthmBasic.h"
#include <cpp_wrapper/align.h>

class PinRPoint : public PinCommImpl<PinRPoint>
{
public:
	PinRPoint()
	{
		m_type = "PinRPoint";
	}

	RPoint m_d;
};

class PinString : public PinCommImpl<PinString>
{
public:
	explicit PinString(const char *s = NULL)
	{
		m_type = "PinString";
		if (s) m_d = s;
	}

	std::string m_d;
};

class PinAlignRec : public PinCommImpl<PinAlignRec>
{
public:
	PinAlignRec()
	{
		m_type = "PinAlignRec";
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

	short run(MyAlgEnv *flow);

};

class AlgthmAlignSrch :public AlgthmCommImpl<AlgthmAlignSrch>
{
public:
	AlgthmAlignSrch()
	{
		m_vInPins.resize(2);
		m_vOutPins.resize(1);
		m_vInPins[0].m_type = "PinString";
		m_vInPins[1].m_type = "PinAlignRec";

		m_vOutPins[0].m_type = "PinRPoint";

	}

	short run(MyAlgEnv *flow);

};
