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

#include <math.h>

//Constants definitions
#define PI 3.1416
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
  float b1_0, b1_1, b1_2, a1_1, a1_2; //Second Order extra coeficients 
  float buf_0[NUM_OF_CHANNELS];
  float buf_1[NUM_OF_CHANNELS];
  float buf_2[NUM_OF_CHANNELS];
  float buf_e0[NUM_OF_CHANNELS];
  float buf_e1[NUM_OF_CHANNELS];
  float buf_e2[NUM_OF_CHANNELS]; 
  FilterType filter_type; //filter type
  float iFilterEnabled; //1 if filter is enabled
  int filter_order;  //filter order
  float gain, freq, Q; //analog filter parameters
  float fs; //sample rate
}Filter;

//Initialize filter instance
Filter *FilterInit(double rate);

//Destroy a filter instance
void FilterClean(Filter *f);

//Compute filter coeficients
static inline void calcCoefs(Filter *filter) //p2 = GAIN p3 = Q
{ 
  float w0=2*PI*(filter->freq/filter->fs);
  float alpha, A, b0, b1, b2, a0, a1, a2, b1_0, b1_1, b1_2, a1_0, a1_1, a1_2;
  alpha = A = b0 = b1 = b2 = a0 = a1 = a2 = b1_0 = b1_1 = b1_2 = a1_0 = a1_1 = a1_2 = 1.0;

  switch((int)filter->filter_type){

    case HPF_ORDER_1:
      b0 = 2; //b0
      b1 = -2; //b1
      b2 = 0.0; //b2
      a0 = w0+2; //a0
      a1 = w0-2; //a1
      a2 = 0.0; //a2
    break;

    case HPF_ORDER_2:  case HPF_ORDER_4:
      alpha = sin(w0)/(2*filter->Q);

      b1_0 = b0 = (1 + cos(w0))/2; //b0
      b1_1 = b1 = -(1 + cos(w0)); //b1
      b1_2 = b2 = (1 + cos(w0))/2; //b2
      a1_0 = a0 = 1 + alpha; //a0
      a1_1 = a1 = -2*cos(w0); //a1
      a1_2 = a2 = 1 - alpha; //a2
    break;

    case HPF_ORDER_3: 
      alpha = sin(w0)/(2*filter->Q);
      b1_0 = 2; //b0
      b1_1 = -2; //b1
      b1_2 = 0.0; //b2
      a1_0 = w0+2; //a0
      a1_1 = w0-2; //a1
      a1_2 = 0.0; //a2
      b0 = (1 + cos(w0))/2; //b0
      b1 = -(1 + cos(w0)); //b1
      b2 = (1 + cos(w0))/2; //b2
      a0 = 1 + alpha; //a0
      a1 = -2*cos(w0); //a1
      a2 = 1 - alpha; //a2
    break;

    case LPF_ORDER_1: 
      b0 = w0; //b0
      b1 = w0; //b1
      b2 = 0.0; //b2
      a0 = w0+2; //a0
      a1 = w0-2; //a1
      a2 = 0.0; //a2
    break;

    case LPF_ORDER_2: case LPF_ORDER_4:
      alpha = sin(w0)/(2*filter->Q);
      b1_0 = b0 = (1 - cos(w0))/2; //b0
      b1_1 = b1 = 1 - cos(w0); //b1
      b1_2 = b2 = (1 - cos(w0))/2; //b2
      a1_0 = a0 = 1 + alpha; //a0
      a1_1 = a1 = -2*cos(w0); //a1
      a1_2 = a2 = 1 - alpha; //a2
    break;

    case LPF_ORDER_3:
      alpha = sin(w0)/(2*filter->Q);
      b1_0 = w0; //b0
      b1_1 = w0; //b1
      b1_2 = 0.0; //b2
      a1_0 = w0+2; //a0
      a1_1 = w0-2; //a1
      a1_2 = 0.0; //a2
      b0 = (1 - cos(w0))/2; //b0
      b1 = 1 - cos(w0); //b1
      b2 = (1 - cos(w0))/2; //b2
      a0 = 1 + alpha; //a0
      a1 = -2*cos(w0); //a1
      a2 = 1 - alpha; //a2
    break;

    case LOW_SHELF:
      A = sqrt((filter->gain));
      //A=pow(10,(filter->gain/40));
      alpha =sin(w0)/2 * (1/filter->Q);

      b0 = A*((A+1)-(A-1)*cos(w0)+2*sqrt(A)*alpha); //b0
      b1 = 2*A*((A-1)-(A+1)*cos(w0)); //b1
      b2 = A*((A+1)-(A-1)*cos(w0)-2*sqrt(A)*alpha); //b2
      a0 = (A+1) + (A-1)*cos(w0) + 2*sqrt(A)*alpha; //a0
      a1 = -2*((A-1) + (A+1)*cos(w0)); //a1
      a2 = (A+1) + (A-1)*cos(w0) - 2*sqrt(A)*alpha; //a2
    break;

    case HIGH_SHELF:
      A = sqrt((filter->gain));
      //A=pow(10,(filter->gain/40));
      alpha =sin(w0)/2 * (1/filter->Q);

      b0 = A*( (A+1) + (A-1)*cos(w0) + 2*sqrt(A)*alpha ); //b0
      b1 = -2*A*( (A-1) + (A+1)*cos(w0)); //b1
      b2 = A*( (A+1) + (A-1)*cos(w0) - 2*sqrt(A)*alpha ); //b2
      a0 = (A+1) - (A-1)*cos(w0) + 2*sqrt(A)*alpha; //a0
      a1 = 2*( (A-1) - (A+1)*cos(w0)); //a1
      a2 = (A+1) - (A-1)*cos(w0) - 2*sqrt(A)*alpha; //a2
    break;

    case PEAK:
      A = sqrt((filter->gain));
      //A=pow(10,(filter->gain/40));
      alpha = sin(w0)/(2*filter->Q);

      b0 = 1 + alpha*A; //b0
      b1 =  -2*cos(w0); //b1
      b2 =   1 - alpha*A; //b2
      a0 =   1 + alpha/A; //a0
      a1 =  -2*cos(w0); //a1
      a2 =   1 - alpha/A; //a2
      
      //TEST
      printf("peak FS = %f\r\n", filter->fs);
      //printf("peak A = %e alpha = %e  w0 = %e  a0 = %e  a1 = %e  a2 = %e\r\n",A,alpha,w0,a0,a1,a2);
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



/***********************
  //Filter coeficients print
  printf("Not Norm b0 = %e\r\n", b0);
  printf("Not Norm b1 = %e\r\n", b1);
  printf("Not Norm b2 = %e\r\n", b2);
  printf("Not Norm a0 = %e\r\n", a0);
  printf("Not Norm a1 = %e\r\n", a1);
  printf("Not Not Norm a2 = %e\r\n", a2);
  printf("Not Norm b1_0 = %e\r\n", b1_0);
  printf("Not Norm b1_1 = %e\r\n", b1_1);
  printf("Not Norm a1_0 = %e\r\n", a1_0);
  printf("Not Norm a1_1 = %e\r\n", a1_1);
  **************************/

  //Normalice coeficients to a0=1
  b0 = b0/a0; //b0
  b1 =b1/a0; //b1
  b2 =b2/a0; //b2
  a1 =a1/a0; //a1
  a2 =a2/a0; //a2
  b1_0 = b1_0/a1_0;
  b1_1 = b1_1/a1_0;
  b1_2 = b1_2/a1_0;
  a1_1 = a1_1/a1_0;
  a1_2 = a1_2/a1_0;

  /*********************************
  //Filter coeficients print
  printf("Norm b0 = %e\r\n", b0);
  printf("Norm b1 = %e\r\n", b1);
  printf("Norm b2 = %e\r\n", b2);
  printf("Norm a1 = %e\r\n", a1);
  printf("Norm a2 = %e\r\n", a2);
  printf("Norm b1_0 = %e\r\n", b1_0);
  printf("Norm b1_1 = %e\r\n", b1_1);
  printf("Norm a1_1 = %e\r\n", a1_1);
  *****************************************/
  
  //Re-calc buffer values only for LPF Filters this is a improved smooth method that only works for LPF
  if( (int)filter->filter_type == LPF_ORDER_2 || (int)filter->filter_type == LPF_ORDER_4)
  {
    int i;
    for(i=0; i< NUM_OF_CHANNELS; i++)
    {
      //Using B
      filter->buf_1[i] *= (filter->b1 / b1);
      filter->buf_2[i] *= (filter->b2 / b2);
      filter->buf_e1[i] *= (filter->b1_1 / b1_1);
      filter->buf_e2[i] *= (filter->b1_2 / b1_2);
    }
  }
    
  filter->b0 = b0;
  filter->b1 = b1;
  filter->b2 = b2;
  filter->a1 = a1;
  filter->a2 = a2;
  filter->b1_0 =b1_0;
  filter->b1_1 =b1_1;
  filter->b1_2 =b1_2;
  filter->a1_1 =a1_1; 
  filter->a1_2 =a1_2;
}

//Clean buffers
void flushBuffers(Filter *f);


#define DENORMAL_TO_ZERO(x) if (fabs(x) < (10e-40)) x = 0.f;
//Compute filter
static inline void computeFilter(Filter *filter, float *inputSample, int ch)
{
  float bypass_in = *inputSample;
  if(filter->iFilterEnabled > 0.1)
  {    
    //Process 1, 2 orders
    //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
    filter->buf_0[ch] = *inputSample-filter->a1*filter->buf_1[ch]-filter->a2*filter->buf_2[ch];
    //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
    DENORMAL_TO_ZERO(filter->buf_0[ch]);
    *inputSample = filter->b0*filter->buf_0[ch] + filter->b1*filter->buf_1[ch]+ filter->b2*filter->buf_2[ch];

    filter->buf_2[ch] = filter->buf_1[ch];
    filter->buf_1[ch] = filter->buf_0[ch];
    
    //Process 3,4 orders if applis
    if(filter->filter_order > 2)
    {
        //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
        filter->buf_e0[ch] = *inputSample-filter->a1_1*filter->buf_e1[ch]-filter->a1_2*filter->buf_e2[ch];
        //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
	DENORMAL_TO_ZERO(filter->buf_e0[ch]);
        *inputSample = filter->b1_0*filter->buf_e0[ch] + filter->b1_1*filter->buf_e1[ch]+ filter->b1_2*filter->buf_e2[ch];
  
        filter->buf_e2[ch] = filter->buf_e1[ch];
        filter->buf_e1[ch] = filter->buf_e0[ch];
    }
   
  //TEST Print buffers
  //printf("buf0 = %e\r\n",filter->buf_0[ch]);
  } //END of if filter activated

  //Crossover with clean
  bypass_in *= (1.0 -  filter->iFilterEnabled);
  *inputSample *= filter->iFilterEnabled;
  *inputSample += bypass_in;
}

//Convert int  to FilterType
static inline FilterType int2FilterType(int iType)
{
  FilterType f;
  f = (FilterType)iType;
  return f;
}



//Check a Change on a band and return 1 if change exist, otherwise return 0
static inline int checkBandChange(Filter *filter, float fGain, float fFreq, float fQ, int iType, float iEnabled)
{ 
  if(filter->gain != fGain || filter->freq != fFreq || filter->Q != fQ || filter->filter_type != int2FilterType(iType) || filter->iFilterEnabled != iEnabled)
  {
    return 1;
  }
  return 0;
}


//Save filter data
static inline void setFilterParams(Filter *filter, float fGain, float fFreq, float fQ, int iType, float iEnabled)
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
}


#endif