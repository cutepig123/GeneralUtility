#include "VL_RegFA.h"

namespace vl
{
	namespace grammar
	{
		
	typedef VL_List<VL_RegNFA::Status* , true> NFAStatusList;
	typedef VL_List<VL_RegDFA::Status* , true> DFAStatusList;

/*********************************************************************************************************
VL_RegNFA
*********************************************************************************************************/

	VL_RegNFA::EpsilonNFA::EpsilonNFA()
	{
		InitStatus=0;
		FinalStatus=0;
	}

	VL_RegNFA::VL_RegNFA(VInt PackSize):VL_Automaton<VLS_RegNFAEdgeData , VLS_RegNFAStatusData>(PackSize)
	{
	}

	void VL_RegNFA::Init(Edge* Data)
	{
		Data->Data.Action=vretEpsilon;
		Data->Data.Parameter=0;
		Data->Data.Instructions.Clear();
	}

	void VL_RegNFA::Init(Status* Data)
	{
		Data->Data.FinalStatus=false;
	}

/*********************************************************************************************************
VL_RegDFA
*********************************************************************************************************/

	VL_RegDFA::VL_RegDFA(VInt PackSize):VL_Automaton<VLS_RegDFAEdgeData , VLS_RegDFAStatusData>(PackSize)
	{
	}

	void VL_RegDFA::Init(Edge* Data)
	{
		Data->Data.Action=vretEpsilon;
		Data->Data.Parameter=0;
		Data->Data.NFAEdges.Clear();
	}

	void VL_RegDFA::Init(Status* Data)
	{
		Data->Data.FinalStatus=false;
		Data->Data.NFAStatus.Clear();
	}

/*********************************************************************************************************
VL_RegFAInfo
*********************************************************************************************************/
	
	class RegexpInformationGenerationAlgorithm : public IVL_RegExpRecursiveAlgorithm
	{
	public:
		VL_RegFAInfo* Info;

		void VisitBegin (VL_RegExpCharset* Node , VBool& VisitChildren , VBool& RunEnd){}
		void VisitBegin (VL_RegExpLoop* Node , VBool& VisitChildren , VBool& RunEnd){}

		void VisitBegin (VL_RegExpFunc* Node , VBool& VisitChildren , VBool& RunEnd)
		{
			switch(Node->Content)
			{
			case vrecNamed:
				if(Node->Expression)
				{
					Info->NamedExpression.Add(Node->Pattern,Node->Expression);
				}
				break;
			case vrecStorage:case vrecMatch:
				Info->StorageIDs.AddDistinct(Node->Pattern);
				break;
			}
		}

		void VisitBegin (VL_RegExpSection* Node , VBool& VisitChildren , VBool& RunEnd){}
		void VisitEnd (VL_RegExpCharset* Node){}
		void VisitEnd (VL_RegExpLoop* Node){}
		void VisitEnd (VL_RegExpFunc* Node){}
		void VisitEnd (VL_RegExpSection* Node){}
	};

	VL_RegFAInfo::VL_RegFAInfo(VL_RegExpPtr RegularExpression , VL_RegExpCharClass* PrecalculatedCharClass)
	{
		MainNFA=0;
		Expression=RegularExpression;
		/*计算CharClass*/
		if(PrecalculatedCharClass)
		{
			memcpy(CharClass.CharClass,PrecalculatedCharClass->CharClass,sizeof(CharClass.CharClass));
			CharClass.CharRange=PrecalculatedCharClass->CharRange;
		}
		else
		{
			RegularExpressionCharsetNormalize(Expression,CharClass);
		}
		/*获取命名子表达式列表和存储管道名列表*/
		RegexpInformationGenerationAlgorithm Algorithm;
		Algorithm.Info=this;
		Expression->Visit(&Algorithm);
	}

/*********************************************************************************************************
Finite Status Automaton Closure Algorithm
*********************************************************************************************************/

	VBool RegNFAClosureAlgorithm::EdgeTest(VL_RegNFA::Edge* aEdge)
	{
		switch(ClosureTest)
		{
		case ctAll:
			return true;
		case ctEpsilon:
			return aEdge->Data.Action==vretEpsilon && aEdge->Data.Instructions.GetCount()==0;
		case ctNoRead:
			return aEdge->Data.Action==vretEpsilon;
		}
		return false;
	}

	VBool RegDFAClosureAlgorithm::EdgeTest(VL_RegDFA::Edge* aEdge)
	{
		switch(ClosureTest)
		{
		case ctAll:
			return true;
		}
		return false;
	}

/*********************************************************************************************************
CopyEpsilonNFA
*********************************************************************************************************/

	VL_RegNFA::EpsilonNFA CopyEpsilonNFA(VL_RegNFA* Automaton , VL_RegNFA::EpsilonNFA EpsilonNFA)
	{
		VL_RegNFA::EpsilonNFA Result;
		NFAStatusList SourceStatusList;
		NFAStatusList DestStatusList;
		/*获得EpsilonNFA的所有状态*/
		{
			RegNFAClosureAlgorithm Algorithm;
			Algorithm.ClosureTest=RegNFAClosureAlgorithm::ctAll;
			EpsilonNFA.InitStatus->Visit(&Algorithm);
			SourceStatusList=Algorithm.Path;
		}
		/*复制出足够的状态*/
		for(VInt i=0;i<SourceStatusList.GetCount();i++)
		{
			VL_RegNFA::Status* Status=Automaton->StatusPool.Alloc();
			Status->Data=SourceStatusList[i]->Data;
			DestStatusList.Add(Status);
		}
		/*复制所有的边*/
		for(VInt i=0;i<SourceStatusList.GetCount();i++)
		{
			VL_RegNFA::Status* SourceStart=SourceStatusList[i];
			for(VInt j=0;j<SourceStart->OutEdges.GetCount();j++)
			{
				VL_RegNFA::Edge* SourceEdge=SourceStart->OutEdges[j];
				VInt StartIndex=i;
				VInt EndIndex=SourceStatusList.IndexOf(SourceEdge->End);
				Automaton->Connect(DestStatusList[StartIndex],DestStatusList[EndIndex])->Data=SourceEdge->Data;
			}
		}
		/*找到起始状态和终结状态*/
		Result.InitStatus=DestStatusList[SourceStatusList.IndexOf(EpsilonNFA.InitStatus)];
		Result.FinalStatus=DestStatusList[SourceStatusList.IndexOf(EpsilonNFA.FinalStatus)];
		return Result;
	}

/*********************************************************************************************************
RegularExpressionToENFA
*********************************************************************************************************/
	
	class RegexpENFAAlgorithm : public IVL_RegExpRecursiveAlgorithm
	{
		typedef VL_List<VL_RegNFA::EpsilonNFA , true>							EpsilonNFAList;
		typedef VL_List<VL_AutoPtr<EpsilonNFAList> , false , EpsilonNFAList>	EpsilonNFAStack;
	protected:
		VL_RegNFA*						FAutomaton;
		VL_RegFAInfo*					FInfo;
		VL_RegFAInfo::EpsilonNFABag*	FEpsilonNFABag;
		VInt							FLevelCounter;
		EpsilonNFAStack					FNFAStack;
		VL_AutoPtr<EpsilonNFAList>		FChildrenStack;
		VL_List<VBool , true>			FReferenceMarks;
		VL_List<VInt , true>			FReplicatorIDStack;
		VInt							FUnusedMinimalReplicatorID;

		EpsilonNFAList& GetCurrentNFAList()
		{
			return *(FNFAStack[FNFAStack.GetCount()-1].Object());
		}

		void StoreEpsilonNFA(VL_RegNFA::EpsilonNFA NFA)
		{
			GetCurrentNFAList().Add(NFA);
		}

		void StoreResult()
		{
			VL_RegNFA::EpsilonNFA Result=(*FChildrenStack.Object())[0];
			Result.FinalStatus->Data.FinalStatus=true;
			FEpsilonNFABag->MainEpsilonNFA=Result;
		}

		void EnterNode()
		{
			if(FLevelCounter==0)
			{
				EnterChildren();
			}
			FLevelCounter++;
		}

		void LeaveNode()
		{
			FLevelCounter--;
			if(FLevelCounter==0)
			{
				LeaveChildren();
				StoreResult();
			}
		}

		void EnterChildren()
		{
			FNFAStack.Add(new EpsilonNFAList);
			FChildrenStack=0;
		}

		void LeaveChildren()
		{
			FChildrenStack=FNFAStack.Fetch();
		}
	public:
		RegexpENFAAlgorithm(VL_RegNFA* Automaton , VL_RegFAInfo* Info , VL_RegFAInfo::EpsilonNFABag* EpsilonNFABag)
		{
			FAutomaton=Automaton;
			FInfo=Info;
			FEpsilonNFABag=EpsilonNFABag;
			FLevelCounter=0;
			FUnusedMinimalReplicatorID=0;
		}

		/*******************************************************/

		void VisitBegin (VL_RegExpCharset* Node , VBool& VisitChildren , VBool& RunEnd)
		{
			EnterNode();
			VL_RegNFA::EpsilonNFA ResultNFA;
			ResultNFA.InitStatus=FAutomaton->StatusPool.Alloc();
			ResultNFA.FinalStatus=FAutomaton->StatusPool.Alloc();
			switch(Node->Type)
			{
			case vrecStringBegin:
				{
					VL_RegNFA::Edge* Edge=FAutomaton->Connect(ResultNFA.InitStatus,ResultNFA.FinalStatus);
					Edge->Data.Action=vretStringBegin;
				}
				break;
			case vrecStringEnd:
				{
					VL_RegNFA::Edge* Edge=FAutomaton->Connect(ResultNFA.InitStatus,ResultNFA.FinalStatus);
					Edge->Data.Action=vretStringEnd;
				}
				break;
			case vrecCharSet:
				/*遍历所有的CharClass*/
				for(VInt i=0;i<FInfo->CharClass.CharRange.GetCount();i++)
				{
					/*检查当前的CharClass有没有被当前的节点包含*/
					VWChar Start=FInfo->CharClass.CharRange[i].Key;
					VBool Included=false;
					for(VInt j=0;j<Node->CharSet.GetCount();j++)
					{
						if((Node->CharSet[j].Key<=Start)&&(Node->CharSet[j].Value>=Start))
						{
							Included=true;
							break;
						}
					}
					/*如果有包含则在初始节点和结束节点之间添加一条带有当前CharClass信息的边*/
					if(Included)
					{
						VL_RegNFA::Edge* Edge=FAutomaton->Connect(ResultNFA.InitStatus,ResultNFA.FinalStatus);
						Edge->Data.Action=vretCharset;
						Edge->Data.Parameter=i;
						/*添加Pass指令*/
						VLS_RegEdgeInstruction Instruction;
						Instruction.Instruction=vreiPass;
						Instruction.Parameter=1;
						Edge->Data.Instructions.Add(Instruction);
					}
				}
				break;
			}
			StoreEpsilonNFA(ResultNFA);
		}

		void VisitEnd (VL_RegExpCharset* Node)
		{
			LeaveNode();
		}

		/*******************************************************/

		void VisitBegin (VL_RegExpLoop* Node , VBool& VisitChildren , VBool& RunEnd)
		{
			EnterNode();
			FReplicatorIDStack.Add(FUnusedMinimalReplicatorID++);
			EnterChildren();
		}

		void VisitEnd (VL_RegExpLoop* Node)
		{
			LeaveChildren();
			/*获得子节点的Epsilon-NFA和当前循环的循环ID*/
			VInt CurrentReplicatorID=FReplicatorIDStack.Fetch();
			/*构造必要的子图的副本*/
			VL_List<VL_RegNFA::EpsilonNFA , true> SubNFAList;
			{
				VL_RegNFA::EpsilonNFA SubNFA=(*FChildrenStack.Object())[0];
				VInt Count=Node->Finite?Node->End:Node->Begin+1;
				SubNFAList.Add(SubNFA);
				for(VInt i=1;i<Count;i++)
				{
					SubNFAList.Add(CopyEpsilonNFA(FAutomaton,SubNFA));
				}
			}
			VL_RegNFA::EpsilonNFA ResultNFA;
			/*构造起始循环部分*/
			for(VInt i=0;i<Node->Begin;i++)
			{
				if(i==0)
				{
					ResultNFA=SubNFAList[i];
				}
				else
				{
					FAutomaton->Connect(ResultNFA.FinalStatus,SubNFAList[i].InitStatus);
					ResultNFA.FinalStatus=SubNFAList[i].FinalStatus;
				}
			};
			if(Node->Finite)
			{
				/*构造有限循环的结束部分*/
				for(VInt i=Node->Begin;i<Node->End;i++)
				{
					if(Node->Short)
					{
						FAutomaton->Connect(SubNFAList[i].InitStatus,SubNFAList[Node->End-1].FinalStatus);
					}
					if(ResultNFA.FinalStatus)
					{
						FAutomaton->Connect(ResultNFA.FinalStatus,SubNFAList[i].InitStatus);
						ResultNFA.FinalStatus=SubNFAList[i].FinalStatus;
					}
					else
					{
						ResultNFA=SubNFAList[i];
					}
					if(!Node->Short)
					{
						FAutomaton->Connect(SubNFAList[i].InitStatus,SubNFAList[Node->End-1].FinalStatus);
					}
				}
			}
			else
			{
				VL_RegNFA::Status* TailInit=0;
				VL_RegNFA::Status* TailFinal=0;
				if(!ResultNFA.FinalStatus)
				{
					TailInit=ResultNFA.InitStatus=FAutomaton->StatusPool.Alloc();
					TailFinal=ResultNFA.FinalStatus=FAutomaton->StatusPool.Alloc();
				}
				else
				{
					TailInit=ResultNFA.FinalStatus;
					TailFinal=ResultNFA.FinalStatus=FAutomaton->StatusPool.Alloc();
				}
				/*构造无限循环的结束部分*/
				if(Node->Short)
				{
					FAutomaton->Connect(TailInit,TailFinal);
				}
				FAutomaton->Connect(TailInit,SubNFAList[Node->Begin].InitStatus);
				FAutomaton->Connect(SubNFAList[Node->Begin].FinalStatus,TailInit);
				if(!Node->Short)
				{
					FAutomaton->Connect(TailInit,TailFinal);
				}
			}
			/*构造循环头尾*/
			VL_RegNFA::Status* InitStatus=FAutomaton->StatusPool.Alloc();
			VL_RegNFA::Status* FinalStatus=FAutomaton->StatusPool.Alloc();
			{
				VL_RegNFA::Edge* Edge=FAutomaton->Connect(InitStatus,ResultNFA.InitStatus);
				/*添加LongLoop/ShortLoop指令*/
				VLS_RegEdgeInstruction Instruction;
				Instruction.Instruction=Node->Short?vreiShortLoop:vreiLongLoop;
				Instruction.Parameter=CurrentReplicatorID;
				Edge->Data.Instructions.Add(Instruction);
			}
			{
				VL_RegNFA::Edge* Edge=FAutomaton->Connect(ResultNFA.FinalStatus,FinalStatus);
				/*添加EndLoop指令*/
				VLS_RegEdgeInstruction Instruction;
				Instruction.Instruction=vreiEndLoop;
				Instruction.Parameter=0;
				Edge->Data.Instructions.Add(Instruction);
			}
			ResultNFA.InitStatus=InitStatus;
			ResultNFA.FinalStatus=FinalStatus;
			StoreEpsilonNFA(ResultNFA);
			LeaveNode();
		}

		/*******************************************************/

		void VisitBegin (VL_RegExpFunc* Node , VBool& VisitChildren , VBool& RunEnd)
		{
			EnterNode();
			if(Node->Content==vrecNamed)
			{
				/*如果当前节点的子节点为引用则将被引用的表达式复制进当前节点*/
				VBool Mark=Node->Expression;
				FReferenceMarks.Add(Mark);
				if(!Mark)
				{
					Node->Expression=FInfo->NamedExpression[Node->Pattern];
				}
			}
			if(Node->Content!=vrecMatch)
			{
				EnterChildren();
			}
		}

		void VisitEnd (VL_RegExpFunc* Node)
		{
			if(Node->Content!=vrecMatch)
			{
				LeaveChildren();
			}
			/*处理子节点的Epsilon-NFA*/
			VL_RegNFA::EpsilonNFA SubNFA;
			switch(Node->Content)
			{
			case vrecNamed:case vrecNormal:
				SubNFA=(*FChildrenStack.Object())[0];
				break;
			case vrecStorage:
				{
					VL_RegNFA::EpsilonNFA NFA=(*FChildrenStack.Object())[0];
					SubNFA.InitStatus=FAutomaton->StatusPool.Alloc();
					SubNFA.FinalStatus=FAutomaton->StatusPool.Alloc();
					{
						VL_RegNFA::Edge* Edge=FAutomaton->Connect(SubNFA.InitStatus,NFA.InitStatus);
						/*添加Storage指令*/
						VLS_RegEdgeInstruction Instruction;
						Instruction.Instruction=vreiStorage;
						Instruction.Parameter=FInfo->StorageIDs.IndexOf(Node->Pattern);
						Edge->Data.Instructions.Add(Instruction);
					}
					{
						VL_RegNFA::Edge* Edge=FAutomaton->Connect(NFA.FinalStatus,SubNFA.FinalStatus);
						/*添加End指令*/
						VLS_RegEdgeInstruction Instruction;
						Instruction.Instruction=vreiEnd;
						Instruction.Parameter=0;
						Edge->Data.Instructions.Add(Instruction);
					}
				}
				break;
			case vrecMatch:
				{
					SubNFA.InitStatus=FAutomaton->StatusPool.Alloc();
					SubNFA.FinalStatus=FAutomaton->StatusPool.Alloc();
					VL_RegNFA::Edge* Edge=FAutomaton->Connect(SubNFA.InitStatus,SubNFA.FinalStatus);
					Edge->Data.Action=vretMatch;
					Edge->Data.Parameter=FInfo->StorageIDs.IndexOf(Node->Pattern);
					/*添加Match指令*/
					VLS_RegEdgeInstruction Instruction;
					Instruction.Instruction=vreiMatch;
					Instruction.Parameter=Edge->Data.Parameter;
					Edge->Data.Instructions.Add(Instruction);
				}
				break;
			}
			/*构造节点*/
			VL_RegNFA::EpsilonNFA ResultNFA;
			switch(Node->Function)
			{
			case vrefCatch:
				{
					ResultNFA.InitStatus=FAutomaton->StatusPool.Alloc();
					ResultNFA.FinalStatus=FAutomaton->StatusPool.Alloc();
					{
						VL_RegNFA::Edge* Edge=FAutomaton->Connect(ResultNFA.InitStatus,SubNFA.InitStatus);
						/*添加Catch指令*/
						VLS_RegEdgeInstruction Instruction;
						Instruction.Instruction=vreiCatch;
						Instruction.Parameter=FInfo->StorageIDs.IndexOf(Node->Pattern);
						Edge->Data.Instructions.Add(Instruction);
					}
					{
						VL_RegNFA::Edge* Edge=FAutomaton->Connect(SubNFA.FinalStatus,ResultNFA.FinalStatus);
						/*添加End指令*/
						VLS_RegEdgeInstruction Instruction;
						Instruction.Instruction=vreiEnd;
						Instruction.Parameter=0;
						Edge->Data.Instructions.Add(Instruction);
					}
				}
				break;
			case vrefUncatch:
				ResultNFA=SubNFA;
				break;
			case vrefPositive:
				{
					/*把子表达式的Epsilon-NFA保存到附加NFA列表中*/
					VInt SubID=FEpsilonNFABag->SubEpsilonNFAs.GetCount();
					SubNFA.FinalStatus->Data.FinalStatus=true;
					FEpsilonNFABag->SubEpsilonNFAs.Add(SubNFA);
					/*构造Positive连接*/
					ResultNFA.InitStatus=FAutomaton->StatusPool.Alloc();
					ResultNFA.FinalStatus=FAutomaton->StatusPool.Alloc();
					VL_RegNFA::Edge* Edge=FAutomaton->Connect(ResultNFA.InitStatus,ResultNFA.FinalStatus);
					Edge->Data.Action=vretPositive;
					Edge->Data.Parameter=SubID;
				}
				break;
			case vrefNegative:
				{
					/*把子表达式的Epsilon-NFA保存到附加NFA列表中*/
					VInt SubID=FEpsilonNFABag->SubEpsilonNFAs.GetCount();
					SubNFA.FinalStatus->Data.FinalStatus=true;
					FEpsilonNFABag->SubEpsilonNFAs.Add(SubNFA);
					/*构造Negative连接*/
					ResultNFA.InitStatus=FAutomaton->StatusPool.Alloc();
					ResultNFA.FinalStatus=FAutomaton->StatusPool.Alloc();
					VL_RegNFA::Edge* Edge=FAutomaton->Connect(ResultNFA.InitStatus,ResultNFA.FinalStatus);
					Edge->Data.Action=vretNegative;
					Edge->Data.Parameter=SubID;
				}
				break;
			}
			StoreEpsilonNFA(ResultNFA);
			if(Node->Content==vrecNamed && !FReferenceMarks.Fetch())
			{
				/*如果当前节点的子节点为引用则令子节点为空，恢复表达式状态*/
				Node->Expression=0;
			}
			LeaveNode();
		}

		/*******************************************************/

		void VisitBegin (VL_RegExpSection* Node , VBool& VisitChildren , VBool& RunEnd)
		{
			EnterNode();
			EnterChildren();
		}

		void VisitEnd (VL_RegExpSection* Node)
		{
			LeaveChildren();
			switch(Node->Type)
			{
			case vresSequence:
				{
					/*把每一个子节点的Epsilon-NFA用Epsilon转换串起来*/
					VL_RegNFA::EpsilonNFA ResultNFA=(*FChildrenStack.Object())[0];
					for(VInt i=1;i<FChildrenStack->GetCount();i++)
					{
						VL_RegNFA::EpsilonNFA NFA=(*FChildrenStack.Object())[i];
						FAutomaton->Connect(ResultNFA.FinalStatus,NFA.InitStatus);
						ResultNFA.FinalStatus=NFA.FinalStatus;
					}
					StoreEpsilonNFA(ResultNFA);
				}
				break;
			case vresBranch:
				{
					VL_RegNFA::EpsilonNFA ResultNFA;
					ResultNFA.InitStatus=FAutomaton->StatusPool.Alloc();
					ResultNFA.FinalStatus=FAutomaton->StatusPool.Alloc();
					/*把当前NFA的初始节点和结束节点跟每一个子节点的Epsilon-NFA的初始节点和结束节点连接起来*/
					for(VInt i=0;i<FChildrenStack->GetCount();i++)
					{
						VL_RegNFA::EpsilonNFA NFA=(*FChildrenStack.Object())[i];
						FAutomaton->Connect(ResultNFA.InitStatus,NFA.InitStatus);
						FAutomaton->Connect(NFA.FinalStatus,ResultNFA.FinalStatus);
					}
					StoreEpsilonNFA(ResultNFA);
				}
				break;
			}
			LeaveNode();
		}

	};

	VL_RegFAInfo::EpsilonNFABag RegularExpressionToEpsilonNFA(VL_RegNFA* Automaton , VL_RegFAInfo* Info)
	{
		VL_RegFAInfo::EpsilonNFABag Result;
		RegexpENFAAlgorithm Algorithm(Automaton,Info,&Result);
		Info->Expression->Visit(&Algorithm);
		return Result;
	}

/*********************************************************************************************************
EplisonNFAToNFA
*********************************************************************************************************/

	void StaticStatus(VL_RegNFA::Status* Start , NFAStatusList& All , NFAStatusList& Unaccessable , VBool (*EdgeTest)(VL_RegNFA::Edge*))
	{
		/*获得NFA的所有节点*/
		RegNFAClosureAlgorithm Algorithm;
		Algorithm.ClosureTest=RegNFAClosureAlgorithm::ctAll;
		Start->Visit(&Algorithm);
		All=Algorithm.Path;

		/*把只能通过EdgeTest访问的节点添加进不可达节点列表*/
		for(VInt i=1;i<All.GetCount();i++)
		{
			VBool AllEpsilon=true;
			VL_RegNFA::Status* Status=All[i];
			for(VInt j=0;j<Status->InEdges.GetCount();j++)
			{
				VL_RegNFA::Edge* Edge=Status->InEdges[j];
				if(!EdgeTest(Edge))
				{
					AllEpsilon=false;
					break;
				}
			}
			if(AllEpsilon)
			{
				Unaccessable.Add(Status);
			}
		}
	}

	void RemoveStatus(VL_RegNFA* Automaton , NFAStatusList& All , NFAStatusList& Unaccessable , VBool (*EdgeTest)(VL_RegNFA::Edge*))
	{
		/*删除EdgeTest检查的边*/
		for(VInt i=0;i<All.GetCount();i++)
		{
			VL_RegNFA::Status* Status=All[i];
			for(VInt j=Status->OutEdges.GetCount()-1;j>=0;j--)
			{
				VL_RegNFA::Edge* Edge=Status->OutEdges[j];
				if(EdgeTest(Edge))
				{
					Automaton->Disconnect(Status,Edge);
				}
			}
		}
		/*删除不可达节点*/
		for(VInt i=0;i<Unaccessable.GetCount();i++)
		{
			All.Delete(All.IndexOf(Unaccessable[i]));
			Automaton->Disconnect(Unaccessable[i]);
		}
	}

	VBool IsEpsilon(VL_RegNFA::Edge* Edge)
	{
		return Edge->Data.Action==vretEpsilon && Edge->Data.Instructions.GetCount()==0;
	}

	VL_RegNFA::Status* EpsilonNFAToNFA(VL_RegNFA* Automaton , VL_RegNFA::EpsilonNFA NFA)
	{
		NFAStatusList StatusList;
		NFAStatusList UnaccessableList;
		StaticStatus(NFA.InitStatus,StatusList,UnaccessableList,IsEpsilon);
		/*添加所有因为Epsilon边而产生的新边*/
		for(VInt i=0;i<StatusList.GetCount();i++)
		{
			VL_RegNFA::Status* Status=StatusList[i];
			if(!UnaccessableList.Exists(Status))
			{
				/*获得当前节点的Epsilon闭包*/
				VL_List<VL_RegNFA::Status* , true> EpsilonClosure;
				{
					RegNFAClosureAlgorithm Algorithm;
					Algorithm.ClosureTest=RegNFAClosureAlgorithm::ctEpsilon;
					Status->Visit(&Algorithm);
					EpsilonClosure=Algorithm.Path;
				}
				/*获得闭包中的所有边*/
				for(VInt j=1;j<EpsilonClosure.GetCount();j++)
				{
					/*如果闭包中包含有终结状态，则将当前状态设置为终结状态*/
					VL_RegNFA::Status* EcStatus=EpsilonClosure[j];
					Status->Data.FinalStatus|=EcStatus->Data.FinalStatus;
					/*遍历闭包中的状态的所有边*/
					for(VInt k=0;k<EcStatus->OutEdges.GetCount();k++)
					{
						VL_RegNFA::Edge* Edge=EcStatus->OutEdges[k];
						/*如果当前边不是Epsilon边，则把这条边添加到当前状态上*/
						if(Edge->Data.Action!=vretEpsilon || Edge->Data.Instructions.GetCount()!=0)
						{
							Automaton->Connect(Status,Edge->End)->Data=Edge->Data;
						}
					}
				}
			}
		}
		/*删除Epsilon边和不可达节点*/
		RemoveStatus(Automaton,StatusList,UnaccessableList,IsEpsilon);
		UnaccessableList.Clear();
		/*遍历每一个剩下来的节点，检查有没有重复的转换*/
		for(VInt i=0;i<StatusList.GetCount();i++)
		{
			VL_RegNFA::Status* Status=StatusList[i];
			/*反向遍历每一条边，并进行检查*/
			for(VInt j=Status->OutEdges.GetCount()-1;j>=0;j--)
			{
				VL_RegNFA::Edge* Edge=Status->OutEdges[j];
				for(VInt k=0;k<j;k++)
				{
					VL_RegNFA::Edge* EdgeAbove=Status->OutEdges[k];
					/*检查两条边是否相同*/
					if(	(Edge->End==EdgeAbove->End)&&
						(Edge->Data.Action==EdgeAbove->Data.Action)&&
						(Edge->Data.Parameter==EdgeAbove->Data.Parameter)&&
						(Edge->Data.Instructions.GetCount()==EdgeAbove->Data.Instructions.GetCount())
						)
					{
						VBool Equivalent=true;
						/*检查两条边中的指令是否一致*/
						for(VInt l=0;l<Edge->Data.Instructions.GetCount();l++)
						{
							if(	(Edge->Data.Instructions[l].Instruction!=EdgeAbove->Data.Instructions[l].Instruction)||
								(Edge->Data.Instructions[l].Parameter!=EdgeAbove->Data.Instructions[l].Parameter)
								)
							{
								Equivalent=false;
								break;
							}
						}
						/*如果两条边相同则删除其中一条边*/
						if(Equivalent)
						{
							Automaton->Disconnect(Status,Edge);
						}
					}
				}
			}
		}
		return NFA.InitStatus;
	}

/*********************************************************************************************************
RegularExpressionToNFA
*********************************************************************************************************/

	VBool IsNoRead(VL_RegNFA::Edge* Edge)
	{
		return (Edge->Data.Action==vretEpsilon) && (Edge->Data.Instructions.GetCount()<=1);
	}

	void AddCompressedEdge(VL_RegNFA* Automaton , VL_RegNFA::Status* Start , VL_RegNFA::Status* InsStart , VL_RegEdgeInsList& InsList , VInt Level , VBool IsFromInitialStatus)
	{
		/*如果当前访问的状态时终结状态*/
		if(Start->Data.FinalStatus && InsList.GetCount()>0)
		{
			/*遍历每一条输入到InsStart的边*/
			for(VInt i=InsStart->InEdges.GetCount()-1;i>=0;i--)
			{
				/*从每一条边的起始状态添加一条指向终结状态的相同属性的边，指令集为那条边的指令集加上InsList*/
				VL_RegNFA::Edge* OldEdge=InsStart->InEdges[i];
				VInt OldEdgeIndex=OldEdge->Start->OutEdges.IndexOf(OldEdge);
				VInt NewEdgeIndex=OldEdge->Start->OutEdges.GetCount();
				/*构造增量的新边，并把边挪到被增量的边的下方*/
				VL_RegNFA::Edge* NewEdge=Automaton->Connect(OldEdge->Start,Start);
				OldEdge->Start->OutEdges.Delete(NewEdgeIndex);
				OldEdge->Start->OutEdges.Insert(OldEdgeIndex+1,NewEdge);
				NewEdge->Data=OldEdge->Data;
				NewEdge->Data.Instructions.Add(InsList.Whole());
			}
			if(IsFromInitialStatus)
			{
				/*检查将要添加的边是否存在*/
				VBool Found=false;
				for(VInt i=0;i<InsStart->OutEdges.GetCount();i++)
				{
					VL_RegNFA::Edge* Edge=InsStart->OutEdges[i];
					if(	(Edge->End==Start) &&
						(Edge->Data.Action==vretEpsilon) &&
						(Edge->Data.Instructions.GetCount()==InsList.GetCount())
						)
					{
						for(VInt j=0;j<Edge->Data.Instructions.GetCount();j++)
						{
							if(	(Edge->Data.Instructions[j].Instruction==InsList[j].Instruction) &&
								(Edge->Data.Instructions[j].Parameter==InsList[j].Parameter)
								)
							{
								Found=true;
								break;
							}
						}
					}
				}
				if(!Found)
				{
					InsStart->Data.FinalStatus=true;
					Automaton->Connect(InsStart,Start)->Data.Instructions=InsList;
				}
			}
		}
		/*遍历所有边*/
		VInt OldEdgeCount=Start->OutEdges.GetCount();
		for(VInt i=0,ScannedEdgeCount=0;ScannedEdgeCount<OldEdgeCount;i++,ScannedEdgeCount++)
		{
			VL_RegNFA::Edge* Edge=Start->OutEdges[i];
			/*如果是没有输入的边，则进行深度优先搜索*/
			if(Edge->Data.Action==vretEpsilon)
			{
				InsList.Add(Edge->Data.Instructions.Whole());
				AddCompressedEdge(Automaton,Edge->End,InsStart,InsList,Level+1,IsFromInitialStatus);
				VInt Count=Edge->Data.Instructions.GetCount();
				InsList.Delete(InsList.GetCount()-Count,Count);
			}
			/*否则，如果指令列表的起始状态跟自身不一致，则添加边*/
			else if(Start!=InsStart)
			{
				VL_RegNFA::Edge* NewEdge=Automaton->Connect(InsStart,Edge->End);
				NewEdge->Data.Action=Edge->Data.Action;
				NewEdge->Data.Parameter=Edge->Data.Parameter;
				NewEdge->Data.Instructions=InsList;
				NewEdge->Data.Instructions.Add(Edge->Data.Instructions.Whole());
			}
			/*最后，这条边是不需要附加更多的边的，把这条边移动到最后*/
			else
			{
				Start->OutEdges.Delete(i);
				Start->OutEdges.Add(Edge);
				i--;
			}
		}
	}

	VL_RegNFA::Status* CompressNFA(VL_RegNFA* Automaton , VL_RegNFA::Status* NFA)
	{
		NFAStatusList		StatusList;
		NFAStatusList		UnaccessableList;
		VL_RegEdgeInsList	TempInsList;
		StaticStatus(NFA,StatusList,UnaccessableList,IsNoRead);
		/*压缩边*/
		for(VInt i=0;i<StatusList.GetCount();i++)
		{
			VL_RegNFA::Status* Status=StatusList[i];
			if(!UnaccessableList.Exists(Status))
			{
				AddCompressedEdge(Automaton,Status,Status,TempInsList,0,i==0);
			}
		}
		/*从不可达状态中删除终结状态*/
		for(VInt i=UnaccessableList.GetCount()-1;i>=0;i--)
		{
			if(UnaccessableList[i]->Data.FinalStatus)
			{
				UnaccessableList.Delete(i);
			}
		}
		/*删除多余的边和状态*/
		RemoveStatus(Automaton,StatusList,UnaccessableList,IsNoRead);
		return NFA;
	}

/*********************************************************************************************************
NFAToDFA
*********************************************************************************************************/

	struct TempEdge
	{
		VLE_RegEdgeType								Action;
		VInt										Parameter;
		VL_List<VL_RegNFA::Edge* , true>			NFAEdgeList;
		VL_OrderedList<VL_RegNFA::Status* , true>	NFAStatusList;
	};

	VL_RegDFA::Status* CompressedNFAToDFA(VL_RegDFA* Automaton , VL_RegNFA::Status* NFA)
	{
		VL_List<VL_RegDFA::Status* , true>	DFAStatusList;
		VL_List<VL_RegNFA::Status* , true>	NFAStatusList;
		/*获得NFA的所有状态*/
		{
			RegNFAClosureAlgorithm Algorithm;
			Algorithm.ClosureTest=RegNFAClosureAlgorithm::ctAll;
			NFA->Visit(&Algorithm);
			NFAStatusList=Algorithm.Path;
		}
		/*构造DFA的起始状态*/
		{
			VL_RegDFA::Status* InitStatus=Automaton->StatusPool.Alloc();
			InitStatus->Data.FinalStatus=NFA->Data.FinalStatus;
			InitStatus->Data.NFAStatus.Add(NFA);
			DFAStatusList.Add(InitStatus);
		}
		/*构造NFA*/
		for(VInt i=0;i<DFAStatusList.GetCount();i++)
		{
			VL_RegDFA::Status* CurrentDFAStatus=DFAStatusList[i];
			VL_List<VL_AutoPtr<TempEdge> , false , TempEdge*> TempDFAEdges;
			/*遍历当前状态包含的所有NFA状态的所有边*/
			for(VInt j=0;j<CurrentDFAStatus->Data.NFAStatus.GetCount();j++)
			{
				VL_RegNFA::Status* CurrentNFAStatus=CurrentDFAStatus->Data.NFAStatus[j];
				for(VInt k=0;k<CurrentNFAStatus->OutEdges.GetCount();k++)
				{
					VL_RegNFA::Edge* CurrentNFAEdge=CurrentNFAStatus->OutEdges[k];
					/*检查当前边是否符合TempDFAEdge中的某条记录*/
					TempEdge* CurrentDFAEdge=0;
					for(VInt l=0;l<TempDFAEdges.GetCount();l++)
					{
						TempEdge* DFAEdge=TempDFAEdges[l].Object();
						if(	(CurrentNFAEdge->Data.Action==DFAEdge->Action) &&
							(CurrentNFAEdge->Data.Parameter==DFAEdge->Parameter))
						{
							CurrentDFAEdge=DFAEdge;
							break;
						}
					}
					/*如果不存在符合的记录，则创建一个新记录*/
					if(!CurrentDFAEdge)
					{
						CurrentDFAEdge=new TempEdge;
						TempDFAEdges.Add(CurrentDFAEdge);
						CurrentDFAEdge->Action=CurrentNFAEdge->Data.Action;
						CurrentDFAEdge->Parameter=CurrentNFAEdge->Data.Parameter;
					}
					/*把当前边添加进符合条件的记录*/
					CurrentDFAEdge->NFAEdgeList.Add(CurrentNFAEdge);
					CurrentDFAEdge->NFAStatusList.AddDistinct(CurrentNFAEdge->End);
				}
			}
			/*遍历所有得到的DFA边记录*/
			for(VInt j=0;j<TempDFAEdges.GetCount();j++)
			{
				TempEdge* CurrentDFAEdge=TempDFAEdges[j].Object();
				/*检查当前DFA边记录指向的DFA状态是否存在*/
				VL_RegDFA::Status* DestDFAStatus=0;
				for(VInt k=0;k<DFAStatusList.GetCount();k++)
				{
					VL_RegDFA::Status* DFAStatus=DFAStatusList[k];
					if(CurrentDFAEdge->NFAStatusList.GetCount()==DFAStatus->Data.NFAStatus.GetCount())
					{
						VBool Correct=true;
						for(VInt l=0;l<CurrentDFAEdge->NFAStatusList.GetCount();l++)
						{
							if(CurrentDFAEdge->NFAStatusList[l]!=DFAStatus->Data.NFAStatus[l])
							{
								Correct=false;
								break;
							}
						}
						if(Correct)
						{
							DestDFAStatus=DFAStatus;
							break;
						}
					}
				}
				/*如果不存在相应的DFA状态，则创建一个新的DFA状态*/
				if(!DestDFAStatus)
				{
					DestDFAStatus=Automaton->StatusPool.Alloc();
					DestDFAStatus->Data.NFAStatus=CurrentDFAEdge->NFAStatusList;
					/*检查新的状态有没有包含终结状态*/
					for(VInt k=0;k<DestDFAStatus->Data.NFAStatus.GetCount();k++)
					{
						DestDFAStatus->Data.FinalStatus|=DestDFAStatus->Data.NFAStatus[k]->Data.FinalStatus;
					}
					DFAStatusList.Add(DestDFAStatus);
				}
				/*创建相应的边*/
				VL_RegDFA::Edge* DFAEdge=Automaton->Connect(CurrentDFAStatus,DestDFAStatus);
				DFAEdge->Data.NFAEdges=CurrentDFAEdge->NFAEdgeList;
				DFAEdge->Data.Action=CurrentDFAEdge->Action;
				DFAEdge->Data.Parameter=CurrentDFAEdge->Parameter;
			}
		}
		return DFAStatusList[0];
	}

/*********************************************************************************************************
NFAToString
*********************************************************************************************************/

	VUnicodeString NFAToString(VL_RegFAInfo* Info , VL_RegNFA::Status* NFA)
	{
		NFAStatusList StatusList;
		/*获得NFA的所有状态*/
		{
			RegNFAClosureAlgorithm Algorithm;
			Algorithm.ClosureTest=RegNFAClosureAlgorithm::ctAll;
			NFA->Visit(&Algorithm);
			StatusList=Algorithm.Path;
		}
		VUnicodeString Result;
		/*产生状态机描述*/
		for(VInt i=0;i<StatusList.GetCount();i++)
		{
			/*构造状态描述*/
			VL_RegNFA::Status* CurrentStatus=StatusList[i];
			if(CurrentStatus->Data.FinalStatus)
			{
				Result+=L"  Status ["+VUnicodeString(i)+L"][Final]\r\n";
			}
			else
			{
				Result+=L"  Status ["+VUnicodeString(i)+L"]\r\n";
			}
			for(VInt j=0;j<CurrentStatus->OutEdges.GetCount();j++)
			{
				/*构造边描述*/
				VL_RegNFA::Edge* CurrentEdge=CurrentStatus->OutEdges[j];
				Result+=L"    Edge to "+VUnicodeString(StatusList.IndexOf(CurrentEdge->End))+L"\r\n";
				Result+=L"      Action = ";
				switch(CurrentEdge->Data.Action)
				{
				case vretEpsilon:
					Result+=L"Epsilon";
					break;
				case vretMatch:
					Result+=L"Match Storage:"+Info->StorageIDs[CurrentEdge->Data.Parameter];
					break;
				case vretCharset:
					Result+=L"Match Charset:"+CharRangeToString(Info->CharClass.CharRange[CurrentEdge->Data.Parameter]);
					break;
				case vretStringBegin:
					Result+=L"Match String Begin";
					break;
				case vretStringEnd:
					Result+=L"Match String End";
					break;
				case vretPositive:
					Result+=L"Positive Look Ahead NFAList["+VUnicodeString(CurrentEdge->Data.Parameter)+L"]";
					break;
				case vretNegative:
					Result+=L"Negative Look Ahead NFAList["+VUnicodeString(CurrentEdge->Data.Parameter)+L"]";
					break;
				}
				Result+=L"\r\n";
				if(CurrentEdge->Data.Instructions.GetCount())
				{
					Result+=L"      Instructions\r\n";
					for(VInt k=0;k<CurrentEdge->Data.Instructions.GetCount();k++)
					{
						VLS_RegEdgeInstruction Ins=CurrentEdge->Data.Instructions[k];
						switch(Ins.Instruction)
						{
						case vreiCatch:
							Result+=L"        Catch";
							break;
						case vreiStorage:
							Result+=L"        Storage "+Info->StorageIDs[Ins.Parameter];
							break;
						case vreiPass:
							Result+=L"        Pass "+VUnicodeString(Ins.Parameter)+L" Chars";
							break;
						case vreiMatch:
							Result+=L"        Match "+Info->StorageIDs[Ins.Parameter];
							break;
						case vreiEnd:
							Result+=L"        End";
							break;
						case vreiLongLoop:
							Result+=L"        Long Loop ["+VUnicodeString(Ins.Parameter)+L"]";
							break;
						case vreiShortLoop:
							Result+=L"        Short Loop ["+VUnicodeString(Ins.Parameter)+L"]";
							break;
						case vreiEndLoop:
							Result+=L"        End Loop";
							break;
						}
						Result+=L"\r\n";
					}
				}
			}
		}
		return Result;
	}

/*********************************************************************************************************
DFAToString
*********************************************************************************************************/

	VUnicodeString DFAToString(VL_List<VUnicodeString , false>* StorageIDs , VL_RegExpCharClass* CharClass , VL_RegDFA::Status* DFA)
	{
		DFAStatusList DFAStatusList;
		NFAStatusList NFAStatusList;
		/*获得DFA的所有状态*/
		{
			RegDFAClosureAlgorithm Algorithm;
			Algorithm.ClosureTest=RegDFAClosureAlgorithm::ctAll;
			DFA->Visit(&Algorithm);
			DFAStatusList=Algorithm.Path;
		}
		/*获得NFA的所有状态*/
		{
			RegNFAClosureAlgorithm Algorithm;
			Algorithm.ClosureTest=RegNFAClosureAlgorithm::ctAll;
			DFA->Data.NFAStatus[0]->Visit(&Algorithm);
			NFAStatusList=Algorithm.Path;
		}
		VUnicodeString Result;
		/*产生状态机描述*/
		for(VInt i=0;i<DFAStatusList.GetCount();i++)
		{
			/*构造状态描述*/
			VL_RegDFA::Status* CurrentStatus=DFAStatusList[i];
			if(CurrentStatus->Data.FinalStatus)
			{
				Result+=L"  Status ["+VUnicodeString(i)+L"][Final]";
			}
			else
			{
				Result+=L"  Status ["+VUnicodeString(i)+L"]";
			}
			Result+=L" NFAStatus";
			{
				VL_OrderedList<VInt , true> IDs;
				for(VInt j=0;j<CurrentStatus->Data.NFAStatus.GetCount();j++)
				{
					IDs.Add(NFAStatusList.IndexOf(CurrentStatus->Data.NFAStatus[j]));
				}
				for(VInt j=0;j<IDs.GetCount();j++)
				{
					Result+=L"["+VUnicodeString(IDs[j])+L"]";
				}
			}
			Result+=L"\r\n";
			for(VInt j=0;j<CurrentStatus->OutEdges.GetCount();j++)
			{
				/*构造边描述*/
				VL_RegDFA::Edge* CurrentEdge=CurrentStatus->OutEdges[j];
				Result+=L"    Edge to "+VUnicodeString(DFAStatusList.IndexOf(CurrentEdge->End))+L"\r\n";
				Result+=L"      Action = ";
				switch(CurrentEdge->Data.Action)
				{
				case vretEpsilon:
					Result+=L"Epsilon";
					break;
				case vretMatch:
					Result+=L"Match Storage:"+(*StorageIDs)[CurrentEdge->Data.Parameter];
					break;
				case vretCharset:
					Result+=L"Match Charset:"+CharRangeToString(CharClass->CharRange[CurrentEdge->Data.Parameter]);
					break;
				case vretStringBegin:
					Result+=L"Match String Begin";
					break;
				case vretStringEnd:
					Result+=L"Match String End";
					break;
				case vretPositive:
					Result+=L"Positive Look Ahead DFAList["+VUnicodeString(CurrentEdge->Data.Parameter)+L"]";
					break;
				case vretNegative:
					Result+=L"Negative Look Ahead DFAList["+VUnicodeString(CurrentEdge->Data.Parameter)+L"]";
					break;
				}
				Result+=L"\r\n";
			}
		}
		return Result;
	}

	VUnicodeString DFAToString(VL_RegFAInfo* Info , VL_RegDFA::Status* DFA)
	{
		return DFAToString(&Info->StorageIDs , &Info->CharClass , DFA);
	}

/*********************************************************************************************************
RegularExpressionToNFA
*********************************************************************************************************/

	void RegularExpressionToNFA(VL_RegNFA* Automaton , VL_RegFAInfo* Info , VBool CompressMain)
	{
		/*把表达式转换为EpsilonNFA*/
		VL_RegFAInfo::EpsilonNFABag EpsilonNFABag=RegularExpressionToEpsilonNFA(Automaton,Info);
		/*把每一个EpsilonNFA转换为相应的NFA并存储起来*/
		Info->MainNFA=EpsilonNFAToNFA(Automaton,EpsilonNFABag.MainEpsilonNFA);
		if(CompressMain)
		{
			Info->MainNFA=CompressNFA(Automaton,Info->MainNFA);
		}
		for(VInt i=0;i<EpsilonNFABag.SubEpsilonNFAs.GetCount();i++)
		{
			Info->SubNFAs.Add(CompressNFA(Automaton,EpsilonNFAToNFA(Automaton,EpsilonNFABag.SubEpsilonNFAs[i])));
		}
	}

	}
}