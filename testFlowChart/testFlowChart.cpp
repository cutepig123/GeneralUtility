// testFlowChart.cpp : Defines the entry point for the console application.
//

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "stdafx.h"
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <assert.h>
#include <memory>


#define	ALG_NAME_START "AlgthmStart"
#define	ALG_NAME_END "AlgthmEnd"

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

class MyAlgEnv;

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
	virtual short run(MyAlgEnv *flow) = 0;
	virtual AlgthmBase* Create() = 0;
	virtual void config(void *p){}	//allow user config the alg
};

std::map<std::string, std::vector<std::shared_ptr<PinTypeBase> > > g_PinTypes;
std::map<std::string, std::vector<std::shared_ptr<AlgthmBase>> > g_algs;

short RegisterPinType(std::shared_ptr<PinTypeBase> type)
{
	g_PinTypes[type->m_type].push_back( type);
	return 0;
}

short RegisterAlgthm(const char *name, std::shared_ptr<AlgthmBase> pAlg)
{
	pAlg->m_name =name;
	g_algs[name].push_back( pAlg);
	return 0;
}

void ResetPool()
{
	for (auto it = g_PinTypes.begin(); it != g_PinTypes.end(); ++it)
		it->second.resize(1);

	for (auto it = g_algs.begin(); it != g_algs.end(); ++it)
		it->second.resize(1);
}

PinTypeBase* CreatePinInst(const char *name)
{
	auto itv = g_PinTypes.find(name);
	assert(itv != g_PinTypes.end());
	/*for (auto it = itv->second.begin(); it != itv->second.end(); ++it)
	{
		if (it->unique())
			return *it;
	}
	return itv->second[0]->Create();*/
	std::shared_ptr<PinTypeBase> t (itv->second[0]->Create());
	itv->second.push_back(t);
	return t.get();
}

AlgthmBase* CrteateAlgInst(const char *name)
{
	auto itv = g_algs.find(name);
	assert(itv != g_algs.end());
	/*for (auto it = itv->second.begin(); it != itv->second.end(); ++it)
	{
		if (it->unique())
			return *it;
	}
	return itv->second[0]->Create();*/
	std::shared_ptr<AlgthmBase> t(itv->second[0]->Create());
	itv->second.push_back(t);
	return t.get();
}

struct MyAlgEnv
{
	std::vector<PinTypeBase*> vInPin;    // size ==alg->m_vPins.size(), Values of input pin
	std::vector<PinTypeBase*> vOutPin;    // size ==alg->m_vPins.size(), Values of output pin

	template <class T>
	T* findInPin(int pinid)
	{
		assert(pinid< vInPin.size());
		return dynamic_cast<T*>(vInPin[pinid]);
	}

	template <class T>
	T* findOutPin(int pinid)
	{
		assert(pinid< vOutPin.size());
		return dynamic_cast<T*>(vOutPin[pinid]);
	}
};

class Flow
{
public:
	std::vector< AlgthmBase* > m_algs;

	struct Conn
	{
		std::pair<int, int> src;
		std::pair<int, int> dst;
	};
	std::vector<Conn > m_conns;

	short ConfigModule(int id, void *p)
	{
		assert(m_algs.size()>id);
		m_algs[id]->config(p);
	}

	short AddModule(const char *name, int *pid, void *p=0)
	{
		AlgthmBase* alg = CrteateAlgInst(name);

		m_algs.push_back(alg);
		*pid = m_algs.size() - 1;

		if (p)
			alg->config(p);

		return 0;
	}

	short ConnectModule(int mod1, int pinid1, int mod2, int pinid2)
	{
		assert(m_algs.size()>mod1);
		assert(m_algs.size()>mod2);

		AlgthmBase* alg1 = m_algs[mod1];
		AlgthmBase* alg2 = m_algs[mod2];
		assert(alg1->m_vOutPins.size()>pinid1);
		assert(alg2->m_vInPins.size()>pinid2);

		Conn conn;
		conn.src = std::make_pair(mod1, pinid1);
		conn.dst = std::make_pair(mod2, pinid2);
		m_conns.push_back(conn);
		return 0;
	}

	struct RunItem{
		int aligId;	// curent algorithm idx
		std::vector<std::vector<std::pair<int, int> > > vtrigPin;	// next alg & pins that be triggerd by this alg
		std::vector<int> vIsInputValid;	// status of every input pins
		MyAlgEnv	m_algEnv;
	};
	
	struct Env
	{
		std::vector<int> m_vGraph;	// Run sequence graph
		std::vector<int> m_preparedJobs;	// size is dynamically changing
		std::vector<RunItem> m_vRunItem;
		int m_startId;	//start and end item
		int m_endId;
	};

	short buildGraph(Env	&env)
	{
		size_t nAlg = m_algs.size();
		
		env.m_vGraph.resize(nAlg*nAlg, -1);	// Run sequence graph
		env.m_vRunItem.resize(nAlg);
		env.m_startId = env.m_startId = -1;

		// fill RunItem except vtrigPin[]
		for (size_t algId = 0; algId<nAlg; algId++)
		{
			RunItem &item = env.m_vRunItem[algId];
			AlgthmBase *alg = m_algs[algId];

			// fill m_startId & m_endId
			if (alg->m_name == ALG_NAME_START)
				env.m_startId = algId;
			else if (alg->m_name == ALG_NAME_END)
				env.m_endId= algId;

			// algid
			item.aligId = algId;

			size_t nInput = alg->m_vInPins.size();
			size_t nOutput = alg->m_vOutPins.size();
			
			// vIsInputValid
			item.vIsInputValid.resize(nInput, 0);
			
			// vInPin
			item.m_algEnv.vInPin.resize(nInput);
			for (size_t i = 0; i<nInput; i++)
				item.m_algEnv.vInPin[i] = CreatePinInst(alg->m_vInPins[i].m_type.c_str());

			// vOutPin
			item.m_algEnv.vOutPin.resize(nOutput);
			for (size_t i = 0; i<nOutput; i++)
				item.m_algEnv.vOutPin[i] = CreatePinInst(alg->m_vOutPins[i].m_type.c_str());
		}

		// fill m_startId to m_preparedJobs 
		assert((env.m_startId >= 0 && env.m_endId >= 0) || (env.m_startId < 0 && env.m_endId < 0));
		if (env.m_startId >= 0 && env.m_endId >= 0)
		{
			env.m_preparedJobs.push_back(env.m_startId);
		}

		// fill preparedJobs
		for (size_t algId = 0; algId < nAlg; algId++)
		{
			AlgthmBase *alg = m_algs[algId];
			if (alg->m_vInPins.size() == 0)
				env.m_preparedJobs.push_back(algId);
		}

		// fill RunItem.vtrigPin, vGraph
		for (size_t i = 0; i < m_conns.size(); i++)
		{
			Conn &conn = m_conns[i];
			assert(conn.src.first <nAlg);
			assert(conn.dst.first <nAlg);
			AlgthmBase *algSrc = m_algs[conn.src.first];
			AlgthmBase *algDst = m_algs[conn.dst.first];
			assert(conn.src.second<algSrc->m_vOutPins.size());
			assert(conn.dst.second<algDst->m_vInPins.size());

			// fill vtrigPin
			RunItem &item = env.m_vRunItem[conn.src.first];
			item.vtrigPin.resize(algSrc->m_vOutPins.size());
			item.vtrigPin[conn.src.second].push_back(conn.dst);

			// fill vGraph
#define G(x,y) env.m_vGraph[y*nAlg+x]
			G(conn.src.first, conn.dst.first) = 1;
		}


		return 0;
	}

	short run(std::vector<PinTypeBase*>    *pvInPins = 0, std::vector<PinTypeBase*>    *pvOutPins = 0)
	{
		Env	env;
		buildGraph(env);

		// fill input pins of start 
		assert((env.m_startId >= 0 && env.m_endId >= 0) || (env.m_startId < 0 && env.m_endId < 0));
		if (env.m_startId >= 0 && env.m_endId >= 0)
		{
			assert(pvInPins->size() == env.m_vRunItem[env.m_startId].m_algEnv.vInPin.size());
			for (size_t i = 0; i < env.m_vRunItem[env.m_startId].m_algEnv.vInPin.size(); i++)
			{
				(*pvInPins)[i]->Copy(env.m_vRunItem[env.m_startId].m_algEnv.vInPin[i]);
				env.m_vRunItem[env.m_startId].vIsInputValid[i] = 1;
			}
		}

		while (1)
		{
			std::vector<int> nextPreparedJobs;
			for (size_t i = 0; i < env.m_preparedJobs.size(); i++)
			{
				int nAlgId = env.m_preparedJobs[i];
				
				// all input should be ready now
				assert(std::find(env.m_vRunItem[nAlgId].vIsInputValid.begin(), env.m_vRunItem[nAlgId].vIsInputValid.end(),
					0) == env.m_vRunItem[nAlgId].vIsInputValid.end());

				// run the task
				m_algs[nAlgId]->run(&env.m_vRunItem[nAlgId].m_algEnv);

				//post-processing
				//set vIsInputValid of next tasks
				for (size_t j = 0; j < env.m_vRunItem[nAlgId].vtrigPin.size(); j++)
				{
					auto vcurrTrigPort = env.m_vRunItem[nAlgId].vtrigPin[j];
					for (size_t k = 0; k < vcurrTrigPort.size(); k++)
					{
						auto next = vcurrTrigPort[k];
						// fill vIsInputValid
						env.m_vRunItem[next.first].vIsInputValid[next.second] = 1;

						// fill data
						env.m_vRunItem[nAlgId].m_algEnv.vOutPin[j]->Copy(env.m_vRunItem[next.first].m_algEnv.vInPin[next.second]);

						// if input of next task is ready, then add to nextPreparedJobs
						if (std::find(env.m_vRunItem[next.first].vIsInputValid.begin(),
							env.m_vRunItem[next.first].vIsInputValid.end(), 0) == env.m_vRunItem[next.first].vIsInputValid.end())
						{
							nextPreparedJobs.push_back(next.first);
						}
					}
				}
			}

			env.m_preparedJobs = nextPreparedJobs;
			if (nextPreparedJobs.empty())
				break;
		}

		// check endid's input is filled!!
		if (env.m_startId >= 0 && env.m_endId >= 0)
		{
			assert(pvOutPins->size() == env.m_vRunItem[env.m_endId].m_algEnv.vOutPin.size());

			// all input should be ready now
			assert(std::find(env.m_vRunItem[env.m_endId].vIsInputValid.begin(), env.m_vRunItem[env.m_endId].vIsInputValid.end(),
				0) == env.m_vRunItem[env.m_endId].vIsInputValid.end());

			for (size_t i = 0; i < env.m_vRunItem[env.m_endId].m_algEnv.vOutPin.size(); i++)
			{
				env.m_vRunItem[env.m_endId].m_algEnv.vOutPin[i]->Copy((*pvOutPins)[i]);
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
		out->m_d = 123;
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
		for (size_t i = 0; i < m_vInPins.size(); i++)
		{
			PinTypeBase* in = flow->findInPin<PinTypeBase>(i);
			PinTypeBase* out = flow->findOutPin<PinTypeBase>(i);
			in->Copy(out);
		}
		
		return 0;
	}

	virtual void config(void *p){
		std::vector<Pin>    *pvPins = (std::vector<Pin>*)p;
		m_vOutPins = m_vInPins = *pvPins;
	}
};

// User codes
void test()
{
	RegisterPinType( std::shared_ptr<PinTypeBase>(new PinDouble));
	RegisterAlgthm("Alg_Add", std::shared_ptr<AlgthmBase>(new AlgthmAdd));
	RegisterAlgthm("AlgthmInDbl", std::shared_ptr<AlgthmBase>(new AlgthmInDbl));
	RegisterAlgthm("AlgthmOutDbl", std::shared_ptr<AlgthmBase>(new AlgthmOutDbl));
	RegisterAlgthm(ALG_NAME_START, std::shared_ptr<AlgthmBase>(new AlgthmStartEnd));
	RegisterAlgthm(ALG_NAME_END, std::shared_ptr<AlgthmBase>(new AlgthmStartEnd));

	if (0)
	{
		Flow flow;

		int mid_add, mid_start, mid_start2, mid_end;

		flow.AddModule("AlgthmOutDbl", &mid_start);
		flow.AddModule("Alg_Add", &mid_add);
		flow.AddModule("AlgthmInDbl", &mid_end);

		flow.ConnectModule(mid_start, 0, mid_add, 0);
		flow.ConnectModule(mid_start, 0, mid_add, 1);
		flow.ConnectModule(mid_add, 0, mid_end, 0);
		flow.run();

		ResetPool();
	}

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

		flow.AddModule(ALG_NAME_START, &mid_start, &vInPins);
		flow.AddModule(ALG_NAME_END, &mid_end, &vOutPins);

		flow.AddModule("Alg_Add", &mid_add);
		
		flow.ConnectModule(mid_start, 0, mid_add, 0);
		flow.ConnectModule(mid_start, 1, mid_add, 1);
		flow.ConnectModule(mid_add, 0, mid_end, 0);

		PinDouble aPinIn[2];
		aPinIn[0].m_d = 1;
		aPinIn[1].m_d = 2;
		std::vector<PinTypeBase*> vPinIn(2);
		vPinIn[0] = &aPinIn[0];
		vPinIn[1] = &aPinIn[1];

		PinDouble aPinOut[2];
		std::vector<PinTypeBase*> vPinOut(1);
		vPinOut[0] = &aPinOut[0];

		flow.run(&vPinIn, &vPinOut);

		printf("Output %f\n", aPinOut[0].m_d);

		ResetPool();
	}

}

int _tmain(int argc, _TCHAR* argv[])
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

	//new int;

	test();
	return 0;
}


