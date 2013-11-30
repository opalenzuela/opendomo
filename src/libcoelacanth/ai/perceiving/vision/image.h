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




#ifndef __PERCEIVING_VISION_IMAGE_H__
#define __PERCEIVING_VISION_IMAGE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ai/perceiving/vision/point.h>
#include <ai/perceiving/vision/zone.h>
#include <ai/perceiving/vision/image_char.h>

//#include <coelacanth/vector.h>

/*
 * image_t structure
 */
typedef struct
{
   /* Points to large array of R,G,B-order data */
   unsigned char *rgb;  

   /* Points to large array of grayscale data */
   unsigned char *grayscale;  

   int generated_grayscale;

   size_t height;   /* Number of rows in image */
   /* Number of columns in image */
   size_t width;    
   
   /* Number of components */
   size_t components;    
}
image_t;


/*
 * Allocate memory for image_t object
 *
 * @param buffer: image data or NULL.
 * @param width: image width
 * @param height: image height
 * @param components: number of components. 3 in RGB images.
 */
image_t *image_alloc(unsigned char *buffer, size_t width, 
                     size_t height, size_t components);

/*
 * Free resources allocated by image_alloc().
 *
 * @param img: image_t object
 */
void image_free(image_t *img);



#define image_get_R(img, x, y) \
   img->rgb[((img->width)*(y) + (x)) * (img->components)]

#define image_get_G(img, x, y) \
   img->rgb[((img->width)*(y) + (x)) * (img->components) +1]

#define image_get_B(img, x, y) \
   img->rgb[((img->width)*(y) + (x)) * (img->components) +2]

#define image_set_R(img, x, y, value) \
   img->rgb[((img->width)*(y) + (x)) * (img->components)]=value

#define image_set_G(img, x, y, value) \
   img->rgb[((img->width)*(y) + (x)) * (img->components) +1]=value

#define image_set_B(img, x, y, value) \
   img->rgb[((img->width)*(y) + (x)) * (img->components) +2]=value


#define image_get(img, x, y) \
   img->grayscale[((img->width)*(y) + (x))]

#define image_set(img, x, y, value) \
   img->grayscale[((img->width)*(y) + (x))]=value


/* 
 * Convert rgb representation to grayscale representation. Both are internal
 * representations of image_t.
 *
 * @param img: image_t object
 */
void image_gen_grayscale(image_t *img);

/*
 * Convert grayscale representation to rgb representation. Both are internal
 * representations of image_t.
 * 
 * @param img: image_t object
 */
void image_grayscale_to_rgb(image_t *img);


/* 
 * Generate new image with differences
 *
 * @param img: image_t object
 * @param img1: first source image
 * @param img2: second source image
 * @return *image_t: new image
 */
image_t* image_diff(image_t *img1, image_t *img2);


/* 
 * Count differences pixel by pixel
 *
 * @param img1: first source image
 * @param img2: second source image
 * @return int: number of different pixels
 */
int image_num_diff(image_t *img1, image_t *img2);

/* 
 * Count differences pixel by pixel within a zone
 *
 * @param img1: first source image
 * @param img2: second source image
 * @return int: number of different pixels
 */
int image_num_diff_zone(image_t *img1, image_t *img2, zone_t * zone);

/* 
 * Draw line in the image
 *
 * @param img: image_t object
 * @param src: source point
 * @param dst: destination point
 *
 */
void image_draw_line(image_t *img, point_t *src, point_t *dst);


/* 
 * Draw a circle in the image
 *
 * @param img: image_t object
 * @param center: point in center
 * @param radius: radius of circle
 */
void image_draw_circle(image_t *img, point_t *center, size_t radius);

/* 
 * Draw a shape in the image
 *
 * @param img: image_t object
 * @param vpoint: vector of points
 * @param vpoint_len: vector length
 */
//void image_draw_shape(image_t *img, void *vpoint, size_t vpoint_len);

/* 
 * Gets the standard deviation of the sum of the three RGB channels
 *
 * @param img: source image
 * @return int: number of different pixels
 * @return int: standard deviation (recommended value 90)
 */
int image_pixel_diff(image_t *img, int threshold);


void image_draw_text(image_t *img, const char *text, point_t *start);


#endif





