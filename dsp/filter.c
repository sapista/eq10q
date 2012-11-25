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
#include <math.h>
#include "db.h"

#define CMAKE_FILTER_CHANNEL_COUNT @Num_Of_Channels@
#if CMAKE_FILTER_CHANNEL_COUNT == 1
  #include "filter_mono.h"
#else
  #include "filter_stereo.h"
#endif

//#define DENORMAL_TO_ZERO(x) if (((*(unsigned int *) &(x)) & 0x7f800000) == 0) x = 0;

//Handle denormals and make it zero
inline void SMALL_TO_ZERO(float *value)
{
  if (abs(*value) < (10e-40))
  {
        *value = 0.f;
  }
}

//Initialize filter
Filter *FilterInit(double rate)
{
  
  Filter *filter = (Filter *)malloc(sizeof(Filter));
  
  //Smooth on/off
  filter->enableStep = 1.0/(0.01*rate);
  filter->enableGain = 0.0;
  
  filter->gain = 0.0;
  filter->freq = 20.0;
  filter->Q = 2.0;
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
inline void computeFilter(Filter *filter, float *inputSample, int ch)
{
  float bypass_in = *inputSample;
  float en_jump;
  //printf("Pre_ EnGain = %f\r\n",filter->enableGain); 
  en_jump = (float)filter->iFilterEnabled - filter->enableGain;
  en_jump = en_jump > filter->enableStep ? filter->enableStep : en_jump;
  en_jump = en_jump < -filter->enableStep ? -filter->enableStep : en_jump;
  filter->enableGain += en_jump;
  //printf("EnGain = %f   Jump = %f\r\n",filter->enableGain, en_jump); 
  //SMALL_TO_ZERO(&(filter->enableGain));

  
  if(filter->enableGain > filter->enableStep)
  {  
    switch(filter->filter_order)
    { 
      //Process first order
      case 1:
        //w(n)=x(n)-a1*w(n-1)
        filter->buffer1[ch][0] = *inputSample-filter->a1_1*filter->buffer1[ch][1];
        //y(n)=bo*w(n)+b1*w(n-1)
	SMALL_TO_ZERO(&(filter->buffer1[ch][0]));
        *inputSample = filter->b1_0*filter->buffer1[ch][0] + filter->b1_1*filter->buffer1[ch][1];
  
        filter->buffer1[ch][1] = filter->buffer1[ch][0];
      break;
  
      //Process second order
      case 2:
        //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
        filter->buffer[ch][0] = *inputSample-filter->a1*filter->buffer[ch][1]-filter->a2*filter->buffer[ch][2];
        //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
	SMALL_TO_ZERO(&(filter->buffer[ch][0]));
        *inputSample = filter->b0*filter->buffer[ch][0] + filter->b1*filter->buffer[ch][1]+ filter->b2*filter->buffer[ch][2];
  
        filter->buffer[ch][2] = filter->buffer[ch][1];
        filter->buffer[ch][1] = filter->buffer[ch][0];
      break;
    
      //Processat 3r ordre
      case 3:
        //w(n)=x(n)-a1*w(n-1)
        filter->buffer1[ch][0] = *inputSample-filter->a1_1*filter->buffer1[ch][1];
        //y(n)=bo*w(n)+b1*w(n-1)
	SMALL_TO_ZERO(&(filter->buffer[ch][0]));
        *inputSample = filter->b1_0*filter->buffer1[ch][0] + filter->b1_1*filter->buffer1[ch][1];
  
        filter->buffer1[ch][1] = filter->buffer1[ch][0];
  
        //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
        filter->buffer[ch][0] = *inputSample-filter->a1*filter->buffer[ch][1]-filter->a2*filter->buffer[ch][2];
        //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
	SMALL_TO_ZERO(&(filter->buffer[ch][0]));
        *inputSample = filter->b0*filter->buffer[ch][0] + filter->b1*filter->buffer[ch][1]+ filter->b2*filter->buffer[ch][2];
  
        filter->buffer[ch][2] = filter->buffer[ch][1];
        filter->buffer[ch][1] = filter->buffer[ch][0];
      break;
    
      //Processat 4t ordre
      case 4:
        //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
        filter->buffer[ch][0] = *inputSample-filter->a1*filter->buffer[ch][1]-filter->a2*filter->buffer[ch][2];
        //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
	SMALL_TO_ZERO(&(filter->buffer[ch][0]));
        *inputSample = filter->b0*filter->buffer[ch][0] + filter->b1*filter->buffer[ch][1]+ filter->b2*filter->buffer[ch][2];
  
        filter->buffer[ch][2] = filter->buffer[ch][1];
        filter->buffer[ch][1] = filter->buffer[ch][0];
  
        //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
        filter->buffer_extra[ch][0] = *inputSample-filter->a1*filter->buffer_extra[ch][1]-filter->a2*filter->buffer_extra[ch][2];
        //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
	SMALL_TO_ZERO(&(filter->buffer_extra[ch][0]));
        *inputSample = filter->b0*filter->buffer_extra[ch][0] + filter->b1*filter->buffer_extra[ch][1]+ filter->b2*filter->buffer_extra[ch][2];
  
        filter->buffer_extra[ch][2] = filter->buffer_extra[ch][1];
        filter->buffer_extra[ch][1] = filter->buffer_extra[ch][0];
      break;
    }//END SWITCH ORDER
    
  //TEST Print buffers
  //printf("buf0 = %e\r\n",filter->buffer[ch][0]);
  } //END of if filter activated

  //Crossover with clean
  bypass_in *= (1.0 -  filter->enableGain);
  *inputSample *= filter->enableGain;
  *inputSample += bypass_in;

  /********************************** SMOOTH ******************************************************************
  if(filter->iFilterEnabled)
  {
    switch(filter->filter_order)
    { 
      //Process first order
      case 1:
        //w(n)=x(n)-a1*w(n-1)
        filter->buffer1[ch][0] = w- computeSmooth(filter->smooth_a1_1, filter->a1_1)*filter->buffer1[ch][1];
        //y(n)=bo*w(n)+b1*w(n-1)
        w = computeSmooth(filter->smooth_b1_0, filter->b1_0)*filter->buffer1[ch][0] + computeSmooth(filter->smooth_b1_1, filter->b1_1) *filter->buffer1[ch][1];
  
        filter->buffer1[ch][1] = filter->buffer1[ch][0];
	break;
  
      //Process second order
      case 2:
        //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
        filter->buffer[ch][0] = w- computeSmooth(filter->smooth_a1,filter->a1) *filter->buffer[ch][1]- computeSmooth(filter->smooth_a2, filter->a2) *filter->buffer[ch][2];
        //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
        w = computeSmooth(filter->smooth_b0, filter->b0) *filter->buffer[ch][0] + computeSmooth(filter->smooth_b1, filter->b1) *filter->buffer[ch][1]+ computeSmooth(filter->smooth_b2, filter->b2) *filter->buffer[ch][2];
  
        filter->buffer[ch][2] = filter->buffer[ch][1];
        filter->buffer[ch][1] = filter->buffer[ch][0];
      break;
    
      //Processat 3r ordre
      case 3:
        //w(n)=x(n)-a1*w(n-1)
        filter->buffer1[ch][0] = w- computeSmooth(filter->smooth_a1_1, filter->a1_1)*filter->buffer1[ch][1];
        //y(n)=bo*w(n)+b1*w(n-1)
        w = computeSmooth(filter->smooth_b1_0, filter->b1_0)*filter->buffer1[ch][0] + computeSmooth(filter->smooth_b1_1, filter->b1_1) *filter->buffer1[ch][1];
  
        filter->buffer1[ch][1] = filter->buffer1[ch][0];
  
        //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
        filter->buffer[ch][0] = w- computeSmooth(filter->smooth_a1,filter->a1) *filter->buffer[ch][1]- computeSmooth(filter->smooth_a2, filter->a2) *filter->buffer[ch][2];
        //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
        w = computeSmooth(filter->smooth_b0, filter->b0) *filter->buffer[ch][0] + computeSmooth(filter->smooth_b1, filter->b1) *filter->buffer[ch][1]+ computeSmooth(filter->smooth_b2, filter->b2) *filter->buffer[ch][2];
  
        filter->buffer[ch][2] = filter->buffer[ch][1];
        filter->buffer[ch][1] = filter->buffer[ch][0];
      break;
    
      //Processat 4t ordre
      case 4:
        //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
        filter->buffer[ch][0] = w- computeSmooth(filter->smooth_a1,filter->a1) *filter->buffer[ch][1]- computeSmooth(filter->smooth_a2, filter->a2) *filter->buffer[ch][2];
        //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
        w = computeSmooth(filter->smooth_b0, filter->b0) *filter->buffer[ch][0] + computeSmooth(filter->smooth_b1, filter->b1) *filter->buffer[ch][1]+ computeSmooth(filter->smooth_b2, filter->b2) *filter->buffer[ch][2];
  
        filter->buffer[ch][2] = filter->buffer[ch][1];
        filter->buffer[ch][1] = filter->buffer[ch][0];
  
        //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
        filter->buffer_extra[ch][0] = w- computeSmooth(filter->smooth_a1,filter->a1) *filter->buffer_extra[ch][1]- computeSmooth(filter->smooth_a2, filter->a2) *filter->buffer_extra[ch][2];
        //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
        w = computeSmooth(filter->smooth_b0, filter->b0) *filter->buffer_extra[ch][0] + computeSmooth(filter->smooth_b1, filter->b1) *filter->buffer_extra[ch][1]+ computeSmooth(filter->smooth_b2, filter->b2) *filter->buffer_extra[ch][2];
  
        filter->buffer_extra[ch][2] = filter->buffer_extra[ch][1];
        filter->buffer_extra[ch][1] = filter->buffer_extra[ch][0];
      break;
    }//END SWITCH ORDER
  }//END of if filter activated
  ***********************************************************************/

}


//Compute filter coeficients
inline void calcCoefs(Filter *filter) //p2 = GAIN p3 = Q
{ 
  float w0=2*PI*(filter->freq/filter->fs);
  float alpha, A, b0, b1, b2, a0, a1, a2, b1_0, b1_1, a1_0, a1_1;
  alpha = A = b0 = b1 = b2 = a0 = a1 = a2 = b1_0 = b1_1 = a1_0 = a1_1 = 1;

  switch((int)filter->filter_type){

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
      A = sqrt(dB2Lin(filter->gain));
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
      A = sqrt(dB2Lin(filter->gain));
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
      A = sqrt(dB2Lin(filter->gain));
      //A=pow(10,(filter->gain/40));
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
  a1_1 = a1_1/a1_0;

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
  if( (int)filter->filter_type == LPF_ORDER_1 ||
	(int)filter->filter_type == LPF_ORDER_2 ||
	(int)filter->filter_type == LPF_ORDER_3 ||
	(int)filter->filter_type == LPF_ORDER_4)
  {

    int i;
    for(i=0; i< CMAKE_FILTER_CHANNEL_COUNT; i++)
    {

      //Using B
      //filter->buffer1[i][0] *= (filter->b1_0 / b1_0);
      filter->buffer1[i][1] *= (filter->b1_1 / b1_1);  
      //filter->buffer[i][0] *= (filter->b0 / b0);
      filter->buffer[i][1] *= (filter->b1 / b1);
      filter->buffer[i][2] *= (filter->b2 / b2);
      //filter->buffer_extra[i][0] *= (filter->b0 / b0);
      filter->buffer_extra[i][1] *= (filter->b1 / b1);
      filter->buffer_extra[i][2] *= (filter->b2 / b2);
    }
  }

    
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
  int i,j;
    for(i=0; i<NUM_OF_CHANNELS; i++)
    {
      for(j=0; j<3; j++)
      {
	filter->buffer_extra[i][j]=0;
	filter->buffer[i][j]=0;
	if(j<2)filter->buffer1[i][j]=0;
      }
    }
}

//Check a Change on a band and return 1 if change exist, otherwise return 0
inline int checkBandChange(Filter *filter, float fGain, float fFreq, float fQ, int iType, int iEnabled)
{
  /*******************************************************
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
  ************************************/
  
  if(filter->gain != fGain || filter->freq != fFreq || filter->Q != fQ || filter->filter_type != int2FilterType(iType) || filter->iFilterEnabled != iEnabled)
  {
    return 1;
  }
  return 0;
}

//Convert int  to FilterType
FilterType int2FilterType(int iType)
{
  FilterType f;
  f = (FilterType)iType;

  return f;
}

//Save filter data
inline void setFilterParams(Filter *filter, float fGain, float fFreq, float fQ, int iType, int iEnabled)
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
