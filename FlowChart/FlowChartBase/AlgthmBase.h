// header file for alg developer

#pragma once
#include "export.h"
#include <vector>
#include <string>
#include <memory>
#include <assert.h>

#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

class PinTypeBase
{
public:
	std::string m_type;
	virtual ~PinTypeBase() = 0{};
	virtual PinTypeBase* Create() = 0;
};


struct MyAlgEnv
{
	std::vector<std::shared_ptr<PinTypeBase> > vInPin;    // size ==alg->m_vPins.size(), Values of input pin
	std::vector<std::shared_ptr<PinTypeBase> > vOutPin;    // size ==alg->m_vPins.size(), Values of output pin

	template <class T>
	T* findInPin(int pinid)
	{
		assert(pinid< vInPin.size());
		return dynamic_cast<T*>(vInPin[pinid].get());
	}

	template <class T>
	T* findOutPin(int pinid)
	{
		assert(pinid< vOutPin.size());
		return dynamic_cast<T*>(vOutPin[pinid].get());
	}
};

class AlgthmBase
{
public:
	virtual ~AlgthmBase() = 0{};

	struct Pin
	{
		std::string				m_type;

		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(m_type);
		}
	};

	std::vector<Pin>    m_vInPins;
	std::vector<Pin>    m_vOutPins;
	std::string            m_name;
	virtual short run(MyAlgEnv *flow) = 0;
	virtual AlgthmBase* Create() = 0;
	virtual void config(void *p){}	//allow user config the alg
};


short FLOWCHARTBASE_XXPORTS RegisterPinType(std::shared_ptr<PinTypeBase> type);
short FLOWCHARTBASE_XXPORTS RegisterAlgthm(const char *name, std::shared_ptr<AlgthmBase> pAlg);
std::shared_ptr<PinTypeBase> FLOWCHARTBASE_XXPORTS CreatePinInst(const char *name);
std::shared_ptr<AlgthmBase> FLOWCHARTBASE_XXPORTS CrteateAlgInst(const char *name);

