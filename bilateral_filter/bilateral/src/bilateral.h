#ifndef BILATERAL_H
#define BILATERAL_H


typedef double data_t;

//data_t * bilateral(const data_t * image, int width, int height, int sigma_s, double sigma_r);

/**
 * Bilateral filter. Tomasi et Al implementation.
 * @param f
 * @param width
 * @param height
 * @param sigma_r
 * @param sigma_d
 * @return 
 */
data_t * bilateral(const data_t * f, int width, int height, double sigma_r, double sigma_d);

#endif