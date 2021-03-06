#include "bilateral.h"

#include "color_spaces.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static inline float d(int eps[2], int x[2]) {
    return hypotf(eps[0]-x[0], eps[1]-x[1]);
}

/**
 * Closeness function 
 * @param eps
 * @param x
 * @param sigma_d
 * @return 
 */
static inline float c(int eps[2], int x[2], double sigma_d) {
    return expf(-0.5*powf((d(eps,x)/sigma_d),2.0));
}

static inline float delta(float phi, float f) {
    return fabs(phi-f);    
}

/**
 * Similarity function
 * @param phi
 * @param f
 * @param sigma_r
 * @return 
 */
static inline float s(float phi, float f, double sigma_r) {
    return expf(-0.5*powf((delta(phi,f)/sigma_r),2.0));
}

/**
 * Similarity function in cielab color space
 * @param phi
 * @param f
 * @param sigma_r
 * @param nch
 * @return 
 */
static inline float s_vec_cielab(float *phi, float *f, double sigma_r, int nch) {
    float dist = 0;
    
    float phi_cielab[3];
    float f_cielab[3];
    rgb_to_cielab_floats(f_cielab, f);
    rgb_to_cielab_floats(phi_cielab, phi);    
    
    for (int l = 0; l < nch; l++) {
        dist+= powf(f_cielab[l]-phi_cielab[l],2.0);
    }
    dist = sqrtf(dist);
    
    return expf(-0.5*powf((dist/sigma_r),2.0));
}

static inline float s_vec_nopowf(float *phi, float *f, double sigma_r, int nch) {
    float dist = 0;  
    
    for (int l = 0; l < nch; l++) {
        dist+=(f[l]-phi[l])*(f[l]-phi[l]);
    }
    
    return expf(-0.5*(dist/sigma_r*sigma_r));
}

static inline float s_vec(float *phi, float *f, double sigma_r, int nch) {
    float dist = 0;  
    
    for (int l = 0; l < nch; l++) {
        dist+= powf(f[l]-phi[l],2.0);
    }
    dist = sqrtf(dist);
    
    return expf(-0.5*powf((dist/sigma_r),2.0));
}

//
/***
 * prolong with last value at the boundary
 * @param nx
 * @param ny
 * @param x
 * @param y
 * @return the index of the 2d row major image
 */
static inline unsigned int p_prolong(int nx, int ny, int x, int y) 
{ 
   x = (x < 0) ? 0 : ( (x>=nx) ? nx-1 : x );
   y = (y < 0) ? 0 : ( (y>=ny) ? ny-1 : y );
   return x+nx*y;
}


/***
 * prolong with the symmetrical value from inside the boundary
 * @param nx
 * @param ny
 * @param x
 * @param y
 * @return the index of the 2d row major image
 */
static inline unsigned int p_symmetry(int nx, int ny, int x, int y) 
{ 
   x = (x < 0) ? -x : ( (x>=nx) ? 2*(nx-1)-x : x ); //nx-1-(x-(nx-1))
   y = (y < 0) ? -y : ( (y>=ny) ? 2*(ny-1)-y : y ); //ny-1-(y-(ny-1))
   return x+nx*y;
}

/***
 * Pre-compute de closeness matrix
 * @param mat input-output param
 * @param win window size is 2*win+1
 * @param sigma_d gaussian sigma distance
 */
void closeness_matrix(float **mat, int win, double sigma_d) {
    for (int v = - win; v < (win + 1); v++) {
        for (int u = - win; u < (win + 1); u++) {
            
            int xo[2] = {0, 0};             
            int eps[2] = {u, v};    
            
            mat[u+win][v+win] = exp(-0.5*powf((d(eps,xo)/sigma_d),2.0));
    
        }
    }
}

/***
 * Converts rgb image f into cielab color space
 * @param f input rgb image
 * @param f_cielab output lab image
 * @param width image width
 * @param height image height
 * @param nch number of channels
 */
void convert_rgb_to_cielab(float *f, float *f_cielab, int width, int height, int nch) {
    
    for (int j = 0; j < height ; j++) {
        for (int i = 0; i < width ; i++) {
            
            int idx = (i+width*j)*nch;
            rgb_to_cielab_floats(f_cielab+idx, f+idx);
        }
    }
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

    void bilateral_grayscale(const float * f, float * h, int width, int height, double sigma_r, double sigma_d) {
     
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

void bilateral_grayscale_2(const float * f, float * h, int width, int height, double sigma_r, double sigma_d) {
     
    int win = 2 * sigma_d;
    
    float **c_mat = malloc((2*win+1) * sizeof(float*));
    for (int i=0 ; i<(2*win+1) ; i++) c_mat[i] = malloc((2*win+1) * sizeof(float));
    
    closeness_matrix(c_mat,win,sigma_d);
    
    for (int j = 0; j < height ; j++) {
        for (int i = 0; i < width ; i++) {
            
            int _x = p_prolong(width,height,i,j);
            
            float k = 0;
            float b = 0;
            for (int v = j - win; v < j + (win + 1); v++) {
                for (int u = i - win; u < i + (win + 1); u++) {
                    
                    int _eps = p_prolong(width,height,u,v);
                    
                    float cs = c_mat[u-(i-win)][v-(j-win)]*s(f[_eps],f[_x],sigma_r);
                    b+=f[_eps]*cs;                  
                    k+=cs;
                }
            }
            
            h[_x] = (1.0/k) * b;
        }
    }
    
    for (int i=0 ; i<(2*win+1) ; i++) free(c_mat[i]);
    free(c_mat);
}

void bilateral_rgb(const float * f, float * h, int width, int height, int nch, double sigma_r, double sigma_d) {
     
    int win = 2 * sigma_d;
    
    for (int j = 0; j < height ; j++) {
        for (int i = 0; i < width ; i++) {
            for (int l = 0; l < nch ; l++) {            
            
                int x[2] = {i,j};
                int _x = p_prolong(width,height,i,j)*nch + l;

                float k = 0;
                float b = 0;
                for (int v = j - win; v < j + (win + 1); v++) {
                    for (int u = i - win; u < i + (win + 1); u++) {

                        int eps[2] = {u, v};
                        int _eps = p_prolong(width,height,u,v)*nch + l;

                        float cs = c(eps,x,sigma_d)*s(f[_eps],f[_x],sigma_r);
                        b+=f[_eps]*cs;                  
                        k+=cs;
                    }
                }

                h[_x] = (1.0/k) * b;
            }
        }
    }
}

void bilateral_rgb_2(float * f, float * h, int width, int height, int nch, double sigma_r, double sigma_d) {
     
    int win = 2 * sigma_d;
    
    float **c_mat = malloc((2*win+1) * sizeof(float*));
    for (int i=0 ; i<(2*win+1) ; i++) c_mat[i] = malloc((2*win+1) * sizeof(float));
    
    closeness_matrix(c_mat,win,sigma_d);
    
    for (int j = 0; j < height ; j++) {
        for (int i = 0; i < width ; i++) {
            for (int l = 0; l < nch ; l++) {

                int _x = p_prolong(width, height, i, j) * nch + l;

                float k = 0;
                float b = 0;
                for (int v = j - win; v < j + (win + 1); v++) {
                    for (int u = i - win; u < i + (win + 1); u++) {

                        int _eps = p_prolong(width, height, u, v) * nch + l;

                        float cs = c_mat[u - (i - win)][v - (j - win)] * s(f[_eps], f[_x], sigma_r);
                        b += f[_eps] * cs;
                        k += cs;
                    }
                }

                h[_x] = (1.0 / k) * b;
            }
        }
    }
    
    for (int i=0 ; i<(2*win+1) ; i++) free(c_mat[i]);
    free(c_mat);
}

void bilateral_rgb_3(float * f, float * h, int width, int height, int nch, double sigma_r, double sigma_d) {

    int win = 2 * sigma_d;
    
    float **c_mat = malloc((2*win+1) * sizeof(float*));
    for (int i=0 ; i<(2*win+1) ; i++) c_mat[i] = malloc((2*win+1) * sizeof(float));
    
    closeness_matrix(c_mat,win,sigma_d);

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {

            int x[2] = {i, j};
            int _x = p_prolong(width, height, i, j) * nch;

            float k = 0;
            float b[3] = {0, 0, 0};
            for (int v = j - win; v < j + (win + 1); v++) {
                for (int u = i - win; u < i + (win + 1); u++) {

                    int eps[2] = {u, v};
                    int _eps = p_prolong(width, height, u, v) * nch;

                    float cs = c_mat[u - (i - win)][v - (j - win)] * s_vec(&f[_eps], &f[_x], sigma_r, nch);
                    for (int l = 0; l < nch; l++)   
                        b[l] += f[_eps + l] * cs;
                    k += cs;
                }
            }

            for (int l = 0; l < nch; l++) 
                h[_x + l] = (1.0 / k) * b[l];
        }
    }
    
    for (int i=0 ; i<(2*win+1) ; i++) free(c_mat[i]);
    free(c_mat);
}

void bilateral_cielab(float * f, float * h, int width, int height, int nch, double sigma_r, double sigma_d) {

    int win = 2 * sigma_d;

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {

            int x[2] = {i, j};
            int _x = p_prolong(width, height, i, j) * nch;

            float k = 0;
            float b[3] = {0, 0, 0};
            for (int v = j - win; v < j + (win + 1); v++) {
                for (int u = i - win; u < i + (win + 1); u++) {

                    int eps[2] = {u, v};
                    int _eps = p_prolong(width, height, u, v) * nch;

                    float cs = c(eps, x, sigma_d) * s_vec_cielab(&f[_eps], &f[_x], sigma_r, nch);
                    for (int l = 0; l < nch; l++)   
                        b[l] += f[_eps + l] * cs;
                    k += cs;
                }
            }

            for (int l = 0; l < nch; l++) 
                h[_x + l] = (1.0 / k) * b[l];
        }
    }
}

void bilateral_cielab_2(float * f, float * h, int width, int height, int nch, double sigma_r, double sigma_d) {

    int win = 2 * sigma_d;
    
    float **c_mat = malloc((2*win+1) * sizeof(float*));
    for (int i=0 ; i<(2*win+1) ; i++) c_mat[i] = malloc((2*win+1) * sizeof(float));
    
    closeness_matrix(c_mat,win,sigma_d); 

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {

            int x[2] = {i, j};
            int _x = p_prolong(width, height, i, j) * nch;

            float k = 0;
            float b[3] = {0, 0, 0};
            for (int v = j - win; v < j + (win + 1); v++) {
                for (int u = i - win; u < i + (win + 1); u++) {

                    int eps[2] = {u, v};
                    int _eps = p_prolong(width, height, u, v) * nch;

                    float cs = c_mat[u - (i - win)][v - (j - win)] * s_vec_cielab(&f[_eps], &f[_x], sigma_r, nch);
                    for (int l = 0; l < nch; l++)   
                        b[l] += f[_eps + l] * cs;
                    k += cs;
                }
            }

            for (int l = 0; l < nch; l++) 
                h[_x + l] = (1.0 / k) * b[l];
        }
    }
    
    for (int i=0 ; i<(2*win+1) ; i++) free(c_mat[i]);
    free(c_mat);
}

void bilateral_cielab_3(float * f, float * h, int width, int height, int nch, double sigma_r, double sigma_d) {

    int win = 2 * sigma_d;
    
    float **c_mat = malloc((2*win+1) * sizeof(float*));
    for (int i=0 ; i<(2*win+1) ; i++) c_mat[i] = malloc((2*win+1) * sizeof(float));
    
    closeness_matrix(c_mat,win,sigma_d);

    float *f_cielab = malloc(width * height * nch * sizeof(float));    
    convert_rgb_to_cielab(f,f_cielab,width,height,nch);

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {

            int x[2] = {i, j};
            int _x = p_prolong(width, height, i, j) * nch;

            float k = 0;
            float b[3] = {0, 0, 0};
            for (int v = j - win; v < j + (win + 1); v++) {
                for (int u = i - win; u < i + (win + 1); u++) {

                    int eps[2] = {u, v};
                    int _eps = p_prolong(width, height, u, v) * nch;

                    float cs = c_mat[u - (i - win)][v - (j - win)] * s_vec(&f_cielab[_eps], &f_cielab[_x], sigma_r, nch);
                    for (int l = 0; l < nch; l++)   
                        b[l] += f[_eps + l] * cs;
                    k += cs;
                }
            }

            for (int l = 0; l < nch; l++) 
                h[_x + l] = (1.0 / k) * b[l];
        }
    }
    
    for (int i=0 ; i<(2*win+1) ; i++) free(c_mat[i]);
    free(c_mat);
    
    free(f_cielab);
}