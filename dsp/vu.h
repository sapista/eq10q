/***************************************************************************
 *   Copyright (C) 2011 by Pere R�fols Soler                               *
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
This file contains a VU meter definitions
****************************************************************************/

#include <stdint.h>

#ifndef  VU_H
  #define VU_H

#define VU_MIN_FREQ 80

typedef struct
{
  int vu_i, vu_max, vu_full;
  float vu_value, vu_output, vu_toOutputPort;
}Vu;

//Initialize the VU meter
Vu *VuInit(double rate);

//Destroy a Vu instance
void VuClean(Vu *vu);

//Clear the VU
void resetVU(Vu *vu);

//Inputs a sample to VU
inline void SetSample(Vu *vu, float sample);

//Compute the VU's
inline float ComputeVu(Vu *vu);
#endif