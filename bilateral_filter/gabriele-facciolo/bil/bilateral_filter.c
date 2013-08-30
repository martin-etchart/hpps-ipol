// This program is free software: you can use, modify and/or redistribute it
// under the terms of the simplified BSD License. You should have received a
// copy of this license along this program. If not, see
// <http://www.opensource.org/licenses/bsd-license.html>.
// 
// Copyright 2013, Gabriele Facciolo <gfacciol@cmla.ens-cachan.fr>
// All rights reserved.

#include <math.h>

// dct type II symmetry at the boundary
// returns the index of the 2d row major image
unsigned int p_sym(int nx, int ny, int x, int y) 
{ 
   if(x < 0) x = -x-1;
   if(y < 0) y = -y-1;
   if(x >= nx) x = -x+2*nx-1;
   if(y >= ny) y = -y+2*ny-1;
   return x+nx*y;
}

// prolong with last value at the boundary
// returns the index of the 2d row major image
unsigned int p_prolong(int nx, int ny, int x, int y) 
{ 
   x = (x < 0) ? 0 : ( (x>=nx) ? nx-1 : x );
   y = (y < 0) ? 0 : ( (y>=ny) ? ny-1 : y );
   return x+nx*y;
}

// compute bilateral filter at pixel x,y
int bilateral_filter_one_pixel( const float *in, const float *msk,
      const int width, const int height, const int nch,
      const float *ref, const int nchref,
      const int halfkernelsize, const float id, const float cd, 
      const int x, const int y, float* outsum)
{
   const unsigned int ctrIdx = y*width+x;

   const float const *ctrPix = &(ref[ctrIdx*nchref]);

   for(int t=0;t<nch;t++) outsum[t] = 0;

   float sumWeight = 0;
   int num_non_masked = 0;

   // neighborhood of current pixel
   int kernelStartX, kernelEndX, kernelStartY, kernelEndY;
   kernelStartX = x-halfkernelsize;
   kernelEndX   = x+halfkernelsize;
   kernelStartY = y-halfkernelsize;
   kernelEndY   = y+halfkernelsize; 

   for(int j= kernelStartY; j<= kernelEndY; j++)
   {       
      for(int i= kernelStartX; i<= kernelEndX; i++)
      {                       
         const unsigned int idx = p_prolong(width,height,i,j);

         const float const *curPix = &(ref[idx*nchref]);

         // define bilateral filter kernel weights
         float colorDist = 0, imageDist = 0;
         for(int t=0;t<nchref;t++) 
            colorDist = hypot(colorDist, curPix[t] - ctrPix[t]);
         imageDist = hypot(i-x,j-y); 

         float currWeight = 1.0f/(exp((imageDist/id)*(imageDist/id)*0.5)*exp((colorDist/cd)*(colorDist/cd)*0.5));

         // consider the contribution only if not masked 
         const int notmasked = msk[idx]>0;
         num_non_masked += notmasked;
         sumWeight += currWeight * notmasked;
         for(int t=0;t<nch;t++) 
            outsum[t] += currWeight * in[idx*nch + t] * notmasked;
      }
   }

   for(int t=0;t<nch;t++) outsum[t]/=sumWeight;

   return num_non_masked;
}

//! bilateral filter:
void bilateral_filter(const float *in, float*out, const float *msk, 
      const float *outmsk, const int width, const int height, const int nch, 
      const float *ref, const int nchref, 
      const int halfkernelsize, const float id, const float cd)
{ 
   // by default out=in
   for(int i=0; i< height*width*nch; i++) out[i] = in[i];

#pragma omp parallel for
   for(int y=0; y< height; y++)
      for(int x=0; x< width; x++) {

         float filteredpixel[nch];
         int num_non_masked = bilateral_filter_one_pixel(in,msk,width,height,nch,
               ref,nchref,halfkernelsize,id,cd,x,y,filteredpixel);

         // only update output if more than 1/3 of the window is known
         for(int t=0;t<nch;t++)  {
            unsigned int ctrIdx = y*width + x;
            if (num_non_masked > 0.3*(halfkernelsize*2+1)*(halfkernelsize*2+1))
               out[ctrIdx*nch + t] = (filteredpixel[t]) * (outmsk[ctrIdx]==0);
            else 
               out[ctrIdx*nch + t] = NAN;
         }
      }
}


////! bilateral filter:
//void bilateral_filter(const float *in, float*out, const float *msk, 
//      const float *outmsk, const int width, const int height, const int nch, 
//      const float *ref, const int nchref, 
//      const int halfkernelsize, const float id, const float cd)
//{ 
//   // by default out=in
//   for(int i=0; i< height*width*nch; i++) out[i] = in[i];
//
//   #pragma omp parallel for
//   for(int y=0; y< height; y++)
//      for(int x=0; x< width; x++) {
//
//         float sumWeight = 0;
//
//         const unsigned int ctrIdx = y*width + x;
//
//         if ( outmsk[ctrIdx] )  continue;
//
//         float ctrPix[nchref];
//         for(int t=0;t<nchref;t++) 
//            ctrPix[t] = ref[ctrIdx*nchref + t];
//
//         int num_non_masked=0;
//         float sum[nch];
//         for(int t=0;t<nch;t++) sum[t] = 0;
//
//         // neighborhood of current pixel
//         int kernelStartX, kernelEndX, kernelStartY, kernelEndY;
//         kernelStartX = x-halfkernelsize;
//         kernelEndX   = x+halfkernelsize;
//         kernelStartY = y-halfkernelsize;
//         kernelEndY   = y+halfkernelsize; 
//
//         for(int j= kernelStartY; j<= kernelEndY; j++)
//         {       
//            for(int i= kernelStartX; i<= kernelEndX; i++)
//            {                       
//               const unsigned int idx = fmax(0, fmin(j, height-1))*width + fmax(0, fmin(i,width-1));
//
//               float curPix[nchref];
//               for(int t=0;t<nchref;t++) 
//                  curPix[t] = ref[idx*nchref + t];
//
//               // define bilateral filter kernel weights
//               float colorDist = 0;
//               for(int t=0;t<nchref;t++) colorDist = hypot(colorDist, curPix[t] - ctrPix[t]) ;
////               for(int t=0;t<nchref;t++) {
////                  float tmp = curPix[t] - ctrPix[t];
////                  colorDist += tmp*tmp;
////               }
////               colorDist = sqrt(colorDist);
//
//               float imageDist = hypot (i-x,j-y); 
//
//               float currWeight = 1.0f/(exp((imageDist/id)*(imageDist/id)*0.5)*exp((colorDist/cd)*(colorDist/cd)*0.5));
//
//               if( msk[idx] )  // consider the contribution only if not masked 
//               { 
//                  num_non_masked++;
//                  sumWeight += currWeight;
//                  for(int t=0;t<nch;t++) 
//                     sum[t] += currWeight * in[idx*nch + t];
//               }
//            }
//         }
//
//         // only update if more than 1/3 of the window is known
//         for(int t=0;t<nch;t++)  
//            if (num_non_masked > 0.3*(halfkernelsize*2+1)*(halfkernelsize*2+1))
//               out[ctrIdx*nch + t] = sum[t]/sumWeight;
//            else 
//               out[ctrIdx*nch + t] = NAN;
//      }
//}
//



#ifdef SINGLE_FILE_MAIN

#include <stdio.h>
#include <stdlib.h>
#include "iio.h"


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


#endif
