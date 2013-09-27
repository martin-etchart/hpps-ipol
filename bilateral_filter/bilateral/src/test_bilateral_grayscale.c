#include "bilateral.h"
#include "iio.h"
#include <stdio.h>
#include <stdlib.h>

void iio_save_image_double_(char* filename_out, double*in,int w,int h){
    int N=w*h;
    float *tmp=malloc(N*sizeof(*tmp));
    for (int i=0;i<N;i++) tmp[i] = in[i];
    iio_save_image_float((char *)filename_out, tmp, w, h);
    free(tmp);
}

double *iio_read_image_double_(const char *fname, int *w, int *h) {
    float *tmp=iio_read_image_float(fname, w, h);
    int N = *w * *h;
    double *out=malloc(N*sizeof(*out));
    for (int i=0;i<N;i++) out[i] = tmp[i];
    free(tmp);
    return out;
}

int main () {
    
    const char * filename_in = "../../data/Lena.pgm";
    const char * filename_out = "../../data/Lena.grayscale.out.png";
    
    int w = 0;
    int h = 0;
    
    
    float *img_in = iio_read_image_float(filename_in, &w, &h);  
    
    float *img_out = malloc(w * h * sizeof (float));
    
    bilateral_grayscale(img_in, img_out, w, h, 30.0, 3.0);
    
    iio_save_image_float((char *)filename_out, img_out, w, h);
    
    free(img_in);
    free(img_out);
    
    return 0;
}
