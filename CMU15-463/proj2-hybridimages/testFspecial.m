h1 = fspecial('gaussian',21,1);
imagesc(log(abs(fftshift(fft2(h1)))))