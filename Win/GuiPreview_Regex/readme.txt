�ص�
 ����controlȫ��̬��������resource�ļ�
 UI �����ڴ�ֻ�ܷ��䲻�ù��ͷ�

ִ������
����--��
WinMain
	VL_WinClass
	VL_WinApplication
	VL_WinCommonDialogService
	InitAccelerator
	main
		new RegForm:VL_WinForm
			
		GetApplication->Run
		
UI��
	VL_WinEdit
		SetHScroll
		SetReadonly
		������
	VL_WinComboBox
		AddString
		SetSelectedIndex
	
	VL_WinStatic
	VL_WinButton
		SetText
	VL_WinTreeView
		AddRootItem
		OnItemSelected.Bind
		Clear
		��VL_WinTreeViewItem
			SetCustomData
			InsertAtLast
			
	VL_WinTab
		AddPage
		VL_WinTabPage
			ApplyPlacement
		
	
UI Event handling
	ÿ��control��ЩԤ�ȶ����event����event����band��ָ���ص�����
	���� treeview��ѡ��ĳ��itemʱ�¼�����
	tvMatchStructure->OnItemSelected.Bind(this,&RegForm::tvMatchStructure_OnItemSelected);
	����OnItemSelectedΪVE_TreeViewItemEvent����
	
	�¼������ڸ������ProcessMessageʵ��
	
UI event����
	VE_NotifyEvent ��ť���
	VE_TreeViewItemEvent
	VE_TreeViewVerifyEvent
	
	event��VL_DEFINE_VOID_EVENT���壬VL_IMPLEMENT_VOID_EVENTʵ�֣����ʵ�֣�����
	
UI���������
VL_Placement
	pHorzFix1
	pHorzScale
	pVertFix1
	pControl
	pBlank
���ֵ�ʵ��
	VL_WinContainer::ApplyPlacement
		Placement->Apply
			��������_pControl��_pPart��ʵ����Apply������������������ɶ�ģ���ʲô��ͬ��ֻ��������������������е�placement����
	
example��
	VL_WinTabPage::ApplyPlacement
		VL_WinContainer::ApplyPlacement
			placement::_pPart::Apply ��������û�����������ٿ��ɣ�����

RegEx��
	GeneralRegex
		Match
		MatchHead
		MatchWhole
		Search
		Split
		Cut 
		above functions return a VL_AutoPtr<GeneralMatchList> object
	GeneralMatchList
		GetCount
		GetItem return a GeneralMatch object
			IsSuccess
			Text
		
��ļ̳й�ϵ
	VL_Base ������
		VL_WinControl �����˻���Ԫ�أ�handle���������Ƴ�״̬�����壬�����ڣ������¼���enable,disable,�ƶ�����ʾ�����أ���С�ı䣬paint������¼��������¼�
			VL_WinContainer ����UI����
				VL_WinGroup
				VL_WinTab
				VL_WinStatic
				VL_WinForm ���㴰����
					RegForm Ӧ�ó�����
		VL_WinApplication
		
�ڴ����		
			VL_WinApplication����һ��ControlMap�������о���Ͷ���Ķ�Ӧ��ϵ������application�����ߣ����ж���ֻ��Ҫnew����delete���ɿ��ʵ�ֵ�
				VL_WinContainer �������һ��VL_WinControlList���͵������Ӷ����б��� VL_WinContainer::Destroyʱdelete��������child��Ա
				VL_WinApplication�������һ��ControlMap���͵����ж��������б�ò�Ʋ��ǣ�������VL_WinControl::_CreateWindow����GetApplication()->FControls.Add��ӵ�applicationȫ�ֱ�������,��VL_WinControl::Destroyʱapplicationɾ���þ����Ӧ��list item��ò��û��delete�ö������mapò��ֻ��Ϊ�˲��Ҵ����õģ��������ڴ����
				
				application�ڴ�����õı�����FMainForm��FForms����VL_WinApplication::Terminateʱ�ῴ��ɾ������forms����VL_WinForm::CreateForm�����VL_WinApplication::RegisterForm�����ڼ���applicationȫ���б�
		