#include "bilateral.h"
#include "iio.h"
#include <stdio.h>
#include <stdlib.h>

int main () {
    
    const char * filename_in = "../../data/Lena.ppm";
    const char * filename_out = "../../data/Lena.color.out.png";
    
    int w = 0;
    int h = 0;
    int nch = 0;
    
    
    float *img_in = iio_read_image_float_vec(filename_in, &w, &h, &nch);  
    
    float *img_out = malloc(w * h * nch * sizeof (float));
    
    bilateral_color(img_in, img_out, w, h, nch, 30.0, 3.0);
    
    iio_save_image_float_vec((char *)filename_out, img_out, w, h, nch);
    
    free(img_in);
    free(img_out);
    
    return 0;
}
