#include "bilateral.h"

#include "iio.h"
#include "random.c"
//#include "sum.c"
#include "qauto.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int add_noise(float *img, float *noissy, int w, int h, int nch, double sigma_n){
    
    for (int j = 0; j < w*h*nch; j++) {
        
        float u = (float)sigma_n*random_normal();
        noissy[j] = img[j] + u;
        
        if (noissy[j]<0) {
            noissy[j] = 0;
        } else if (noissy[j]>255) {
            noissy[j] = 255;
        }     
    }
    
    return 0;
}

int diff(float *img, float *denoised, float *diff, int w, int h, int nch){
    
    for (int j = 0; j < w*h*nch; j++) {
        diff[j] = img[j]-denoised[j];
    }
    qauto(diff,w,h,nch); //rescale to 0..255 range
    
    return 0;
}


int main(int argc, char *argv[]) {

    double sigma_r = 30.0/2.56;
    double sigma_d = 3.0;
    double sigma_n = 3.0;
    const char * filename_input = "../../data/Lena.png";
    const char * filename_noissy = "../../data/Lena.noissy.png";    
    const char * filename_denoised = "../../data/Lena.denoised.png";
    const char * filename_difference = "../../data/Lena.difference.png";

    if (argc > 1)   filename_input = argv[1];
    if (argc > 2)   filename_noissy = argv[2];
    if (argc > 3)   filename_denoised = argv[3];
    if (argc > 4)   filename_difference = argv[4];
    if (argc > 5)   sigma_r = atof(argv[5]);
    if (argc > 6)   sigma_d = atof(argv[6]);
    if (argc > 7)   sigma_n = atof(argv[7]);

    int w = 0;
    int h = 0;
    int nch = 0;

    float *img = iio_read_image_float_vec(filename_input, &w, &h, &nch);

    float *noissy = malloc(w * h * nch * sizeof (float));
    float *denoised = malloc(w * h * nch * sizeof (float));
    float *difference = malloc(w * h * nch * sizeof (float));    
    
    //noissy
    add_noise(img, noissy, w, h, nch, sigma_n);

    //denoised
    bilateral_cielab_3(noissy, denoised, w, h, nch, sigma_r, sigma_d);
    
    //diff
    diff(img,denoised,difference,w,h,nch);

    iio_save_image_float_vec(filename_noissy, noissy, w, h, nch);
    iio_save_image_float_vec(filename_denoised, denoised, w, h, nch);
    iio_save_image_float_vec(filename_difference, difference, w, h, nch);

    free(img);
    free(noissy);
    free(denoised);
    free(difference);

    return 0;
}
