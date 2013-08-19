#include "bilateral.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//data_t * bilateral(const data_t * img_in, int width, int height, int sigma_d, double sigma_r) {
//
//    data_t * img_out = malloc(width * height * sizeof (data_t));
//
//    //img copy
//    for (int i = 0; i < height; i++) {
//        for (int j = 0; j < width; j++) {
//            img_out[i * width + j] = img_in[i * width + j];
//        }
//    }
//    
//    sigma_r = sigma_d;
//    sigma_d = sigma_r;
//
//    return img_out;
//}

data_t d(unsigned int eps[2], unsigned int x[2]) {
    return sqrt(pow(eps[0]-x[0],2) + pow(eps[1]-x[1],2));
}

/**
 * Closeness function 
 * @param eps
 * @param x
 * @param sigma_d
 * @return 
 */
data_t c(unsigned int eps[2], unsigned int x[2], double sigma_d) {
    return exp(-0.5*pow((d(eps,x)/sigma_d),2));
}

data_t delta(data_t phi, data_t f) {
    return fabs(phi-f);    
}

/**
 * Similarity function
 * @param phi
 * @param f
 * @param sigma_r
 * @return 
 */
data_t s(data_t phi, data_t f, double sigma_r) {
    return exp(-0.5*pow((delta(phi,f)/sigma_r),2));
}



data_t * bilateral(const data_t * f, int width, int height, double sigma_r, double sigma_d) {

    data_t * h = malloc(width * height * sizeof (data_t));
    
//    unsigned int win = (unsigned int) sigma_d/2;

    unsigned int win = 16;
    
    for (unsigned int i = win; i < height-win; i++) {
        for (unsigned int j = win; j < width-win; j++) {
            
            unsigned int x[2] = {i,j};
            unsigned int _x = i*width+j;
            
            data_t k = 0;
            data_t b = 0;
            for (unsigned int u = i-win; u < i+win; u++) {
                for (unsigned int v = j-win; v < j+win; v++) {
                    
                    unsigned int eps[2] = {u,v};
                    unsigned int _eps = u*win+v;
                    
                    data_t cs = c(eps,x,sigma_d)*s(f[_eps],f[_x],sigma_r);
                    b+=f[_eps]*cs;                  
                    k+=cs;
                }
            }
            
            h[_x] = (1.0/k) * b;
        }
    }

    return h;
}