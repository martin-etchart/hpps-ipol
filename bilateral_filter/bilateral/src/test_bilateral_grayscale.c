#include "bilateral.h"
#include "iio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char *argv[]) {

    double sigma_r = 3.0;    
    double sigma_d = 30.0;
    const char * filename_in = "../../data/Lena.pgm";

    if (argc > 1)   filename_in = argv[1];
    if (argc > 2)   sigma_r = atof(argv[2]);
    if (argc > 3)   sigma_d = atof(argv[3]);
    
    char filename_out[100];
    sprintf(filename_out,"%s.greyscale.out.png",filename_in);

    int w = 0;
    int h = 0;
    
    
    float *img_in = iio_read_image_float(filename_in, &w, &h);  
    
    float *img_out = malloc(w * h * sizeof (float));
    
    bilateral_grayscale(img_in, img_out, w, h, 30.0, 3.0);
    
    iio_save_image_float(filename_out, img_out, w, h);
    
    free(img_in);
    free(img_out);
    
    return 0;
}
