========================================================================
    todos
========================================================================

- (done) display color dir image for each line
desc of logging

1.bmp			source image
abs_grad_x.bmp	sobel result along x	-> color image, >0:r, <0:g
dir_save.bmp	edge direction image	-> color image, >0:r, <0:g
edge_label_save.bmp	thined edge			-> color image, id%3==0:r, id%3==1:g, id%3==2:b
Mi_thin.bmp		thined edge				-> no update
combined.bmp	combined image, r: source image, g: edge>0, b: edge<0

- compute average line dist, min line dist, max line dist
- how to save tiff float image?
- support criteria
- better logging

- vDirChkOk no works

- weakness
1) if edge is not detected in a kernel size (?) then the line is broken, --> cause score much lower
however, we donot use the score directly, so it seems no much influence

2) in the seed selection stage, there MUST be a edge in the delected y coordinate
can be solved by apply different "seeds"

