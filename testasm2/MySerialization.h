#pragma once
#include <dsp_pu.h>
#include <vector>

template <class T>
struct	PUHead;

struct MyOArchive
{
	IMG_ULWORD		*pulOrigData;	 // no change
	IMG_ULWORD		*pulData;		// update during writing
	IMG_LWORD		lwLength;		// 
	IMG_UWORD		uwHeadDepth;	// Number of head in the stack, used to check the validity

	MyOArchive(void *p=0):pulOrigData((IMG_ULWORD*)p), pulData((IMG_ULWORD*)p), lwLength(0)
	{
	}

	bool is_saveing(){
		return true;
	}
};

struct MyIArchive
{
	IMG_ULWORD		*pulOrigData;	 // no change
	IMG_ULWORD		*pulData;		// update during reading

	MyIArchive(void *p=0):pulOrigData((IMG_ULWORD*)p), pulData((IMG_ULWORD*)p)
	{
	}

	bool is_saveing(){
		return false;
	}
};


template <>
struct	PUHead<MyIArchive>
{
	MyIArchive &m_ar;
	PUHead(MyIArchive &ar):m_ar(ar)
	{
		PU_UnpackDataHeader( (const IMG_ULWORD **)&m_ar.pulData, NULL );
	}

	~PUHead()
	{
		PU_SkipToEnd( (const IMG_ULWORD **)&m_ar.pulData );
	}
};

template <>
struct	PUHead<MyOArchive>
{
	MyOArchive		&m_ar;
	IMG_UBYTE		*m_pstHdr;
	IMG_ULWORD		*m_pulDataStart;
	IMG_LWORD		m_lwLengthStart;
	PUHead(MyOArchive &ar):m_ar(ar), m_pstHdr(0)
	{
		if(m_ar.uwHeadDepth==0)
		{
			SYS_Assert(m_ar.lwLength==0);
			SYS_Assert(m_ar.pulData==m_ar.pulOrigData);
			m_ar.lwLength=0;
			m_ar.pulData=m_ar.pulOrigData;
		}
		m_pulDataStart =m_ar.pulData;
		m_lwLengthStart =m_ar.lwLength;

		m_ar.uwHeadDepth++;

		m_pstHdr = (IMG_UBYTE*)( m_ar.pulData ? (m_ar.pulData) : NULL );
		
		if(m_ar.pulData)
		{
			PU_DATA_HEADER stHdrDummy;
			PU_InitDataHeader(&stHdrDummy);		
			PU_PackDataHeader(&stHdrDummy, &m_ar.pulData);
		}
		m_ar.lwLength += PU_PSizeDataHeader(NULL);
	}

	~PUHead()
	{
		if(m_pstHdr)
		{
			PU_DATA_HEADER stHdrDummy;
			PU_InitDataHeader(&stHdrDummy);		
			SYS_Assert( *m_pstHdr == PU_PSizeDataHeader(&stHdrDummy) );
			*(IMG_LWORD*)(m_pstHdr+1) =m_ar.lwLength -m_lwLengthStart;
		}
		
		m_ar.uwHeadDepth--;

		if(m_pulDataStart)
		{
			SYS_Assert( (char*)m_ar.pulData -(char*)m_pulDataStart == m_ar.lwLength -m_lwLengthStart);;
		}
	}
};

MyIArchive& operator & (MyIArchive &ar, IMG_LWORD &t)
{
	PU_UnpackDspLword( (const IMG_ULWORD **)&ar.pulData, &t );
	return ar;
}

MyOArchive& operator & (MyOArchive &ar, IMG_LWORD &t)
{
	
	if(ar.pulData)
		PU_PackDspLword( &t, (IMG_ULWORD **)&ar.pulData );
	ar.lwLength += PU_PSizeDspLword();
	return ar;
}

MyIArchive& operator & (MyIArchive &ar, IMG_REAL &t)
{
	PU_UnpackDspReal( (const IMG_ULWORD **)&ar.pulData, &t );
	return ar;
}

MyOArchive& operator & (MyOArchive &ar, IMG_REAL &t)
{
	
	if(ar.pulData)
		PU_PackDspReal( &t, (IMG_ULWORD **)&ar.pulData );
	ar.lwLength += PU_PSizeDspReal();
	return ar;
}

MyIArchive& operator & (MyIArchive &ar, IMG_UWORD &t)
{
	PU_UnpackDspUword( (const IMG_ULWORD **)&ar.pulData, &t );
	return ar;
}

MyOArchive& operator & (MyOArchive &ar, IMG_UWORD &t)
{
	if(ar.pulData)
		PU_PackDspUword( &t, (IMG_ULWORD **)&ar.pulData );
	ar.lwLength += PU_PSizeDspUword();
	return ar;
}

MyIArchive& operator & (MyIArchive &ar, IMG_UBYTE &t)
{
	PU_UnpackDspByte_dense( (const IMG_BYTE **)&ar.pulData, (IMG_BYTE*)&t );
	return ar;
}

MyOArchive& operator & (MyOArchive &ar, IMG_UBYTE &t)
{
	if(ar.pulData)
		PU_PackDspByte_dense( (IMG_BYTE*)&t, (IMG_BYTE **)&ar.pulData );
	ar.lwLength += PU_PSizeDspByte_dense();
	return ar;
}


template <class Archive, class T>
Archive& operator &(Archive &ar, T &t)
{
	t.serialize(ar);
	return ar;
}

template <class Archive, class T>
Archive& operator&(Archive &ar, std::vector<T> &vt)
{
	PUHead<Archive> x(ar);

	IMG_UWORD uwN=0;

	if(ar.is_saveing())
		uwN =vt.size();

	ar & uwN;     
	
	if(!ar.is_saveing())
		vt.resize(uwN);

	for( IMG_UWORD i=0; i<uwN; i++)
	{
		ar & vt[i];
	}
	return ar;
}
