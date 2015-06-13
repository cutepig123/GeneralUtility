function [buf,sts]=loadTiff(f)
	import('myaddin.dll','tiffSize','tiffSize','int32','string f, int32& w, int32& h');
	import('myaddin.dll','readTiff','readTiff','int32','string f, int32 w, int32 h, float[w*h] &buff');
	w=0;
	h=0;
	sts=tiffSize(f,w,h);
	if sts~=0
		return
	end
	buf=float(zeros(h,w));
	sts=readTiff(f,w,h,buf);
	if sts~=0
		return
	end
end