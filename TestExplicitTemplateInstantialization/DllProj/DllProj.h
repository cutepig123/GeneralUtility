// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 DLLPROJ_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// DLLPROJ_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef DLLPROJ_EXPORTS
#define DLLPROJ_API __declspec(dllexport)
#else
#define DLLPROJ_API __declspec(dllimport)
#endif

// 此类是从 DllProj.dll 导出的
class DLLPROJ_API CDllProj {
public:
	CDllProj(void);
	// TODO:  在此添加您的方法。
};

extern DLLPROJ_API int nDllProj;

DLLPROJ_API int fnDllProj(void);

// Demo of Export a class template
// 
template <class T>
class DLLPROJ_API TmplA
{
public:
	void f();
};

// Demo of Export a function template
// Note: this one fails.. reason unknown

template <class T> 
DLLPROJ_API void TmplF(T );

