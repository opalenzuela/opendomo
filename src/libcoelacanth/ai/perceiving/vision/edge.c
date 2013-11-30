/*****************************************************************************
 *  This file is part of the OpenDomo project.
 *  Copyright(C) 2011 OpenDomo Services SL
 *  
 *  Daniel Lerch Hostalot <dlerch@opendomo.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/



/*
 * Canny implementation based on Mike Heath work <heath@csee.usf.edu> .
 */


#include <values.h>
#include <math.h>
#include <ai/perceiving/vision/edge.h>


#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

#define MAX_BRIGHTNESS  255
#define BOOSTBLURFACTOR 90.0
#define NOEDGE 255
#define POSSIBLE_EDGE 128
#define EDGE 0
#define EDGE_NUM_LIMIT	10
#define MAX_FOLLOW_EDGES 200




/* Sebel Filtering */
void _edge_detection_sobel_filter(image_t *img)
{
   /* Sobel Operator */
   int sobel[3][3] = 
   {
      { -1,  0,  1 },
      { -2,  0,  2 },
      { -1,  0,  1 }
   };


   double pixel_value;
   double min, max;
   int x, y, i, j;  


   /* Get Max and Min 
   min = DBL_MAX;
   max = -DBL_MAX;

   for (y = 1; y < img->width - 1; y++) 
   {
      for (x = 1; x < img->height - 1; x++) 
      {
         pixel_value = 0.0;
         for (j = -1; j <= 1; j++) 
         {
            for (i = -1; i <= 1; i++) 
            {
               double val = (image_get_R(img,x+i,y+j) + image_get_G(img,x+i,y+j)
                           + image_get_B(img,x+i,y+j))/3;
               pixel_value += sobel[j + 1][i + 1] * val;
            }
         }

         if (pixel_value < min) 
            min = pixel_value;
         
         if (pixel_value > max) 
            max = pixel_value;
      }
   }

   if ((int)(max - min) == 0) 
   {
      printf("Nothing exists!!!\n\n");
      exit(1);
   }
   */

   max = 255;
   min = 0;


   /* Initialization of img2 */
   image_t *img2 = image_alloc(img->rgb, img->width, img->height, 
                     img->components);
   for (y = 0; y < img2->height; y++) 
   {
      for (x = 0; x < img2->width; x++) 
      {
         image_set_R(img2, x, y, 255);
         image_set_G(img2, x, y, 255);
         image_set_B(img2, x, y, 255);
      }
   }

   for (y = 1; y < img2->height - 1; y++) 
   {
      for (x = 1; x < img2->width - 1; x++) 
      {
         pixel_value = 0.0;
         for (j = -1; j <= 1; j++) 
         {
            for (i = -1; i <= 1; i++) 
            {
               double val = (image_get_R(img,x+i,y+j) + image_get_G(img,x+i,y+j)
                           + image_get_B(img,x+i,y+j))/3;
            
               pixel_value += sobel[j + 1][i + 1] * val;
            }
         }

         pixel_value = MAX_BRIGHTNESS * (pixel_value - min) / (max - min);
   
         if(pixel_value > 100)
         {
            image_set_R(img2, x, y, 0);
            image_set_G(img2, x, y, 0);
            image_set_B(img2, x, y, 0);
         }
      }
   }
   

   memcpy(img->rgb, img2->rgb, 
      (img->width)*(img->height)*(img->components));
   image_free(img2);
}





/* Create a one dimensional gaussian kernel */
void make_gaussian_kernel(float sigma, float *kernel, int windowsize)
{
   int i, center;
   float x, fx, sum=0.0;

   center = (windowsize) / 2;

   for(i=0;i<(windowsize);i++){
      x = (float)(i - center);
      fx = pow(2.71828, -0.5*x*x/(sigma*sigma)) / (sigma * sqrt(6.2831853));
      kernel[i] = fx;
      sum += fx;
   }

   for(i=0;i<(windowsize);i++) 
      kernel[i] /= sum;
}


/* Blur an image with a gaussian filter */
void gaussian_smooth(unsigned char *image, int rows, int cols, float sigma,
      short int **smoothedim, float *dirim)
{



   int r, c, rr, cc,     /* Counter variables. */
       windowsize,        /* Dimension of the gaussian kernel. */
       center;            /* Half of the windowsize. */
   float *tempim,        /* Buffer for separable filter gaussian smoothing. */
         kernel[20],        /* A one dimensional gaussian kernel. */
         dot,            /* Dot product summing variable. */
         sum;            /* Sum of the kernel weights variable. */

   /* Create a 1-dimensional gaussian smoothing kernel */
   windowsize = 1 + 2 * ceil(2.5 * sigma);
   make_gaussian_kernel(sigma, kernel, windowsize);
   center = windowsize / 2;

   /* Temporary buffer image and the smoothed image */
   tempim = dirim;

   /* Blur in the x - direction */
   for(r=0;r<rows;r++)
   {
      for(c=0;c<cols;c++)
      {
         dot = 0.0;
         sum = 0.0;
         for(cc=(-center);cc<=center;cc++)
         {
            if(((c+cc) >= 0) && ((c+cc) < cols))
            {
               dot += (float)image[r*cols+(c+cc)] * kernel[center+cc];
               sum += kernel[center+cc];
            }
         }
         tempim[r*cols+c] = dot/sum;
      }
   }

   /* Blur in the y - direction */
   for(c=0;c<cols;c++)
   {
      for(r=0;r<rows;r++)
      {
         sum = 0.0;
         dot = 0.0;
         for(rr=(-center);rr<=center;rr++)
         {
            if(((r+rr) >= 0) && ((r+rr) < rows))
            {
               dot += tempim[(r+rr)*cols+c] * kernel[center+rr];
               sum += kernel[center+rr];
            }
         }
         (*smoothedim)[r*cols+c] = (short int)(dot*BOOSTBLURFACTOR/sum + 0.5);
      }
   }

}


/* Compute the first derivative of the image in both the x any y
   directions. The differential filters that are used are: 

   -1
   dx =  -1 0 +1     and       dy =  0
   +1
 */
void derrivative_x_y(short int *smoothedim, int rows, int cols,
      short int **delta_x, short int **delta_y)
{
   int r, c, pos;

   /* Compute the x-derivative. Adjust the derivative at the borders to avoid
      losing pixels */
   for(r=0;r<rows;r++)
   {
      pos = r * cols;
      (*delta_x)[pos] = smoothedim[pos+1] - smoothedim[pos];
      pos++;
      for(c=1;c<(cols-1);c++,pos++){
         (*delta_x)[pos] = smoothedim[pos+1] - smoothedim[pos-1];
      }
      (*delta_x)[pos] = smoothedim[pos] - smoothedim[pos-1];
   }

   /* Compute the y-derivative. Adjust the derivative at the borders to avoid
      losing pixels. */
   for(c=0;c<cols;c++)
   {
      pos = c;
      (*delta_y)[pos] = smoothedim[pos+cols] - smoothedim[pos];
      pos += cols;
      for(r=1;r<(rows-1);r++,pos+=cols)
      {
         (*delta_y)[pos] = smoothedim[pos+cols] - smoothedim[pos-cols];
      }
      (*delta_y)[pos] = smoothedim[pos] - smoothedim[pos-cols];
   }
}



/* This procedure computes the angle of a vector with components x and
   y. It returns this angle in radians with the answer being in the range
   0 <= angle <2*PI. */
double angle_radians(double x, double y)
{
   double xu, yu, ang;
   double retval;

   xu = fabs(x);
   yu = fabs(y);

   if((xu == 0)) 
   {
      retval=(0);
   } 
   else 
   {
      if ((yu == 0)) 
      {
         retval=(0);
      } 
      else 
      {
         ang = atan(yu/xu);

         if(x >= 0)
         {
            if(y >= 0) 
               retval=(ang);

            else 
               retval=(2*M_PI - ang);
         }
         else
         {
            if(y >= 0) 
               retval=(M_PI - ang);

            else 
               retval=(M_PI + ang);
         }
      }
   }

   return retval;
}


/* Compute a direction of the gradient image from component dx and
   dy images. Because not all derriviatives are computed in the same way, this
   code allows for dx or dy to have been calculated in different ways.

   FOR X:  xdirtag = -1  for  [-1 0  1]
   xdirtag =  1  for  [ 1 0 -1]

   FOR Y:  ydirtag = -1  for  [-1 0  1]'
   ydirtag =  1  for  [ 1 0 -1]'

   The resulting angle is in radians measured counterclockwise from the
   xdirection. The angle points "up the gradient" */
void radian_direction(short int *delta_x, short int *delta_y, int rows,
      int cols, float **dir_radians, int xdirtag, int ydirtag, float *dirim)
{
   int r, c, pos;
   double dx, dy;

   /* Allocate an image to store the direction of the gradient */
   *dir_radians = dirim;

   for(r=0,pos=0;r<rows;r++)
   {
      for(c=0;c<cols;c++,pos++)
      {
         dx = (double)delta_x[pos];
         dy = (double)delta_y[pos];

         if(xdirtag == 1) dx = -dx;
         if(ydirtag == -1) dy = -dy;

         dirim[pos] = (float)angle_radians(dx, dy);
      }
   }
}

/* Compute the magnitude of the gradient. This is the square root of
   the sum of the squared derivative values */
void magnitude_x_y(short int *delta_x, short int *delta_y, int rows, int cols,
      short int **magnitude)
{
   int r, c, pos, sq1, sq2;

   /* Allocate an image to store the magnitude of the gradient */
   for(r=0,pos=0;r<rows;r++){
      for(c=0;c<cols;c++,pos++){
         sq1 = (int)delta_x[pos] * (int)delta_x[pos];
         sq2 = (int)delta_y[pos] * (int)delta_y[pos];
         (*magnitude)[pos] = (short)(0.5 + sqrt((float)sq1 + (float)sq2));
      }
   }

}

/* This routine applies non-maximal suppression to the magnitude of
   the gradient image. */
   void
non_max_supp(short *mag, short *gradx, short *grady, int nrows, int ncols,
      unsigned char *result) 
{
   int rowcount, colcount,count;
   short *magrowptr,*magptr;
   short *gxrowptr,*gxptr;
   short *gyrowptr,*gyptr,z1,z2;
   short m00,gx,gy;
   float mag1,mag2,xperp,yperp;
   unsigned char *resultrowptr, *resultptr;


   /* Zero the edges of the result image. */
   for(count=0,resultrowptr=result,resultptr=result+ncols*(nrows-1); 
         count<ncols; resultptr++,resultrowptr++,count++)
   {
      *resultrowptr = *resultptr = (unsigned char) 0;
   }

   for(count=0,resultptr=result,resultrowptr=result+ncols-1;
         count<nrows; count++,resultptr+=ncols,resultrowptr+=ncols)
   {
      *resultptr = *resultrowptr = (unsigned char) 0;
   }

   /* Suppress non-maximum points */
   for(rowcount=1,magrowptr=mag+ncols+1,gxrowptr=gradx+ncols+1,
         gyrowptr=grady+ncols+1,resultrowptr=result+ncols+1;
         rowcount<nrows-2; 
         rowcount++,magrowptr+=ncols,gyrowptr+=ncols,gxrowptr+=ncols,
         resultrowptr+=ncols)
   {   

      for(colcount=1,magptr=magrowptr,gxptr=gxrowptr,gyptr=gyrowptr,
            resultptr=resultrowptr;colcount<ncols-2; 
            colcount++,magptr++,gxptr++,gyptr++,resultptr++)
      {   
         m00 = *magptr;
         if(m00 == 0)
         {
            *resultptr = (unsigned char) NOEDGE;
         }
         else
         {
            xperp = -(gx = *gxptr)/((float)m00);
            yperp = (gy = *gyptr)/((float)m00);
         }

         if(gx >= 0)
         {
            if(gy >= 0)
            {
               if (gx >= gy)
               {  
                  /* 111 */
                  /* Left point */
                  z1 = *(magptr - 1);
                  z2 = *(magptr - ncols - 1);

                  mag1 = (m00 - z1)*xperp + (z2 - z1)*yperp;

                  /* Right point */
                  z1 = *(magptr + 1);
                  z2 = *(magptr + ncols + 1);

                  mag2 = (m00 - z1)*xperp + (z2 - z1)*yperp;
               }
               else
               {    
                  /* 110 */
                  /* Left point */
                  z1 = *(magptr - ncols);
                  z2 = *(magptr - ncols - 1);

                  mag1 = (z1 - z2)*xperp + (z1 - m00)*yperp;

                  /* Right point */
                  z1 = *(magptr + ncols);
                  z2 = *(magptr + ncols + 1);

                  mag2 = (z1 - z2)*xperp + (z1 - m00)*yperp; 
               }
            }
            else
            {
               if (gx >= -gy)
               {
                  /* 101 */
                  /* Left point */
                  z1 = *(magptr - 1);
                  z2 = *(magptr + ncols - 1);

                  mag1 = (m00 - z1)*xperp + (z1 - z2)*yperp;

                  /* Right point */
                  z1 = *(magptr + 1);
                  z2 = *(magptr - ncols + 1);

                  mag2 = (m00 - z1)*xperp + (z1 - z2)*yperp;
               }
               else
               {    
                  /* 100 */
                  /* Left point */
                  z1 = *(magptr + ncols);
                  z2 = *(magptr + ncols - 1);

                  mag1 = (z1 - z2)*xperp + (m00 - z1)*yperp;

                  /* Right point */
                  z1 = *(magptr - ncols);
                  z2 = *(magptr - ncols + 1);

                  mag2 = (z1 - z2)*xperp  + (m00 - z1)*yperp; 
               }
            }
         }
         else
         {
            if ((gy = *gyptr) >= 0)
            {
               if (-gx >= gy)
               {          
                  /* 011 */
                  /* Left point */
                  z1 = *(magptr + 1);
                  z2 = *(magptr - ncols + 1);

                  mag1 = (z1 - m00)*xperp + (z2 - z1)*yperp;

                  /* Right point */
                  z1 = *(magptr - 1);
                  z2 = *(magptr + ncols - 1);

                  mag2 = (z1 - m00)*xperp + (z2 - z1)*yperp;
               }
               else
               {
                  /* 010 */
                  /* Left point */
                  z1 = *(magptr - ncols);
                  z2 = *(magptr - ncols + 1);

                  mag1 = (z2 - z1)*xperp + (z1 - m00)*yperp;

                  /* Right point */
                  z1 = *(magptr + ncols);
                  z2 = *(magptr + ncols - 1);

                  mag2 = (z2 - z1)*xperp + (z1 - m00)*yperp;
               }
            }
            else
            {
               if (-gx > -gy)
               {
                  /* 001 */
                  /* Left point */
                  z1 = *(magptr + 1);
                  z2 = *(magptr + ncols + 1);

                  mag1 = (z1 - m00)*xperp + (z1 - z2)*yperp;

                  /* Right point */
                  z1 = *(magptr - 1);
                  z2 = *(magptr - ncols - 1);

                  mag2 = (z1 - m00)*xperp + (z1 - z2)*yperp;
               }
               else
               {
                  /* 000 */
                  /* Left point */
                  z1 = *(magptr + ncols);
                  z2 = *(magptr + ncols + 1);

                  mag1 = (z2 - z1)*xperp + (m00 - z1)*yperp;

                  /* Right point */
                  z1 = *(magptr - ncols);
                  z2 = *(magptr - ncols - 1);

                  mag2 = (z2 - z1)*xperp + (m00 - z1)*yperp;
               }
            }
         } 

         /* Now determine if the current point is a maximum point */
         if ((mag1 > 0.0)) 
         {
            *resultptr = (unsigned char) NOEDGE;
         }
         else 
         {

            if ((mag2 > 0.0))
            {
               *resultptr = (unsigned char) NOEDGE;
            }
            else
            {    
               if (mag2 == 0.0)
                  *resultptr = (unsigned char) NOEDGE;
               else
                  *resultptr = (unsigned char) POSSIBLE_EDGE;
            }

         }

      }
   }
}

/* This procedure edges is a recursive routine that traces edgs along
   all paths whose magnitude values remain above some specifyable lower
   threshhold. */
void follow_edges(unsigned char *edgemapptr, short *edgemagptr, short lowval,
      int cols)
{
   short *centermagptr;
   unsigned char *centermapptr;
   short *tempmagptr;
   unsigned char *tempmapptr;
   int i;
   int follow_ctr = 0;
   int array_ctr=0;
   unsigned char* mapptr[MAX_FOLLOW_EDGES+1];
   short* magptr[MAX_FOLLOW_EDGES+1];
   int x[8] = {1,1,0,-1,-1,-1,0,1},
       y[8] = {0,1,1,1,0,-1,-1,-1};

   centermapptr=edgemapptr;
   centermagptr=edgemagptr;
   do 
   {
      /* MAXITER = MAX_FOLLOW_EDGES */
      for(i=0;i<8;i++)
      { 
         /* MAXITER = 8 */
         tempmapptr = centermapptr - y[i]*cols + x[i];
         tempmagptr = centermagptr - y[i]*cols + x[i];
         if((*tempmapptr == POSSIBLE_EDGE) && (*tempmagptr > lowval))
         {
            *tempmapptr = (unsigned char) EDGE;
            if (array_ctr <MAX_FOLLOW_EDGES)
            {
               mapptr[array_ctr]= tempmapptr;
               magptr[array_ctr]= tempmagptr;
               array_ctr++;
            }
         }
      }
      centermapptr=mapptr[follow_ctr]; 
      centermagptr=magptr[follow_ctr]; 
      follow_ctr++;
   }
   while (follow_ctr<=array_ctr);
}

/* This routine finds edges that are above some high threshhold or
 * are connected to a high pixel by a path of pixels greater than a low
 * threshold. */
void apply_hysteresis(short int *mag, unsigned char *nms, int rows, int cols,
      float tlow, float thigh, unsigned char *edge)
{
   int r, c, pos, numedges, highcount, lowthreshold, highthreshold,
       hist[cols*rows], edge_ctr;
   short int maximum_mag;

   /* Initialize the edge map to possible edges everywhere the non-maximal
      suppression suggested there could be an edge except for the border. At
      the border we say there can not be an edge because it makes the
      follow_edges algorithm more efficient to not worry about tracking an
      edge off the side of the image. */
   for(r=0,pos=0;r<rows;r++)
   {
      for(c=0;c<cols;c++,pos++)
      {
         if(nms[pos] == POSSIBLE_EDGE) edge[pos] = POSSIBLE_EDGE;
         else edge[pos] = NOEDGE;
      }
   }

   for(r=0,pos=0;r<rows;r++,pos+=cols)
   {
      edge[pos] = NOEDGE;
      edge[pos+cols-1] = NOEDGE;
   }
   pos = (rows-1) * cols;
   for(c=0;c<cols;c++,pos++)
   {
      edge[c] = NOEDGE;
      edge[pos] = NOEDGE;
   }

   /* Compute the histogram of the magnitude image. Then use the histogram to
      compute hysteresis thresholds. */
   for(r=0;r<cols*rows;r++) hist[r] = 0;
   for(r=0,pos=0;r<rows;r++)
   {
      for(c=0;c<cols;c++,pos++)
      {
         if(edge[pos] == POSSIBLE_EDGE) hist[mag[pos]]++;
      }
   }

   /* Compute the number of pixels that passed the nonmaximal suppression.*/
   for(r=1,numedges=0;r<cols*rows;r++)
   {
      if(hist[r] != 0) maximum_mag = r;
      numedges += hist[r];
   }

   highcount = (int)(numedges * thigh + 0.5);

   /* Compute the high threshold value as the (100 * thigh) percentage point
      in the magnitude of the gradient histogram of all the pixels that passes
      non-maximal suppression. Then calculate the low threshold as a fraction
      of the computed high threshold value. John Canny said in his paper
      "A Computational Approach to Edge Detection" that "The ratio of the
      high to low threshold in the implementation is in the range two or three
      to one." That means that in terms of this implementation, we should
      choose tlow ~= 0.5 or 0.33333. */
   r = 1;
   numedges = hist[1];
   {
      int tmp;
      while((tmp=(r<(maximum_mag-1)) && (numedges < highcount)))
      {
         r++;
         numedges += hist[r];
      }
   }

   highthreshold = r;
   lowthreshold = (int)(highthreshold * tlow + 0.5);


   /* This loop looks for pixels above the highthreshold to locate edges and
      then calls follow_edges to continue the edge. */
   for(r=0,pos=0;r<rows;r++)
   {
      for(c=0;c<cols;c++,pos++)
      {
         if((edge[pos] == POSSIBLE_EDGE) && (mag[pos] >= highthreshold) && 
               edge_ctr < EDGE_NUM_LIMIT)
         {
            edge[pos] = EDGE;
            edge_ctr++;
            follow_edges((edge+pos), (mag+pos), lowthreshold, cols);
         }
      }
   }

   /* Set all the remaining possible edges to non-edges. */
   for(r=0,pos=0;r<rows;r++){
      for(c=0;c<cols;c++,pos++) if(edge[pos] != EDGE) edge[pos] = NOEDGE;
   }
}





void _edge_detection_canny_filter(image_t *img)
{
   /* Standard deviation of the gaussian kernel */
   float sigma = 1.5;
   float tlow = 0.35;
   float thigh = 0.75;

   /* The first devivative image, x-direction. */
   short *delta_x;

   /* The first derivative image, y-direction. */
   short *delta_y;  

   /* The image after gaussian smoothing */
   short int *smoothedim;

   /* The magnitude of the gadient image */
   short int *magnitude;      

   /* Points that are local maximal magnitude */
   unsigned char *nms;        

   float	 *dir_radians;


   /* Gradient direction image */
   float dirim[img->width*img->height];   

   /* Temporary images */
   short tmpimg1[img->width*img->height];  
   short tmpimg2[img->width*img->height];  
   short tmpimg3[img->width*img->height];  
   unsigned char uctemp[img->width*img->height];  


   smoothedim = tmpimg1;
   delta_x = tmpimg2;
   delta_y = tmpimg3;

   image_gen_grayscale(img);


   /* Perform gaussian smoothing on the image using the input standard
      deviation */
   gaussian_smooth(img->grayscale, img->height, img->width, 
         sigma, &smoothedim, dirim);

   /* Compute the first derivative in the x and y directions */
   derrivative_x_y(smoothedim, img->height, img->width, &delta_x, &delta_y);

   /* This option to write out the direction of the edge gradient was added
      to make the information available for computing an edge quality figure
      of merit 

      Compute the direction up the gradient, in radians that are
      specified counteclockwise from the positive x-axis */
   radian_direction(delta_x, delta_y, img->height, img->width, 
         &dir_radians, -1, -1, dirim);

   /* Compute the magnitude of the gradient */
   magnitude = tmpimg1;
   magnitude_x_y(delta_x, delta_y, img->height, img->width, &magnitude);

   /* Perform non-maximal suppression */
   nms = uctemp;
   non_max_supp(magnitude, delta_x, delta_y, img->height, img->width, nms);

   /* Use hysteresis to mark the edge pixels */
   apply_hysteresis(magnitude, nms, img->height, img->width, tlow, 
         thigh, img->grayscale);

   image_grayscale_to_rgb(img);

}


void edge_detection(image_t *img, int filter)
{
   switch(filter)
   {
      case SOBEL_FILTER:
         _edge_detection_sobel_filter(img);
         break;

      case CANNY_FILTER:
         _edge_detection_canny_filter(img);
         break;
   }
}





