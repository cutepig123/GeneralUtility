// testFlowChart.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <assert.h>

class PinTypeBase
{
public:
	std::string m_type;
	virtual ~PinTypeBase() = 0{};
	//virtual void Pack(void *data, std::vector<char> *)=0;
	//virtual void Unpack(std::vector<char> const&, void *data )=0;
	virtual void Copy(PinTypeBase *des) = 0;
	virtual PinTypeBase* Create() = 0;
};

class Flow;

class AlgthmBase
{
public:
	virtual ~AlgthmBase() = 0{};

	struct Pin
	{
		std::string				m_type;
	};

	std::vector<Pin>    m_vInPins;
	std::vector<Pin>    m_vOutPins;
	std::string            m_name;
	virtual short run(Flow *flow) = 0;
	virtual AlgthmBase* Create() = 0;
};

std::map<std::string, PinTypeBase*> g_PinTypes;
std::map<std::string, AlgthmBase*> g_algs;

short RegisterPinType(PinTypeBase *type)
{
	g_PinTypes[type->m_type] = type;
	return 0;
}

short RegisterAlgthm(AlgthmBase *pAlg)
{
	g_algs[pAlg->m_name] = pAlg;
	return 0;
}

PinTypeBase* CreatePinInst(const char *name)
{
	auto it = g_PinTypes.find(name);
	assert(it != g_PinTypes.end());
	return it->second->Create();
}

AlgthmBase* CrteateAlgInst(const char *name)
{
	auto it = g_algs.find(name);
	assert(it != g_algs.end());
	return it->second->Create();
}

class Flow
{
public:
	struct Alg
	{
		AlgthmBase* alg;
		std::vector<PinTypeBase*> vInPin;    // size ==alg->m_vPins.size()
		std::vector<PinTypeBase*> vOutPin;    // size ==alg->m_vPins.size()
	};

	std::vector< Alg > m_algs;

	struct COnn
	{
		std::pair<int, int> src;
		std::pair<int, int> dst;
	};
	std::vector<COnn > m_conns;

	template <class T>
	T* findInPin(AlgthmBase *alg, int pinid)
	{
		for (size_t i = 0, n = m_algs.size(); i<n; i++)
		{
			if (m_algs[i].alg == alg)
			{
				assert(pinid< m_algs[i].vInPin.size());
				return dynamic_cast<T*>(m_algs[i].vInPin[pinid]);
			}
		}
		return 0;
	}

	template <class T>
	T* findOutPin(AlgthmBase *alg, int pinid)
	{
		for (size_t i = 0, n = m_algs.size(); i<n; i++)
		{
			if (m_algs[i].alg == alg)
			{
				assert(pinid< m_algs[i].vOutPin.size());
				return dynamic_cast<T*>(m_algs[i].vOutPin[pinid]);
			}
		}
		return 0;
	}

	short AddModule(const char *name, int *pid)
	{
		AlgthmBase* alg = CrteateAlgInst(name);

		Alg stAlg;
		stAlg.alg = alg;

		size_t n = alg->m_vInPins.size();
		stAlg.vInPin.resize(n);
		for (size_t i = 0; i<n; i++)
			stAlg.vInPin[i] = CreatePinInst(alg->m_vInPins[i].m_type.c_str());
		
		n = alg->m_vOutPins.size();
		stAlg.vOutPin.resize(n);
		for (size_t i = 0; i<n; i++)
			stAlg.vOutPin[i] = CreatePinInst(alg->m_vOutPins[i].m_type.c_str());
		
		m_algs.push_back(stAlg);
		*pid = m_algs.size() - 1;
		return 0;
	}

	short ConnectModule(int mod1, int pinid1, int mod2, int pinid2)
	{
		assert(m_algs.size()>mod1);
		assert(m_algs.size()>mod2);

		Alg alg1 = m_algs[mod1];
		Alg alg2 = m_algs[mod2];
		assert(alg1.vOutPin.size()>pinid1);
		assert(alg2.vInPin.size()>pinid2);

		COnn conn;
		conn.src = std::make_pair(mod1, pinid1);
		conn.dst = std::make_pair(mod2, pinid2);
		m_conns.push_back(conn);
		return 0;
	}

	struct RunItem{
		int aligId;
		std::vector<std::pair<int, int> > vtrigPin;
	};
	
	short buildRunItem(int nAlgId, RunItem &item)
	{
		assert(m_algs.size() > nAlgId);
		Alg &alg = m_algs[nAlgId];
		size_t n=alg.alg->m_vOutPins.size();
		item.vtrigPin.resize(n);
		item.aligId = nAlgId;
		for (size_t i = 0; i < n; i++)
		{
			auto it = std::find_if(m_conns.begin(), m_conns.end(), [nAlgId, i](COnn const&l){return l.src.first == nAlgId && l.src.second == i; });
			assert(it != m_conns.end());
			item.vtrigPin[i] = it->dst;
		}
		return 0;
	}
	short buildRunSeq(std::vector< RunItem> &algSeqs)
	{
		algSeqs.resize(4);
		buildRunItem(0, algSeqs[0]);
		buildRunItem(1, algSeqs[1]);
		buildRunItem(2, algSeqs[2]);
		buildRunItem(3, algSeqs[3]);
		//while( has_connection)
		//    find a alg with only output nodes
		//    add the alg to algSeqs[]
		//    remove all connections related to the alg
/*
		for (int i = 0; i<m_algs.size(); i++)
		{
			if (m_algs[i].vInPin.empty())
			{
				RunItem item;
				buildRunItem(i, item);
				algSeqs.push_back(item);
			}
		}

		std::vector<COnn > conns(m_conns);

		while (!conns.empty())
		{
			for (int i = 0; i<algSeqs->size(); i++)
			{
				for (auto it = conns.begin(); it != conns.end();)
				{
					if (algSeqs[i] == conns[j].src.first)
						it = conns.erase(it);
					else
						++it;
				}
			}

			? ?
		}*/
		return 0;
	}

	short run()
	{
		std::vector< RunItem> algSeqs;
		buildRunSeq(algSeqs);

		for (size_t i = 0; i < algSeqs.size(); i++)
		{
			// run each algorithm
			int nId = algSeqs[i].aligId;
			m_algs[nId].alg->run(this);

			// trig out pin
			assert(m_algs[nId].alg->m_vOutPins.size() == algSeqs[i].vtrigPin.size() );

			for (size_t j = 0; j < algSeqs[i].vtrigPin.size();j++)
			{
				int nDesAlgID = algSeqs[i].vtrigPin[j].first;
				int nDesPinID = algSeqs[i].vtrigPin[j].second;
				assert(nDesAlgID < m_algs.size());
				assert(m_algs[nDesAlgID].vInPin.size()>nDesPinID);
				m_algs[nId].vOutPin[j]->Copy(m_algs[nDesAlgID].vInPin[nDesPinID]);
			}
		}
		return 0;
	}
};


// plugin codes
class PinDouble : public PinTypeBase
{
public:
	PinDouble()
	{
		m_type = "double";
	}
	//virtual void Pack(void *data,std::vector<char> *vData)
	//{
	//    vData.resize(sizeof(double));
	//    *((double*)&vData[0]) =*((double*)data);
	//}
	//virtual void Unpack(std::vector<char> const& vData,void *data){
	//    *((double*)data) =*((double*)&vData[0]);
	//}

	virtual void Copy(PinTypeBase *des)
	{
		PinDouble *pDes = (PinDouble*)des;
		pDes->m_d = m_d;
		printf("PinDouble Copy %f\n", m_d);
	}
	virtual PinTypeBase* Create()
	{
		return new PinDouble();
	}

	double m_d;
};

class AlgthmAdd :public AlgthmBase
{
public:
	AlgthmAdd()
	{
		m_vInPins.resize(2);
		m_vOutPins.resize(1);
		m_vInPins[0].m_type = "double";
		m_vInPins[1].m_type = "double";
		m_vOutPins[0].m_type = "double";

		m_name = "Alg_Add";
	}

	short run(Flow *flow)
	{
		PinDouble* in0 = flow->findInPin<PinDouble>(this, 0);
		PinDouble* in1 = flow->findInPin<PinDouble>(this, 1);
		PinDouble* out = flow->findOutPin<PinDouble>(this, 0);
		out->m_d = in0->m_d + in1->m_d;
		printf("AlgthmAdd run %f %f->%f\n", in0->m_d, in1->m_d, out->m_d);
		return 0;
	}

	AlgthmBase *Create()
	{
		return new AlgthmAdd;
	}
};

class AlgthmOutDbl :public AlgthmBase
{
public:
	AlgthmOutDbl()
	{
		m_vOutPins.resize(1);
		m_vOutPins[0].m_type = "double";
		
		m_name = "AlgthmOutDbl";
	}

	short run(Flow *flow)
	{
		PinDouble* out = flow->findOutPin<PinDouble>(this, 0);
		out->m_d = 123;
		printf("AlgthmOutDbl run ->%f\n", out->m_d);
		return 0;
	}

	AlgthmBase *Create()
	{
		return new AlgthmOutDbl;
	}
};

class AlgthmInDbl :public AlgthmBase
{
public:
	AlgthmInDbl()
	{
		m_vInPins.resize(1);
		m_vInPins[0].m_type = "double";

		m_name = "AlgthmInDbl";
	}

	short run(Flow *flow)
	{
		PinDouble* in = flow->findInPin<PinDouble>(this, 0);
		printf("AlgthmInDbl run %f->\n", in->m_d);
		return 0;
	}

	AlgthmBase *Create()
	{
		return new AlgthmInDbl;
	}
};

// User codes
void test()
{
	RegisterPinType(new PinDouble);
	RegisterAlgthm(new AlgthmAdd);
	RegisterAlgthm(new AlgthmInDbl);
	RegisterAlgthm(new AlgthmOutDbl);

	Flow flow;

	int mid_add, mid_start, mid_start2, mid_end;
	flow.AddModule("AlgthmOutDbl", &mid_start); 
	flow.AddModule("AlgthmOutDbl", &mid_start2);
	flow.AddModule("Alg_Add", &mid_add);
	flow.AddModule("AlgthmInDbl", &mid_end);

	flow.ConnectModule(mid_start, 0, mid_add, 0);
	flow.ConnectModule(mid_start2, 0, mid_add, 1);
	flow.ConnectModule(mid_add, 0, mid_end, 0);
	flow.run();
}

int _tmain(int argc, _TCHAR* argv[])
{
	test();
	return 0;
}


