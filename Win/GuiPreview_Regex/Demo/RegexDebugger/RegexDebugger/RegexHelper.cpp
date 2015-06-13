#include "RegexHelper.h"

/*��������ʽ�﷨���ڵ�ת��Ϊ�ַ�����Ϣ���㷨*/
class SyntaxTreeInfoProvider : public IVL_RegExpRecursiveAlgorithm
{
public:
	VUnicodeString		Text;
	VUnicodeString		Information;

	void VisitBegin(VL_RegExpCharset* Node , VBool& VisitChildren , VBool& RunEnd)
	{
		Text=L"";
		Information=L"";
		switch(Node->Type)
		{
		case vrecCharSet:
			Text=(Node->Reverse?L"��ת��":L"")+CharSetToString(Node->CharSet);
			break;
		case vrecStringBegin:
			Text=L"�ַ�����ͷ";
			break;
		case vrecStringEnd:
			Text=L"�ַ�����β";
			break;
		}
		Information+=L"�ڵ����ͣ��ַ���ƥ��\r\n";
		Information+=L"ƥ��Ŀ�꣺"+Text+L"\r\n";
		VisitChildren=false;
		RunEnd=false;
	}

	void VisitBegin(VL_RegExpLoop* Node , VBool& VisitChildren , VBool& RunEnd)
	{
		Text=L"";
		Information=L"";
		if(Node->Finite)
		{
			Text=L"{"+VUnicodeString(Node->Begin)+L","+VUnicodeString(Node->End)+L"}";
		}
		else
		{
			Text=L"{"+VUnicodeString(Node->Begin)+L",}";
		}
		if(Node->Short)
		{
			Text+=L"?";
		}
		Information+=L"�ڵ����ͣ�ѭ��\r\n";
		Information+=L"��Сѭ��������"+VUnicodeString(Node->Begin)+L"\r\n";
		Information+=L"���ѭ��������"+VUnicodeString(Node->End)+L"\r\n";
		Information+=L"����ѭ����"+VUnicodeString(Node->Finite?L"��":L"��")+L"\r\n";
		Information+=L"�����ȣ�"+VUnicodeString(Node->Short?L"��":L"��")+L"\r\n";
		VisitChildren=false;
		RunEnd=false;
	}

	void VisitBegin(VL_RegExpFunc* Node , VBool& VisitChildren , VBool& RunEnd)
	{
		Text=L"";
		Information=L"";
		Information+=L"�ڵ����ͣ����ܽڵ�\r\n";
		switch(Node->Function)
		{
		case vrefCatch:
			Text+=L"[��ȡ]";
			Information+=L"���ܣ���ȡ\r\n";
			break;
		case vrefUncatch:
			Text+=L"[����ȡ]";
			Information+=L"���ܣ�����ȡ\r\n";
			break;
		case vrefPositive:
			Text+=L"[����Ԥ��]";
			Information+=L"���ܣ�����Ԥ��\r\n";
			break;
		case vrefNegative:
			Text+=L"[����Ԥ��]";
			Information+=L"���ܣ�����Ԥ��\r\n";
			break;
		}
		switch(Node->Content)
		{
		case vrecNamed:
			Text+=L"[�������ʽ]";
			Information+=L"���ݣ��������ʽ\r\n";
			break;
		case vrecStorage:
			Text+=L"[ָ���ܵ��洢]";
			Information+=L"���ݣ�ָ���ܵ��洢\r\n";
			break;
		case vrecMatch:
			Text+=L"[ָ���ܵ�ƥ��]";
			Information+=L"���ݣ�ָ���ܵ�ƥ��\r\n";
			break;
		case vrecNormal:
			Text+=L"[��ͨ]";
			Information+=L"���ݣ���ͨ\r\n";
			break;
		}
		Text+=L"["+Node->Pattern+L"]";
		Information+=L"ģʽ���ƣ�"+Node->Pattern+L"\r\n";
		VisitChildren=false;
		RunEnd=false;
	}

	void VisitBegin(VL_RegExpSection* Node , VBool& VisitChildren , VBool& RunEnd)
	{
		Text=L"";
		Information=L"";
		switch(Node->Type)
		{
		case vresSequence:
			Text=L"����";
			break;
		case vresBranch:
			Text=L"��֧";
			break;
		}
		Information+=L"�ڵ����ͣ�"+Text+L"\r\n";
		VisitChildren=false;
		RunEnd=false;
	}

	void VisitEnd(VL_RegExpCharset* Node)
	{
	}

	void VisitEnd(VL_RegExpLoop* Node)
	{
	}

	void VisitEnd(VL_RegExpFunc* Node)
	{
	}

	void VisitEnd(VL_RegExpSection* Node)
	{
	}
};

/*��������ʽ�﷨��ת��ΪTreeView���㷨*/
class SyntaxTreeBuilder : public IVL_RegExpRecursiveAlgorithm
{
public:
	VL_WinTreeView*							TreeView;
	VL_List<VL_WinTreeViewItem , false>		ItemStack;

	SyntaxTreeBuilder(VL_WinTreeView* aTreeView)
	{
		TreeView=aTreeView;
	}

	void MakeNode(VL_RegExpBase* Node)
	{
		VL_WinTreeViewItem Item;
		if(ItemStack.GetCount())
		{
			Item=ItemStack[ItemStack.GetCount()-1].InsertAtLast(GetNodeText(Node));
		}
		else
		{
			Item=TreeView->AddRootItem(GetNodeText(Node));
		}
		Item.SetCustomData(Node);
		ItemStack.Add(Item);
	}

	void VisitBegin(VL_RegExpCharset* Node , VBool& VisitChildren , VBool& RunEnd)
	{
		MakeNode(Node);
	}

	void VisitBegin(VL_RegExpLoop* Node , VBool& VisitChildren , VBool& RunEnd)
	{
		MakeNode(Node);
	}

	void VisitBegin(VL_RegExpFunc* Node , VBool& VisitChildren , VBool& RunEnd)
	{
		MakeNode(Node);
	}

	void VisitBegin(VL_RegExpSection* Node , VBool& VisitChildren , VBool& RunEnd)
	{
		MakeNode(Node);
	}

	void VisitEnd(VL_RegExpCharset* Node)
	{
		ItemStack.Fetch().Expand();
	}

	void VisitEnd(VL_RegExpLoop* Node)
	{
		ItemStack.Fetch().Expand();
	}

	void VisitEnd(VL_RegExpFunc* Node)
	{
		ItemStack.Fetch().Expand();
	}

	void VisitEnd(VL_RegExpSection* Node)
	{
		ItemStack.Fetch().Expand();
	}
};

VUnicodeString GetNodeText(VL_RegExpBase* Node)
{
	SyntaxTreeInfoProvider Algorithm;
	Node->Visit(&Algorithm);
	return Algorithm.Text;
}

VUnicodeString GetNodeInfo(VL_RegExpBase* Node)
{
	SyntaxTreeInfoProvider Algorithm;
	Node->Visit(&Algorithm);
	return Algorithm.Information;
}

void FillTreeView(VL_RegExpBase* Node , VL_WinTreeView* TreeView)
{
	SyntaxTreeBuilder Algorithm(TreeView);
	Node->Visit(&Algorithm);
}

void GetRegexDiscription(VL_RegExpResult& RegexResult , VUnicodeString& InfoSyntaxTree , VUnicodeString& InfoEpsilonNFA , VUnicodeString& InfoNFA , VUnicodeString& InfoDFA)
{
	InfoSyntaxTree=L"";
	InfoEpsilonNFA=L"";
	InfoNFA=L"";
	InfoDFA=L"";
	/*�õ��﷨�����ַ�������*/
	InfoSyntaxTree=RegularExpressionToString(RegexResult.Expression);
	/*�����Զ���*/
	VL_RegNFA						NFAAutomaton;
	VL_RegDFA						DFAAutomaton;
	VL_AutoPtr<VL_RegFAInfo>		FAInfo=new VL_RegFAInfo(RegexResult.Expression);
	/*����epsilon-NFA*/
	VL_RegFAInfo::EpsilonNFABag		eNFA=RegularExpressionToEpsilonNFA(&NFAAutomaton,FAInfo.Object());
	InfoEpsilonNFA+=L"--------����״̬����--------\r\n";
	InfoEpsilonNFA+=NFAToString(FAInfo.Object(),eNFA.MainEpsilonNFA.InitStatus);
	for(VInt i=0;i<eNFA.SubEpsilonNFAs.GetCount();i++)
	{
		InfoEpsilonNFA+=L"--------����״̬��"+VUnicodeString(i)+L"��--------\r\n";
		InfoEpsilonNFA+=NFAToString(FAInfo.Object(),eNFA.SubEpsilonNFAs[i].InitStatus);
	}
	/*����NFA*/
	FAInfo->MainNFA=CompressNFA(&NFAAutomaton,EpsilonNFAToNFA(&NFAAutomaton,eNFA.MainEpsilonNFA));
	for(VInt i=0;i<eNFA.SubEpsilonNFAs.GetCount();i++)
	{
		FAInfo->SubNFAs.Add(CompressNFA(&NFAAutomaton,EpsilonNFAToNFA(&NFAAutomaton,eNFA.SubEpsilonNFAs[i])));
	}
	InfoNFA+=L"--------����״̬����--------\r\n";
	InfoNFA+=NFAToString(FAInfo.Object(),FAInfo->MainNFA);
	for(VInt i=0;i<FAInfo->SubNFAs.GetCount();i++)
	{
		InfoNFA+=L"--------����״̬��"+VUnicodeString(i)+L"��--------\r\n";
		InfoNFA+=NFAToString(FAInfo.Object(),FAInfo->SubNFAs[i]);
	}
	/*����DFA*/
	InfoDFA+=L"--------����״̬����--------\r\n";
	InfoDFA+=DFAToString(FAInfo.Object(),CompressedNFAToDFA(&DFAAutomaton,FAInfo->MainNFA));
	for(VInt i=0;i<FAInfo->SubNFAs.GetCount();i++)
	{
		InfoDFA+=L"--------����״̬��"+VUnicodeString(i)+L"��--------\r\n";
		InfoDFA+=DFAToString(FAInfo.Object(),CompressedNFAToDFA(&DFAAutomaton,FAInfo->SubNFAs[i]));
	}
}