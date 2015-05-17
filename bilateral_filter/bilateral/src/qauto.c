#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "iio.h"

#define xmalloc malloc

static int compare_floats(const void *a, const void *b)
{
	const float *da = (const float *) a;
	const float *db = (const float *) b;
	return (*da > *db) - (*da < *db);
}

static void get_rminmax(float *rmin, float *rmax, float *x, int n, int rb)
{
	float *tx = xmalloc(n*sizeof*tx);
	int N = 0;
	for (int i = 0; i < n; i++)
		if (!isnan(x[i]))
			tx[N++] = x[i];
	if (rb >= N/2) {
		fprintf(stderr, "too many NANs");
		abort();
	}
	qsort(tx, N, sizeof*tx, compare_floats);
	*rmin = tx[rb];
	*rmax = tx[N-1-rb];
	free(tx);
}

static void qauto(float *x, int w, int h, int pd) {

//int _main(int c, char *v[])
//{
//	if (c != 3 && c != 2 && c != 1) {
//		fprintf(stderr, "usage:\n\t%s [in [out]]\n", *v);
		//                          0  1   2C
//		return EXIT_FAILURE;
//	}
//	char *in = c > 1 ? v[1] : "-";
//	char *out = c > 2 ? v[2] : "-";

//	int w, h, pd;
//	float *x = iio_read_image_float_vec(in, &w, &h, &pd);

	float rmin, rmax;
	get_rminmax(&rmin, &rmax, x, w*h*pd, w*h*pd/200);
//	fprintf(stderr, "qauto: rminmax = %g %g\n", rmin, rmax);

//	uint8_t *y = xmalloc(w*h*pd);
	for (int i = 0; i < w*h*pd; i++) {
		float g = x[i];
		g = floor(255 * (g - rmin)/(rmax - rmin));
		if (g < 0) g = 0;
		if (g > 255) g = 255;
		x[i] = g;
	}
//	iio_save_image_uint8_vec(out, y, w, h, pd);
//	return EXIT_SUCCESS;
    return;
}
