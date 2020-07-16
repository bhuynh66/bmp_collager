Implementation are in A2_bmp_helpers.c

BMP info:  displays data for the bmp file such as 
width, height, bit per pixel , padding, data offset and the middle pixel and its color.

To run it, in the unix command line run the program with the following parameters:

    ./bmp_info < image here >

Example:

      ./bmp_info mario_16_bit.bmp

BMP Mask: Opens a BMP file and modify the data so that the desired rectangular region is set to a fixed color.

To run:

    ./bmp_mask < input file > < output file name > < coordinate 1 > < coordinate 2 > < RBG value >


Example:

    ./bmp_mask mario_16_bit.bmp mario_masked.bmp 150 150 200 200 0 0 0


BMP Collage: Takes two images and a x,y offset and collages the two images at the desired offset.

To run: 

    ./bmp_collage < image1 > < image2 > <output image name> < x offset > < y offset >

Example:

    ./bmp_collage utah.bmp mario_16_bit.bmp mixed_collage.bmp 100 50

or run: 

     bash create_metro_collage.bash

to see a collage of the Montreal Metro Map.


 

