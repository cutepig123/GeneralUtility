// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� DLLPROJ_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// DLLPROJ_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef DLLPROJ_EXPORTS
#define DLLPROJ_API __declspec(dllexport)
#else
#define DLLPROJ_API __declspec(dllimport)
#endif

// �����Ǵ� DllProj.dll ������
class DLLPROJ_API CDllProj {
public:
	CDllProj(void);
	// TODO:  �ڴ�������ķ�����
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

