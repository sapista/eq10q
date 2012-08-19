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
This file contains the filter definitions
****************************************************************************/

#ifndef  FILTER_H
  #define FILTER_H

//Constants definitions
#define PI 3.1416

#define BUFFER_SIZE 3
#define BUFFER_1_SIZE 2
#define BUFFER_EXTRA_SIZE 3

#define NUM_OF_CHANNELS @Num_Of_Channels@

typedef enum
{
  NOT_SET,
  LPF_ORDER_1,
  LPF_ORDER_2,
  LPF_ORDER_3,
  LPF_ORDER_4,
  HPF_ORDER_1,
  HPF_ORDER_2,
  HPF_ORDER_3,
  HPF_ORDER_4,
  LOW_SHELF,
  HIGH_SHELF,
  PEAK,
  NOTCH,
}FilterType;

typedef struct
{
  float b0, b1, b2, a1, a2; //Second Order coeficients
  float b1_0, b1_1, a1_1; //First order coeficients
  float buffer[NUM_OF_CHANNELS][BUFFER_SIZE];  //second order buffers, format: pointer to poiner acording buffer[channel][buffer_position] the channel is iniialized wih malloc
  float buffer1[NUM_OF_CHANNELS][BUFFER_1_SIZE]; //First order buffers, format: pointer to poiner acording buffer[channel][buffer_position] the channel is iniialized wih malloc
  float buffer_extra[NUM_OF_CHANNELS][BUFFER_EXTRA_SIZE]; //4st order buffers, format: pointer to poiner acording buffer[channel][buffer_position] the channel is iniialized wih malloc      
  FilterType filter_type; //filter type
  int iFilterEnabled; //1 if filter is enabled
  int filter_order;  //filter order
  float gain, freq, Q; //analog filter parameters
  float fs; //sample rate
}Filter;

//Initialize filter instance
Filter *FilterInit(double rate);

//Destroy a filter instance
void FilterClean(Filter *f);

//Compute filter coeficients acording the order and type
inline void calcCoefs(Filter *f);

//Clean buffers
void flushBuffers(Filter *f);

//The DSP processor
inline float computeFilter(Filter *filter, float inputSample, int ch);

//Check a Change on a band and return 1 if change exisit, otherwise return 0
int checkBandChange(Filter *filter, float fGain, float fFreq, float fQ, int iType, int iEnabled);

//Convert int to FilterType
FilterType int2FilterType(int iType);

//Save filter data
void setFilterParams(Filter *f, float fGain, float fFreq, float fQ, int iType, int iEnabled);
#endif