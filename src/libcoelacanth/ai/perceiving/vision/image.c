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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <syslog.h>
#include <ai/perceiving/vision/image.h>

// {{{ image_alloc()
image_t *image_alloc(unsigned char *buffer, size_t width, 
                     size_t height, size_t components)
{
   image_t *img;
   
   img = malloc(sizeof(image_t));
   if(!img)
      return NULL;


   if(!(img->rgb = malloc(width * height * components)))
   {
      free(img);
      return NULL;
   }

   if(!(img->grayscale = malloc(width * height)))
   {
      free(img->rgb);
      free(img);
      return NULL;
   }

   if(buffer!=NULL)
      memcpy(img->rgb, buffer, width*height*components);

   img->width = width;
   img->height = height;
   img->components = components;
   img->generated_grayscale=0;

   return img;
}
// }}}

// {{{ image_free()
void image_free(image_t *img)
{
   free(img->rgb);
   free(img->grayscale);
   free(img);
}
// }}}

// {{{ image_gen_grayscale()
void image_gen_grayscale(image_t *img)
{
   if(img->generated_grayscale)
      return;

   int x, y;

   for(y=0; y<img->height; y++)
      for(x=0; x<img->width; x++)
         image_set(img, x, y, 
            0.3*image_get_R(img, x, y) +
            0.59*image_get_G(img, x, y) +
            0.11*image_get_B(img, x, y)
         );

   img->generated_grayscale=1;
}
// }}}

// {{{ image_grayscale_to_rgb()
void image_grayscale_to_rgb(image_t *img)
{
   int x, y;
   for(y=0; y<img->height; y++)
   {
      for(x=0; x<img->width; x++)
      {
         image_set_R(img, x, y, image_get(img, x, y));
         image_set_G(img, x, y, image_get(img, x, y));
         image_set_B(img, x, y, image_get(img, x, y));
      }
   }

}
// }}}

// {{{ image_diff()
image_t* image_diff(image_t *img1, image_t *img2)
{
   int maxdiff = 100;

   if(img1->width!=img2->width || 
      img1->height!=img2->height || 
      img1->components != img2->components)
   {
      fprintf(stderr, "diferent size\n");
      return NULL;
   }
      
   image_t *result = 
      image_alloc(NULL, img1->width, img1->height, img1->components);

   int x, y;
   for(y=0; y<img1->height; y++)
   {
      for(x=0; x<img1->width; x++)
      {
         int r = image_get_R(img1, x, y) - image_get_R(img2, x, y);
         int g = image_get_G(img1, x, y) - image_get_G(img2, x, y);
         int b = image_get_B(img1, x, y) - image_get_B(img2, x, y);

         if(r+g+b<maxdiff)
         {
            image_set_R(result, x, y, 0);
            image_set_G(result, x, y, 0);
            image_set_B(result, x, y, 0);
         }
         else
         {
            image_set_R(result, x, y, 255);
            image_set_G(result, x, y, 255);
            image_set_B(result, x, y, 255);
         }
      }
   }

   return result;
}
// }}}

// {{{ image_num_diff()
int image_num_diff(image_t *img1, image_t *img2)
{
	int cnt=0;

   if(img1->width!=img2->width || 
      img1->height!=img2->height || 
      img1->components != img2->components)
   {
      fprintf(stderr, "diferent size\n");
      return -1;
   }
      

   int x, y;
   for(y=0; y<img1->height; y++)
   {
      for(x=0; x<img1->width; x++)
      {

         int r = image_get_R(img1, x, y) - image_get_R(img2, x, y);
         int g = image_get_G(img1, x, y) - image_get_G(img2, x, y);
         int b = image_get_B(img1, x, y) - image_get_B(img2, x, y);

         if(r+g+b>100)
         {
				cnt++;
         }
      }
   }

   return cnt;
}
// }}}

// {{{ image_num_diff()
int image_num_diff_zone(image_t *img1, image_t *img2, zone_t *zone)
{
	int cnt=0;
	int tmp_pix = 0;

	if(img1->width!=img2->width || 
	   img1->height!=img2->height || 
	   img1->components != img2->components)
	{
		fprintf(stderr, "diferent size\n");
		return -1;
	}
	
	int x1_pix = (zone->x_top_left * img1->width) / 100;
	int y1_pix = (zone->y_top_left * img1->height) / 100;	
	int x2_pix = (zone->x_bottom_right * img1->width) / 100;
	int y2_pix = (zone->y_bottom_right * img1->height) / 100;	

	/* check zones logic*/
	if (x1_pix > x2_pix)
	{
		tmp_pix = x1_pix;
	    x1_pix = x2_pix;
		x2_pix = tmp_pix;
	}
	/* check zones logic*/		
    if(y1_pix > y2_pix)
	{
		tmp_pix = y1_pix;
	    y1_pix = y2_pix;
		y2_pix = tmp_pix;
		
	}
	/* check for the right values */			
	if ((y1_pix > img1->height || y2_pix > img1->height) || 
		(x1_pix > img1->width || x2_pix > img1->width) ) {
		fprintf(stderr, "zones are not initialized!\n");
		return -1;
	}
	
	int x, y;
	for(y=y1_pix; y<y2_pix; y++)
	{
		for(x=x1_pix; x<x2_pix; x++)
		{
			int r = image_get_R(img1, x, y) - image_get_R(img2, x, y);
			int g = image_get_G(img1, x, y) - image_get_G(img2, x, y);
			int b = image_get_B(img1, x, y) - image_get_B(img2, x, y);
			
			if(r+g+b>100)
			{
				cnt++;
			}
		}
	}
	
	return cnt;
}
// }}}

// {{{ image_pixel_diff()
int image_pixel_diff(image_t *img, int threshold)
{
	int cnt=0;
	double avg = 0, variance = 0;
	double accum = 0;
	double std_dev = 0;	
	double total = 0;
	
	int r = 0, g = 0, b = 0;
		
	// First step to obtain the average
	int x, y;
	for(y=0; y<img->height; y++)
	{
		for(x=0; x<img->width; x++)
		{
			r = image_get_R(img, x, y);
			g = image_get_G(img, x, y);
			b = image_get_B(img, x, y);
			
			cnt++;
			// average
			total += r+g+b;
		}
	}
	
	if(cnt > 0)
	   avg = total / cnt;	
	else
		return -1;

	// Second step to optain the standard deviation	
	for(y=0; y<img->height; y++)
	{
		for(x=0; x<img->width; x++)
		{
			r = image_get_R(img, x, y);
			g = image_get_G(img, x, y);
			b = image_get_B(img, x, y);			
			// Variance
			accum += pow((r+g+b) - avg,2);
		}
	}
    // standard deviation
	variance = accum / (cnt - 1);	
	std_dev = sqrt(variance);

	//printf("DEV: %f\n",std_dev);

	if ((int) std_dev < threshold) {
		return 1;
	}
	
	return 0;
}
// }}}



// {{{ image_draw_line() 
// TODO: improve performance
void image_draw_line(image_t *img, point_t *src, point_t *dst)
{
   int x1;
   int x2;
   int y1;
   int y2;

   if(src->x>=dst->x)
   {
      x2 = src->x;
      x1 = dst->x;
      y2 = src->y;
      y1 = dst->y;
   }
   else
   {
      x1 = src->x;
      x2 = dst->x;
      y1 = src->y;
      y2 = dst->y;
   }


   // line: y=mx+c where m=(y2-y1)/(x2-x1) and c=y1-m*x1 
   float c;
   float m;
   int x, y;

   if(x2==x1) 
      m=0;
   else
      m = (float)(y2-y1)/(x2-x1);

   c = y1-m*x1;

   /* draw points */
   for(x=x1; x<=x2; x++)
   {
      y = x*m+c;
      if(y>=y1 && y<=y2)
      {
         image_set_R(img, x, y, 255);
         image_set_G(img, x, y, 0);
         image_set_B(img, x, y, 0);
      }
   }

   for(y=y1; y<=y2; y++)
   {
      x = (y-c)/m;
      if(x>=x1 && x<=x2)
      {
         image_set_R(img, x, y, 255);
         image_set_G(img, x, y, 0);
         image_set_B(img, x, y, 0);
      }
   }



}
// }}} 

// {{{ image_draw_circle()
// TODO: improve performance
void image_draw_circle(image_t *img, point_t *center, size_t radius)
{
   // (x-cx)^2 + (y-cy)^2 = r^2

   for(int x=0; x<img->width; x++)
   {
      int d = radius*radius - (x-center->x)*(x-center->x);
      int y1 =0;
      int y2 =0;
      if(d>=0)
      {
         y1 = sqrt(d) + center->y;
         y2 = -sqrt(d) + center->y;
      }
      else
         continue;

      //printf("%d,%d\n", x, y1);
      image_set_R(img, x, y1, 255);
      image_set_G(img, x, y1, 0);
      image_set_B(img, x, y1, 0);

      //printf("%d,%d\n", x, y2);
      image_set_R(img, x, y2, 255);
      image_set_G(img, x, y2, 0);
      image_set_B(img, x, y2, 0);
   }

   for(int y=0; y<img->height; y++)
   {
      int d = radius*radius - (y-center->y)*(y-center->y);
      int x1 =0;
      int x2 =0;
      if(d>=0)
      {
         x1 = sqrt(d) + center->x;
         x2 = -sqrt(d) + center->x;
      }
      else
         continue;

      //printf("%d,%d\n", x, y1);
      image_set_R(img, x1, y, 255);
      image_set_G(img, x1, y, 0);
      image_set_B(img, x1, y, 0);

      //printf("%d,%d\n", x, y2);
      image_set_R(img, x2, y, 255);
      image_set_G(img, x2, y, 0);
      image_set_B(img, x2, y, 0);
   }




}
// }}}


/*
// {{{ image_draw_shape()
void image_draw_shape(image_t *img, void *vpoint, size_t vpoint_len)
{
   vector_ptr(v, point_t);
   v = vpoint;

   for(int i=0; i<vpoint_len-1; i++)
      image_draw_line(img, &v[i], &v[i+1]);

   image_draw_line(img, &v[vpoint_len-1], &v[0]);
}
// }}}
*/


// {{{ image_draw_text
void image_draw_text(image_t *img, const char *text, point_t *start)
{

	// TODO: debug, not ready yet

	if(!start)
	{
      syslog(LOG_ERR, "image_draw_text() Invalid starting point\n");
		return;
	}

	if(!text)
	{
      syslog(LOG_ERR, "image_draw_text() text empty\n");
		return;
	}

	int idx=0;
	int i, j;
	int y=start->y;
	int x=start->x;
	int c;
	while((c=text[idx]))
	{
		c-=33; // index correction

		for(j=0; j<IC_COL_SIZE; j++)
		{
			for(i=0; i<IC_ROW_SIZE; i++)
			{
				int v=0;
				if(image_char_table[c][j][i]==2)
					v=255;
				else
					v=0;

				//syslog(LOG_NOTICE, "set (%d+%d, %d+%d) to %d\n", x,i, y,j, v);

				image_set_R(img, x+i, y+j, v);
				image_set_G(img, x+i, y+j, v);
				image_set_B(img, x+i, y+j, v);
			}
		}
		x+=IC_ROW_SIZE;
		idx++;
	}

}
// }}}





