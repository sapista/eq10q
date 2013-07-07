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

#include "smooth.h"
//#include <stdio.h>
#include <stdlib.h>

//Initialize smooth
Smooth *SmoothInit(double rate, float max_step_per_second)
{
  Smooth *s = (Smooth *)malloc(sizeof(Smooth));
  s->fs=(float)rate;
  float w0=2*PI*(F_CUT_OFF/s->fs);
  float b1_0, b1_1, a1_0, a1_1;
  
  //Filter coefs.
  b1_0 = w0; //b0
  b1_1 = w0; //b1
  a1_0 = w0+2; //a0
  a1_1 = w0-2; //a1

  //Normalize coefs and save
  s->b1_0 = b1_0/a1_0;
  s->b1_1 = b1_1/a1_0;
  s->a1_1 = a1_1/a1_0;

  //Flush the buffer to zero
  int i;
  for(i=0; i < 2; i++)
  {
    s->bufferA[i] = 0.0;
    s->bufferB[i] = 0.0;
  }
  
  
  //NEW WAY TEST
  s->step = max_step_per_second/(float)rate;
  s->current_sample = 0.0;

  return s;
}

//Destroy a smooth instance
void SmoothClean(Smooth *s)
{
  free(s);
}
