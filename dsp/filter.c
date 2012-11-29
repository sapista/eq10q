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

#include <stdio.h>
#include <stdlib.h>

#if CMAKE_FILTER_CHANNEL_COUNT == 1
  #include "filter_mono.h"
#else
  #include "filter_stereo.h"
#endif

//Initialize filter
Filter *FilterInit(double rate)
{
  
  Filter *filter = (Filter *)malloc(sizeof(Filter));
  
  filter->gain = 0.0;
  filter->freq = 20.0;
  filter->Q = 2.0;
  filter->filter_type = NOT_SET;
  filter->iFilterEnabled = 0;
  flushBuffers(filter);
  filter->fs=(float)rate;
  printf("Init rate = %f   FS = %f\r\n",rate, filter->fs); 
  return filter;
}

//Destroy a filter instance
void FilterClean(Filter *filter)
{ 
  free(filter);
}

//Clean buffers
void flushBuffers(Filter *filter)
{
  int i;
    for(i=0; i<NUM_OF_CHANNELS; i++)
    {   
      filter->buf_0[i] = 0.0;
      filter->buf_1[i] = 0.0;
      filter->buf_2[i] = 0.0;
      filter->buf_e0[i] = 0.0;
      filter->buf_e1[i] = 0.0;
      filter->buf_e2[i] = 0.0;
    }
}
