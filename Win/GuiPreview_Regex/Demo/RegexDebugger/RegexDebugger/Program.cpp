#include "..\..\..\Library\Windows\VL_WinGUI.h"
#include "GeneralRegex.h"
#include "RegexHelper.h"

using namespace vl;
using namespace vl::windows;
using namespace vl::windows::placement;
using namespace vl::grammar;

/*���ڱ���*/
PCWChar WindowTitle=L"Vczh Regular Expression Debugger [GUI Framework Preview Demo]";

/*������*/
class RegForm : public VL_WinForm
{
protected:
	enum RegexStatus
	{
		rsAnalysing,
		rsModified,
		rsCorrect,
		rsWrong,
		rsSearching
	};

	/*�ؼ�*/
	VL_WinEdit*						txtRegex;
	VL_WinComboBox*					cbRegex;
	VL_WinStatic*					lbErrorMessage;
	VL_WinButton*					btnGo;
	VL_WinTreeView*					tvSyntaxTree;
	VL_WinTab*						tabDebugger;
	VL_WinStatus*					stStatus;
	/*��Ϣҳ�ؼ�*/
	VL_WinStatic*					lbInfo;
	VL_WinComboBox*					cbInfo;
	VL_WinEdit*						txtInfo;
	/*ƥ��ҳ�ؼ�*/
	VL_WinComboBox*					cbMatchMethod;
	VL_WinCheck*					chkMultiline;
	VL_WinCheck*					chkKeepEmptyPart;
	VL_WinButton*					btnMatch;
	VL_WinEdit*						txtMatchInput;
	VL_WinEdit*						txtMatchOutput;
	VL_WinTreeView*					tvMatchStructure;
	/*������ʽ����*/
	VUnicodeString					InfoSyntaxTree;
	VUnicodeString					InfoEpsilonNFA;
	VUnicodeString					InfoNFA;
	VUnicodeString					InfoDFA;
	VL_RegExpResult					RegexResult;
	RegexStatus						Status;
	VL_AutoPtr<GeneralRegex>		RegexEngine;
	VL_AutoPtr<GeneralMatchList>	RegexMatchList;
	VL_List<GeneralIns , false>		RegexIns;

	/*��������ִ��������Ϣ*/
	void SetRunningTaskText(VUnicodeString Text)
	{
		stStatus->GetItem(0).SetTextLeft(Text);
	}

	/*ǿ������״̬*/
	void SetStatus(RegexStatus aStatus)
	{
		switch(Status=aStatus)
		{
		case rsAnalysing:
			stStatus->GetItem(1).SetTextLeft(L"����");
			break;
		case rsModified:
			stStatus->GetItem(1).SetTextLeft(L"�޸�");
			break;
		case rsCorrect:
			stStatus->GetItem(1).SetTextLeft(L"��ȷ");
			break;
		case rsWrong:
			stStatus->GetItem(1).SetTextLeft(L"����");
			break;
		case rsSearching:
			stStatus->GetItem(1).SetTextLeft(L"����");
			break;
		}
		SetRunningTaskText(L"����");
	}

	/*����״̬*/
	void ChangeStatus(RegexStatus aStatus)
	{
		if(Status!=aStatus)
		{
			SetStatus(aStatus);
		}
	}

	/*������Ϣҳ�ı�������*/
	void ResetInfoPageContent()
	{
		switch(cbInfo->GetSelectedIndex())
		{
		case 0:
			{
				VL_WinTreeViewItem Item=tvSyntaxTree->GetCurrentSelectedItem();
				if(Item)
				{
					txtInfo->SetText(GetNodeInfo(((VL_RegExpBase*)Item.GetCustomData())));
				}
				else
				{
					txtInfo->SetText(L"û��ѡ�еĽڵ�");
				}
			}
			break;
		case 1:
			txtInfo->SetText(InfoSyntaxTree);
			break;
		case 2:
			txtInfo->SetText(InfoEpsilonNFA);
			break;
		case 3:
			txtInfo->SetText(InfoNFA);
			break;
		case 4:
			txtInfo->SetText(InfoDFA);
			break;
		}
	}

	/*����������ʽ״̬���Լ��ַ�������*/
	void AnalysisRegexResult()
	{
		/*�����ʽ�﷨�������û�з��ִ���������﷨��*/
		if(!RegexResult.Error)
		{
			GetRegexDiscription(RegexResult,InfoSyntaxTree,InfoEpsilonNFA,InfoNFA,InfoDFA);
		}
		else
		{
			InfoSyntaxTree=L"������ʽ�﷨����";
			InfoEpsilonNFA=L"������ʽ�﷨����";
			InfoNFA=L"������ʽ�﷨����";
			InfoDFA=L"������ʽ�﷨����";
		}
	}

	/*��ʾ������ʽ�﷨��*/
	void BuildSyntaxTree()
	{
		FillTreeView(RegexResult.Expression.Object(),tvSyntaxTree);
	}

	/*��ʾ������Ϣ*/
	void ResetErrorStatus(VL_RegExpResult& Result)
	{
		btnMatch->SetEnabled(!Result.Error);
		if(Result.Error)
		{
			lbErrorMessage->SetText(L"���ִ���"+Result.ErrorMessage);
		}
		else
		{
			lbErrorMessage->SetText(L"��ȷ��������ʽ��");
		}
	}

	/*����������ʽ����*/
	void BuildRegexEngine()
	{
		switch(cbRegex->GetSelectedIndex())
		{
		case 0:
			RegexEngine=CreatePureRegex(txtRegex->GetText());
			break;
		case 1:
			RegexEngine=CreateSafeRegex(txtRegex->GetText());
			break;
		case 2:
			RegexEngine=CreateGreedRegex(txtRegex->GetText());
			break;
		}
	}

	/*�����ı����е�������ʽ*/
	void AnalysisRegex()
	{
		ChangeStatus(rsAnalysing);
		RegexEngine=0;
		SetRunningTaskText(L"���ڷ���������ʽ�﷨...");
		tvSyntaxTree->Clear();
		VUnicodeString Text=txtRegex->GetText();
		RegexResult=RegularExpressionAnalysis(Text,cbRegex->GetSelectedIndex()!=0);
		ResetErrorStatus(RegexResult);
		if(RegexResult.Error)
		{
			if(RegexResult.ErrorPosition!=-1)
			{
				txtRegex->Select(RegexResult.ErrorPosition,Text.Length()-RegexResult.ErrorPosition);
			}
			AnalysisRegexResult();
			ShowMessage(this,RegexResult.ErrorMessage);
			txtRegex->SetFocused();
			ChangeStatus(rsWrong);
		}
		else
		{
			SetRunningTaskText(L"��������������ʽ��ϸ��Ϣ...");
			BuildSyntaxTree();
			AnalysisRegexResult();
			BuildRegexEngine();
			ChangeStatus(rsCorrect);
		}
		ResetInfoPageContent();
	}

	/*����ƥ�临ѡ��ť�Ŀ�����*/
	void SetCheckBoxEnabled()
	{
		chkMultiline->SetEnabled(cbMatchMethod->GetSelectedIndex()>=3 && cbRegex->GetSelectedIndex()!=0);
		chkKeepEmptyPart->SetEnabled(cbMatchMethod->GetSelectedIndex()>=4);
	}

	/*��״̬ת��Ϊ�޸Ĳ���������ʽ���﷨���*/
	void RegexModified()
	{
		ChangeStatus(rsModified);
		VL_RegExpResult Result=RegularExpressionAnalysis(txtRegex->GetText(),cbRegex->GetSelectedIndex()!=0);
		ResetErrorStatus(Result);
	}

	/*��ʼ��״̬*/
	void InitStatus()
	{
		SetStatus(rsModified);
		/*����ʾ�����ʽ*/
		txtRegex->SetText(L"(?:<sec>(?:<#part>25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]\\d|[0-9]))(.(?:<sec>)){3}");
		AnalysisRegex();
		txtRegex->SelectAll();
		txtRegex->SetFocused();
		/*���ÿؼ�����*/
		SetCheckBoxEnabled();
		ResetInfoPageContent();
	}

	/*�ؼ���ʼ������*/
	void InitControls()
	{
		/*��ʼ���ؼ�*/
		txtRegex=new VL_WinEdit(this,false);
		cbRegex=new VL_WinComboBox(this,true);
		cbRegex->AddString(L"��������ʽ");
		cbRegex->AddString(L"��չ������ʽ");
		cbRegex->AddString(L"̰��������ʽ");
		cbRegex->SetSelectedIndex(2);
		lbErrorMessage=new VL_WinStatic(this);
		btnGo=new VL_WinButton(this);
		btnGo->SetText(L"����");
		tvSyntaxTree=new VL_WinTreeView(this);
		tabDebugger=new VL_WinTab(this);
		{
			/*��ʼ����Ϣҳ*/
			VL_WinTabPage Page=tabDebugger->AddPage(L"��Ϣ");
			lbInfo=new VL_WinStatic(Page);
			lbInfo->SetText(L"��Ϣ���ݣ�");
			cbInfo=new VL_WinComboBox(Page,true);
			cbInfo->AddString(L"�ڵ���ϸ��Ϣ");
			cbInfo->AddString(L"�﷨��");
			cbInfo->AddString(L"��-NFA");
			cbInfo->AddString(L"NFA");
			cbInfo->AddString(L"DFA");
			cbInfo->SetSelectedIndex(0);
			txtInfo=new VL_WinEdit(Page,true);
			txtInfo->SetHScroll(true);
			txtInfo->SetVScroll(true);
			txtInfo->SetReadonly(true);
			/*������Ϣҳ�ռ��Ű�*/
			Page.ApplyPlacement(
				pHorzFix1(10,10,
					pVertFix1(0,10,
						pControl(lbInfo,60,20),
						pVertFix1(0,10,
							pControl(cbInfo,120,20),
							pBlank(0,0)
						)
					),
					pControl(txtInfo)
				)
			);
		}
		{
			VL_WinTabPage Page=tabDebugger->AddPage(L"ƥ��");
			cbMatchMethod=new VL_WinComboBox(Page,true);
			cbMatchMethod->AddString(L"ƥ���һ��");
			cbMatchMethod->AddString(L"ƥ���ַ�����ͷ");
			cbMatchMethod->AddString(L"ƥ�������ַ���");
			cbMatchMethod->AddString(L"��������ƥ��");
			cbMatchMethod->AddString(L"�����ַ���");
			cbMatchMethod->AddString(L"�и��ַ���");
			cbMatchMethod->SetSelectedIndex(3);
			chkMultiline=new VL_WinCheck(Page);
			chkMultiline->SetText(L"����ƥ��");
			chkKeepEmptyPart=new VL_WinCheck(Page);
			chkKeepEmptyPart->SetText(L"�����հ��и�");
			btnMatch=new VL_WinButton(Page);
			btnMatch->SetText(L"ƥ��");
			txtMatchInput=new VL_WinEdit(Page,true,true);
			txtMatchInput->SetHScroll(true);
			txtMatchInput->SetVScroll(true);
			txtMatchInput->SetText(L"���������뱻ƥ����ַ�����");
			txtMatchOutput=new VL_WinEdit(Page,true);
			txtMatchOutput->SetHScroll(true);
			txtMatchOutput->SetVScroll(true);
			txtMatchOutput->SetReadonly(true);
			tvMatchStructure=new VL_WinTreeView(Page);
			Page.ApplyPlacement(
				pHorzFix1(10,10,
					pVertFix1(0,10,
						pVertFix1(0,10,
							pVertFix1(0,10,
								pControl(chkMultiline,70,0),
								pControl(chkKeepEmptyPart,100,0)
							),
							pVertFix1(0,10,
								pControl(cbMatchMethod,100,21),
								pControl(btnMatch,40,0)
							)
						),
						pBlank(0,0)
					),
					pHorzScale(0,10,0.4,
						pControl(txtMatchInput),
						pVertScale(0,10,0.4,
							pControl(tvMatchStructure),
							pControl(txtMatchOutput)
						)
					)
				)
			);
		}
		stStatus=new VL_WinStatus(this);
		stStatus->AddItem().SetWidth(200);
		stStatus->AddItem().SetWidth(50);
		stStatus->AddItem().SetTextRight(L"�����ߣ������(vczh) http://www.cppblog.com/vczh   ");
		/*���ÿؼ��Ű棬�ڴ��ڳߴ��޸ĵ�ʱ���Զ������ؼ��ߴ�*/
		ApplyPlacement(
			pHorzFix2(0,0,
				pHorzFix1(10,10,
					pVertFix1(0,10,
						pControl(cbRegex,120,21),
						pVertFix2(0,10,
							pControl(txtRegex),
							pControl(btnGo,40,0)
						)
					),
					pHorzFix1(0,10,
						pControl(lbErrorMessage,0,20),
						pVertFix1(0,10,
							pControl(tvSyntaxTree,300,300),
							pControl(tabDebugger,400,300)
						)
					)
				),
				pControl(stStatus,0,20)
			)
		);
		/*���ÿؼ��¼�*/
		cbRegex->OnSelChanged.Bind(this,&RegForm::cbRegex_OnSelChanged);
		txtRegex->OnChanged.Bind(this,&RegForm::txtRegex_OnChanged);
		txtRegex->OnKeyDown.Bind(this,&RegForm::txtRegex_OnKeyDown);
		btnGo->OnClick.Bind(this,&RegForm::btnGo_OnClick);
		tvSyntaxTree->OnItemSelected.Bind(this,&RegForm::tvSyntaxTree_OnItemSelected);
		cbInfo->OnSelChanged.Bind(this,&RegForm::cbInfo_OnSelChanged);
		cbMatchMethod->OnSelChanged.Bind(this,&RegForm::cbMatchMethod_OnSelChanged);
		btnMatch->OnClick.Bind(this,&RegForm::btnMatch_OnClick);
		tvMatchStructure->OnItemSelected.Bind(this,&RegForm::tvMatchStructure_OnItemSelected);
	}

	/*�ؼ��¼�*/

	void cbRegex_OnSelChanged(VL_Base* Sender)
	{
		SetCheckBoxEnabled();
		RegexModified();
	}

	void txtRegex_OnChanged(VL_Base* Sender)
	{
		RegexModified();
	}

	void txtRegex_OnKeyDown(VL_Base* Sender,VLS_KeyStruct KeyStruct)
	{
		if(KeyStruct.KeyCode==VK_RETURN)
		{
			AnalysisRegex();
		}
	}

	void btnGo_OnClick(VL_Base* Sender)
	{
		AnalysisRegex();
	}

	void tvSyntaxTree_OnItemSelected(VL_Base* Sender , VL_WinTreeViewItem Item)
	{
		if(cbInfo->GetSelectedIndex()==0)
		{
			ResetInfoPageContent();
		}
	}

	void cbInfo_OnSelChanged(VL_Base* Sender)
	{
		ResetInfoPageContent();
	}

	void cbMatchMethod_OnSelChanged(VL_Base* Sender)
	{
		SetCheckBoxEnabled();
	}

	void btnMatch_OnClick(VL_Base* Sender)
	{
		RegexStatus aStatus=Status;
		ChangeStatus(rsSearching);
		SetRunningTaskText(L"����ƥ��...");
		if(Status!=rsCorrect)
		{
			BuildRegexEngine();
		}
		switch(cbMatchMethod->GetSelectedIndex())
		{
		case 0:
			RegexMatchList=RegexEngine->Match(txtMatchInput->GetText());
			break;
		case 1:
			RegexMatchList=RegexEngine->MatchHead(txtMatchInput->GetText());
			break;
		case 2:
			RegexMatchList=RegexEngine->MatchWhole(txtMatchInput->GetText());
			break;
		case 3:
			RegexMatchList=RegexEngine->Search(txtMatchInput->GetText(),chkMultiline->GetChecked());
			break;
		case 4:
			RegexMatchList=RegexEngine->Split(txtMatchInput->GetText(),chkMultiline->GetChecked(),chkKeepEmptyPart->GetChecked());
			break;
		case 5:
			RegexMatchList=RegexEngine->Cut(txtMatchInput->GetText(),chkMultiline->GetChecked(),chkKeepEmptyPart->GetChecked());
			break;
		}
		SetRunningTaskText(L"��������ƥ����...");
		tvMatchStructure->Clear();
		if(RegexMatchList->GetCount())
		{
			for(VInt i=0;i<RegexMatchList->GetCount();i++)
			{
				VUnicodeString Text;
				GeneralMatch* Match=RegexMatchList->GetItem(i);
				if(Match->IsSuccess())
				{
					Text=L"ƥ��["+VUnicodeString(i)+L"]��"+Match->Text();
				}
				else
				{
					Text=L"�ı�["+VUnicodeString(i)+L"]��"+Match->Text();
				}
				VL_WinTreeViewItem _Match=tvMatchStructure->AddRootItem(Text);
				_Match.SetCustomData((VPointer)RegexIns.GetCount());
				RegexIns.Add(GeneralIns(L"",gitGetText,0,i));
				if(Match->GetCatchedCount())
				{
					VL_WinTreeViewItem _Catch=_Match.InsertAtLast(L"��������");
					_Catch.SetCustomData((VPointer)-1);
					for(VInt j=0;j<Match->GetCatchedCount();j++)
					{
						VL_WinTreeViewItem _CatchItem=_Catch.InsertAtLast(Match->GetCatched(j));
						_CatchItem.SetCustomData((VPointer)RegexIns.GetCount());
						RegexIns.Add(GeneralIns(L"",gitGetCatched,j,i));
					}
				}
				VInt Count=Match->GetStorageCount();
				if(Count)
				{
					for(VInt k=0;k<Count;k++)
					{
						VUnicodeString Name=Match->GetStorageName(k);
						VL_WinTreeViewItem _Catch=_Match.InsertAtLast(L"��������["+Name+L"]");
						_Catch.SetCustomData((VPointer)-1);
						for(VInt j=0;j<Match->GetStorageItemCount(Name);j++)
						{
							VL_WinTreeViewItem _CatchItem=_Catch.InsertAtLast(Match->GetStorageItem(Name,j));
							_CatchItem.SetCustomData((VPointer)RegexIns.GetCount());
							RegexIns.Add(GeneralIns(Name,gitGetStorage,j,i));
						}
					}
				}
			}
		}
		else
		{
			tvMatchStructure->AddRootItem(L"�Ҳ����κ�����Ҫ���ƥ��").SetCustomData((VPointer)-1);
		}
		SetRunningTaskText(L"����");
		ChangeStatus(aStatus);
	}

	void tvMatchStructure_OnItemSelected(VL_Base* Sender , VL_WinTreeViewItem Item)
	{
		if(Status!=rsSearching)
		{
			VInt Index=(VInt)Item.GetCustomData();
			if(Index!=-1)
			{
				txtMatchOutput->SetText(RegexIns[Index].GetText(RegexMatchList.Object()));
				GeneralMatch* Match=RegexMatchList->GetItem(RegexIns[Index].MatchIndex);
				VInt Line=Match->Line();
				VInt Start=txtMatchInput->GetCharIndex(Line)+Match->Position();
				txtMatchInput->Select(Start,Match->Text().Length());
				txtMatchInput->ScrollToCaret();
			}
		}
	}

public:
	RegForm():VL_WinForm(true)
	{
		SetText(WindowTitle);
		InitControls();
		InitStatus();
		MoveCenter();
	}
};

void main()
{
	(new RegForm())->ShowMaximized();
	GetApplication()->Run();
}