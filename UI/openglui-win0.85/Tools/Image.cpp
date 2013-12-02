#include "Image.h"
#include "MediaPathManager.h"

Image::Image(const char *path)
{
  internalFormat =    0;
  components     =    0;
  dataBuffer     = NULL;
  format         =    0;
  height         =    0;
  width          =    0;
  depth          =    0;

  if(path)
   load(path);
}

void Image::setWidth(unsigned short w)
{
  width = w;
}

void Image::setHeight(unsigned short h){
  height = h;
}

void Image::setDataBuffer(const unsigned char *buffer)
{
  if(!buffer)
    return;

  if(dataBuffer)
    deleteArray(dataBuffer);

  dataBuffer = new unsigned char[width*height*components];
  memcpy(dataBuffer, buffer, width*height*components);
}

void Image::setFormat(unsigned int  f)
{
  format = f;
}

void Image::setInternalFormat(unsigned int  iformat)
{
  internalFormat = iformat;
}

void Image::setComponentsCount(unsigned int  c)
{
  components = c < 1 ? 1 : c > 4 ? 4 : c;
}

void Image::flipVertically()
{
  if(depth)
    return;

  unsigned char *newDataBuffer = new unsigned char[width*height*components];
  int      counterDown   = 0,
           counterUp     = 0;

   if(components == 3){
    for(int y = 0, y1 = height - 1; y < height; y++, y1--)
      for(int x = 0; x < width; x++){
        counterUp   = (x + y1*width)*3;
        counterDown = (x +  y*width)*3;
        newDataBuffer[counterUp + 0]   = dataBuffer[counterDown + 0];
        newDataBuffer[counterUp + 1]   = dataBuffer[counterDown + 1];
        newDataBuffer[counterUp + 2]   = dataBuffer[counterDown + 2];
      }
  }

  if(components == 4){
    for(int y = 0, y1 = height - 1; y < height; y++, y1--)
      for(int x = 0; x < width; x++){
        counterUp   = (x + y1*width)*components;
        counterDown = (x +  y*width)*components;
        newDataBuffer[counterUp + 0]   = dataBuffer[counterDown + 0];
        newDataBuffer[counterUp + 1]   = dataBuffer[counterDown + 1];
        newDataBuffer[counterUp + 2]   = dataBuffer[counterDown + 2];
        newDataBuffer[counterUp + 3]   = dataBuffer[counterDown + 3];
      }
  }

  if(components == 1){
    for(int y = 0, y1 = height - 1; y < height; y++, y1--)
      for(int x = 0; x < width; x++){
        counterUp   = x + y1*width;
        counterDown = x +  y*width;
        newDataBuffer[counterUp + 0]   = dataBuffer[counterDown + 0];
      }
  }

  setDataBuffer(newDataBuffer);
  deleteArray(newDataBuffer);
}

bool Image::load(const char* path_)
{
  const char *verifiedPath = MediaPathManager::lookUpMediaPath(path_);

  if(!verifiedPath)
    return false;

  path = verifiedPath;

  if(isPNG(verifiedPath))
    return loadPNG(verifiedPath, this);

  if(isJPEG(verifiedPath))
    return loadJPG(verifiedPath);

  if(isTGA(verifiedPath))
    return loadTGA(verifiedPath);

  return false;
}


const unsigned int    Image::getComponentsCount() const { return components;     }
const unsigned int    Image::getInternalFormat()  const { return internalFormat; }
const unsigned char* Image::getDataBuffer()      const { return dataBuffer;     }
const unsigned int    Image::getFormat()          const { return format;         }
const String  &Image::getPath()            const { return path;           }

const unsigned short Image::getHeight() const {return height;}
const unsigned short Image::getWidth()  const {return width; }
const unsigned short Image::getDepth()  const {return depth; }

Image::~Image()
{
  if(dataBuffer)
    deleteArray(dataBuffer);
}

/*******************************************************************************************/
/*JPEG Loader                                                                              */
/*Info:                                                                                    */
/*                                                                                         */
/*******************************************************************************************/


#ifndef max
#define max(a, b) (((a)>(b))?(a):(b))
#endif

#ifndef min
#define min(a, b) (((a)<(b))?(a):(b))
#endif

unsigned char getByte();
char fileOpen(const char *);

void decodeQTable(int);
void strmSkip(int n);
void decodeBlock();
void fileClose();
void getInfo();
void fidct();

int  wordDec(int);

typedef unsigned short qTable[64];

typedef struct
{
  struct tables{
    unsigned char size;
    unsigned char code;
  } TSmall[512], TLarge[65536];
} HuffTable;

void decodeHuffTable(int);

typedef struct
{
  HuffTable *huffAC, *huffDC;
  qTable *qTab;
  int dcPrev,smpx, smpy;
  float t[256];
}ComponentTable;

ComponentTable component[4];
HuffTable      huffTableAC[4],
               huffTableDC[4];
qTable         qtable[4];

unsigned int xblock, yblock, blockx, blocky,
             bsize, restartInt, bfree,
             dt;
unsigned char *data, *bpos  , *dend,
              eof  , ssStart, ssEnd,
              sbits, prec   , ncomp;
float dctt[64];

int zigzag[64]=
{
   0,	1,	8,	16,	9,	2,	3,	10,
  17,	24,	32,	25,	18,	11,	4,	5,
  12,	19,	26,	33,	40,	48,	41,	34,
  27,	20,	13,	6,	7,	14,	21,	28,
  35,	42,	49,	56,	57,	50,	43,	36,
  29,	22,	15,	23,	30,	37,	44,	51,
  58,	59,	52,	45,	38,	31,	39,	46,
  53,	60,	61,	54,	47,	55,	62,	63
};


char fileOpen(const char *filename)
{
  FILE *stream;
  data = NULL;
  if((stream=fopen(filename, "rb"))==NULL)
    return 0;
  else{
    fseek(stream, 0, SEEK_END);
    bsize = ftell(stream);
    fseek(stream, 0, SEEK_SET);
    data = new unsigned char[bsize];
    fread(data, 1, bsize, stream);
    fclose(stream);
    return 1;
  }
}

void fileClose(void)
{
  if(data)
    delete[] data;
}

unsigned char getByte(void)
{
  if(bpos>=dend){
    eof = 1;
    return 0;
  }
  else
    return *bpos++;
}

void strmSkip(int n)
{
  unsigned char a, b;
  bfree+=n;
  dt<<=n;

  while(bfree>=8){
    bfree-=8;
    b = getByte();
    if(b==255)
      a=getByte();
    dt|=(b<<bfree);
  }
}

int huffDec(HuffTable *h)
{
  unsigned int id, n, c;

  id = (dt>>(23));
  n  = h->TSmall[id].size;
  c  = h->TSmall[id].code;

  if(n==255){
    id = (dt>>(16));
    n  = h->TLarge[id].size;
    c  = h->TLarge[id].code;
  }

  strmSkip(n);
  return c;
}

int wordDec(int n)
{
  int w;
  unsigned int s;

  if(n==0)
    return 0;
  else{
   s= (dt>>(31));
   w= (dt>>(32-n));
   strmSkip(n);
   if(s==0)
     w = (w|(0xffffffff<<n))+1;
  }
  return w;
}

void Image::getJPGInfo()
{
  unsigned char cn, sf, qt;
  int i;

  prec = getByte();

  if(prec!=8)
    return;

  height = ((getByte()<<8)+getByte());
  width  = ((getByte()<<8)+getByte());
  ncomp  = getByte();

  if((ncomp!=3)&&(ncomp!=1))
    return;

  for(i=0;i<ncomp;i++)
  {
    cn = getByte();
    sf = getByte();
    qt = getByte();

    component[cn-1].qTab = &qtable[qt];
    component[cn-1].smpy = sf&15;
    component[cn-1].smpx = (sf>>4)&15;
  }

  if(component[0].smpx == 1)
    blockx = 8;
  else
    blockx = 16;

  if(component[0].smpy==1)
    blocky = 8;
  else
    blocky = 16;

  xblock=width/blockx;

  if((width & (blockx-1))!=0)
    xblock++;

  yblock = height/blocky;

  if((height&(blocky-1))!=0)
    yblock++;
}

void decodeHuffTable(int len)
{
  int length[257], i, j, n, code, codelen, delta, rcode, cd, rng;
  unsigned char lengths[16], b, symbol[257];
  HuffTable *h;

  len-=2;

  while(len>0){
    b = getByte();
    len--;
    h = &huffTableDC[0];

    switch(b){
      case 0:
      h = &huffTableDC[0];
      break;

      case 1:
      h = &huffTableDC[1];
      break;

      case 16:
      h = &huffTableAC[0];
      break;

      case 17:
      h=&huffTableAC[1];
      break;
    }

    for(i=0;i<16;i++)
      lengths[i] = getByte();

    len -= 16;
    n    =  0;

    for(i=0;i<16;i++){
      len-=lengths[i];
      for(j=0;j<lengths[i];j++){
        symbol[n]   = getByte();
        length[n++] = i+1;
      }
    }

    code = 0;
    codelen = length[0];

    for(i=0;i<n;i++){

      rcode = code<<(16-codelen);
      cd = rcode>>7;

      if(codelen<=9){
        rng = 1 <<(9-codelen);

        for(j=cd;j<cd+rng;j++){
          h->TSmall[j].code = (unsigned char)symbol[i];
          h->TSmall[j].size = (unsigned char)codelen;
        }
      }
      else{
        h->TSmall[cd].size=(unsigned char)255;
        rng = 1<<(16-codelen);

        for(j=rcode;j<rcode+rng;j++){
          h->TLarge[j].code=(unsigned char)symbol[i];
          h->TLarge[j].size=(unsigned char)codelen;
        }
      }

      code++;
      delta=length[i+1]-codelen;
      code<<=delta;
      codelen+=delta;
    }
  }
}

void fidct(void)
{
  float a = 0.353553385f,
        b = 0.490392625f,
        c = 0.415734798f,
        d = 0.277785122f,
        e = 0.097545162f,
        f = 0.461939752f,
        g = 0.191341713f,
        cd =0.6935199499f,
        be =0.5879377723f,
        bc =0.9061274529f,
        de =0.3753302693f,
        a0, f2, g2, a4, f6, g6, s0, s1, s2, s3,
        t0, t1, t2, t3, m0, m1, m2, m3,
        h0, h1, h2, h3, r0, r1, r2, r3, w;
  int i;

  for(i=0;i<64;i+=8){
    if((dctt[i+1]!=0)||(dctt[i+2]!=0)||(dctt[i+3]!=0)||(dctt[i+4]!=0)||(dctt[i+5]!=0)||(dctt[i+6]!=0)||(dctt[i+7]!=0))
    {
      a0 = a*dctt[i];
      f2 = f*dctt[i+2];
      g2 = g*dctt[i+2];
      a4 = a*dctt[i+4];
      g6 = g*dctt[i+6];
      f6 = f*dctt[i+6];
      m0 = a0+a4;
      m1 = a0-a4;
      m2 = f2+g6;
      m3 = g2-f6;
      s0 = m0+m2;
      s1 = m1+m3;
      s2 = m1-m3;
      s3 = m0-m2;
      h2 = dctt[i+7]+dctt[i+1];
      h3 = dctt[i+7]-dctt[i+1];
      r2 = dctt[i+3]+dctt[i+5];
      r3 = dctt[i+3]-dctt[i+5];
      h0 = cd*dctt[i+1];
      h1 = be*dctt[i+1];
      r0 = be*dctt[i+5];
      r1 = cd*dctt[i+3];
      w = de*r3;
      t0 = h1+r1+e*(h3+r3)-w;
      t1 = h0-r0-d*(h2-r3)-w;
      w = bc*r2;
      t2 = h0+r0+c*(h3+r2)-w;
      t3 = h1-r1-b*(h2+r2)+w;
      dctt[i] = s0+t0;
      dctt[i+1] = s1+t1;
      dctt[i+2] = s2+t2;
      dctt[i+3] = s3+t3;
      dctt[i+4] = s3-t3;
      dctt[i+5] = s2-t2;
      dctt[i+6] = s1-t1;
      dctt[i+7] = s0-t0;
    }
    else{
      a0 = dctt[i]*a;
      dctt[i]=dctt[i+1]=dctt[i+2]=dctt[i+3]=dctt[i+4]=dctt[i+5]=dctt[i+6]=dctt[i+7]=a0;
    }
  }

  for(i=0;i<8;i++){
    if((dctt[8+i]!=0)||(dctt[16+i]!=0)||(dctt[24+i]!=0)||(dctt[32+i]!=0)||(dctt[40+i]!=0)||(dctt[48+i]!=0)||(dctt[56+i]!=0))
    {
      a0 = a*dctt[i];
      f2 = f*dctt[16+i];
      g2 = g*dctt[16+i];
      a4 = a*dctt[32+i];
      g6 = g*dctt[48+i];
      f6 = f*dctt[48+i];
      m0 = a0+a4;
      m1 = a0-a4;
      m2 = f2+g6;
      m3 = g2-f6;
      s0 = m0+m2;
      s1 = m1+m3;
      s2 = m1-m3;
      s3 = m0-m2;
      h2 = dctt[56+i]+dctt[8+i];
      h3 = dctt[56+i]-dctt[8+i];
      r2 = dctt[24+i]+dctt[40+i];
      r3 = dctt[24+i]-dctt[40+i];
      h0 = cd*dctt[8+i];
      h1 = be*dctt[8+i];
      r0 = be*dctt[40+i];
      r1 = cd*dctt[24+i];
      w = de*r3;
      t0 = h1+r1+e*(h3+r3)-w;
      t1 = h0-r0-d*(h2-r3)-w;
      w = bc*r2;
      t2 = h0+r0+c*(h3+r2)-w;
      t3 = h1-r1-b*(h2+r2)+w;
      dctt[i] = s0+t0;
      dctt[i+8] = s1+t1;
      dctt[i+16] = s2+t2;
      dctt[i+24] = s3+t3;
      dctt[i+32] = s3-t3;
      dctt[i+40] = s2-t2;
      dctt[i+48] = s1-t1;
      dctt[i+56] = s0-t0;
    }
    else{
      a0 = dctt[i]*a;
      dctt[i]=dctt[i+8]=dctt[i+16]=dctt[i+24]=dctt[i+32]=dctt[i+40]=dctt[i+48]=dctt[i+56]=a0;
    }
  }
}

void decodeQTable(int len)
{
  int i;
  unsigned char b;

  len-=2;

  while(len>0){
    b = (unsigned char)getByte();
    len--;

    if((b&16)==0){
      for(i=0;i<64;i++)
        qtable[b&15][i]=getByte();
      len-=64;
    }
    else{
      for(i=0;i<64;i++)
        qtable[b&15][i]=((getByte()<<8)+getByte());
      len-=128;
    }
  }
}

void decodeBlock(void)
{
  int compn, i, j, b, p, codelen, code, cx, cy, otab[64];
  qTable *qtab;

  for(compn=0;compn<ncomp;compn++)
  {
    qtab = component[compn].qTab;

    for(cy=0;cy<component[compn].smpy;cy++){
      for(cx=0;cx<component[compn].smpx;cx++){
        for(i=0;i<64;i++)
          otab[i]=0;

          codelen= huffDec(component[compn].huffDC);
          code=wordDec(codelen);

         otab[0] = code+component[compn].dcPrev;
         component[compn].dcPrev = otab[0];
         i=1;

         while(i<64){
           codelen=huffDec(component[compn].huffAC);
             if(codelen==0)
               i=64;
             else
               if(codelen==0xf0)
                 i+=16;
             else{
               code = wordDec(codelen&15);
               i = i+(codelen>>4);
               otab[i++]=code;
             }
           }

           for(i=0;i<64;i++)
             dctt[zigzag[i]]=(float)((*qtab)[i]*otab[i]);

             fidct();
             b=(cy<<7)+(cx<<3);
             p=0;

             for(i=0;i<8;i++){
               for(j=0;j<8;j++)
                 component[compn].t[b++]=dctt[p++]+128;
                 b+=8;
               }
             }
     }
  }
}

int Image::decodeScanJPG()
{
  unsigned int nnx, nny, i, j,
               xmin, ymin, xmax, ymax, blockn, adr1, adr2,
               y1, u1, v1, y2, u2, v2, u3, v3,
               dux, duy, dvx, dvy;
  unsigned char sc, ts;
  float cy, cu, cv;
  components = GLuint(getByte());

  setFormat(GL_BGR);
  setInternalFormat(GL_RGB8);

  if(dataBuffer)
    deleteArray(dataBuffer);

  dataBuffer = new unsigned char[width*height*components];

  for(i=0;i<components;i++){
    sc = getByte();
    ts = getByte();

    component[sc-1].huffDC = &huffTableDC[ts>>4];
    component[sc-1].huffAC = &huffTableAC[ts&15];
  }

  ssStart = getByte();
  ssEnd   = getByte();
  sbits   = getByte();

  if((ssStart!=0)||(ssEnd!=63))
   return 0;

  if(components == 3){
    dux = 2+component[1].smpx-component[0].smpx;
    duy = 2+component[1].smpy-component[0].smpy;
    dvx = 2+component[2].smpx-component[0].smpx;
    dvy = 2+component[2].smpy-component[0].smpy;
  }

  dt    = 0;
  bfree = 0;
  strmSkip(32);

  blockn=0;
  ymin=0;

  for(nny=0;nny<yblock;nny++){
    ymax = ymin+blocky;

    if(ymax>height)
      ymax = height;

    xmin=0;

    for(nnx=0;nnx<xblock;nnx++){
      xmax=xmin+blockx;

      if(xmax>width)
        xmax=width;

      decodeBlock();

      blockn++;
      if((blockn==restartInt)&&((nnx<xblock-1)||(nny<yblock-1)))
      {
        blockn=0;

         if(bfree!=0)
           strmSkip(8-bfree);

         if(wordDec(8)!=255)
           return 0;

         for(i=0;i<components;i++)
          component[i].dcPrev=0;
        }

        if(components ==3){
          y1=u1=v1=0;
          adr1=(height-1-ymin)*width+xmin;

          for(i=ymin;i<ymax;i++){
            adr2=adr1;
            adr1-=width;
            y2=y1;
            y1+=16;
            u3=(u1>>1)<<4;
            u1+=duy;
            v3=(v1>>1)<<4;
            v1+=dvy;
            u2=v2=0;

            for(j=xmin;j<xmax;j++){
              int cr, cg, cb;
              cy=component[0].t[y2++];
              cu=component[1].t[u3+(u2>>1)]-128.0f;
              cv=component[2].t[v3+(v2>>1)]-128.0f;

              cr=(int)(cy+1.402f*cv);
              cg=(int)(cy-0.34414f*cu-0.71414f*cv);
              cb=(int)(cy+1.772f*cu);

              dataBuffer[3*adr2]   = max(0, min(255, cb));
              dataBuffer[3*adr2+1] = max(0, min(255, cg));
              dataBuffer[3*adr2+2] = max(0, min(255, cr));
              adr2++;

              u2+=dux;
              v2+=dvx;
            }
          }
        }
        else
          if(components==1)
          {
            y1=0;
            adr1=(height-1-ymin)*width+xmin;

            for(i=ymin;i<ymax;i++){
              adr2=adr1;
              adr1-=width;
              y2=y1;
              y1+=16;

              for(j=xmin;j<xmax;j++){
                int lum=(int)component[0].t[y2++];
                dataBuffer[adr2++]=max(0, min(255, lum));
              }
            }
          }
          xmin=xmax;
        }
      ymin=ymax;
    }
  return 1;
}

int Image::decodeJPG()
{
  int w;
  unsigned char a, hdr=0, scan=0;

  eof=0;
  bpos=data;
  dend=bpos+bsize;
  w=((getByte()<<8)+getByte());

  if(w!=0xffd8)
   return 0;

  while(eof==0)
  {
    a=(unsigned char)getByte();

    if(a!=0xff)
      return 0;

    a=(unsigned char)getByte();
    w=((getByte()<<8)+getByte());

    switch(a){
      case 0xe0:
        if(hdr!=0)
      break;

      if(getByte()!='J')
        return 0;

      if(getByte()!='F')
        return 0;

      if(getByte()!='I')
        return 0;

      if(getByte()!='F')
        return 0;

      hdr=1;
      w-=4;
      break;

     case 0xc0:
     getJPGInfo();
     w=0;
     break;

     case 0xc4:
       decodeHuffTable(w);
       w=0;
       break;

       case 0xd9:
       w=0;
       break;

       case 0xda:
       if(scan!=0)
         break;

       scan=1;

       if(!decodeScanJPG())
         return 0;

        w=0;
        eof=1;
        break;

        case 0xdb:
         decodeQTable(w);
         w=0;
         break;

        case 0xdd:
        restartInt=((getByte()<<8)+getByte());
        w=0;
        break;
      }

      while(w>2){
        getByte();
        w--;
      }
    }
  return 1;
}

bool Image::loadJPG(const char *filename)
{
  int i;
  for(i=0;i<4;i++)
    component[i].dcPrev=0;

    restartInt=-1;

   if(fileOpen(filename)){
      int ret= decodeJPG();
      fileClose();
      if(!ret)
        return false;
       return true;
    }
    else
     return false;
}

/*******************************************************************************************/
/*PNG Loader                                                                               */
/*Info:                                                                                    */
/*                                                                                         */
/*******************************************************************************************/

bool loadPNG(const char* filename, Image* image)
{
  pngRawInfo info;

  if(!pngLoadRaw(filename, &info))
    return false;

  image->setComponentsCount(info.Components);
  image->setWidth(info.Width);
  image->setHeight(info.Height);
  image->setFormat(info.Components == 4 ? GL_RGBA :
                   info.Components == 3 ? GL_RGB  : GL_LUMINANCE );
  image->setInternalFormat(info.Components == 4 ? GL_RGBA8 :
                           info.Components == 3 ? GL_RGB8  : GL_LUMINANCE8);
  image->setDataBuffer(info.Data);

  delete info.Data;
  delete info.Palette;

  return true;
}

/*******************************************************************************************/
/*TGA loader                                                                               */
/*Info:                                                                                    */
/*                                                                                         */
/*******************************************************************************************/

bool Image::loadTGA(const char* filename)
{
  unsigned char uncompressed8BitTGAHeader[12]= {0, 1, 1, 0, 0, 0, 1, 24, 0, 0, 0, 0},
                uncompressedTGAHeader[12]    = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                compressedTGAHeader[12]      = {0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  unsigned char TGAcompare[12];           //Used to compare TGA header

  FILE * file = fopen(filename, "rb");    //Open the TGA file

  if(file==NULL )                         //Does the file exist?
    return Logger::writeErrorLog(String("Could not find file -> ") + filename);


  //read the header
  fread(TGAcompare, 1, sizeof(TGAcompare), file);
  fclose(file);

  if(!memcmp(uncompressedTGAHeader, TGAcompare, sizeof(uncompressedTGAHeader)))
  {
    return loadUncompressedTrueColorTGA(filename);
  }
  else if(!memcmp(compressedTGAHeader, TGAcompare, sizeof(compressedTGAHeader)))
  {
    return loadCompressedTrueColorTGA(filename);
  }
  else if(!memcmp(uncompressed8BitTGAHeader, TGAcompare, sizeof(uncompressed8BitTGAHeader)))
  {
    return loadUncompressed8BitTGA(filename);
  }
  else
    return Logger::writeErrorLog(String("Unrecognized TGA format -> ") + filename);

  return false;
}

bool Image::loadUncompressed8BitTGA(const char * filename)
{
  unsigned char   TGAHeader[12]={0, 1, 1, 0, 0, 0, 1, 24, 0, 0, 0, 0};
  unsigned char   TGAcompare[12];           //Used to compare TGA header
  unsigned char   header[6];              //First 6 useful bytes of the header

  FILE * file = fopen(filename, "rb");        //Open the TGA file

  if(file == NULL)                //Does the file exist?
    return Logger::writeErrorLog(String("Could not find file at -> ") + filename);

  if(fread(TGAcompare, 1, sizeof(TGAcompare), file)!=sizeof(TGAcompare)|| //Are there 12 bytes to read?
    memcmp(TGAHeader, TGAcompare, sizeof(TGAHeader))!=0 ||          //Is the header correct?
    fread(header, 1, sizeof(header), file)!=sizeof(header))   //Read next 6 bytes
  {
    fclose(file);               //If anything else failed, close the file
    return Logger::writeErrorLog(String("Could not process file at -> ") + filename);
  }

  //save data into class member variables
  setWidth(header[1]*256+header[0]);
  setHeight(header[3]*256+header[2]);
  setComponentsCount(header[4]/8);

  if(width  <=0 || //if width <=0
     height <=0 || //or height<=0
     header[4] != 8)    //bpp not 8
  {
    fclose(file);                     //close the file
    return Logger::writeErrorLog(String("The height or width is less than zero, or the TGA is not 8 bpp -> ") + filename);
  }

  setFormat(GL_RGB);
  setInternalFormat(GL_RGB8);

  //make space for palette
  unsigned char * palette=new unsigned char[256*3];
  if(!palette)
    return Logger::writeErrorLog("Unable to allocate memory for palette");


  //load the palette
  fread(palette, 256*3, 1, file);

  //allocate space for color indices
  unsigned char * indices=new unsigned char[width*height];
  if(!indices)
    return Logger::writeErrorLog("Unable to allocate memory for indices");


  //load indices
  fread(indices, 1, width*height, file);

  //close the file
  fclose(file);

  if(dataBuffer)
    deleteArray(dataBuffer);

  //allocate space for the image data
  dataBuffer = new unsigned char[width*height*3];

  if(!dataBuffer)
  {
    fclose(file);
    return Logger::writeErrorLog(String("Unable to allocate memory for ->") + filename);
  }

  //calculate the color values
  for(int currentRow=0; currentRow<height; currentRow++)
  {
    for(int i=0; i<width; i++)
    {
      dataBuffer[(currentRow*width+i)*3+0]=palette[indices[currentRow*width+i]*3+2];
      dataBuffer[(currentRow*width+i)*3+1]=palette[indices[currentRow*width+i]*3+1];
      dataBuffer[(currentRow*width+i)*3+2]=palette[indices[currentRow*width+i]*3+0];//BGR
    }
  }
  return true;
}

bool Image::loadUncompressedTrueColorTGA(const char * filename)
{
  unsigned char TGAheader[12]={0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //Uncompressed TGA header
  unsigned char TGAcompare[12];           //Used to compare TGA header
  unsigned char header[6];              //First 6 useful bytes of the header
  unsigned int  imageSize;              //Stores Image size when in RAM

  FILE * file = fopen(filename, "rb");        //Open the TGA file

  if(file == NULL)                //Does the file exist?
    return Logger::writeErrorLog(String("Could not load file at -> ") + filename);

  if( fread(TGAcompare, 1, sizeof(TGAcompare), file)!=sizeof(TGAcompare)||  //Are there 12 bytes to read?
    memcmp(TGAheader, TGAcompare, sizeof(TGAheader))!=0 ||          //Is the header correct?
    fread(header, 1, sizeof(header), file)!=sizeof(header))   //Read next 6 bytes
  {
    fclose(file);               //If anything else failed, close the file
    return Logger::writeErrorLog(String("Could not process file at -> ") + filename);
  }

  //save data into class member variables
  setWidth(header[1]*256+header[0]);           //determine the image width
  setHeight(header[3]*256+header[2]);            //determine image height
  setComponentsCount(header[4]/3);

  if(width <=0 ||                      //if width <=0
     height<=0 ||                      //or height<=0
     (header[4] !=24 && header[4]!=32))                   //bpp not 24 or 32
  {
    fclose(file);                     //close the file
    return Logger::writeErrorLog(String("The height or width is less than zero, or the TGA is not 24 bpp -> ") + filename);
  }

  //set format
  if(header[4] == 24){
    setFormat(GL_BGR);
    setInternalFormat(GL_RGB8);
  }
  else{
    setFormat(GL_BGRA);
    setInternalFormat(GL_RGBA8);
  }

  imageSize = width*height*getComponentsCount();

  if(dataBuffer)
    deleteArray(dataBuffer);

  dataBuffer = new unsigned char[imageSize];

  if(dataBuffer==NULL)                     //Does the storage memory exist?
  {
    fclose(file);
    return Logger::writeErrorLog(String("Unable to allocate memory for image ->") + filename);
  }

  //read in the image data
  fread(dataBuffer, 1, imageSize, file);
  fclose(file);
  return true;
}

//load a compressed TGA texture (24 or 32 bpp)
bool Image::loadCompressedTrueColorTGA(const char * filename)
{
  unsigned char TGAheader[12]={0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0};  //Compressed TGA header
  unsigned char TGAcompare[12];           //Used to compare TGA header
  unsigned char header[6];              //First 6 useful bytes of the header
  unsigned int  bytesPerPixel;            //bytes per pixel
  unsigned int  imageSize;              //Stores Image size when in RAM

  FILE * file = fopen(filename, "rb");        //Open the TGA file

  if(file == NULL)                //Does the file exist?
    return Logger::writeErrorLog(String("Could not load file at -> ") + filename);


  if( fread(TGAcompare, 1, sizeof(TGAcompare), file)!=sizeof(TGAcompare)||  //Are there 12 bytes to read?
    memcmp(TGAheader, TGAcompare, sizeof(TGAheader))!=0 ||          //Is the header correct?
    fread(header, 1, sizeof(header), file)!=sizeof(header))   //Read next 6 bytes
  {
    fclose(file);               //If anything else failed, close the file
    return Logger::writeErrorLog(String("Could not process file at -> ") + filename);
  }

  //save data into class member variables
  setWidth(header[1]*256+header[0]);            //determine the image width
  setHeight(header[3]*256+header[2]);            //determine image height
  setComponentsCount(header[4]/8);
  bytesPerPixel = getComponentsCount();

  if(width  <=0 ||                      //if width <=0
     height <=0 ||                      //or height<=0
     (header[4] !=24 && header[4] !=32))                   //bpp not 24 or 32
  {
    fclose(file);                     //close the file
    return Logger::writeErrorLog(String("The height or width is less than zero, or the TGA is not 24 bpp -> ") + filename);
  }

  //set format
  if(header[4] == 24)
  {
    setFormat(GL_RGB);
    setInternalFormat(GL_RGB8);
  }
  else
  {
    setFormat(GL_RGBA);
    setInternalFormat(GL_RGBA8);
  }

  imageSize = width*height*getComponentsCount();

  if(dataBuffer)
    deleteArray(dataBuffer);
  dataBuffer = new unsigned char[imageSize];

  if(!dataBuffer)                         //Does the storage memory exist?
  {
    fclose(file);
    return Logger::writeErrorLog(String("Unable to allocate memory for image -> ") + filename);
  }

  //read in the image data
  int pixelCount  = width*height;
  int currentPixel= 0;
  int currentByte = 0;
  unsigned char *colorBuffer = new unsigned char[getComponentsCount()];

  do
  {
    unsigned char chunkHeader=0;

    if(!fread(&chunkHeader, sizeof(unsigned char), 1, file))
    {
      if(file)
        fclose(file);
      deleteArray(dataBuffer);
      return Logger::writeErrorLog("Could not read RLE chunk header");
    }

    if(chunkHeader<128) //Read raw color values
    {
      chunkHeader++;

      for(short counter=0; counter<chunkHeader; counter++)
      {
        if(fread(colorBuffer, 1, bytesPerPixel, file) != bytesPerPixel)
        {
          if(file)
            fclose(file);
          deleteArray(colorBuffer);
          deleteArray(dataBuffer);
          return Logger::writeErrorLog(String("Could not read image data -> ") + filename);
        }

        //transfer pixel color to data (swapping r and b values)
        dataBuffer[currentByte]   = colorBuffer[2];
        dataBuffer[currentByte+1] = colorBuffer[1];
        dataBuffer[currentByte+2] = colorBuffer[0];

        if(bytesPerPixel==4)
          dataBuffer[currentByte+3]=colorBuffer[3];

        currentByte+=bytesPerPixel;
        currentPixel++;

        if(currentPixel > pixelCount)
        {
          if(file)
            fclose(file);
          deleteArray(colorBuffer);
          deleteArray(dataBuffer);
          return Logger::writeErrorLog("Too many pixels read");
        }
      }
    }
    else  //chunkHeader>=128
    {
      chunkHeader-=127;

      if(fread(colorBuffer, 1, bytesPerPixel, file) != bytesPerPixel)
      {
        if(file)
          fclose(file);
        deleteArray(colorBuffer);
        deleteArray(dataBuffer);
        return Logger::writeErrorLog(String("Unable to read image data -> ") + filename);
      }

      for(short counter=0; counter<chunkHeader; counter++)
      {
        //transfer pixel color to data (swapping r and b values)
        dataBuffer[currentByte]   = colorBuffer[2];
        dataBuffer[currentByte+1] = colorBuffer[1];
        dataBuffer[currentByte+2] = colorBuffer[0];

        if(bytesPerPixel==4)
          dataBuffer[currentByte+3]=colorBuffer[3];

        currentByte+=bytesPerPixel;
        currentPixel++;

        if(currentPixel > pixelCount)
        {
          if(file)
            fclose(file);
          deleteArray(colorBuffer);
          deleteArray(dataBuffer);
          return Logger::writeErrorLog("Too many pixels read");
        }
      }
    }
  }while(currentPixel<pixelCount);

  fclose(file);
  deleteArray(colorBuffer);
  return true;
}
