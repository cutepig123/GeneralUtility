//  Copyright (C) 2000 Tridia Corporation. All Rights Reserved.
//  Copyright (C) 1999 AT&T Laboratories Cambridge. All Rights Reserved.
//
//  This file is part of the VNC system.
//
//  The VNC system is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
//  USA.
//
// TightVNC distribution homepage on the Web: http://www.tightvnc.com/
//
// If the source code for the VNC system is not available from the place 
// whence you received this file, check http://www.uk.research.att.com/vnc or contact
// the authors on vnc@uk.research.att.com for information on obtaining it.


//  vncEncodeZlibHex

// This file implements the vncEncoder-derived vncEncodeZlibHex class.
// This class overrides some vncEncoder functions to produce a
// Hextile encoder with zlib.  Hextile splits all top-level update rectangles
// into smaller, 16x16 rectangles and encodes these using the
// optimized Hextile sub-encodings, including zlib.

#include "vncEncodeZlibHex.h"
#include "rfb.h"
// #include "MinMax.h"
#include <stdlib.h>
#include <time.h>

//#define IN_LEN		(128*1024)
//#define OUT_LEN		(IN_LEN + IN_LEN / 64 + 16 + 3)
//#define HEAP_ALLOC(var,size) \
//	lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]
//static HEAP_ALLOC(wrkmem,LZO1X_1_MEM_COMPRESS);


vncEncodeZlibHex::vncEncodeZlibHex()
{

	m_buffer = NULL;
	m_bufflen = 0;
	m_Queuebuffer = NULL;
	m_Queuelen = 0;
	MaxQueuebufflen=128*1024;
	compStreamRaw.total_in = ZLIBHEX_COMP_UNINITED;
	compStreamEncoded.total_in = ZLIBHEX_COMP_UNINITED;
	//lzo=false;
	m_Queuebuffer = new BYTE [MaxQueuebufflen+1];
		if (m_Queuebuffer == NULL)
		{
			vnclog.Print(LL_INTINFO, VNCLOG("Memory error"));
		}

}

vncEncodeZlibHex::~vncEncodeZlibHex()
{
	if (m_buffer != NULL)
	{
		delete [] m_buffer;
		m_buffer = NULL;
		m_bufflen = 0;
	}
	if ( compStreamRaw.total_in != ZLIBHEX_COMP_UNINITED )
	{
		deflateEnd( &compStreamRaw );
		compStreamRaw.total_in = ZLIBHEX_COMP_UNINITED;
	}
	if ( compStreamEncoded.total_in != ZLIBHEX_COMP_UNINITED )
	{
		deflateEnd( &compStreamEncoded );
		compStreamEncoded.total_in = ZLIBHEX_COMP_UNINITED;
	}
	if (m_Queuebuffer != NULL)
	{
		delete [] m_Queuebuffer;
		m_Queuebuffer = NULL;
	}

}

void
vncEncodeZlibHex::Init()
{
	vncEncoder::Init();
}

UINT
vncEncodeZlibHex::RequiredBuffSize(UINT width, UINT height)
{
	int accumSize;

	// Start with the raw encoding size, which includes the
	// rectangle header size.
	accumSize = vncEncoder::RequiredBuffSize(width, height);
	// Add overhead associated with zlib compression, worst case.
	accumSize += ((accumSize / 100) + 8);
	// Add zlib/other subencoding overhead, worst case.
	accumSize += (((width/16)+1) * ((height/16)+1) * ((3 * m_remoteformat.bitsPerPixel / 8) + 2));

	return accumSize;
}

UINT
vncEncodeZlibHex::NumCodedRects(RECT &rect)
{
	return 0;
	return 1;
}

/*
 * hextile.c
 *
 * Routines to implement Hextile Encoding
 */

#include <stdio.h>
#include "rfb.h"

/*
 * vncEncodeZlibHex::EncodeRect - send a rectangle using hextile encoding.
 */

UINT
vncEncodeZlibHex::EncodeRect(BYTE *source, VSocket *outConn, BYTE *dest, const RECT &rect)
{
	const int rectW = rect.right - rect.left;
	const int rectH = rect.bottom - rect.top;

	// Create the rectangle header
	rfbFramebufferUpdateRectHeader *surh=(rfbFramebufferUpdateRectHeader *)dest;
	surh->r.x = (CARD16) (rect.left-m_SWOffsetx);
	surh->r.y = (CARD16) (rect.top-m_SWOffsety);
	surh->r.w = (CARD16) (rectW);
	surh->r.h = (CARD16) (rectH);
	surh->r.x = Swap16IfLE(surh->r.x);
	surh->r.y = Swap16IfLE(surh->r.y);
	surh->r.w = Swap16IfLE(surh->r.w);
	surh->r.h = Swap16IfLE(surh->r.h);
	surh->encoding = Swap32IfLE(rfbEncodingZlibHex);

	rectangleOverhead += sz_rfbFramebufferUpdateRectHeader;
	dataSize += ( rectW * rectH * m_remoteformat.bitsPerPixel) / 8;

	// Go ahead and send the RFB update header, in case partial updates
	// are send in EncodeHextiles#() below.
	outConn->SendExactQueue( (char *)dest, sz_rfbFramebufferUpdateRectHeader);
//	AddToQueu((BYTE *)dest,sz_rfbFramebufferUpdateRectHeader,outConn);
	
	transmittedSize += sz_rfbFramebufferUpdateRectHeader;

	// Do the encoding
	UINT retval;
    switch (m_remoteformat.bitsPerPixel)
	{
	case 8:
		retval= EncodeHextiles8(source, dest, outConn, rect.left, rect.top, rectW, rectH);
		SendZlibHexrects(outConn);
		return retval;
    case 16:
		retval= EncodeHextiles16(source, dest, outConn, rect.left, rect.top, rectW, rectH);
		SendZlibHexrects(outConn);
		return retval;
    case 32:
		retval= EncodeHextiles32(source, dest, outConn, rect.left, rect.top, rectW, rectH);
		SendZlibHexrects(outConn);
		return retval;
    }

	return vncEncoder::EncodeRect(source, dest, rect);
}

UINT
vncEncodeZlibHex::zlibCompress(BYTE *from_buf, BYTE *to_buf, UINT length, struct z_stream_s *compressor)
{
	int totalCompDataLen = 0;
	int previousTotalOut;
	int deflateResult;
//	unsigned int out_len;

	// Initialize input/output buffer assignment for compressor state.
	compressor->avail_in = length;
	compressor->next_in = from_buf;
	compressor->avail_out = (2 * length);
	compressor->next_out = to_buf;
	compressor->data_type = Z_BINARY;
	/*if (lzo==false)
		{
			if (lzo_init() == LZO_E_OK) lzo=true;
		}
	if (lzo1x_1_compress(from_buf,length,to_buf,&out_len,wrkmem)!=LZO_E_OK)
		vnclog.Print(LL_INTINFO, VNCLOG("Error compressing  \n"));
	return out_len;*/

	//vnclog.Print(LL_INTINFO, VNCLOG("ZlibHex length %d %d \n"), length,out_len);


	// If necessary, the first time, initialize the compressor state.
	if ( compressor->total_in == ZLIBHEX_COMP_UNINITED )
	{

		compressor->total_in = 0;
		compressor->total_out = 0;
		compressor->zalloc = Z_NULL;
		compressor->zfree = Z_NULL;
		compressor->opaque = Z_NULL;

		vnclog.Print(LL_INTINFO, VNCLOG("calling deflateInit2 with zlib level:%d\n"), m_compresslevel);

		deflateResult = deflateInit2( compressor,
			                          m_compresslevel,
					                  Z_DEFLATED,
					                  MAX_WBITS,
					                  MAX_MEM_LEVEL,
					                  Z_DEFAULT_STRATEGY );
		if ( deflateResult != Z_OK )
		{
			vnclog.Print(LL_INTINFO, VNCLOG("deflateInit2 returned error:%d:%s\n"), deflateResult, compressor->msg);
			return -1;
		}

	}

	// Record previous total output size.
	previousTotalOut = compressor->total_out;

	// Compress the raw data into the result buffer.
	deflateResult = deflate( compressor, Z_SYNC_FLUSH );

	if ( deflateResult != Z_OK )
	{
		vnclog.Print(LL_INTINFO, VNCLOG("deflate returned error:%d:%s\n"), deflateResult, compressor->msg);
		return -1;
	}
//	vnclog.Print(LL_INTINFO, VNCLOG("ZlibHex length %d %d %d \n"), length,compressor->total_out - previousTotalOut,out_len);
//	if (length/2>out_len) vnclog.Print(LL_INTINFO, VNCLOG("ZlibHex length %d #####50######### \n"), length);
//	if (length/3*2>out_len) vnclog.Print(LL_INTINFO, VNCLOG("ZlibHex length %d #####33######### \n"), length);
//	if (length/4*3>out_len) vnclog.Print(LL_INTINFO, VNCLOG("ZlibHex length %d #####25######### \n"), length);

	return compressor->total_out - previousTotalOut;
}


#define PUT_PIXEL8(pix) (dest[destoffset++] = (pix))

#define PUT_PIXEL16(pix) (dest[destoffset++] = ((char*)&(pix))[0],			\
			  dest[destoffset++] = ((char*)&(pix))[1])

#define PUT_PIXEL32(pix) (dest[destoffset++] = ((char*)&(pix))[0],			\
			  dest[destoffset++] = ((char*)&(pix))[1],						\
			  dest[destoffset++] = ((char*)&(pix))[2],						\
			  dest[destoffset++] = ((char*)&(pix))[3])

#define DEFINE_SEND_HEXTILES(bpp)											\
																			\
static UINT subrectEncode##bpp(CARD##bpp *src, BYTE *dest,					\
				int w, int h, CARD##bpp bg,									\
			    CARD##bpp fg, BOOL mono);									\
static void testColours##bpp(CARD##bpp *data, int size, BOOL *mono,			\
			     BOOL *solid, CARD##bpp *bg, CARD##bpp *fg);				\
																			\
																			\
/*																			\
 * rfbSendHextiles															\
 */																			\
																			\
																			\
UINT																		\
vncEncodeZlibHex::EncodeHextiles##bpp(BYTE *source, BYTE *dest,				\
				  VSocket *outConn, int rx, int ry, int rw, int rh)			\
{																			\
    int x, y, w, h;															\
    int rectoffset, destoffset;												\
    int encodedBytes, compressedSize;										\
	CARD16* card16ptr;														\
	CARD##bpp bg, fg, newBg, newFg;											\
	bg=0;fg=0;newBg=0;newFg=0;												\
	BOOL mono, solid;														\
	BOOL validBg = FALSE;													\
	BOOL validFg = FALSE;													\
	int subEncodedLen;														\
	CARD##bpp clientPixelData[(16*16+2)*(bpp/8)+8+14+2];					\
																			\
	destoffset = 0;															\
																			\
    for (y = ry; y < ry+rh; y += 16)										\
	{																		\
		for (x = rx; x < rx+rw; x += 16)									\
		{																	\
		    w = h = 16;														\
		    if (rx+rw - x < 16)												\
				w = rx+rw - x;												\
		    if (ry+rh - y < 16)												\
				h = ry+rh - y;												\
																			\
			RECT hexrect;													\
			hexrect.left = x;												\
			hexrect.top = y;												\
			hexrect.right = x+w;											\
			hexrect.bottom = y+h;											\
			Translate(source, (BYTE *) clientPixelData, hexrect);			\
																			\
			rectoffset = destoffset;										\
			dest[rectoffset] = 0;											\
			destoffset++;													\
																			\
			testColours##bpp(clientPixelData, w * h,						\
			     &mono, &solid, &newBg, &newFg);							\
																			\
			if (!validBg || (newBg != bg))									\
			{																\
				validBg = TRUE;												\
				bg = newBg;													\
				dest[rectoffset] |= rfbHextileBackgroundSpecified;			\
				PUT_PIXEL##bpp(bg);											\
			}																\
																			\
			if (solid)														\
				continue;													\
																			\
			dest[rectoffset] |= rfbHextileAnySubrects;						\
																			\
			if (mono)														\
			{																\
				if (!validFg || (newFg != fg))								\
				{															\
					validFg = TRUE;											\
					fg = newFg;												\
					dest[rectoffset] |= rfbHextileForegroundSpecified;		\
					PUT_PIXEL##bpp(fg);										\
				}															\
			}																\
			else															\
			{																\
				validFg = FALSE;											\
				dest[rectoffset] |= rfbHextileSubrectsColoured;			    \
			}																\
																			\
			encodedBytes = subrectEncode##bpp(clientPixelData,				\
											dest + destoffset,				\
											w, h, bg, fg, mono);			\
																			\
			if (encodedBytes == 0)											\
			{																\
																			\
				/* hextile encoding was too large, use raw/zlib */			\
				if ((w*h*(bpp/8)) > VNC_ENCODE_ZLIBHEX_MIN_COMP_SIZE)		\
				{															\
					/* raw data large enough to use zlib */					\
					validBg = FALSE;										\
					validFg = FALSE;										\
					destoffset = rectoffset;								\
					dest[destoffset++] = rfbHextileZlibRaw;				\
																			\
					Translate(source, (BYTE *) clientPixelData, hexrect);	\
																			\
					compressedSize = zlibCompress((BYTE *) clientPixelData,	\
													dest + destoffset + 2,	\
													(w*h*(bpp/8)),			\
													&compStreamRaw);		\
																			\
																			\
					card16ptr = (CARD16*) (dest + destoffset);				\
					*card16ptr = Swap16IfLE(compressedSize);				\
					destoffset += compressedSize + 2;						\
																			\
			    }															\
				else														\
				{															\
					/* raw size small enough, use directly */				\
					validBg = FALSE;										\
					validFg = FALSE;										\
					destoffset = rectoffset;								\
					dest[destoffset++] = rfbHextileRaw;						\
																			\
					Translate(source, (dest + destoffset), hexrect);		\
																			\
					destoffset += (w*h*(bpp/8));							\
																			\
				}															\
			}																\
			else /* when (encodedBytes != 0) */								\
			{																\
				/* Hextile encoding smaller than raw, compress further? */	\
				/* Subencoded data harder to compress, need larger batch? */	\
				if (encodedBytes > (VNC_ENCODE_ZLIBHEX_MIN_COMP_SIZE * 2))	\
				{															\
					/* hex encoded data large enough to use zlib */			\
					subEncodedLen = (encodedBytes + destoffset - rectoffset - 1);		\
					destoffset = rectoffset + 1;								\
					memcpy( clientPixelData, (dest + destoffset), subEncodedLen);		\
					dest[rectoffset] |= rfbHextileZlibHex;					\
																			\
					compressedSize = zlibCompress((BYTE *) clientPixelData,	\
													dest + destoffset + 2,	\
													subEncodedLen,			\
													&compStreamEncoded);	\
																			\
																			\
					card16ptr = (CARD16*) (dest + destoffset);				\
					*card16ptr = Swap16IfLE(compressedSize);				\
					destoffset += compressedSize + 2;						\
			    }															\
				else														\
				{															\
					/* hex encoded data too small for zlib, send as is */	\
					destoffset += encodedBytes;								\
			    }															\
			}																\
																			\
		}																	\
																			\
		if (destoffset > VNC_ENCODE_ZLIBHEX_MIN_DATAXFER)					\
		{																	\
			/* Send the encoded data as partial update */					\
			AddToQueu((BYTE *)dest,destoffset,outConn);				        \
			transmittedSize += destoffset;									\
			encodedSize += destoffset;										\
			destoffset = 0;													\
																			\
		}																	\
    }																		\
	transmittedSize += destoffset;											\
	encodedSize += destoffset;												\
																			\
    return destoffset;														\
}																			\
																			\
static UINT																	\
subrectEncode##bpp(CARD##bpp *src, BYTE *dest, int w, int h, CARD##bpp bg,	\
		   CARD##bpp fg, BOOL mono)											\
{																			\
    CARD##bpp cl;															\
    int x,y;																\
    int i,j;																\
    int hx=0,hy,vx=0,vy;													\
    int hyflag;																\
    CARD##bpp *seg;															\
    CARD##bpp *line;														\
    int hw,hh,vw,vh;														\
    int thex,they,thew,theh;												\
    int numsubs = 0;														\
    int newLen;																\
    int rectoffset;															\
	int destoffset;															\
																			\
	destoffset = 0;															\
    rectoffset = destoffset;												\
    destoffset++;															\
																			\
    for (y=0; y<h; y++)														\
	{																		\
		line = src+(y*w);													\
		for (x=0; x<w; x++)													\
		{																	\
		    if (line[x] != bg)												\
			{																\
				cl = line[x];												\
				hy = y-1;													\
				hyflag = 1;													\
				for (j=y; j<h; j++)											\
				{															\
					seg = src+(j*w);										\
					if (seg[x] != cl) {break;}								\
					i = x;													\
					while ((seg[i] == cl) && (i < w)) i += 1;				\
					i -= 1;													\
					if (j == y) vx = hx = i;								\
					if (i < vx) vx = i;										\
					if ((hyflag > 0) && (i >= hx))							\
					{														\
						hy += 1;											\
					}														\
					else													\
					{														\
						hyflag = 0;											\
					}														\
				}															\
				vy = j-1;													\
																			\
				/* We now have two possible subrects: (x,y,hx,hy) and		\
				 * (x,y,vx,vy).  We'll choose the bigger of the two.		\
				 */															\
				hw = hx-x+1;												\
				hh = hy-y+1;												\
				vw = vx-x+1;												\
				vh = vy-y+1;												\
																			\
				thex = x;													\
				they = y;													\
																			\
				if ((hw*hh) > (vw*vh))										\
				{															\
				    thew = hw;												\
				    theh = hh;												\
				}															\
				else														\
				{															\
				    thew = vw;												\
				    theh = vh;												\
				}															\
																			\
				if (mono)													\
				{															\
				    newLen = destoffset - rectoffset + 2;					\
				}															\
				else														\
				{															\
				    newLen = destoffset - rectoffset + bpp/8 + 2;			\
				}															\
																			\
				if (newLen > (w * h * (bpp/8)))								\
				    return 0;												\
																			\
				numsubs += 1;												\
																			\
				if (!mono) PUT_PIXEL##bpp(cl);								\
																			\
				dest[destoffset++] = rfbHextilePackXY(thex,they);			\
				dest[destoffset++] = rfbHextilePackWH(thew,theh);			\
																			\
				/*															\
				 * Now mark the subrect as done.							\
				 */															\
				for (j=they; j < (they+theh); j++)							\
				{															\
					for (i=thex; i < (thex+thew); i++)						\
					{														\
						src[j*w+i] = bg;									\
					}														\
				}															\
		    }																\
		}																	\
    }																		\
																			\
    dest[rectoffset] = numsubs;												\
																			\
    return destoffset;														\
}																			\
																			\
																			\
/*																			\
 * testColours() tests if there are one (solid), two (mono) or more			\
 * colours in a tile and gets a reasonable guess at the best background	    \
 * pixel, and the foreground pixel for mono.								\
 */																			\
																			\
static void																	\
testColours##bpp(CARD##bpp *data, int size,									\
				 BOOL *mono, BOOL *solid,									\
				 CARD##bpp *bg, CARD##bpp *fg)								\
{																			\
    CARD##bpp colour1, colour2;												\
    int n1 = 0, n2 = 0;														\
    *mono = TRUE;															\
    *solid = TRUE;															\
	colour1=0;																\
	colour2=0;																\
																			\
    for (; size > 0; size--, data++)										\
	{																		\
																			\
		if (n1 == 0)														\
		    colour1 = *data;												\
																			\
		if (*data == colour1)												\
		{																	\
		    n1++;															\
		    continue;														\
		}																	\
																			\
		if (n2 == 0)														\
		{																	\
		    *solid = FALSE;													\
		    colour2 = *data;												\
		}																	\
																			\
		if (*data == colour2)												\
		{																	\
		    n2++;															\
		    continue;														\
		}																	\
																			\
		*mono = FALSE;														\
		break;																\
	}																		\
																			\
    if (n1 > n2)															\
	{																		\
		*bg = colour1;														\
		*fg = colour2;														\
    }																		\
	else																	\
	{																		\
		*bg = colour2;														\
		*fg = colour1;														\
    }																		\
}

DEFINE_SEND_HEXTILES(8)
DEFINE_SEND_HEXTILES(16)
DEFINE_SEND_HEXTILES(32)


void
vncEncodeZlibHex::AddToQueu(BYTE *source,int sizerect,VSocket *outConn)
{
	if (m_Queuelen+sizerect>(MaxQueuebufflen)) SendZlibHexrects(outConn);
	if (sizerect>(MaxQueuebufflen)) 
		{
			outConn->SendExactQueue( (char *)source,sizerect);
			return;
		}
//	vnclog.Print(LL_INTINFO, VNCLOG("Add %i %i \n"),sizerect,m_Queuelen);
	memcpy(m_Queuebuffer+m_Queuelen,source,sizerect);
	m_Queuelen+=sizerect;
}

void
vncEncodeZlibHex::SendZlibHexrects(VSocket *outConn)
{
	const int rawDataSize = (m_Queuelen);

	if (m_Queuelen==0) return; // NO update
	
		outConn->SendExactQueue( (char *)m_Queuebuffer, m_Queuelen); // 1 Small update
		m_Queuelen=0;
		encodedSize += m_Queuelen-sz_rfbFramebufferUpdateRectHeader;
		return;
}

void
vncEncodeZlibHex::LastRect(VSocket *outConn)
{
	SendZlibHexrects(outConn);
}