function [buf2,sts]=loadTiff(f)
	import('myaddin.dll','Pow2','Pow2','void','int32 N, float& a');    
	import('myaddin.dll','tiffSize','tiffSize','int32','string f, int32& w, int32& h, int32& t');
	import('myaddin.dll','readTiff','readTiff_16S','int32','string f, int32 w, int32 h,int32 t, int16[h*w] &buff');
 	import('myaddin.dll','readTiff','readTiff_32F','int32','string f, int32 w, int32 h,int32 t, float[h*w] &buff');
   	import('myaddin.dll','readTiff','readTiff_16U','int32','string f, int32 w, int32 h,int32 t, uint16[h*w] &buff');
	w=int32(0);
	h=int32(0);
    t=int32(0);
 
   TYPE_32F	= int32(1);
   TYPE_16S	= int32(2);
   TYPE_16U	= int32(3);

   %a=float(ones(1000));
   % Pow2(int32(1000),a);
    %a
    
    sts=tiffSize(f,w,h,t);
	if sts~=0
		return
	end
 
    buf=0
    if( TYPE_32F==t )
         buf=float(zeros(w,h));
         sts=readTiff_32F(f,w,h,t,buf);
    elseif ( TYPE_16S==t )
        buf=int16(zeros(w,h));
        sts=readTiff_16S(f,w,h,t,buf);
    elseif ( TYPE_16U==t )
        buf=uint16(zeros(w,h));
        sts=readTiff_16U(f,w,h,t,buf);
    else
        return
	end
 
	
	if sts~=0
		return
	end
    buf2 = buf';
end