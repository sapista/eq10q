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
 This file contains the smooth definitions
smooth consist of a low pas filter in order to minimize fast transitions in a
control port. With this idea we get a filter that can be used with automotion
without care of the speed of the automation control.
****************************************************************************/

#ifndef  SMOOTH_H
  #define SMOOTH_H

//Cut off frequency of the LPF filter, this freq is adjusted experimentally
#define F_CUT_OFF 500  ///TODO REMOVE THAT
#define PI 3.1416 ///TODO REMOVE THAT

#define MAX_STEP_PER_SECOND 500.0

typedef struct
{
  float b1_0, b1_1, a1_1; //First order coeficients
  float bufferA[2]; //First order buffers for the first stage
  float bufferB[2]; //First order buffers for the second stage
  float fs; //sample rate
  
  float step;
  float current_sample;
} Smooth;

//Initialize smooth instance
Smooth *SmoothInit(double rate);

//Destroy a smooth instance
void SmoothClean(Smooth *s);

//The DSP processor
inline float computeSmooth(Smooth *s, float inputSample);

#endif