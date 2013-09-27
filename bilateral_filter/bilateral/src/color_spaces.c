//author: juan.cardelino@gmail.com
//hosted: http://dev.ipol.im/git/?p=juanc/tools.git

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "color_spaces.h"

#ifndef BAD_MIN
#define BAD_MIN(a,b)  (((a)<(b))? (a) : (b))
#endif
#ifndef BAD_MAX
#define BAD_MAX(a,b)  (((a)<(b))? (b) : (a))
#endif


void assert_rgb(double t[3])
{
	for (int i = 0; i < 3; i++)
		assert(t[i] >= 0 && t[i] <= 1);
}

void assert_hsv(double t[3])
{
	if (t[0] < 0 || t[0] >= 360) fprintf(stderr,"queca %g\n", t[0]);
	assert(t[0] >= 0 && t[0] < 360);
	if (!(t[1] >= 0 && t[1] <= 1))
		fprintf(stderr, "CACA S = %g\n", t[1]);
	assert(t[1] >= 0 && t[1] <= 1);
	assert(t[2] >= 0 && t[2] <= 1);
}

void rgb_to_hsv_doubles(double *out, double *in)
{
	assert_rgb(in);
	double r, g, b, h, s, v, M, m;
	r = in[0]; g = in[1]; b = in[2];

	//printf("rgb %g,%g,%g...\n", r, g, b);

	if (g >= r && g >= b) {
		M = g;
		m = BAD_MIN(r, b);
		h = M == m ? 0 : 60*(b-r)/(M-m)+120;
	}
	else if (b >= g && b >= r) {
		M = b;
		m = BAD_MIN(r, b);
		h = M == m ? 0 : 60*(r-g)/(M-m)+240;
	}
	else {
		assert(r >= g && r >= b);
		M = r;
		if (g >= b) {
			m = b;
			h = M == m ? 0 : 60*(g-b)/(M-m)+0;
		} else {
			m = g;
			h = M == m ? 0 : 60*(g-b)/(M-m)+360;
		}
	}

	s = M == 0 ? 0 : (M - m)/M;
	v = M;
	h = fmod(h, 360);

	//printf("\thsv %g,%g,%g\n", h, s, v);
	out[0] = h; out[1] = s; out[2] = v;
	assert_hsv(out);
}

void hsv_to_rgb_doubles(double *out, double *in)
{
	assert_hsv(in);
	double r, g, b, h, s, v; r=g=b=h=s=v=0;
	h = in[0]; s = in[1]; v = in[2];
	if (s == 0)
		r = g = b = v;
	else {
		int H = fmod(floor(h/60),6);
		double p, q, t, f = h/60 - H;
		p = v * (1 - s);
		q = v * (1 - f*s);
		t = v * (1 - (1 - f)*s);
		switch (H) {
			case 0: r = v; g = t; b = p; break;
			case 1: r = q; g = v; b = p; break;
			case 2: r = p; g = v; b = t; break;
			case 3: r = p; g = q; b = v; break;
			case 4: r = t; g = p; b = v; break;
			case 5: r = v; g = p; b = q; break;
			default: assert(false);
		}
	}
	out[0] = r; out[1] = g; out[2] = b;
	assert_rgb(out);
}

void hsv_to_rgb_floats(float *out, float *in)
{
	float r, g, b, h, s, v; r=g=b=h=s=v=0;
	h = in[0]*360.0/255.0; s = in[1]/255.0; v = in[2]/255.0;
	if (s == 0)
		r = g = b = v;
	else {
		int H = fmod(floor(h/60),6);
		float p, q, t, f = h/60 - H;
		p = v * (1 - s);
		q = v * (1 - f*s);
		t = v * (1 - (1 - f)*s);
		switch (H) {
			case 0: r = v; g = t; b = p; break;
			case 1: r = q; g = v; b = p; break;
			case 2: r = p; g = v; b = t; break;
			case 3: r = p; g = q; b = v; break;
			case 4: r = t; g = p; b = v; break;
			case 5: r = v; g = p; b = q; break;
			default: assert(false);
		}
	}
	out[0] = r*255.0; out[1] = g*255.0; out[2] = b*255.0;
}

void ycbcr_to_rgb_floats(float *out, float *in)
{
	out[0] = 1.0*in[0]+ 0.0*(in[1]-127.5)+ 1.40199958865734*(in[2]-127.5);
	out[1] = 1.0*in[0]+ -0.344135678165337*(in[1]-127.5)+ -0.714136155581812*(in[2]-127.5);
	out[2] = 1.0*in[0]+ 1.77200006607382*(in[1]-127.5)+ 0.0*(in[2]-127.5);
}


void fix_rgb(float *in)
{
	for(int i=0;i<3;i++)
	{
		in[i]=rint(in[i]);
		if(in[i]>255.0) in[i]=255.0;
		if(in[i]<0.0) in[i]=0.0;
	}
}

void cielab_to_xyz_floats ( float *out, float *in )
{
	float var_Y = ( in[0] + 16.0 ) / 116.0;
	float var_X = in[1] / 500.0 + var_Y;
	float var_Z = var_Y - in[2] / 200.0;

	if ( pow ( var_Y,3 ) > 0.008856 ) var_Y = pow ( var_Y,3 );
	else                      var_Y = ( var_Y - 16.0 / 116.0 ) / 7.787;
	if ( pow ( var_X,3 ) > 0.008856 ) var_X = pow ( var_X,3 );
	else                      var_X = ( var_X - 16.0 / 116.0 ) / 7.787;
	if ( pow ( var_Z,3 ) > 0.008856 ) var_Z = pow ( var_Z,3 );
	else                      var_Z = ( var_Z - 16.0 / 116.0 ) / 7.787;

	//Observer= 2°, Illuminant= D65
	float ref_X =  95.047;
	float	ref_Y = 100.000;
	float	ref_Z = 108.883;

	out[0] = ref_X * var_X;    //ref_X =  95.047     Observer= 2°, Illuminant= D65
	out[1] = ref_Y * var_Y;    //ref_Y = 100.000
	out[2] = ref_Z * var_Z;    //ref_Z = 108.883

}

void xyz_to_cielab_floats(float *out, float *in)
{
	//Observer= 2°, Illuminant= D65
	float ref_X =  95.047;
	float	ref_Y = 100.000;
	float	ref_Z = 108.883;

	float var_X = in[0] / ref_X ;       //ref_X =  95.047   Observer= 2°, Illuminant= D65
	float	var_Y = in[1] / ref_Y ;         //ref_Y = 100.000
	float	var_Z = in[2] / ref_Z ;        //ref_Z = 108.883

	if ( var_X > 0.008856 ) var_X = pow(var_X , ( 1.0/3.0 ));
	else                    var_X = ( 7.787 * var_X ) + ( 16.0/ 116.0 );
	if ( var_Y > 0.008856 ) var_Y = pow(var_Y , ( 1.0/3.0 ));
	else                    var_Y = ( 7.787 * var_Y ) + ( 16.0 / 116.0);
	if ( var_Z > 0.008856 ) var_Z = pow(var_Z , ( 1.0/3.0 ));
	else                    var_Z = ( 7.787 * var_Z ) + ( 16.0/ 116.0 );

	out[0] = ( 116 * var_Y ) - 16;
	out[1] = 500 * ( var_X - var_Y );
	out[2] = 200 * ( var_Y - var_Z );
}

void xyz_to_rgb_floats(float *out, float *in)
{
	float var_X = in[0] / 100 ;       //X from 0 to  95.047      (Observer = 2°, Illuminant = D65)
	float var_Y = in[1] / 100  ;      //Y from 0 to 100.000
	float var_Z = in[2] / 100;        //Z from 0 to 108.883

	float var_R = var_X *  3.2406 + var_Y * -1.5372 + var_Z * -0.4986;
	float var_G = var_X * -0.9689 + var_Y *  1.8758 + var_Z *  0.0415;
	float var_B = var_X *  0.0557 + var_Y * -0.2040 + var_Z *  1.0570;

	if ( var_R > 0.0031308 ) var_R = 1.055 * ( pow(var_R,  ( 1.0 / 2.4 )) ) - 0.055;
	else                     var_R = 12.92 * var_R;
	if ( var_G > 0.0031308 ) var_G = 1.055 * ( pow(var_G,  ( 1.0 / 2.4 )) ) - 0.055;
	else                     var_G = 12.92 * var_G;
	if ( var_B > 0.0031308 ) var_B = 1.055 * ( pow(var_B,  ( 1.0 / 2.4 )) ) - 0.055;
	else                     var_B = 12.92 * var_B;

	out[0] = var_R * 255;
	out[1] = var_G * 255;
	out[2] = var_B * 255;
}

void rgb_to_xyz_floats(float *out, float *in)
{
	float var_R = ( in[0] / 255 ) ;       //R from 0 to 255
	float var_G = ( in[1] / 255 ) ;       //G from 0 to 255
	float var_B = ( in[2] / 255 ) ;       //B from 0 to 255

	if ( var_R > 0.04045 ) var_R = pow(( ( var_R + 0.055 ) / 1.055 ) , 2.4);
	else                   var_R = var_R / 12.92;
	if ( var_G > 0.04045 ) var_G = pow(( ( var_G + 0.055 ) / 1.055 ) , 2.4);
	else                   var_G = var_G / 12.92;
	if ( var_B > 0.04045 ) var_B = pow(( ( var_B + 0.055 ) / 1.055 ) , 2.4);
	else                   var_B = var_B / 12.92;

	var_R = var_R * 100;
	var_G = var_G * 100;
	var_B = var_B * 100;

	//Observer. = 2°, Illuminant = D65
	out[0] = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
	out[1] = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
	out[2] = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;
}

void rgb_to_ycbcr_floats(float *out, float *in)
{
	out[0] = 0		+0.299*in[0]+ 0.587*in[1]+ 0.114*in[2];
	out[1] = 127.5 + -0.168736*in[0]+ -0.331264*in[1]+ 0.5*in[2];
	out[2] = 127.5 + 0.5*in[0]+ -0.418688*in[1]+ -0.081312*in[2];
}

void rgb_to_grey_floats(float *out, float *in)
{
	out[0] = 0		+0.299*in[0]+ 0.587*in[1]+ 0.114*in[2];
}

void rgb_to_cielab_floats(float *out, float *in)
{
	float xyz[3];
	rgb_to_xyz_floats(xyz,in);
	xyz_to_cielab_floats(out,xyz);
}

void cielab_to_rgb_floats(float *out, float *in)
{
	float xyz[3];
	cielab_to_xyz_floats(xyz,in);
	xyz_to_rgb_floats(out,xyz);
}

void colorspace_range ( float *range, int space, int c )
{
	float min[3]={0,0,0};
	float max[3]={255,255,255};
	switch ( space )
	{
		case FALSE_COLOR:
			break;
		case CIELab:
			max[0]=100;
			min[0]=-0.1;
			min[1]=-255;
			min[2]=-255;
			break;
		case YCbCr:
			break;
		case RGB:

			break;
		default:
			fprintf(stderr,"ERROR: wrong colorspace" );
			abort();
			break;
	}

	range[0]=min[c];
	range[1]=max[c];
}

void build_colormap(int *map, int map_length, int col_step){
	int r=0,g=0,b=0,j=0;
	//int r=0,g=0,b=col_step,j=0;
	int *p;

	p=map;
	/* printf("map: %x \n",map); */

	for(r=0;r<255;r+=col_step){
		for(g=0;g<255;g+=col_step){
			for(b=0;b<255;b+=col_step){
				*p++=r;
				*p++=g;
				*p++=b;
				j=p-map;
				p=j%(3*map_length)+map;
				/* printf("p: %d col: %d %d %d \n",j,r,g,b); */
			}
		}
	}

}

void switch_color(int opt_col, int* color)
{
	switch(opt_col){
		case 0:
			color[0]=255;
			color[1]=0;
			color[2]=0;
			break;
		case 1:
			color[0]=255;
			color[1]=255;
			color[2]=255;
			break;
		case 2:
			color[0]=0;
			color[1]=0;
			color[2]=0;
			break;
		case 4:
			color[0]=0;
			color[1]=255;
			color[2]=0;
			break;
		case 6:
			color[0]=0;
			color[1]=0;
			color[2]=255;
			break;
		case 7:
			color[0]=0;
			color[1]=255;
			color[2]=255;
			break;
		default:
			break;
	}
}

