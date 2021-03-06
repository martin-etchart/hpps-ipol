/* 
 * File:   testbilateral_opencv.c
 * Author: roho
 *
 * Created on August 18, 2013, 10:15 PM
 */

#include <stdio.h>
#include <stdlib.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "bilateral.h"

//FIXME: this excecutable gives different result to testbilateral, debugg if needed.
int main(int argc, char** argv) {
    
    const char * filename_in = "../../data/Lena.pgm";
    const char * filename_out = "../../data/Lena.out.pgm";
    
    IplImage *image = cvLoadImage(filename_in,CV_LOAD_IMAGE_GRAYSCALE);
    
    int width = image->width;
    int height = image->height;
    int depth = image->depth;
    
    float *f = malloc(width * height * sizeof (float));

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            f[ i + j * width ] = image->imageData[ i + j * width];
        }
    }
    cvReleaseImage(&image);
    
    float * h = malloc(width * height * sizeof (float));
    bilateral_grayscale(f, h, width, height, 30.0, 3.0);
    free(f);
    
    IplImage *image_out = cvCreateImage(cvSize(width,height),depth,1);
    
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            image_out->imageData[ i + j * width ] = h[ i + j * width];
        }
    }
    free(h);  
    
    cvSaveImage(filename_out,image_out,0);
    
    cvReleaseImage(&image_out);

    return (EXIT_SUCCESS);
}

