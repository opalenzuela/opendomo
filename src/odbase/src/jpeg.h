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


#ifndef __PERCEIVING_VISION_JPEG_H__
#define __PERCEIVING_VISION_JPEG_H__


#include <src/image.h>


void jpeg_write(image_t *img, int quality, char *filename);

char* jpeg_write_alloc(image_t *img, int quality, unsigned long *buflen);

/* bufflen=0 when input is a filename, buflen>0 if is a buffer */
image_t *jpeg_read(char *input, int buflen);


#endif


