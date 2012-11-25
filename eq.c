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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <math.h>


#include "lv2.h"
#include "gui/eq_defines.h"
#include "dsp/smooth.h"
#include "dsp/vu.h"
#include "dsp/db.h"

//Data from CMake
#define NUM_BANDS @Eq_Bands_Count@
#define NUM_CHANNELS @Eq_Channels_Count@
#define EQ_URI @Eq_Uri@

//Conditional inclusion of filter.h
#if NUM_CHANNELS == 1
  #include "dsp/filter_mono.h"
#else
  #include "dsp/filter_stereo.h"
#endif

#define EQ_INPUT_GAIN 10000.0
#define EQ_OUTPUT_GAIN 0.0001

//LV2 Control port polling period in seconds
#define PORT_POLLING_PERIOD 0.05

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
  Smooth *smooth[NUM_BANDS][3]; //3 smooth per cada banda (Gain, Freq, Q)
  Vu *InputVu[NUM_CHANNELS];
  Vu *OutputVu[NUM_CHANNELS];
 
  int sample_count;
  int port_polling_samples;
  
} EQ;

static void cleanupEQ(LV2_Handle instance)
{
  ///printf("Entinring cleanupEQ...\n\r");
  EQ *plugin = (EQ *)instance;
  int i,j;
  for(i=0; i<NUM_BANDS; i++)
  {
    FilterClean(plugin->filter[i]);
    for(j=0;j<3;j++) SmoothClean(plugin->smooth[i][j]);
  }

  for(i=0; i<NUM_CHANNELS; i++)
  {
    VuClean(plugin->InputVu[i]);
    VuClean(plugin->OutputVu[i]);
  }
  free(instance);
  ///printf("cleanupEQ Return\n\r");
}

static void connectPortEQ(LV2_Handle instance, uint32_t port, void *data)
{
  ///printf("Entinring connectPortEQ...\n\r");
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
  ///printf("connectPortEQ Return\n\r");
}

static LV2_Handle instantiateEQ(const LV2_Descriptor *descriptor, double s_rate, const char *path, const LV2_Feature *const * features)
{
  ///printf("Entinring instantiateEQ...\n\r"); 

  EQ *plugin_data = (EQ *)malloc(sizeof(EQ));
  ///printf("Malloc return OK");
  
  //Prepare polling interval
  plugin_data->sample_count = 0;
  plugin_data->port_polling_samples = (int)(s_rate*PORT_POLLING_PERIOD);
  
  int i, j;
  for(i=0; i<NUM_BANDS; i++)
  {
    plugin_data->filter[i] = FilterInit(s_rate);
    
    for(j=0;j<3;j++) plugin_data->smooth[i][j] = SmoothInit(s_rate);
  }

  for(i=0; i<NUM_CHANNELS; i++)
  {
    plugin_data->InputVu[i] = VuInit(s_rate);
    plugin_data->OutputVu[i] = VuInit(s_rate);
  }
  
  ///printf("intantiateEQ Return\n\r");
  return (LV2_Handle)plugin_data;
}

static void runEQ(LV2_Handle instance, uint32_t sample_count)
{
  ///printf("Entinring runEQ...\n\r");
  EQ *plugin_data = (EQ *)instance;

  //Get values of control ports
  const int iBypass = *(plugin_data->fBypass) > 0.0f ? 1 : 0;
  
  const float fInGain = dB2Lin(*(plugin_data->fInGain));
  const float fOutGain = dB2Lin(*(plugin_data->fOutGain));

  float fBandGain[NUM_BANDS];
  float fBandFreq[NUM_BANDS];
  float fBandParam[NUM_BANDS];
  int   iBandType[NUM_BANDS];
  int   iBandEnabled[NUM_BANDS];

  int i, j, pos; //loop index
  float sample; //actual processing sample

/***
  //Filter parameters from ports
  plugin_data->sample_count += sample_count;
  if(plugin_data->sample_count > plugin_data->port_polling_samples)
  {
    plugin_data->sample_count = 0;
        
    for(i = 0; i<NUM_BANDS; i++)
    {
      //Smooth ///TODO Remove that
      
      //fBandGain[i] = computeSmooth(plugin_data->smooth[i][0], *(plugin_data->fBandGain[i]));
      //fBandFreq[i] = computeSmooth(plugin_data->smooth[i][1], *(plugin_data->fBandFreq[i]));
      //fBandParam[i] = computeSmooth(plugin_data->smooth[i][2], *(plugin_data->fBandParam[i]));
      
      
      //No Smooth
      fBandGain[i] = *(plugin_data->fBandGain[i]);
      fBandFreq[i] = *(plugin_data->fBandFreq[i]);
      fBandParam[i] = *(plugin_data->fBandParam[i]);

      iBandType[i] = (int)(*(plugin_data->fBandType[i]));
      iBandEnabled[i] = (int)(*(plugin_data->fBandEnabled[i]));
      if(checkBandChange(plugin_data->filter[i], fBandGain[i], fBandFreq[i], fBandParam[i], iBandType[i], iBandEnabled[i]))
      {
        setFilterParams(plugin_data->filter[i], fBandGain[i], fBandFreq[i], fBandParam[i], iBandType[i], iBandEnabled[i]);
        calcCoefs(plugin_data->filter[i]);
      }

    }
  }
  ******/

  //STEP2: Compute the filter
  for (pos = 0; pos < sample_count; pos++) 
  { 
    
    for(i = 0; i<NUM_BANDS; i++)
    {
      fBandGain[i] = computeSmooth(plugin_data->smooth[i][0], *(plugin_data->fBandGain[i]));
      fBandFreq[i] = computeSmooth(plugin_data->smooth[i][1], *(plugin_data->fBandFreq[i]));
      fBandParam[i] = computeSmooth(plugin_data->smooth[i][2], *(plugin_data->fBandParam[i]));
      iBandType[i] = (int)(*(plugin_data->fBandType[i]));
      iBandEnabled[i] = (int)(*(plugin_data->fBandEnabled[i]));
      if(checkBandChange(plugin_data->filter[i], fBandGain[i], fBandFreq[i], fBandParam[i], iBandType[i], iBandEnabled[i]))
      {
        setFilterParams(plugin_data->filter[i], fBandGain[i], fBandFreq[i], fBandParam[i], iBandType[i], iBandEnabled[i]);
        calcCoefs(plugin_data->filter[i]);
      }
    }  
    
    for(i = 0; i<NUM_CHANNELS; i++)
    { 
      //Get input
      sample = plugin_data->fInput[i][pos];
      
      //Process every band
      if(iBypass != 1)
      {
        //The input amplifier
        sample *= fInGain;

        //Update VU input sample
        SetSample(plugin_data->InputVu[i], sample);

	//Apply gain to the sample to move it to de-normalized state
	sample *= EQ_INPUT_GAIN;
        for(j = 0; j< NUM_BANDS; j++)
        {  
          computeFilter(plugin_data->filter[j], &sample, i);
        }
        //Apply gain to the sample to move it to de-normalized state
	sample *= EQ_OUTPUT_GAIN;
        
        //The output amplifier
        sample *= fOutGain;
        
        //Update VU output sample
        SetSample(plugin_data->OutputVu[i], sample);
      }
      
      else
      {
        resetVU(plugin_data->InputVu[i]);
        resetVU(plugin_data->OutputVu[i]);
      }
      
      //Write on output
      plugin_data->fOutput[i][pos] = sample;
    }
  }
  
  //Update VU ports
  for(i = 0; i<NUM_CHANNELS; i++)
  {
    *(plugin_data->fVuIn[i]) = ComputeVu(plugin_data->InputVu[i], sample_count);
    *(plugin_data->fVuOut[i]) = ComputeVu(plugin_data->OutputVu[i], sample_count);
  }
  
  
  ///printf("runEQ Return\n\r");
}

static void init()
{
  ///printf("Entinring init...\n\r");
  eqDescriptor = (LV2_Descriptor *)malloc(sizeof(LV2_Descriptor));

  eqDescriptor->URI = EQ_URI;
  eqDescriptor->activate = NULL;
  eqDescriptor->cleanup = cleanupEQ;
  eqDescriptor->connect_port = connectPortEQ;
  eqDescriptor->deactivate = NULL;
  eqDescriptor->instantiate = instantiateEQ;
  eqDescriptor->run = runEQ;
  eqDescriptor->extension_data = NULL;
  
  ///printf("init Return\n\r");
}

LV2_SYMBOL_EXPORT
const LV2_Descriptor *lv2_descriptor(uint32_t index)
{
  ///printf("Entinring lv2_descriptor...\n\r");
  if (!eqDescriptor) init();

  switch (index) {
  case 0:
    ///printf("lv2_descriptr Return OK\n\r");
    return eqDescriptor;
  default:
    ///printf("lv2_descriptr Return NULL\n\r");
    return NULL;
  }
}
