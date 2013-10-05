#include "bilateral.h"
#include "iio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    double sigma_r = 30.0/2.56;
    double sigma_d = 3.0;
    const char * filename_in = "../../data/Lena.ppm";

    if (argc > 1)   filename_in = argv[1];
    if (argc > 2)   sigma_r = atof(argv[2]);
    if (argc > 3)   sigma_d = atof(argv[3]);
    
    char filename_out[100];
    sprintf(filename_out,"%s.cielab.out.png",filename_in);

    int w = 0;
    int h = 0;
    int nch = 0;

    float *img_in = iio_read_image_float_vec(filename_in, &w, &h, &nch);

    float *img_out = malloc(w * h * nch * sizeof (float));

    bilateral_cielab_3(img_in, img_out, w, h, nch, sigma_r, sigma_d);

    iio_save_image_float_vec( filename_out, img_out, w, h, nch);

    free(img_in);
    free(img_out);

    return 0;
}
