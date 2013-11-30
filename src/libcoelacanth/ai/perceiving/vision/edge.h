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



#ifndef __PERCEIVING_VISION_EDGE_H__
#define __PERCEIVING_VISION_EDGE_H__

#include <ai/perceiving/vision/image.h>

/* Supported Filters */
#define SOBEL_FILTER 0
#define CANNY_FILTER 1


/**
 * Algorithms for Edge Detection
 *
 * @param img: Source Image
 * @param filter: Filter to apply
 */
void edge_detection(image_t *img, int filter);




#endif

