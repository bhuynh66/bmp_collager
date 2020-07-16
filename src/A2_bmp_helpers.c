// FILE: A2_bmp_helpers.c is where you will code your answers for Assignment 2.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

int bmp_open( char* bmp_filename,        unsigned int *width, 
              unsigned int *height,      unsigned int *bits_per_pixel, 
              unsigned int *padding,     unsigned int *data_size, 
              unsigned int *data_offset, unsigned char** img_data ){

              
  
    FILE *file;   
 
    unsigned char *po = ( unsigned char *)malloc(60*sizeof(unsigned char));//only need to check parameters of the 1st 54 elements  

    file = fopen(bmp_filename, "rb");

    if(file == NULL){  //first check to see if file opens correctly
        return -1;
    }

    fread(po ,sizeof(unsigned char) , 54 , file);
    fclose(file);

    if(po[0] != 66 || po[1] != 77){   //This just mean that the file is not a bmp 
	      return -1;
    }
  
    *width=po[18]*1+po[19]*256+po[20]*65536+po[21]*16777216;    

    unsigned int pad_pre = *width*3;

    if(pad_pre%4!=0){
	      pad_pre = pad_pre +4 - pad_pre%4;	
    }


    *height=po[22]*1+po[23]*256+po[24]*65536+po[25]*16777216;              
    *bits_per_pixel=po[28]+po[29]*256+po[30]*65536+po[31]*16777216;				
    *padding= pad_pre - *width*3;
    *data_size=po[2]+po[3]*256+po[4]*65536+po[5]*16777216;
    *data_offset=po[10];
 
    free(po);
    unsigned char *so = ( unsigned char *)malloc(*data_size *sizeof(unsigned char)); //The address to store our image info  
 
    if(so==NULL){
	      printf("allocation failed");
	      return -1;  
    }

    file = fopen(bmp_filename, "rb");

    if(file == NULL){  //first check to see if file opens correctly
	      return -1;
    }

    fread(so ,sizeof(unsigned char) , *data_size , file);
    fclose(file);
    *img_data= so;

  
  
  return 0;  
}


void bmp_close( unsigned char **img_data ){

  if( *img_data != NULL ){
    free( *img_data );
    *img_data = NULL;
  }
}

int bmp_mask( char* input_bmp_filename, char* output_bmp_filename, 
              unsigned int x_min, unsigned int y_min, unsigned int x_max, unsigned int y_max,
              unsigned char red, unsigned char green, unsigned char blue )
{
  unsigned int img_width;
  unsigned int img_height;
  unsigned int bits_per_pixel;
  unsigned int data_size;
  unsigned int padding;
  unsigned int data_offset;
  unsigned char* img_data    = NULL;
  
  int open_return_code = bmp_open( input_bmp_filename, &img_width, &img_height, &bits_per_pixel, &padding, &data_size, &data_offset, &img_data ); 
  
  if( open_return_code ){
     printf( "bmp_open failed. Returning from bmp_mask without attempting changes.\n" );
     return -1;
  }
 
 
  unsigned char *pixel_cpy = (unsigned char *)malloc(1000*data_size * sizeof(unsigned char));
  memcpy(pixel_cpy , img_data , data_size );
   
  unsigned int num_colors = bits_per_pixel/8;

  for(int i = y_min; i<y_max+1 ; i++){
	    for(int j = x_min; j< x_max ; j++){
          pixel_cpy[ (i*(img_width *num_colors+ padding) + j*num_colors + 0)+ data_offset ] = blue;
          pixel_cpy[ (i*(img_width *num_colors+ padding) + j*num_colors + 1)+ data_offset ] = green;
          pixel_cpy[ (i*(img_width *num_colors+ padding) + j*num_colors + 2)+ data_offset ] = red;
	    }
 }
     
  FILE *ptr = fopen(output_bmp_filename , "wb");
  fwrite(pixel_cpy, 1 , data_size, ptr);
  fclose(ptr);
  bmp_close( &img_data );
  
  return 0;
}         

int bmp_collage( char* bmp_input1, char* bmp_input2, char* bmp_result, int x_offset, int y_offset ){

  unsigned int img_width1;
  unsigned int img_height1;
  unsigned int bits_per_pixel1;
  unsigned int data_size1;
  unsigned int padding1;
  unsigned int data_offset1;
  unsigned char* img_data1    = NULL;
  
  int open_return_code = bmp_open( bmp_input1, &img_width1, &img_height1, &bits_per_pixel1, &padding1, &data_size1, &data_offset1, &img_data1 ); 
  
  if( open_return_code ){
     printf( "bmp_open failed for %s. Returning from bmp_collage without attempting changes.\n", bmp_input1 );
     return -1;
  }
  
  unsigned int img_width2;
  unsigned int img_height2;
  unsigned int bits_per_pixel2;
  unsigned int data_size2;
  unsigned int padding2;
  unsigned int data_offset2;
  unsigned char* img_data2    = NULL;
  
  open_return_code = bmp_open( bmp_input2, &img_width2, &img_height2, &bits_per_pixel2, &padding2, &data_size2, &data_offset2, &img_data2 ); 
  
  if( open_return_code ){
     printf( "bmp_open failed for %s. Returning from bmp_collage without attempting changes.\n", bmp_input2 );
     return -1;
  }
  
  
  
	unsigned int collage_width;		//new boundary is the largest width of the second picture + the offset 

	if((img_width2+(x_offset))> img_width1){
		 collage_width = img_width2+(x_offset);
	}
	else if (x_offset <0 && (img_width2+x_offset)> img_width1){
	   collage_width = img_width1-x_offset;
	}
	else{
		 collage_width = img_width1;
	}
	

  unsigned int collage_height; 
  unsigned int pre_pad = collage_width*3;

  if(pre_pad%4!=0){
	  pre_pad = pre_pad +4 - pre_pad%4;
  } 

  unsigned int collage_padding = pre_pad- collage_width*3;                        
  
  
	if(img_height2+(y_offset) > img_height1){
		collage_height = img_height2 + (y_offset);
	}
	else if(y_offset <0 && img_height2 < img_height1){
		collage_height = img_height1 - y_offset;
	}
	else{
		collage_height = img_height1;
	}
	  

  unsigned int collage_data_size = (3*collage_height*(collage_width + collage_padding/3)) + data_offset1;
  
  unsigned int img1_start_col;
  unsigned int img1_start_row;
  unsigned int img2_start_row;
  unsigned int img2_start_col;
  unsigned int img1_end_col;
  unsigned int img1_end_row;
  unsigned int img2_end_row;
  unsigned int img2_end_col;
  
 
  /*
 If the offset is negative then we want to shift the coordinate of the first image by the respective coordinate
 otherwise we shift the coordinate of the second image.
 */
 
 
 
 

if(x_offset>=0 && y_offset>=0){
   img1_start_row = 0;
   img1_start_col = 0;
   img2_start_row = x_offset;
   img2_start_col=  y_offset;	 
 
   img1_end_row = img_width1;
   img1_end_col = img_height1;
   img2_end_row = img_width2+x_offset; 
   img2_end_col = img_height2+y_offset;
}
else if(x_offset<0 && y_offset<0){
   img1_start_row = abs(x_offset);
   img1_start_col = abs(y_offset);
   img2_start_row = 0;
   img2_start_col=  0;	 
 
   img1_end_row = img_width1+abs(x_offset);
   img1_end_col = img_height1+abs(y_offset);
   img2_end_row = img_width2; 
   img2_end_col = img_height2;
}
else if(x_offset>=0 && y_offset<0){
   img1_start_row = 0;
   img1_start_col = abs(y_offset);
   img2_start_row = x_offset;
   img2_start_col=  0;	 
 
   img1_end_row = img_width1;
   img1_end_col = img_height1+abs(y_offset);
   img2_end_row = img_width2+x_offset; 
   img2_end_col = img_height2;
}
else{  //x< 0 ; y>=0
   img1_start_row = abs(x_offset);
   img1_start_col = 0;
   img2_start_row = 0;
   img2_start_col = y_offset;	 
 
   img1_end_row = img_width1+abs(x_offset);
   img1_end_col = img_height1;
   img2_end_row = img_width2; 
   img2_end_col = img_height2+y_offset;
}


   //copy the header info into our new bmp image
   
unsigned char *img_collage = (unsigned char *)malloc(100*collage_data_size * sizeof(unsigned char)); 
    
memcpy(img_collage , img_data1,data_offset1);
memcpy(img_collage+2,&collage_data_size,4);
memcpy(img_collage+18,&collage_width,4);
memcpy(img_collage+22,&collage_height,4);
memcpy(img_collage+28,&bits_per_pixel1,4);

unsigned int num = bits_per_pixel1/8;

//now we loop over the two images and collaging it
 
  //this loop if for both positive offset
if(x_offset>=0 && y_offset>=0){
   for(int h =0 ;h< collage_height  ; h++){ 
      for(int v =0 ; v< collage_width ; v++){ 

         if(v>=img2_start_row && v < img2_end_row && h>=img2_start_col && h< img2_end_col){
            img_collage[(h)*(collage_width*num+collage_padding)+(v)*num +0 + data_offset1] = img_data2[(h-y_offset)*(img_width2*( bits_per_pixel2/8)+padding2)+(v-x_offset)*( bits_per_pixel2/8) +0+data_offset2];
   
            img_collage[(h)*(collage_width*num+collage_padding)+(v)*num +1 + data_offset1] = img_data2[(h-y_offset)*(img_width2*( bits_per_pixel2/8)+padding2)+(v-x_offset)*( bits_per_pixel2/8) +1+data_offset2];
    
    
	         img_collage[(h)*(collage_width*num+collage_padding)+(v)*num +2 + data_offset1] = img_data2[(h-y_offset)*(img_width2*( bits_per_pixel2/8)+padding2)+(v-x_offset)*( bits_per_pixel2/8) +2+data_offset2];
          }
	        else if(v>= img1_start_row && v< img1_end_row && h>= img1_start_col && h< img1_end_col){
	          img_collage[h*(collage_width*num+collage_padding)+v*num +0 + data_offset1] = img_data1[(h)*(img_width1*num+padding1)+(v)*num +0+data_offset1];
      
             img_collage[h*(collage_width*num+collage_padding)+v*num +1 + data_offset1] = img_data1[(h)*(img_width1*num+padding1)+(v)*num +1+data_offset1];
      
	          img_collage[h*(collage_width*num+collage_padding)+v*num +2 + data_offset1] = img_data1[(h)*(img_width1*num+padding1)+v*num +2+data_offset1];
	        }	
       }
    }
 }
else if(x_offset>=0 && y_offset<0){ //loop  for x>0 ; y<0
   for(int h =0 ;h< collage_height  ; h++){ 
      for(int v =0 ; v< collage_width ; v++){ 
         if(v>=img2_start_row && v < img2_end_row && h>=img2_start_col && h< img2_end_col){
            img_collage[(h)*(collage_width*num+collage_padding)+(v)*num +0 + data_offset1] = img_data2[(h)*(img_width2*( bits_per_pixel2/8)+padding2)+(v-x_offset)*( bits_per_pixel2/8) +0+data_offset2];
   
            img_collage[(h)*(collage_width*num+collage_padding)+(v)*num +1 + data_offset1] = img_data2[(h)*(img_width2*( bits_per_pixel2/8)+padding2)+(v-x_offset)*( bits_per_pixel2/8) +1+data_offset2];
  
	         img_collage[(h)*(collage_width*num+collage_padding)+(v)*num +2 + data_offset1] = img_data2[(h)*(img_width2*( bits_per_pixel2/8)+padding2)+(v-x_offset)*( bits_per_pixel2/8) +2+data_offset2];
          }

	        else if(v>= img1_start_row && v< img1_end_row && h>= img1_start_col && h< img1_end_col){
	         img_collage[h*(collage_width*num+collage_padding)+v*num +0 + data_offset1]  = img_data1[(h+y_offset)*(img_width1*num+padding1)+(v)*num +0+data_offset1];
      
            img_collage[h*(collage_width*num+collage_padding)+v*num +1 + data_offset1] = img_data1[(h+y_offset)*(img_width1*num+padding1)+(v)*num +1+data_offset1];
      
	         img_collage[h*(collage_width*num+collage_padding)+v*num +2 + data_offset1] = img_data1[(h+y_offset)*(img_width1*num+padding1)+v*num +2+data_offset1];
	        }
	
       }

    }
}
else if(x_offset<0 && y_offset<0){ //this loop if for both negatve offset
   for(int h =0 ;h< collage_height  ; h++){ 
      for(int v =0 ; v< collage_width ; v++){ 
         if(v>=img2_start_row && v < img2_end_row && h>=img2_start_col && h< img2_end_col){
            img_collage[(h)*(collage_width*num+collage_padding)+(v)*num +0 + data_offset1] = img_data2[(h)*(img_width2*( bits_per_pixel2/8)+padding2)+(v)*( bits_per_pixel2/8) +0+data_offset2];
   
            img_collage[(h)*(collage_width*num+collage_padding)+(v)*num +1 + data_offset1] = img_data2[(h)*(img_width2*( bits_per_pixel2/8)+padding2)+(v)*( bits_per_pixel2/8) +1+data_offset2];
    
	         img_collage[(h)*(collage_width*num+collage_padding)+(v)*num +2 + data_offset1] = img_data2[(h)*(img_width2*( bits_per_pixel2/8)+padding2)+(v)*( bits_per_pixel2/8) +2+data_offset2];
         }
	       else if(v>= img1_start_row && v< img1_end_row && h>= img1_start_col && h< img1_end_col){
	         img_collage[h*(collage_width*num+collage_padding)+v*num +0 + data_offset1] = img_data1[(h+y_offset)*(img_width1*num+padding1)+(v+x_offset)*num +0+data_offset1];
      
            img_collage[h*(collage_width*num+collage_padding)+v*num +1 + data_offset1] = img_data1[(h+y_offset)*(img_width1*num+padding1)+(v+x_offset)*num +1+data_offset1];
      
	         img_collage[h*(collage_width*num+collage_padding)+v*num +2 + data_offset1] = img_data1[(h+y_offset)*(img_width1*num+padding1)+(v+x_offset)*num +2+data_offset1];
	       } 	
       } 
     }
}

else{//this loop if for both -x +y
   for(int h =0 ;h< collage_height  ; h++){ 
      for(int v =0 ; v< collage_width ; v++){ 
         if(v>=img2_start_row && v < img2_end_row && h>=img2_start_col && h< img2_end_col){
            img_collage[(h)*(collage_width*num+collage_padding)+(v)*num +0 + data_offset1] = img_data2[(h-y_offset)*(img_width2*( bits_per_pixel2/8)+padding2)+(v)*( bits_per_pixel2/8) +0+data_offset2];
   
            img_collage[(h)*(collage_width*num+collage_padding)+(v)*num +1 + data_offset1] = img_data2[(h-y_offset)*(img_width2*( bits_per_pixel2/8)+padding2)+(v)*( bits_per_pixel2/8) +1+data_offset2];
    
	         img_collage[(h)*(collage_width*num+collage_padding)+(v)*num +2 + data_offset1] = img_data2[(h-y_offset)*(img_width2*( bits_per_pixel2/8)+padding2)+(v)*( bits_per_pixel2/8) +2+data_offset2];
         }
	       else if(v>= img1_start_row && v< img1_end_row && h>= img1_start_col && h< img1_end_col){
	         img_collage[h*(collage_width*num+collage_padding)+v*num +0 + data_offset1] = img_data1[(h)*(img_width1*num+padding1)+(v+x_offset)*num +0+data_offset1];
      
            img_collage[h*(collage_width*num+collage_padding)+v*num +1 + data_offset1] = img_data1[(h)*(img_width1*num+padding1)+(v+x_offset)*num +1+data_offset1];
      
	         img_collage[h*(collage_width*num+collage_padding)+v*num +2 + data_offset1] = img_data1[(h)*(img_width1*num+padding1)+(v+x_offset)*num +2+data_offset1];
	       }
	
       }

     }
}

FILE *ptr1 = fopen(bmp_result , "wb");
fwrite(img_collage, 1 , collage_data_size, ptr1);
fclose(ptr1);
free(img_collage);
        
bmp_close( &img_data1 );
bmp_close( &img_data2 );
  
return 0;
}                  
