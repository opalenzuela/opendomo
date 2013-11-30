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




#include<ai/perceiving/vision/thresholding.h>


/* iterative method: a special one-dimensuonal case of the k-means clustering
   algorithm */
void _iterative_method(image_t *img)
{
   int threshold = 127;
   int converges = 0;
   int x, y;

   image_gen_grayscale(img);


   while(!converges)
   {
      int g1=0;
      int g2=0;
      int g1_counter=0;
      int g2_counter=0;

      for(y=0; y<img->height; y++)
      {
         for(x=0; x<img->width; x++)
         {
            int pixel = image_get(img, x, y);
            if(pixel>threshold)
            {
               g1 += pixel;
               g1_counter++;
            }
            else
            {
               g2 += pixel;
               g2_counter++;
            }
         }
      }

      int new_threshold = (g1/g1_counter + g2/g2_counter)/2;
      if(threshold==new_threshold)
         converges = 1;
      else
         threshold = new_threshold;
   }


   printf("threshold: %d\n", threshold);

   for(y=0; y<img->height; y++)
   {
      for(x=0; x<img->width; x++)
      {
         int pixel = image_get(img, x, y);
         if(pixel>threshold)
            image_set(img, x, y, 255);
         else
            image_set(img, x, y, 0);
      }
   }

   image_grayscale_to_rgb(img);
}


void thresholding(image_t *img, int type)
{
   switch(type)
   {
      case ITERATIVE_METHOD:
         _iterative_method(img);
         break;
   }
}



