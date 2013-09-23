#include "bilateral.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float d(int eps[2], int x[2]) {
    return hypot(eps[0]-x[0], eps[1]-x[1]);
}

/**
 * Closeness function 
 * @param eps
 * @param x
 * @param sigma_d
 * @return 
 */
float c(int eps[2], int x[2], double sigma_d) {
    return exp(-0.5*powf((d(eps,x)/sigma_d),2.0));
}

float delta(float phi, float f) {
    return fabs(phi-f);    
}

/**
 * Similarity function
 * @param phi
 * @param f
 * @param sigma_r
 * @return 
 */
float s(float phi, float f, double sigma_r) {
    return exp(-0.5*powf((delta(phi,f)/sigma_r),2.0));
}

// prolong with last value at the boundary
// returns the index of the 2d row major image
unsigned int p_prolong(int nx, int ny, int x, int y) 
{ 
   x = (x < 0) ? 0 : ( (x>=nx) ? nx-1 : x );
   y = (y < 0) ? 0 : ( (y>=ny) ? ny-1 : y );
   return x+nx*y;
}


void bilateral_no_symmetry(const float * f, float * h, int width, int height, double sigma_r, double sigma_d) {
     
    int win = 2 * sigma_d;
    
    for (int j = win; j < height - (win + 1); j++) {
        for (int i = win; i < width - (win + 1); i++) {
            
            int x[2] = {i,j};
            int _x = i + j * width;
            
            float k = 0;
            float b = 0;
            for (int v = j - win; v < j + (win + 1); v++) {
                for (int u = i - win; u < i + (win + 1); u++) {
                    
                    int eps[2] = {u, v};
                    int _eps = u + v * width;
                    
                    float cs = c(eps,x,sigma_d)*s(f[_eps],f[_x],sigma_r);
                    b+=f[_eps]*cs;                  
                    k+=cs;
                }
            }
            
            h[_x] = (1.0/k) * b;
        }
    }
}

void bilateral(const float * f, float * h, int width, int height, double sigma_r, double sigma_d) {
     
    int win = 2 * sigma_d;
    
    for (int j = 0; j < height ; j++) {
        for (int i = 0; i < width ; i++) {
            
            int x[2] = {i,j};
            int _x = p_prolong(width,height,i,j);
            
            float k = 0;
            float b = 0;
            for (int v = j - win; v < j + (win + 1); v++) {
                for (int u = i - win; u < i + (win + 1); u++) {
                    
                    int eps[2] = {u, v};
                    int _eps = p_prolong(width,height,u,v);
                    
                    float cs = c(eps,x,sigma_d)*s(f[_eps],f[_x],sigma_r);
                    b+=f[_eps]*cs;                  
                    k+=cs;
                }
            }
            
            h[_x] = (1.0/k) * b;
        }
    }
}
