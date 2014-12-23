function [im1, im2] = align_images(im1, im2)
% Aligns im1 and im2 (translation, scale, rotation) after getting two pairs
% of points from the user.  In the output of im1 and im2, the two pairs of
% points will have approximately the same coordinates.

% get image sizes
[h1, w1, b1] = size(im1);
[h2, w2, b2] = size(im2);

% displays image
figure(1), hold off, imagesc(im1), axis image, colormap gray

% gets two points from the user
disp('Select two points from each image define rotation, scale, translation')
[x1, y1] = ginput(2);
cx1 = mean(x1); cy1 = mean(y1);
figure(1), hold off, imagesc(im2), axis image
[x2, y2] = ginput(2);
cx2 = mean(x2); cy2 = mean(y2);

% translate first so that center of ref points is center of image
tx = round((w1/2-cx1)*2);
if tx > 0,  im1 = padarray(im1, [0 tx], 'pre');
else        im1 = padarray(im1, [0 -tx], 'post');
end
ty = round((h1/2-cy1)*2);
if ty > 0,  im1 = padarray(im1, [ty 0], 'pre');
else        im1 = padarray(im1, [-ty 0], 'post');
end  
tx = round((w2/2-cx2)*2) ;
if tx > 0,  im2 = padarray(im2, [0 tx], 'pre');
else        im2 = padarray(im2, [0 -tx], 'post');
end
ty = round((h2/2-cy2)*2);
if ty > 0,  im2 = padarray(im2, [ty 0], 'pre');
else        im2 = padarray(im2, [-ty 0], 'post');
end

% downscale larger image so that lengths between ref points are the same
len1 = sqrt((y1(2)-y1(1)).^2+(x1(2)-x1(1)).^2);
len2 = sqrt((y2(2)-y2(1)).^2+(x2(2)-x2(1)).^2);
dscale = len2 ./ len1;
if dscale < 1
    im1 = imresize(im1, dscale, 'bilinear'); 
else
    im2 = imresize(im2, 1./dscale, 'bilinear');
end



% rotate im1 so that angle between points is the same
theta1 = atan2(-(y1(2)-y1(1)), x1(2)-x1(1)); % in matlab, y==1 is at the top of the N x M image, and y==N is at the bottom
theta2 = atan2(-(y2(2)-y2(1)), x2(2)-x2(1));
dtheta = theta2-theta1;
im1 = imrotate(im1, dtheta*180/pi, 'bilinear'); % imrotate uses degree units

% Crop images (on both sides of border) to be same height and width
[h1, w1, b1] = size(im1);
[h2, w2, b2] = size(im2);

minw = min(w1, w2);
brd = (max(w1, w2)-minw)/2;
if minw == w1 % crop w2
    im2 = im2(:, (ceil(brd)+1):end-floor(brd), :);
    tx = tx-ceil(brd);
else
    im1 = im1(:, (ceil(brd)+1):end-floor(brd), :);
    tx = tx+ceil(brd);    
end

minh = min(h1, h2);
brd = (max(h1, h2)-minh)/2;
if minh == h1 % crop w2
    im2 = im2((ceil(brd)+1):end-floor(brd), :, :);
    ty = ty-ceil(brd);
else
    im1 = im1((ceil(brd)+1):end-floor(brd), :, :);
    ty = ty+ceil(brd);    
end

figure(1), hold off, imagesc(im1), axis image, colormap gray
figure(2), hold off, imagesc(im2), axis image, colormap gray