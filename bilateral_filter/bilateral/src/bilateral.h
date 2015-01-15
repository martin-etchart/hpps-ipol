#ifndef BILATERAL_H
#define BILATERAL_H

void bilateral_cielab(float * f, float * h, int width, int height, int nch, double sigma_r, double sigma_d);
void bilateral_cielab_2(float * f, float * h, int width, int height, int nch, double sigma_r, double sigma_d);
void bilateral_cielab_3(float * f, float * h, int width, int height, int nch, double sigma_r, double sigma_d);

void bilateral_rgb(const float * f, float * h, int width, int height, int nch, double sigma_r, double sigma_d);
void bilateral_rgb_2(float * f, float * h, int width, int height, int nch, double sigma_r, double sigma_d);
void bilateral_rgb_3(float * f, float * h, int width, int height, int nch, double sigma_r, double sigma_d);

/**
 * Bilateral filter. Implementation of paper "Bilateral Filtering for Gray and Color Images" by C. Tomasi and R. Manduchi.
 * @param f Input image. Grayscale float image.
 * @param h Output image. Grayscale float image. [Alloc before call]
 * @param width
 * @param height
 * @param sigma_r Standard deviation of range filtering. Associated to similarity function. 
 * @param sigma_d Standard deviation of domain filtering. Associated to closeness function.
 */
void bilateral_grayscale(const float * f, float * h, int width, int height, double sigma_r, double sigma_d);
void bilateral_grayscale_2(const float * f, float * h, int width, int height, double sigma_r, double sigma_d);

#endif