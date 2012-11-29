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
#define PORT_POLLING_PERIOD 0.001
#define SMOOTH_GAIN_STEP_PER_SECOND 1000
#define SMOOTH_FREQ_STEP_PER_SECOND 12000.0
#define SMOOTH_Q_STEP_PER_SECOND 500
#define SMOOTH_ON_STEP_PER_SECOND 10

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
  Smooth *smooth_Gain[NUM_BANDS];
  Smooth *smooth_Freq[NUM_BANDS];
  Smooth *smooth_Q[NUM_BANDS];
  Smooth *smooth_OnOff[NUM_BANDS];
  Vu *InputVu[NUM_CHANNELS];
  Vu *OutputVu[NUM_CHANNELS];
 
  int port_polling_samples;
  
} EQ;

static void cleanupEQ(LV2_Handle instance)
{
  ///printf("Entinring cleanupEQ...\n\r");
  EQ *plugin = (EQ *)instance;
  int i;
  for(i=0; i<NUM_BANDS; i++)
  {
    FilterClean(plugin->filter[i]);
    SmoothClean(plugin->smooth_Gain[i]);
    SmoothClean(plugin->smooth_Freq[i]);
    SmoothClean(plugin->smooth_Q[i]);
    SmoothClean(plugin->smooth_OnOff[i]);
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
  plugin_data->port_polling_samples = (int)(s_rate*PORT_POLLING_PERIOD);
  
  int i;
  for(i=0; i<NUM_BANDS; i++)
  {
    plugin_data->filter[i] = FilterInit(s_rate);
    plugin_data->smooth_Gain[i] = SmoothInit(1.0/PORT_POLLING_PERIOD, SMOOTH_GAIN_STEP_PER_SECOND);
    plugin_data->smooth_Freq[i] = SmoothInit(1.0/PORT_POLLING_PERIOD, SMOOTH_FREQ_STEP_PER_SECOND);
    plugin_data->smooth_Q[i] = SmoothInit(1.0/PORT_POLLING_PERIOD, SMOOTH_Q_STEP_PER_SECOND);
    plugin_data->smooth_OnOff[i] = SmoothInit(1.0/PORT_POLLING_PERIOD, SMOOTH_ON_STEP_PER_SECOND);
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
  float iBandEnabled[NUM_BANDS];

  int ch, bd, pos; //loop index
  float sample; //actual processing sample

  //Compute the filter
  for(ch = 0; ch<NUM_CHANNELS; ch++)
  { 
    for (pos = 0; pos < sample_count; pos++) 
    {    
      //Get input
      sample = plugin_data->fInput[ch][pos];
 
      //Process every band
      if(!iBypass)
      {
	//The input amplifier
	sample *= fInGain;

	//Update VU input sample
	SetSample(plugin_data->InputVu[ch], sample);

	//Apply gain to the sample to move it to de-normalized state
	sample *= EQ_INPUT_GAIN;

	//EQ PROCESSOR
	for(bd = 0; bd<NUM_BANDS; bd++)
	{
	  //Check Filter control ports
	  if( !(ch + pos%plugin_data->port_polling_samples))
	  {
	    fBandGain[bd] = computeSmooth(plugin_data->smooth_Gain[bd], dB2Lin(*(plugin_data->fBandGain[bd])));
	    fBandFreq[bd] = computeSmooth(plugin_data->smooth_Freq[bd], *(plugin_data->fBandFreq[bd]));
	    fBandParam[bd] = computeSmooth(plugin_data->smooth_Q[bd], *(plugin_data->fBandParam[bd]));
	    iBandEnabled[bd] = computeSmooth(plugin_data->smooth_OnOff[bd], *(plugin_data->fBandEnabled[bd]));
	    iBandType[bd] = (int)(*(plugin_data->fBandType[bd]));    
	    if(checkBandChange(plugin_data->filter[bd], fBandGain[bd], fBandFreq[bd], fBandParam[bd], iBandType[bd], iBandEnabled[bd]))
	    {
	      setFilterParams(plugin_data->filter[bd], fBandGain[bd], fBandFreq[bd], fBandParam[bd], iBandType[bd], iBandEnabled[bd]);
	      calcCoefs(plugin_data->filter[bd]);
	    }
	  }

	  //Comnpute filter
	  computeFilter(plugin_data->filter[bd], &sample, ch);
	}
	
	//Apply gain to the sample to move it to de-normalized state
	sample *= EQ_OUTPUT_GAIN;
	  
	//The output amplifier
	sample *= fOutGain;
	
	//Update VU output sample
	SetSample(plugin_data->OutputVu[ch], sample);
      }
      
      else
      {
	resetVU(plugin_data->InputVu[ch]);
	resetVU(plugin_data->OutputVu[ch]);
      }

	//Write on output
	plugin_data->fOutput[ch][pos] = sample;
	
    }
    
    //Update VU ports
    *(plugin_data->fVuIn[ch]) = ComputeVu(plugin_data->InputVu[ch], sample_count);
    *(plugin_data->fVuOut[ch]) = ComputeVu(plugin_data->OutputVu[ch], sample_count);
  }
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
