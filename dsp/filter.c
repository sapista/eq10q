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

#include "filter.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Initialize filter
Filter *FilterInit(double rate)
{
  Filter *filter = (Filter *)malloc(sizeof(Filter));

  filter->gain = GAIN_DEFAULT;
  filter->freq = FREQ_MIN;
  filter->Q = PEAK_Q_DEFAULT;
  filter->filter_type = NOT_SET;
  filter->iFilterEnabled = 0;
  flushBuffers(filter);
  filter->fs=(float)rate;

  return filter;
}

//Destroy a filter instance
void FilterClean(Filter *filter)
{
  free(filter);
}
//Compute filter
inline float computeFilter(Filter *filter, float inputSample)
{
  float w = inputSample;

  if(filter->iFilterEnabled)
  {
    switch(filter->filter_order)
    { 
      //Process first order
      case 1:
        //w(n)=x(n)-a1*w(n-1)
        filter->buffer1[0] = w-filter->a1_1*filter->buffer1[1];
        //y(n)=bo*w(n)+b1*w(n-1)
        w = filter->b1_0*filter->buffer1[0] + filter->b1_1*filter->buffer1[1];
  
        filter->buffer1[1] = filter->buffer1[0];
      break;
  
      //Process second order
      case 2:
        //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
        filter->buffer[0] = w-filter->a1*filter->buffer[1]-filter->a2*filter->buffer[2];
        //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
        w = filter->b0*filter->buffer[0] + filter->b1*filter->buffer[1]+ filter->b2*filter->buffer[2];
  
        filter->buffer[2] = filter->buffer[1];
        filter->buffer[1] = filter->buffer[0];
      break;
    
      //Processat 3r ordre
      case 3:
        //w(n)=x(n)-a1*w(n-1)
        filter->buffer1[0] = w-filter->a1_1*filter->buffer1[1];
        //y(n)=bo*w(n)+b1*w(n-1)
        w = filter->b1_0*filter->buffer1[0] + filter->b1_1*filter->buffer1[1];
  
        filter->buffer1[1] = filter->buffer1[0];
  
        //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
        filter->buffer[0] = w-filter->a1*filter->buffer[1]-filter->a2*filter->buffer[2];
        //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
        w = filter->b0*filter->buffer[0] + filter->b1*filter->buffer[1]+ filter->b2*filter->buffer[2];
  
        filter->buffer[2] = filter->buffer[1];
        filter->buffer[1] = filter->buffer[0];
      break;
    
      //Processat 4t ordre
      case 4:
        //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
        filter->buffer[0] = w-filter->a1*filter->buffer[1]-filter->a2*filter->buffer[2];
        //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
        w = filter->b0*filter->buffer[0] + filter->b1*filter->buffer[1]+ filter->b2*filter->buffer[2];
  
        filter->buffer[2] = filter->buffer[1];
        filter->buffer[1] = filter->buffer[0];
  
        //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
        filter->buffer_extra[0] = w-filter->a1*filter->buffer_extra[1]-filter->a2*filter->buffer_extra[2];
        //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
        w = filter->b0*filter->buffer_extra[0] + filter->b1*filter->buffer_extra[1]+ filter->b2*filter->buffer_extra[2];
  
        filter->buffer_extra[2] = filter->buffer_extra[1];
        filter->buffer_extra[1] = filter->buffer_extra[0];
      break;
    }//END SWITCH ORDER
  }//END of if filter activated
  return w;
}


//Compute filter coeficients
inline void calcCoefs(Filter *filter) //p2 = GAIN p3 = Q
{ 

  float w0=2*PI*(filter->freq/filter->fs);
  float alpha, A, b0, b1, b2, a0, a1, a2, b1_0, b1_1, a1_0, a1_1;

  switch((int)filter->filter_type){
    case NOT_SET: //Do nothing
    break;

    case HPF_ORDER_1:
      b1_0 = 2; //b0
      b1_1 = -2; //b1
      a1_0 = w0+2; //a0
      a1_1 = w0-2; //a1
    break;

    case HPF_ORDER_2:  case HPF_ORDER_4:
      alpha = sin(w0)/(2*filter->Q);

      b0 = (1 + cos(w0))/2; //b0
      b1 = -(1 + cos(w0)); //b1
      b2 = (1 + cos(w0))/2; //b2
      a0 = 1 + alpha; //a0
      a1 = -2*cos(w0); //a1
      a2 = 1 - alpha; //a2
    break;

    case HPF_ORDER_3: 
      alpha = sin(w0)/(2*filter->Q);
      b1_0 = 2; //b0
      b1_1 = -2; //b1
      a1_0 = w0+2; //a0
      a1_1 = w0-2; //a1
      b0 = (1 + cos(w0))/2; //b0
      b1 = -(1 + cos(w0)); //b1
      b2 = (1 + cos(w0))/2; //b2
      a0 = 1 + alpha; //a0
      a1 = -2*cos(w0); //a1
      a2 = 1 - alpha; //a2
    break;

    case LPF_ORDER_1: 

      b1_0 = w0; //b0
      b1_1 = w0; //b1
      a1_0 = w0+2; //a0
      a1_1 = w0-2; //a1
    break;

    case LPF_ORDER_2: case LPF_ORDER_4:
      alpha = sin(w0)/(2*filter->Q);

      b0 = (1 - cos(w0))/2; //b0
      b1 = 1 - cos(w0); //b1
      b2 = (1 - cos(w0))/2; //b2
      a0 = 1 + alpha; //a0
      a1 = -2*cos(w0); //a1
      a2 = 1 - alpha; //a2
    break;

    case LPF_ORDER_3:
      alpha = sin(w0)/(2*filter->Q);
      b1_0 = w0; //b0
      b1_1 = w0; //b1
      a1_0 = w0+2; //a0
      a1_1 = w0-2; //a1
      b0 = (1 - cos(w0))/2; //b0
      b1 = 1 - cos(w0); //b1
      b2 = (1 - cos(w0))/2; //b2
      a0 = 1 + alpha; //a0
      a1 = -2*cos(w0); //a1
      a2 = 1 - alpha; //a2
    break;

    case LOW_SHELF:
      A=pow(10,(filter->gain/40));
      alpha =sin(w0)/2 * (1/filter->Q);

      b0 = A*((A+1)-(A-1)*cos(w0)+2*sqrt(A)*alpha); //b0
      b1 = 2*A*((A-1)-(A+1)*cos(w0)); //b1
      b2 = A*((A+1)-(A-1)*cos(w0)-2*sqrt(A)*alpha); //b2
      a0 = (A+1) + (A-1)*cos(w0) + 2*sqrt(A)*alpha; //a0
      a1 = -2*((A-1) + (A+1)*cos(w0)); //a1
      a2 = (A+1) + (A-1)*cos(w0) - 2*sqrt(A)*alpha; //a2
    break;

    case HIGH_SHELF:
      A=pow(10,(filter->gain/40));
      alpha =sin(w0)/2 * (1/filter->Q);

      b0 = A*( (A+1) + (A-1)*cos(w0) + 2*sqrt(A)*alpha ); //b0
      b1 = -2*A*( (A-1) + (A+1)*cos(w0)); //b1
      b2 = A*( (A+1) + (A-1)*cos(w0) - 2*sqrt(A)*alpha ); //b2
      a0 = (A+1) - (A-1)*cos(w0) + 2*sqrt(A)*alpha; //a0
      a1 = 2*( (A-1) - (A+1)*cos(w0)); //a1
      a2 = (A+1) - (A-1)*cos(w0) - 2*sqrt(A)*alpha; //a2
    break;

    case PEAK:
      A=pow(10,(filter->gain/40));
      alpha = sin(w0)/(2*filter->Q);

      b0 = 1 + alpha*A; //b0
      b1 =  -2*cos(w0); //b1
      b2 =   1 - alpha*A; //b2
      a0 =   1 + alpha/A; //a0
      a1 =  -2*cos(w0); //a1
      a2 =   1 - alpha/A; //a2
    break;

    case NOTCH:
      alpha = sin(w0)/(2*filter->Q);

      b0 =  1; //b0
      b1 = -2*cos(w0); //b1
      b2 =  1; //b2
      a0 =  1 + alpha; //a0
      a1 = -2*cos(w0); //a1
      a2 =  1 - alpha; //a2
    break;
 } //End of switch

  //Normalice coeficients to a0=1
  b0 = b0/a0; //b0
  b1 =b1/a0; //b1
  b2 =b2/a0; //b2
  a1 =a1/a0; //a1
  a2 =a2/a0; //a2

  b1_0 = b1_0/a1_0;
  b1_1 = b1_1/a1_0;
  a1_1 = a1_1/a1_0;


  filter->b0 = b0;
  filter->b1 = b1;
  filter->b2 = b2;
  filter->a1 = a1;
  filter->a2 = a2;

  filter->b1_0 =b1_0;
  filter->b1_1 =b1_1;
  filter->a1_1 =a1_1;
}

//Clean buffers
void flushBuffers(Filter *filter)
{
  int j;
    for(j=0; j<3; j++)
    {
      filter->buffer_extra[j]=0;
      filter->buffer[j]=0;
      if(j<2)filter->buffer1[j]=0;
    }
}

//Check a Change on a band and return 1 if change exist, otherwise return 0
int checkBandChange(Filter *filter, float fGain, float fFreq, float fQ, int iType, int iEnabled)
{
  if(filter->gain < fGain - 0.05 || filter->gain > fGain + 0.05)
    return 1;
  if(filter->freq < fFreq * 0.9999 || filter->freq > fFreq * 1.0001) 
    return 1;
  if(filter->Q < fQ - 0.05 || filter->Q > fQ + 0.05)
    return 1;
  if(filter->filter_type != int2FilterType(iType))
    return 1;
  if(filter->iFilterEnabled != iEnabled)
    return 1;

  return 0;
}

//Convert float to FilterType
FilterType int2FilterType(int iType)
{
  FilterType f;
  f = iType;

  return f;
}

//Save filter data
void setFilterParams(Filter *filter, float fGain, float fFreq, float fQ, int iType, int iEnabled)
{
  filter->gain = fGain;
  filter->freq = fFreq;
  filter->Q = fQ;
  filter->filter_type = int2FilterType(iType);
  filter->iFilterEnabled = iEnabled;

  if(filter->iFilterEnabled)
  {
    //Get filter order
    switch((int)filter->filter_type)
    {
      case NOT_SET:
        //Do nothing
      break;
  
      case LPF_ORDER_1: case HPF_ORDER_1:
        filter->filter_order = 1;
      break;
    
      case LPF_ORDER_2: case HPF_ORDER_2:
      case LOW_SHELF: case HIGH_SHELF: 
      case PEAK: case NOTCH:
        filter->filter_order = 2;
      break;
    
      case LPF_ORDER_3: case HPF_ORDER_3:
        filter->filter_order = 3;
      break;
    
      case LPF_ORDER_4: case HPF_ORDER_4:
        filter->filter_order = 4;
      break;
    }
  }
  else
  {
   flushBuffers(filter);
  }
 
}
