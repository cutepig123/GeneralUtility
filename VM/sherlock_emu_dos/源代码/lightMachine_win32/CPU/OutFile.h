#include<stdio.h>
#include<string>
using namespace std;
typedef unsigned char U1;
typedef unsigned short U2;
typedef unsigned int U4;
typedef char S1;
typedef short S2;
typedef int S4;
struct Reg_CPU
{
	U4 ax,bx,cx,dx;
	U4 sp,bp,di,si;
	U4 ip,flags;
	U4 cs, ds, es, ss;
	bool cf,pf,af,zf,sf,df,of,IF,TF;
};
class OutFile
{
public:
	FILE* pfile;
	OutFile(string path)
	{
		pfile = fopen(&path[0],"w");
	}
	void toFile(const Reg_CPU& reg)
	{
		fprintf(pfile,"AX=%4X, BX=%4X, CX=%4X, DX=%4X\nSP=%4X, BP=%4X, DI=%4X, SI=%4X, IP=%4X\nCS=%4X, DS=%4X, ES=%4X, SS=%4X\nOF=%1X, SF=%1X, ZF=%1X, CF=%1X, AF=%1X, PF=%1X, DF=%1X, TF=%1X\n\n",
			reg.ax,reg.bx,reg.cx,reg.dx,
			reg.sp,reg.bp,reg.di,reg.si,reg.ip,
			reg.cs,reg.ds,reg.es,reg.ss,
			!!reg.of, !!reg.sf, !!reg.zf, !!reg.cf, !!reg.af, !!reg.pf, !!reg.df,  !!reg.TF);
	}
	~OutFile()
	{
		fclose(pfile);
	}
};
