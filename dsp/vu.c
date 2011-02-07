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
This file contains a VU meter implementation
****************************************************************************/

//Enable this for debugin mode
//#define IS_DEBUG_

#include "vu.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Initialize the VU meter
Vu *VuInit(double rate)
{
#ifdef IS_DEBUG_
  printf("Vu Init...\n\r");
#endif

  Vu *vu = (Vu *)malloc(sizeof(Vu));

  vu->vu_value = 0.0;
  vu->vu_max = 0.0;
  
  m_min = (1.0 / 256);
  m_decay = exp(log(m_min) / (1 * rate));
 
  return vu;
#ifdef IS_DEBUG_
  printf("Vu ready\n\r");
#endif
}

//Clear the VU
void resetVU(Vu *vu)
{
  vu->vu_full = 1;
  vu->vu_output = 0.0;
}

//Destroy a Vu instance
void VuClean(Vu *vu){
  free(vu);
}

//Inputs a sample to VU
inline void SetSample(Vu *vu, float sample)
{
  vu->vu_value = fabsf(sample);
  vu->vu_max = vu->vu_value > vu->vu_max ? vu->vu_value :  vu->vu_max;
}

//Compute the VU's
inline float ComputeVu(Vu *vu, uint32_t nframes)
{
  const float fVuOut = vu_max > m_min ? vu_max : 0;
      if (vu_max > m_min)
		vu_max *= pow(m_decay, nframes);
      else
	vu_max = 0.0;
	
  return fVuOut;
}
