#include <apt_ds.h>
#include <tiffio.h>
IMG_WORD	LoadBmp_MEM(IMG_CHAR const *pcFile, IMG_UBBUF	&ubbS);

struct memfile
{
/* a struct emulating a file [memory mapped] */
    unsigned char *buffer;
    tsize_t size;
    tsize_t eof;
    toff_t current;
};

static tsize_t
readproc (thandle_t clientdata, tdata_t data, tsize_t size)
{
/* emulating the read()  function */
    struct memfile *mem = (struct memfile *)clientdata;
    tsize_t len;
    if (mem->current >= (toff_t) mem->eof)
	return 0;
    len = size;
    if ((mem->current + size) >= (toff_t) mem->eof)
	len = (tsize_t) (mem->eof - mem->current);
    memcpy (data, mem->buffer + mem->current, len);
    mem->current += len;
    return len;
}

static tsize_t
writeproc (thandle_t clientdata, tdata_t data, tsize_t size)
{
/* emulating the write()  function */
    struct memfile *mem = (struct memfile *)clientdata;
    if ((mem->current + size) >= (toff_t) mem->size)
	return -1;
    memcpy (mem->buffer + mem->current, (unsigned char *) data, size);
    mem->current += size;
    if (mem->current > (toff_t) mem->eof)
	mem->eof = (tsize_t) (mem->current);
    return size;
}

static toff_t
seekproc (thandle_t clientdata, toff_t offset, int whence)
{
/* emulating the lseek()  function */
    struct memfile *mem = (struct memfile *)clientdata;
    switch (whence)
      {
      case SEEK_CUR:
	  if ((int) (mem->current + offset) < 0)
	      return (toff_t) - 1;
	  mem->current += offset;
	  if ((toff_t) mem->eof < mem->current)
	      mem->eof = (tsize_t) (mem->current);
	  break;
      case SEEK_END:
	  if ((int) (mem->eof + offset) < 0)
	      return (toff_t) - 1;
	  mem->current = mem->eof + offset;
	  if ((toff_t) mem->eof < mem->current)
	      mem->eof = (tsize_t) (mem->current);
	  break;
      case SEEK_SET:
      default:
	  if ((int) offset < 0)
	      return (toff_t) - 1;
	  mem->current = offset;
	  if ((toff_t) mem->eof < mem->current)
	      mem->eof = (tsize_t) (mem->current);
	  break;
      };
    return mem->current;
}

static int
closeproc (thandle_t clientdata)
{
/* emulating the close()  function */
    if (clientdata)
	return 0;		/* does absolutely nothing - required in order to suppress warnings */
    return 0;
}

static toff_t
sizeproc (thandle_t clientdata)
{
/* returning the pseudo-file current size */
    struct memfile *mem = (struct memfile *)clientdata;
    return mem->eof;
}

static int
mapproc (thandle_t clientdata, tdata_t * data, toff_t * offset)
{
    if (clientdata || data || offset)
	return 0;		/* does absolutely nothing - required in order to suppress warnings */
    return 0;
}

static void
unmapproc (thandle_t clientdata, tdata_t data, toff_t offset)
{
    if (clientdata || data || offset)
	return;			/* does absolutely nothing - required in order to suppress warnings */
    return;
}

template <class DType>
IMG_WORD est_tif_size(const IMG_UBBUF* img, size_t *extimated_size)
{
	*extimated_size = (256 * 1024) + (img->size.x * img->size.y *sizeof(DType));
	return OK;
}

template <class DType>
IMG_WORD image_to_tiff (const IMG_UBBUF* img, struct memfile &clientdata)
{
/* compressing a grayscale image as TIFF  GRAYSCALE */
	IMG_WORD	wStatus =OK;
    unsigned char *tiff_image = NULL;
    TIFF *out;
    int row;
    int col;
    tsize_t line_bytes;
    DType *scanline = NULL;
    DType *line_ptr;
    int pixel;
    clientdata.eof = 0;
    clientdata.current = 0;
    out =
	TIFFClientOpen ("tiff", "wb", &clientdata, readproc, writeproc, seekproc,
			closeproc, sizeproc, mapproc, unmapproc);
    if (out == NULL)
		return ERR_FAIL;
/* setting up the TIFF headers */
    TIFFSetField (out, TIFFTAG_SUBFILETYPE, 0);
    TIFFSetField (out, TIFFTAG_IMAGEWIDTH, img->size.x);
    TIFFSetField (out, TIFFTAG_IMAGELENGTH, img->size.y);
    TIFFSetField (out, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField (out, TIFFTAG_BITSPERSAMPLE, 8*sizeof(DType));
    TIFFSetField (out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField (out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField (out, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
	if(sizeof(DType)==4)	//Float
		TIFFSetField (out, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP);
    TIFFSetField (out, TIFFTAG_ROWSPERSTRIP, 1);
/* allocating the scan-line buffer */
    line_bytes = img->size.x *sizeof(DType);
    scanline = (DType *) _TIFFmalloc (line_bytes);
    for (row = 0; row < img->size.y; row++)
      {
	  line_ptr = scanline;
	  for (col = 0; col < img->size.x; col++)
	    {
		pixel = img->ptr[row* img->linestep + col];
		*line_ptr++ = pixel;
	    }
	  TIFFWriteScanline (out, scanline, row, 0);
      }
    _TIFFfree (scanline);
    TIFFClose (out);
   
    return wStatus;
}

IMG_WORD est_tif_size_float(const IMG_UBBUF* img, size_t *extimated_size)
{
	return est_tif_size<float>( img, extimated_size);
}

IMG_WORD image_to_tiff_float (const IMG_UBBUF* img, struct memfile &clientdata)
{
	return image_to_tiff<float>( img, clientdata);
}

IMG_WORD	FILE_SaveTifToMem(IMG_UBBUF const *pubbSbuf, IMG_CHAR const *pcFile)
{
	IMG_COORD	coZ ={0,0};
	IMG_WORD	wSts =OK;
	struct memfile clientdata;

	MEM_PushAllHeapStatus();
	// Query buffer size
	size_t extimated_size =0;
	wSts =est_tif_size<IMG_UBYTE>( pubbSbuf, &extimated_size);
	if( DSP_ERROR_STATUS(wSts)) goto Exit;

	memset (&clientdata, '\0', sizeof(clientdata));
	clientdata.buffer = (IMG_UBYTE*)MEM_Malloc(extimated_size, SYS_BANK1_8);
    if(!clientdata.buffer)
	{
		wSts =MEM_ERR_MALLOC; goto Exit;
	}
	
	memset (clientdata.buffer, '\0', extimated_size);
    clientdata.size = extimated_size;
	
	// Save to MEM
	wSts =image_to_tiff<IMG_UBYTE> ( pubbSbuf, clientdata);;
	if( DSP_ERROR_STATUS(wSts)) goto Exit;

	// Save to file
	FILE *fp =fopen(pcFile, "wb");
	fwrite( clientdata.buffer, clientdata.eof, 1, fp);
	fclose(fp);
Exit:
	MEM_PopAllHeapStatus();
	return wSts;
}

IMG_WORD	FILE_SaveFloatTifToMem(IMG_UBBUF const *pubbSbuf, IMG_CHAR const *pcFile)
{
	IMG_COORD	coZ ={0,0};
	IMG_WORD	wSts =OK;
	struct memfile clientdata;

	MEM_PushAllHeapStatus();
	// Query buffer size
	size_t extimated_size =0;
	wSts =est_tif_size_float( pubbSbuf, &extimated_size);
	if( DSP_ERROR_STATUS(wSts)) goto Exit;

	memset (&clientdata, '\0', sizeof(clientdata));
	clientdata.buffer = (IMG_UBYTE*)MEM_Malloc(extimated_size, SYS_BANK1_8);
    if(!clientdata.buffer)
	{
		wSts =MEM_ERR_MALLOC; goto Exit;
	}
	
	memset (clientdata.buffer, '\0', extimated_size);
    clientdata.size = extimated_size;
	
	// Save to MEM
	wSts =image_to_tiff_float ( pubbSbuf, clientdata);
	if( DSP_ERROR_STATUS(wSts)) goto Exit;

	// Save to file
	FILE *fp =fopen(pcFile, "wb");
	fwrite( clientdata.buffer, clientdata.eof, 1, fp);
	fclose(fp);
Exit:
	MEM_PopAllHeapStatus();
	return wSts;
}

#define ITF_SPI_CHK_ERROR	if (DSP_ERROR_STATUS(wStatus)) goto Exit;

static IMG_WORD	TestMilLoadSaveTiffImage(IMG_CHAR const *acProfPath)
{
	IMG_WORD	wStatus =OK;
	IMG_RBUF	rbbuf;
	IMG_WORD	wIsFile;
	IMG_SIZE	szImg;
	
	MEM_PushAllHeapStatus();

	if( !DOS_IsFileOrDirectoryExist(acProfPath, &wIsFile) || !wIsFile)
	{
		wStatus =ERR_INVALID_ARG; ITF_SPI_CHK_ERROR;
	}

	wStatus =FILE_ImageSize( (IMG_UBYTE*)acProfPath, &szImg);
	ITF_SPI_CHK_ERROR;

	wStatus =MEM_AllocBufAtBank( &szImg, SYS_BANK1_32, (IMG_VVBUF*)&rbbuf );
	ITF_SPI_CHK_ERROR;

	wStatus =FILE_LoadImageTiff_r( acProfPath,NULL, NULL,&rbbuf);
	ITF_SPI_CHK_ERROR;

	FILE_SaveImageTiff_r( &rbbuf,NULL, NULL,"T.tiff");
	
Exit:
	MEM_PopAllHeapStatus();
	return wStatus;
}


IMG_WORD Test2()
{
	MEM_PushAllHeapStatus();

	IMG_UBBUF	ubbS;
	IMG_COORD	coZ ={0,0};
	IMG_WORD	wStatus =OK;
	IMG_CHAR	*pcFile ="S.bmp";
	
	wStatus =LoadBmp_MEM( pcFile, ubbS);
	if (DSP_ERROR_STATUS(wStatus)) goto Exit;

	wStatus =FILE_SaveFloatTifToMem( &ubbS, "D.tiff");
	if (DSP_ERROR_STATUS(wStatus)) goto Exit;

	wStatus =TestMilLoadSaveTiffImage("D.tiff");
	if (DSP_ERROR_STATUS(wStatus)) goto Exit;
Exit:
	MEM_PopAllHeapStatus();
	return wStatus;
}
