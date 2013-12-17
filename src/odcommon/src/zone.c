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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "zone.h"


/**
 This function initialises the rectangles vector struct
 @param[in] rectangle rectangles Reference to the application struct
 @return void
 */
void zone_init(zone_t * zones)
{
	memset(zones, 0, sizeof(zones) * MAX_ZONES);
}

