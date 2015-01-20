// dllmain.h : Declaration of module class.

class CMyObjectModule : public ATL::CAtlDllModuleT< CMyObjectModule >
{
public :
	DECLARE_LIBID(LIBID_MyObjectLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MYOBJECT, "{954DBCBF-CE7A-4392-9E95-1879139EA9B0}")
	int SayHi(void);
};

extern class CMyObjectModule _AtlModule;
