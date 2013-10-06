// TestStruct.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <assert.h>

#define DEFINE_TYPE(x) TYPE_##x,
enum DataType
{
	#include "DataType.h"
};
#undef DEFINE_TYPE

#define DEFINE_TYPE(x) sizeof(x),
static int g_typeSz[]={
	#include "DataType.h"
};
#undef DEFINE_TYPE

#define DEFINE_TYPE(x) #x,
static char* g_typeStr[]={
	#include "DataType.h"
};
#undef DEFINE_TYPE


int getDataTypeSz(DataType dataType)
{
	assert(dataType<sizeof(g_typeSz)/sizeof(g_typeSz[0]));
	return g_typeSz[dataType];
}

const char* getDataTypeStr(DataType dataType)
{
	assert(dataType<sizeof(g_typeStr)/sizeof(g_typeStr[0]));
	return g_typeStr[dataType];
}

#define MIN(a,b) (a)<(b)?(a):(b)

int alignData(int nAddr, int nAlign)
{
	return (nAddr+nAlign-1)/nAlign*nAlign;
}

void printStruct(int nAlign, int n, DataType astData[])
{
	printf("my computation generated:\n");

	int nSz =0;
	for(int i=0;i<n;i++)
	{
		int nCurSz =getDataTypeSz(astData[i]);
		int nCurAlign =MIN(nAlign, nCurSz);
		nSz =alignData(nSz, nCurAlign);
		printf("%10s @ %10d\n", getDataTypeStr(astData[i]),nSz);
		nSz += nCurSz;
	}
	printf("Total Sz %d\n", nSz);
}

#define DEFINE_STRUCT(a,b,c)  struct L{a x1;b x2;c x3;};
#include "structType.h"
#undef DEFINE_STRUCT

#define DEFINE_STRUCT(a,b,c)  DataType L2[]={TYPE_##a,TYPE_##b,TYPE_##c };
#include "structType.h"
#undef DEFINE_STRUCT

void printStruct2(L const &l)
{
	printf("compiler generated:\n");
	printf("%d ",(char*)&l.x1 - (char*)&l);
	printf("%d ",(char*)&l.x2 - (char*)&l);
	printf("%d\n",(char*)&l.x3 - (char*)&l);
	printf("total %d\n", sizeof(l));
}

int main(int argc, char *argv[])
{
	int align =8;
	printStruct(align, sizeof(L2)/sizeof(L2[0]),L2); 
	printStruct2(L());
	
	return 0;
}
