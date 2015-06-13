特点
 所有control全动态创建，无resource文件
 UI 窗口内存只管分配不用管释放

执行流程
启动--》
WinMain
	VL_WinClass
	VL_WinApplication
	VL_WinCommonDialogService
	InitAccelerator
	main
		new RegForm:VL_WinForm
			
		GetApplication->Run
		
UI类
	VL_WinEdit
		SetHScroll
		SetReadonly
		。。。
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
		》VL_WinTreeViewItem
			SetCustomData
			InsertAtLast
			
	VL_WinTab
		AddPage
		VL_WinTabPage
			ApplyPlacement
		
	
UI Event handling
	每个control有些预先定义的event，该event可以band到指定回调函数
	比如 treeview在选择某项item时事件处理
	tvMatchStructure->OnItemSelected.Bind(this,&RegForm::tvMatchStructure_OnItemSelected);
	其中OnItemSelected为VE_TreeViewItemEvent类型
	
	事件处理在各个类的ProcessMessage实现
	
UI event类型
	VE_NotifyEvent 按钮点击
	VE_TreeViewItemEvent
	VE_TreeViewVerifyEvent
	
	event用VL_DEFINE_VOID_EVENT定义，VL_IMPLEMENT_VOID_EVENT实现（如何实现？？）
	
UI布局相关类
VL_Placement
	pHorzFix1
	pHorzScale
	pVertFix1
	pControl
	pBlank
布局的实现
	VL_WinContainer::ApplyPlacement
		Placement->Apply
			有两个类_pControl，_pPart均实现了Apply函数（这两个类是做啥的？有什么不同？只用这两个类就王城了所有的placement？）
	
example：
	VL_WinTabPage::ApplyPlacement
		VL_WinContainer::ApplyPlacement
			placement::_pPart::Apply （再往下没看懂，慢慢再看吧？？）

RegEx类
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
		
类的继承关系
	VL_Base 纯虚类
		VL_WinControl 定义了基本元素（handle，鼠标进入推出状态，字体，父窗口，）和事件（enable,disable,移动，显示，隐藏，大小改变，paint，鼠标事件，键盘事件
			VL_WinContainer 容器UI基类
				VL_WinGroup
				VL_WinTab
				VL_WinStatic
				VL_WinForm 顶层窗口类
					RegForm 应用程序类
		VL_WinApplication
		
内存管理		
			VL_WinApplication包含一个ControlMap，即所有句柄和对象的对应关系，对于application开发者，所有对象只需要new，而delete是由框架实现的
				VL_WinContainer 里面包含一个VL_WinControlList类型的所有子对象列表，在 VL_WinContainer::Destroy时delete掉所有其child成员
				VL_WinApplication里面包含一个ControlMap类型的所有顶级窗口列表（貌似不是！），在VL_WinControl::_CreateWindow里面GetApplication()->FControls.Add添加到application全局变量里面,在VL_WinControl::Destroy时application删除该句柄对应的list item（貌似没有delete该对象，这个map貌似只是为了查找窗口用的，而不是内存管理）
				
				application内存管理用的变量是FMainForm和FForms，在VL_WinApplication::Terminate时会看到删除所有forms，在VL_WinForm::CreateForm会调用VL_WinApplication::RegisterForm将窗口加入application全局列表
		