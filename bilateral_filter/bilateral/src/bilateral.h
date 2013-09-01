#ifndef BILATERAL_H
#define BILATERAL_H

/**
 * Bilateral filter. Tomasi et Al implementation.
 * @param f Input image. Grayscale float image.
 * @param h Output image. Grayscale float image. [Alloc before call]
 * @param width
 * @param height
 * @param sigma_r Standard deviation of range filtering. Associated to similarity function. 
 * @param sigma_d Standard deviation of domain filtering. Associated to closeness function.
 */
void bilateral(const float * f, float * h, int width, int height, double sigma_r, double sigma_d);

#endif