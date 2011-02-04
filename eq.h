/***************************************************************************
 *   Copyright (C) 2011 by Pere Ràfols Soler                               *
 *   sapista2@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/***************************************************************************
This file contains some definitions of the EQ plugin
This plugin is inside the Sapista Plugins Bundle
****************************************************************************/

#include "eq_type.h"
/**
* eq_type.h definition
* this file must define the following simbols, otherwise there will be compilation errors
* #define NUM_CHANNELS <integer>
* #define NUM_BANDS <integer>
* #define IS_DBUS <boolean>
* #define EQ_URI       "http://lv2plug.in/plugins/example_amp"; 
*
* NUM_CHANNELS is the number of audio inputs/outputs of the plugin
* NUM_BANDS is the number of equalitzation bands of the plugin
* IS_DBUS is NULL value if the plugin is controlled using LV2 control port
*         if IS_DBUS is diferent than NULL, DBUS control is assumed and no LV2 control port is created
*         is recomended to use DBUS if the EQ plugins have a lot of bands
* EQ_URI the LV2 URI of the plugin
**/


#ifndef  EQ_H
  #define EQ_H

#define EQ_BYPASS    0
#define EQ_INGAIN    1
#define EQ_OUTGAIN   2

#define PORT_OFFSET  3

#endif