a=imread('a.bmp');
gb=mean(a,3);
gb=gb(1:322,1:278);

b=imread('b.bmp');
ga=mean(b,3);

h1 = fspecial('gaussian',21,50);
fa=filter2(h1,ga);

h2 = fspecial('gaussian',21,1);
fb=gb-filter2(h2,gb);
d=(fa+fb)/2;
imshow(uint8(d))
imwrite(uint8(d),'des.bmp')

figure, imagesc(log(abs(fftshift(fft2(ga)))))