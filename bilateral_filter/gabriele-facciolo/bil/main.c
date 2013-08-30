// This program is free software: you can use, modify and/or redistribute it
// under the terms of the simplified BSD License. You should have received a
// copy of this license along this program. If not, see
// <http://www.opensource.org/licenses/bsd-license.html>.
// 
// Copyright 2013, Gabriele Facciolo <gfacciol@cmla.ens-cachan.fr>
// All rights reserved.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "iio.h"


extern void bilateral_filter(const float *in, float*out, const float *msk, 
      const float *outmsk, const int width, const int height, const int nch, 
      const float *ref, const int nchref, 
      const int halfkernelsize, const float id, const float cd);

int main (int argc, char **argv)
{
   /* ppatameter parsing - parameters*/
   if(argc<2) 
   {
      fprintf (stderr, "too few parameters\n");
      fprintf (stderr, "bilateral filter\n");
		fprintf(stderr, "   usage:\n\t"
			"%s in=stdin out=stdout ref=in halfkern=8 sigma_img=3 sigma_col=20 msk outmsk\n", argv[0]);
		//  0     1       2           3        4         5           6         7   8       
      return 1;
   }

   //read the parameters
   int i = 1;
   char* f_in          = (argc>i)? argv[i]: "-";       i++;
   char* f_out         = (argc>i)? argv[i]: "-";       i++;
   char* f_ref         = (argc>i)? argv[i]: NULL;      i++;
   int   halfkernelsize = (argc>i)? atoi(argv[i]): 8;       i++;
   float sigma_image    = (argc>i)? atof(argv[i]): 3;       i++;
   float sigma_color    = (argc>i)? atof(argv[i]): 20;       i++;
   char* f_msk         = (argc>i)? argv[i]: NULL;      i++;
   char* f_outmsk      = (argc>i)? argv[i]: NULL;      i++;

   // read input image and reference (if needed)
   int w,h,nch,wr,hr,nchref;
   float *in = iio_read_image_float_vec(f_in, &w, &h, &nch);
   float *ref = in; nchref = nch; // by default the reference is the input
   if (f_ref)  {
      ref = iio_read_image_float_vec(f_ref, &wr, &hr, &nchref);
		if (w != wr || h != hr)
			fprintf(stderr, "input and reference image sizes mismatch\n");
	}

   // allocate output
   float *out = malloc(w*h*nch*sizeof*out);

   // compute/load the masks
   float *msk, *outmsk;
   // data mask: filter only pixels that are nonzero
   if (f_msk)  {
      msk = iio_read_image_float(f_msk, &wr, &hr);
		if (w != wr || h != hr)
			fprintf(stderr, "input and mask image sizes mismatch\n");
   }
   else{
      msk = malloc(w*h*sizeof*msk);
      for(int t=0;t<w*h;t++) 
         msk[t] = (!isnan (in[t]) && isfinite(in[t]) )? 1 : 0 ;
   }

   // output mask: compute the output only at these locations
   if (f_outmsk)  {
      outmsk = iio_read_image_float(f_outmsk, &wr, &hr);
		if (w != wr || h != hr)
			fprintf(stderr, "input and mask image sizes mismatch\n");
   }
   else {
      outmsk = malloc(w*h*sizeof*outmsk);
      for(int t=0;t<w*h;t++) outmsk[t] = 0;
   }


   // call the algorithm
   bilateral_filter(in, out, msk, outmsk, w, h, nch, ref, nchref, 
         halfkernelsize, sigma_image, sigma_color);

   // save output
	iio_save_image_float_vec(f_out, out, w, h, nch);

   // free memory
   if (in!=ref) free(ref);
   free(out);
   free(in);
   free(msk);
   free(outmsk);

   return 0;
}


