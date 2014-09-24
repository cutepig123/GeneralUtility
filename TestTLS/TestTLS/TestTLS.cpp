// TestTLS.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "assert.h"
#include <map>
#include <string>

struct	TLS_Store
{
	DWORD dwTlsIndex; 
	typedef std::map<std::string, void*> TLS_DATA;

	short Init()
	{
		if ((dwTlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES) 
			return -1;

		 TLS_DATA *pData =new TLS_DATA;
		 if(!pData)
			 return -2;

		 if(! TlsSetValue(dwTlsIndex, pData) )
			 return -3;

		 return 0;
	}

	short Free()
	{
		LPVOID lpvData = TlsGetValue(dwTlsIndex); 
		 if(!lpvData)
			return -1;

		 TLS_DATA *pData =(TLS_DATA*)lpvData;
		 delete pData;

		TlsFree(dwTlsIndex);
		return 0;
	}
	
	short Find(const char *tag, void **p)
	{
		 LPVOID lpvData = TlsGetValue(dwTlsIndex); 
		 if(!lpvData)
			return -1;

		 TLS_DATA *pData =(TLS_DATA*)lpvData;
		 TLS_DATA::iterator it= pData->find(tag);
		 if( it==pData->end() )
			 return -2;

		 *p =it->second;
		 return 0;
	}

	short Malloc(const char *tag, size_t n, void **p=0)
	{
		LPVOID lpvData = TlsGetValue(dwTlsIndex); 
		if(!lpvData)
			return -1;

		TLS_DATA *pData =(TLS_DATA*)lpvData;
		TLS_DATA::iterator it= pData->find(tag);
		if( it!=pData->end() )
			return -2;

		void *p2 =malloc(n);
		if(!p2)
			return -3;

		(*pData)[tag] =p2;
		if(p)
			*p =p2;
		return 0;
	}

	short Remove(const char*tag)
	{
		LPVOID lpvData = TlsGetValue(dwTlsIndex); 
		if(!lpvData)
		return -1;

		TLS_DATA *pData =(TLS_DATA*)lpvData;
		TLS_DATA::iterator it= pData->find(tag);
		if( it==pData->end() )
			return -2;

		free(it->second);
		pData->erase(it);
		return 0;
	}
};

#define THREADCOUNT 4 
#define CHK_STS(x)	if((x)!=0) {assert(0);}

int _tmain(int argc, _TCHAR* argv[])
{
	TLS_Store tls;
	CHK_STS(tls.Init());

	void *p=0;
	assert(tls.Find("a", &p)<0);
	
	CHK_STS(tls.Malloc("a",100, &p));
	memset( p, 100, 100);

	void *p2=0;
	assert(tls.Find("a", &p2)==0);
	char *p3=(char*)p2;
	assert( p3[0]==100 );
	assert( p3[100-1]==100 );
	assert( p==p2 );

	assert(tls.Remove("a")==0);
	assert(tls.Remove("a")<0);

	assert(tls.Find("a", &p)<0);

	printf("Done\n");

	return 0;
}

