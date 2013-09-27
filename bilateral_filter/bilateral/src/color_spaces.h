//author: juan.cardelino@gmail.com
//hosted: http://dev.ipol.im/git/?p=juanc/tools.git

#ifndef _COLOR_SPACES_H
#define _COLOR_SPACES_H


typedef enum
{
	FALSE_COLOR		=-1,
	RGB				=0,
	HSV				=1,
	YCbCr				=2,
	CIELab			=3,
	XYZ				=4,
	BINARY		   =5,
} colorspace_t;



void rgb_to_grey_floats(float *out, float *in);
void cielab_to_xyz_floats ( float *out, float *in );
void rgb_to_hsv_doubles(double *, double *);
void hsv_to_rgb_floats(float *out, float *in);
void ycbcr_to_rgb_floats(float *out, float *in);
void rgb_to_xyz_floats(float *out, float *in);
void xyz_to_rgb_floats(float *out, float *in);
void xyz_to_cielab_floats(float *out, float *in);
void rgb_to_ycbcr_floats(float *out, float *in);
void fix_rgb(float *in);
void rgb_to_cielab_floats(float *out, float *in);
void cielab_to_rgb_floats(float *out, float *in);
void colorspace_range ( float *range, int colorspace, int c );

/** builds a colormap. */
 void build_colormap(int *map,int col_plot,int col_step);
void switch_color(int opt_col, int* color);


#endif	//_COLOR_SPACES_H
