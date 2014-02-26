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
This file is the implementation of the EQ plugin
This plugin is inside the Sapista Plugins Bundle
This file implements functionalities for a large numbers of equalizers
****************************************************************************/

//TODO Comment out stdio.h
#include <stdio.h>

#include <stdlib.h>

#include "lv2.h"
#include "gui/eq_defines.h"
#include "dsp/vu.h"
#include "dsp/db.h"
#include "dsp/filter.h"

//Data from CMake
#define NUM_BANDS @Eq_Bands_Count@
#define NUM_CHANNELS @Eq_Channels_Count@
#define EQ_URI @Eq_Uri@

#define EQ_INPUT_GAIN 10000.0
#define EQ_OUTPUT_GAIN 0.0001

static LV2_Descriptor *eqDescriptor = NULL;

typedef struct {
  //Plugin ports
  float *fBypass;
  float *fInGain;
  float *fOutGain;
  float *fBandGain[NUM_BANDS];
  float *fBandFreq[NUM_BANDS];
  float *fBandParam[NUM_BANDS];
  float *fBandType[NUM_BANDS];
  float *fBandEnabled[NUM_BANDS];
  float *fInput[NUM_CHANNELS];
  float *fOutput[NUM_CHANNELS];
  float *fVuIn[NUM_CHANNELS];
  float *fVuOut[NUM_CHANNELS];

  //Plugin DSP
  Filter *filter[NUM_BANDS];
  Buffers buf[NUM_BANDS][NUM_CHANNELS];
  Vu *InputVu[NUM_CHANNELS];
  Vu *OutputVu[NUM_CHANNELS];  
} EQ;

static void cleanupEQ(LV2_Handle instance)
{
  EQ *plugin = (EQ *)instance;
  int i;
  for(i=0; i<NUM_BANDS; i++)
  {
    FilterClean(plugin->filter[i]);
  }

  for(i=0; i<NUM_CHANNELS; i++)
  {
    VuClean(plugin->InputVu[i]);
    VuClean(plugin->OutputVu[i]);
  }
  free(instance);
}

static void connectPortEQ(LV2_Handle instance, uint32_t port, void *data)
{
  EQ *plugin = (EQ *)instance;

  //Connect standar ports
  switch (port)
  {
    case EQ_BYPASS:
      plugin->fBypass = data;
    break;

    case EQ_INGAIN:
      plugin->fInGain = data;
    break;

    case EQ_OUTGAIN:
      plugin->fOutGain = data;
    break;

    default:
      //Connect audio input ports
      if(port >= PORT_OFFSET && port < (PORT_OFFSET + NUM_CHANNELS))
      {
        plugin->fInput[port - PORT_OFFSET] = data;
      }

      //Connect audio output ports
      if(port >= (PORT_OFFSET + NUM_CHANNELS) && port < (PORT_OFFSET + 2*NUM_CHANNELS))
      {
        plugin->fOutput[port - PORT_OFFSET - NUM_CHANNELS] = data;
      }

      //Connect BandGain ports
      else if(port >= (PORT_OFFSET + 2*NUM_CHANNELS) && port < (PORT_OFFSET + 2*NUM_CHANNELS + NUM_BANDS))
      {
        plugin->fBandGain[port - PORT_OFFSET - 2*NUM_CHANNELS] = data;
      }

      //Connect BandFreq ports
      else if(port >= (PORT_OFFSET + 2*NUM_CHANNELS + NUM_BANDS) && port < (PORT_OFFSET + 2*NUM_CHANNELS + 2*NUM_BANDS))
      {
        plugin->fBandFreq[port - PORT_OFFSET - 2*NUM_CHANNELS - NUM_BANDS] = data;
      }

      //Connect BandParam ports
      else if(port >= (PORT_OFFSET + 2*NUM_CHANNELS + 2*NUM_BANDS) && port < (PORT_OFFSET + 2*NUM_CHANNELS + 3*NUM_BANDS))
      {
        plugin->fBandParam[port - PORT_OFFSET - 2*NUM_CHANNELS - 2*NUM_BANDS] = data;
      }

      //Connect BandType ports
      else if(port >= (PORT_OFFSET + 2*NUM_CHANNELS + 3*NUM_BANDS) && port < (PORT_OFFSET + 2*NUM_CHANNELS + 4*NUM_BANDS))
      {
        plugin->fBandType[port - PORT_OFFSET - 2*NUM_CHANNELS - 3*NUM_BANDS] = data;
      }

      //Connect BandEnabled ports
      else if(port >= (PORT_OFFSET + 2*NUM_CHANNELS + 4*NUM_BANDS) && port < (PORT_OFFSET + 2*NUM_CHANNELS + 5*NUM_BANDS))
      {
        plugin->fBandEnabled[port - PORT_OFFSET - 2*NUM_CHANNELS - 4*NUM_BANDS] = data;
      }

      //Connect VuInput ports
      else if(port >= (PORT_OFFSET + 2*NUM_CHANNELS + 5*NUM_BANDS) && port < (PORT_OFFSET + 2*NUM_CHANNELS + 5*NUM_BANDS + NUM_CHANNELS))
      {
        plugin->fVuIn[port - PORT_OFFSET - 2*NUM_CHANNELS - 5*NUM_BANDS] = data;
      }

      //Connect VuOutput ports
      else if(port >= (PORT_OFFSET + 2*NUM_CHANNELS + 5*NUM_BANDS + NUM_CHANNELS) && port < (PORT_OFFSET + 2*NUM_CHANNELS + 5*NUM_BANDS + 2*NUM_CHANNELS))
      {
        plugin->fVuOut[port - PORT_OFFSET - 2*NUM_CHANNELS - 5*NUM_BANDS - NUM_CHANNELS] = data;
      }
    break;
  }
}

static LV2_Handle instantiateEQ(const LV2_Descriptor *descriptor, double s_rate, const char *path, const LV2_Feature *const * features)
{
  int i,ch;
  EQ *plugin_data = (EQ *)malloc(sizeof(EQ));  
  
  for(i=0; i<NUM_BANDS; i++)
  {
    plugin_data->filter[i] = FilterInit(s_rate);
    for(ch=0; ch<NUM_CHANNELS; ch++)
    {
      flushBuffers(&plugin_data->buf[i][ch]);
    }
  }
  

  for(ch=0; ch<NUM_CHANNELS; ch++)
  {
    plugin_data->InputVu[ch] = VuInit(s_rate);
    plugin_data->OutputVu[ch] = VuInit(s_rate);
  }

  return (LV2_Handle)plugin_data;
}

static void runEQ_v2(LV2_Handle instance, uint32_t sample_count)
{
  EQ *plugin_data = (EQ *)instance;
 
  //Get values of control ports
  const int iBypass = *(plugin_data->fBypass) > 0.0f ? 1 : 0;  
  const float fInGain = dB2Lin(*(plugin_data->fInGain));
  const float fOutGain = dB2Lin(*(plugin_data->fOutGain));
  int bd, pos; //loop index
  
  //Interpolation coefs force to recompute
  int recalcCoefs[NUM_BANDS];
  int forceRecalcCoefs = 0;
  
  float  sampleL; //Current processing sample left signal
  #if NUM_CHANNELS == 2
  float sampleR; //Current processing sample right signal
  #endif
   
  //Read EQ Ports and mark to recompute if changed
  for(bd = 0; bd<NUM_BANDS; bd++)
  {
    if(dB2Lin(*(plugin_data->fBandGain[bd])) != plugin_data->filter[bd]->gain ||
	*plugin_data->fBandFreq[bd] != plugin_data->filter[bd]->freq ||
	*plugin_data->fBandParam[bd] != plugin_data->filter[bd]->q ||
	(int)(*plugin_data->fBandType[bd]) != plugin_data->filter[bd]->iType ||
	*plugin_data->fBandEnabled[bd] != plugin_data->filter[bd]->enable)
    {
      recalcCoefs[bd] = 1;
      forceRecalcCoefs = 1;
    }
    else
    {
      recalcCoefs[bd] = 0;
    }
  }
    
  //Compute the filter
  for (pos = 0; pos < sample_count; pos++) 
  {       
    //Get input
    sampleL = plugin_data->fInput[0][pos];
    DENORMAL_TO_ZERO(sampleL);
    
    #if NUM_CHANNELS == 2
    sampleR = plugin_data->fInput[1][pos];
    DENORMAL_TO_ZERO(sampleR);
    #endif    


    //Process every band
    if(iBypass)
    {
      resetVU(plugin_data->InputVu[0]);
      resetVU(plugin_data->OutputVu[0]);
      #if NUM_CHANNELS == 2
      resetVU(plugin_data->InputVu[1]);
      resetVU(plugin_data->OutputVu[1]);   
      #endif   
    }
    else
    {    
      //The input amplifier
      sampleL *= fInGain;    
      //Update VU input sample
      SetSample(plugin_data->InputVu[0], sampleL);
      //Apply gain to the sample to move it to de-normalized state
      //sampleL *= EQ_INPUT_GAIN;///TODO Testing lattice
      
      #if NUM_CHANNELS == 2
      //The input amplifier
      sampleR *= fInGain;    
      //Update VU input sample
      SetSample(plugin_data->InputVu[1], sampleR);
      //Apply gain to the sample to move it to de-normalized state
      //sampleR *= EQ_INPUT_GAIN;///TODO Testing lattice
      #endif
      
      //Coefs Interpolation
      if(forceRecalcCoefs)
      {
	for(bd = 0; bd<NUM_BANDS; bd++)
	{
	  if(recalcCoefs[bd])
	  {
	    calcCoefs(plugin_data->filter[bd],
		    dB2Lin(*(plugin_data->fBandGain[bd])),
		    *plugin_data->fBandFreq[bd],
		    *plugin_data->fBandParam[bd],
		    (int)(*plugin_data->fBandType[bd]),
		    *plugin_data->fBandEnabled[bd]);
	    }
	  }
	}
      
      
      //EQ PROCESSOR
	computeFilter(plugin_data->filter[0], &plugin_data->buf[0][0],&sampleL);
	
	#if NUM_BANDS >= 4
	computeFilter(plugin_data->filter[1], &plugin_data->buf[1][0],&sampleL);
	computeFilter(plugin_data->filter[2], &plugin_data->buf[2][0],&sampleL);
	computeFilter(plugin_data->filter[3], &plugin_data->buf[3][0],&sampleL);
	#endif
	
	#if NUM_BANDS >= 6
	computeFilter(plugin_data->filter[4], &plugin_data->buf[4][0],&sampleL);
	computeFilter(plugin_data->filter[5], &plugin_data->buf[5][0],&sampleL);
	#endif
	
	#if NUM_BANDS ==10
	computeFilter(plugin_data->filter[6], &plugin_data->buf[6][0],&sampleL);
	computeFilter(plugin_data->filter[7], &plugin_data->buf[7][0],&sampleL);
	computeFilter(plugin_data->filter[8], &plugin_data->buf[8][0],&sampleL);
	computeFilter(plugin_data->filter[9], &plugin_data->buf[9][0],&sampleL);
	#endif
	
	#if NUM_CHANNELS == 2
	  computeFilter(plugin_data->filter[0], &plugin_data->buf[0][1],&sampleR);
	  
	  #if NUM_BANDS >= 4
	  computeFilter(plugin_data->filter[1], &plugin_data->buf[1][1],&sampleR);
	  computeFilter(plugin_data->filter[2], &plugin_data->buf[2][1],&sampleR);
	  computeFilter(plugin_data->filter[3], &plugin_data->buf[3][1],&sampleR);
	  #endif
	  
	  #if NUM_BANDS >= 6
	  computeFilter(plugin_data->filter[4], &plugin_data->buf[4][1],&sampleR);
	  computeFilter(plugin_data->filter[5], &plugin_data->buf[5][1],&sampleR);
	  #endif
	  
	  #if NUM_BANDS ==10
	  computeFilter(plugin_data->filter[6], &plugin_data->buf[6][1],&sampleR);
	  computeFilter(plugin_data->filter[7], &plugin_data->buf[7][1],&sampleR);
	  computeFilter(plugin_data->filter[8], &plugin_data->buf[8][1],&sampleR);
	  computeFilter(plugin_data->filter[9], &plugin_data->buf[9][1],&sampleR);
	  #endif
	
	#endif
           
      //Apply gain to the sample to move it to de-normalized state
      //sampleL *= EQ_OUTPUT_GAIN; ///TODO Testing lattice
      //The output amplifier
      sampleL *= fOutGain; 
      //Update VU output sample
      SetSample(plugin_data->OutputVu[0], sampleL);
      
      #if NUM_CHANNELS == 2
      //Apply gain to the sample to move it to de-normalized state
      //sampleR *= EQ_OUTPUT_GAIN;///TODO Testing lattice
      //The output amplifier
      sampleR *= fOutGain;
      //Update VU output sample
      SetSample(plugin_data->OutputVu[1], sampleR);
      #endif
    }

    //Write on output
    plugin_data->fOutput[0][pos] = sampleL;
    #if NUM_CHANNELS == 2
    plugin_data->fOutput[1][pos] = sampleR;
    #endif
  }
  
  //Update VU ports
  *(plugin_data->fVuIn[0]) = ComputeVu(plugin_data->InputVu[0], sample_count);
  *(plugin_data->fVuOut[0]) = ComputeVu(plugin_data->OutputVu[0], sample_count);
  #if NUM_CHANNELS == 2
  *(plugin_data->fVuIn[1]) = ComputeVu(plugin_data->InputVu[1], sample_count);
  *(plugin_data->fVuOut[1]) = ComputeVu(plugin_data->OutputVu[1], sample_count);
  #endif
  
  ///TEST TODO REMOVE THIS TEST
  //Monitor Buffers
  //#if NUM_CHANNELS == 2
  //printf("DF_BUF: %f\t%f\r\n", plugin_data->buf[0][1].buf_0, plugin_data->buf[0][1].buf_1);
  //printf("LAT_BUF: %f\t%f\r\n", plugin_data->buf[0][0].buf_0, plugin_data->buf[0][0].buf_1);
  //#endif
      
}

static void init()
{
  eqDescriptor = (LV2_Descriptor *)malloc(sizeof(LV2_Descriptor));

  eqDescriptor->URI = EQ_URI;
  eqDescriptor->activate = NULL;
  eqDescriptor->cleanup = cleanupEQ;
  eqDescriptor->connect_port = connectPortEQ;
  eqDescriptor->deactivate = NULL;
  eqDescriptor->instantiate = instantiateEQ;
  eqDescriptor->run = runEQ_v2;
  eqDescriptor->extension_data = NULL;
}

LV2_SYMBOL_EXPORT
const LV2_Descriptor *lv2_descriptor(uint32_t index)
{
  if (!eqDescriptor) init();

  switch (index) {
  case 0:
    return eqDescriptor;
  default:
    return NULL;
  }
}
