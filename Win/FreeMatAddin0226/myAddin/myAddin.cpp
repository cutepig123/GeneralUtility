// myAddin.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "myAddin.h"
#include <tiffio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CmyAddinApp

BEGIN_MESSAGE_MAP(CmyAddinApp, CWinApp)
END_MESSAGE_MAP()


// CmyAddinApp construction

CmyAddinApp::CmyAddinApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CmyAddinApp object

CmyAddinApp theApp;

void MyTRACE(char const *acFormat,...)	//for debug
{
			
	va_list l;
	char acStr[500];
	va_start(l,acFormat);
	::_vsnprintf(acStr,500,acFormat,l);
	OutputDebugStringA(acStr);
	
}
// CmyAddinApp initialization

BOOL CmyAddinApp::InitInstance()
{
	CWinApp::InitInstance();

	
	return TRUE;
}

void addArrays(int N, float *a, float *b, float *c) {
	
  int i;
 
  for (i=0;i<N;i++)
   c[i] = a[i] + b[i];

  MyTRACE("addArrays exit\n");
}

void Pow2(int N, float *a)
{
	for(int i=0; i<N*N;i++)
		a[i] = a[i]*a[i];
}

#define	TYPE_32F	1
#define	TYPE_16S	2
#define	TYPE_16U	3

int tiffSize_( TIFF *image, int *W, int *H, int *Type)
{
  uint16 bps, spp, sfm;
  uint32 width;
  tsize_t stripSize;
  unsigned long imageOffset;
  int stripMax;
  
  //unsigned long bufferSize, count;
  
  // Check that it is of a type that we support
  if((TIFFGetField(image, TIFFTAG_BITSPERSAMPLE, &bps) == 0) ){
    return(42);
  }
  
  MyTRACE( "bits per sample(%d)\n", bps);

  //float type
   if((TIFFGetField(image, TIFFTAG_SAMPLEFORMAT, &sfm) == 0) ){
    
    return(42);
  }

   MyTRACE( "TIFFTAG_SAMPLEFORMAT(%d)\n", sfm);

   if (bps == 32 && sfm == SAMPLEFORMAT_IEEEFP)
	   *Type = TYPE_32F;
   else if (bps == 16 && (sfm == SAMPLEFORMAT_INT))
	   *Type = TYPE_16S;
   else if (bps == 16 && (sfm == SAMPLEFORMAT_UINT))
	   *Type = TYPE_16U;
   else
	   return 43;

  if((TIFFGetField(image, TIFFTAG_SAMPLESPERPIXEL, &spp) == 0) || (spp != 1)){
    MyTRACE( "Either undefined or unsupported number of samples per pixel\n");
    return(42);
  }
  // Read in the possibly multiple strips
  stripSize = TIFFStripSize (image);
  stripMax = TIFFNumberOfStrips (image);
  imageOffset = 0;
  long height = stripMax;
  *H = height;
  // Do whatever it is we do with the buffer -- we dump it in hex
  if(TIFFGetField(image, TIFFTAG_IMAGEWIDTH, &width) == 0){
    MyTRACE( "Image does not define its width\n");
    return(42);
  }
  *W = width;

  return 0;
}

int tiffSize( const char*file, int *W, int *H, int *Type)
{
  TIFF *image;
 
  // Open the TIFF image
  if((image = TIFFOpen(file, "r")) == NULL){
    MyTRACE( "Could not open incoming image\n");
    return(42);
  }
  
  int sts = tiffSize_( image, W, H, Type );
  if(sts!=0) goto Exit;

Exit:
  TIFFClose(image);

  return sts;
}

int readTiff(const char*file, int W, int H,int T, char *buffer){
  TIFF *image;
  uint16 photo;
  int width, height;
  tsize_t stripSize;
  int Type;

  unsigned long imageOffset, result;
  int stripCount;
  unsigned long bufferSize;
  // Open the TIFF image
  if((image = TIFFOpen(file, "r")) == NULL){
    MyTRACE( "Could not open incoming image\n");
    return(42);
  }

   int sts = tiffSize_( image, &width, &height, &Type );
   if(sts!=0) goto Exit;

   if( width!=W || height!=H || Type!=T || (Type!=TYPE_32F && Type!=TYPE_16S && Type!=TYPE_16U ))
   {
	   sts = 41; goto Exit;
   }
  // Read in the possibly multiple strips
  stripSize = TIFFStripSize (image);	//linestep
  height = TIFFNumberOfStrips (image);
  imageOffset = 0;
 	
  bufferSize = height * stripSize;

  for (stripCount = 0; stripCount < height; stripCount++){	//each row
    if((result = TIFFReadEncodedStrip (image, stripCount,
                      (char*)buffer + imageOffset,
                      stripSize)) == -1){
      MyTRACE( "Read error on input strip number %d\n", stripCount);
      return(42);
    }
    imageOffset += result;
  }
  // Deal with photometric interpretations
  if(TIFFGetField(image, TIFFTAG_PHOTOMETRIC, &photo) == 0){
    MyTRACE( "Image has an undefined photometric interpretation\n");
    return(42);
  }
  
  MyTRACE( "TIFFTAG_PHOTOMETRIC %d\n", photo);

  //if(photo != PHOTOMETRIC_MINISWHITE){
	
  //  // Flip bits
  //  // MyTRACE("Fixing the photometric interpretation\n");
  //  // for(count = 0; count < bufferSize; count++)
  //    // buffer[count] = ~buffer[count];
  //}
  // Deal with fillorder
 // if(TIFFGetField(image, TIFFTAG_FILLORDER, &fillorder) == 0){
 //   MyTRACE( "Image has an undefined fillorder\n");
 //   return(42);
 // }
 // 
 // if(fillorder != FILLORDER_MSB2LSB){
 //   // We need to swap bits -- ABCDEFGH becomes HGFEDCBA
	//MyTRACE( "Image has an undefined fillorder2\n");
 //   // MyTRACE("Fixing the fillorder\n");
 //   // for(count = 0; count < bufferSize; count++){
 //     // tempbyte = 0;
 //     // if(buffer[count] & 128) tempbyte += 1;
 //     // if(buffer[count] & 64) tempbyte += 2;
 //     // if(buffer[count] & 32) tempbyte += 4;
 //     // if(buffer[count] & 16) tempbyte += 8;
 //     // if(buffer[count] & 8) tempbyte += 16;
 //     // if(buffer[count] & 4) tempbyte += 32;
 //     // if(buffer[count] & 2) tempbyte += 64;
 //     // if(buffer[count] & 1) tempbyte += 128;
 //     // buffer[count] = tempbyte;
 //   // }
 // }
     
  
  // FILE *fp = fopen("c:\\temp\\tiff.log","w");

  // for(count = 0; count < bufferSize; count++){
    // fprintf(fp, "%03f ",  buffer[count]);
    // if((count + 1) % (width / 8) == 0) fprintf(fp, "\n");
    // else fprintf(fp, " ");
  // }
  // fclose(fp);
Exit:
  TIFFClose(image);
  return 0;
}

// for 
//int readTiff_16S(const char*file, int W, int H,int T, short *buffer)
//{
//	return readTiff