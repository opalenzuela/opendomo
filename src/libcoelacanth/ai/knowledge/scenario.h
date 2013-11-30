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


#include<knowledge/element.h>
#include<knowledge/network.h>
#include<coelacanth/element.h>

#ifndef __KNOWLEDGE_SCENARIO_H__
#define __KNOWLEDGE_SCENARIO_H__


typedef knowledge_scenario_t
{
   knowledge_network_t when;
   knowledge_network_t where;
   knowledge_element_t *who;  
   knowledge_network_t *what; // what happens
}
knowledge_scenario_t;


#define __KNOWLEDGE_SCENARIO_H__



