#include "bilateral.h"
#include "iio.h"
#include <stdio.h>
#include <stdlib.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

int main () {
    
    const char * filename_in = "../../data/Lena.pgm";
    const char * filename_out = "../../data/Lena.out.png";
    
    int w = 0;
    int h = 0;
    
    data_t *img_in = iio_read_image_double(filename_in, &w, &h);  
    
    data_t *img_out = bilateral(img_in, w, h, 25.6, 16.0);
    
    IplImage *image_out = cvCreateImage(cvSize(w,h),8,1);
    
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            image_out->imageData[ i + j * w ] = (uchar) round( img_out[ i + j * w] );
        }
    } 
    
    cvSaveImage(filename_out,image_out,0);    
    cvReleaseImage(&image_out);

    
//    iio_save_image_double((char *)filename_out, img_out, w, h);
    
    free(img_in);
    free(img_out);
    
    return 0;
}
