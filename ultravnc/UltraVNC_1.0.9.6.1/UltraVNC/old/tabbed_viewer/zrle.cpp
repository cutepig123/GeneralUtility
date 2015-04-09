//
// Copyright (C) 2002 RealVNC Ltd.  All Rights Reserved.
//

#include <rdr/ZlibInStream.h>
#include <rdr/FdInStream.h>
#include <rdr/Exception.h>
#include "stdhdrs.h"
#include "vncviewer.h"
// #include "ClientConnection.h"

// Instantiate the decoding function for 8, 16 and 32 BPP

//#define FAVOUR_FILL_RECT

#define zrleDecode ClientConnection::zrleDecode

#define ENDIAN_LITTLE 0
#define ENDIAN_BIG 1
#define ENDIAN_NO 2
#define BPP 8
#define ZYWRLE_ENDIAN ENDIAN_NO
#define IMAGE_RECT(x,y,w,h,data)                \
    SETUP_COLOR_SHORTCUTS;                      \
    if (m_DIBbits) ConvertAll(w,h,x, y,8/8,(BYTE *)m_netbuf,(BYTE *)m_DIBbits,m_si.framebufferWidth)
#define FILL_RECT(x,y,w,h,pix)                          \
    SETUP_COLOR_SHORTCUTS;                              \
    COLORREF color = COLOR_FROM_PIXEL8_ADDRESS(&pix);   \
    FillSolidRect8(x,y,w,h,&pix)
#include <rfb/zrleDecode.h>
#undef BPP
#undef ZYWRLE_ENDIAN
#undef IMAGE_RECT
#undef FILL_RECT

#define BPP 16
#define ZYWRLE_ENDIAN ENDIAN_LITTLE
#define IMAGE_RECT(x,y,w,h,data)                \
    SETUP_COLOR_SHORTCUTS;                      \
    if (m_DIBbits) ConvertAll(w,h,x, y,16/8,(BYTE *)m_netbuf,(BYTE *)m_DIBbits,m_si.framebufferWidth)
#define FILL_RECT(x,y,w,h,pix)                          \
    SETUP_COLOR_SHORTCUTS;                              \
    COLORREF color = COLOR_FROM_PIXEL16_ADDRESS(&pix);  \
    FillSolidRect16(x,y,w,h,(CARD16*)&pix)
#include <rfb/zrleDecode.h>
#undef BPP
#undef ZYWRLE_ENDIAN
#define BPP 15
#define ZYWRLE_ENDIAN ENDIAN_LITTLE
#include <rfb/zrleDecode.h>
#undef BPP
#undef ZYWRLE_ENDIAN
#undef IMAGE_RECT
#undef FILL_RECT

#define IMAGE_RECT(x,y,w,h,data)                \
    SETUP_COLOR_SHORTCUTS;                      \
    if (m_DIBbits) ConvertAll(w,h,x, y,32/8,(BYTE *)m_netbuf,(BYTE *)m_DIBbits,m_si.framebufferWidth)
#define FILL_RECT(x,y,w,h,pix)                          \
    SETUP_COLOR_SHORTCUTS;                              \
    COLORREF color = COLOR_FROM_PIXEL32_ADDRESS(&pix);  \
    FillSolidRect32(x,y,w,h,(CARD32*)&pix)

#define BPP 32
#define ZYWRLE_ENDIAN ENDIAN_LITTLE
#include <rfb/zrleDecode.h>
#define CPIXEL 24A
#include <rfb/zrleDecode.h>
#undef CPIXEL
#define CPIXEL 24B
#include <rfb/zrleDecode.h>
#undef CPIXEL
#undef BPP
#undef ZYWRLE_ENDIAN
#undef IMAGE_RECT
#undef FILL_RECT

#undef zrleDecode

void ClientConnection::zrleDecode(int x, int y, int w, int h)
{
  try {
    CheckBufferSize(rfbZRLETileWidth * rfbZRLETileHeight * 4);
    omni_mutex_lock l(m_bitmapdcMutex);
    ObjectSelector b(m_hBitmapDC, m_hBitmap);
    PaletteSelector p(m_hBitmapDC, m_hPalette);

	if( zywrle ){
	  if( !m_opts.m_enableJpegCompression ){
		  zywrle_level = 1;
	  }else if( m_opts.m_jpegQualityLevel < 3 ){
		  zywrle_level = 3;
	  }else if( m_opts.m_jpegQualityLevel < 6 ){
		  zywrle_level = 2;
	  }else{
		  zywrle_level = 1;
	  }
	}else{
	  zywrle_level = 0;
	}

    switch (m_myFormat.bitsPerPixel) {

    case 8:
      zrleDecode8NE(x,y,w,h,fis,zis,(rdr::U8*)m_netbuf);
      break;

    case 16:
      if( m_myFormat.greenMax > 0x1F ){
        zrleDecode16LE(x,y,w,h,fis,zis,(rdr::U16*)m_netbuf);
	  }else{
        zrleDecode15LE(x,y,w,h,fis,zis,(rdr::U16*)m_netbuf);
	  }
      break;

    case 32:
      bool fitsInLS3Bytes
        = ((m_myFormat.redMax   << m_myFormat.redShift)   < (1<<24) &&
           (m_myFormat.greenMax << m_myFormat.greenShift) < (1<<24) &&
           (m_myFormat.blueMax  << m_myFormat.blueShift)  < (1<<24));

      bool fitsInMS3Bytes = (m_myFormat.redShift   > 7  &&
                             m_myFormat.greenShift > 7  &&
                             m_myFormat.blueShift  > 7);

      if ((fitsInLS3Bytes && !m_myFormat.bigEndian) ||
          (fitsInMS3Bytes && m_myFormat.bigEndian))
      {
        zrleDecode24ALE(x,y,w,h,fis,zis,(rdr::U32*)m_netbuf);
      }
      else if ((fitsInLS3Bytes && m_myFormat.bigEndian) ||
               (fitsInMS3Bytes && !m_myFormat.bigEndian))
      {
        zrleDecode24BLE(x,y,w,h,fis,zis,(rdr::U32*)m_netbuf);
      }
      else
      {
        zrleDecode32LE(x,y,w,h,fis,zis,(rdr::U32*)m_netbuf);
      }
      break;
    }

  } catch (rdr::Exception& e) {
    fprintf(stderr,"ZRLE decoder exception: %s\n",e.str());
    throw;
  }
}
