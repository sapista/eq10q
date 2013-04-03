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
#include "dsp/filter.h"


//Data from CMake
#define NUM_BANDS @Eq_Bands_Count@
#define NUM_CHANNELS @Eq_Channels_Count@
#define EQ_URI @Eq_Uri@

#define EQ_INPUT_GAIN 10000.0
#define EQ_OUTPUT_GAIN 0.0001

//LV2 Control port polling period in seconds TODO REMOVE!!!
#define PORT_POLLING_PERIOD 0.005
#define SMOOTH_GAIN_STEP_PER_SECOND 1000
#define SMOOTH_FREQ_STEP_PER_SECOND 12000.0
#define SMOOTH_Q_STEP_PER_SECOND 500
#define SMOOTH_ON_STEP_PER_SECOND 10
#include <sys/stat.h>

//LV2 Port polling new
#define PORT_POLL_SAMPLES 1024

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
  Smooth *smooth_Gain[NUM_BANDS];
  Smooth *smooth_Freq[NUM_BANDS];
  Smooth *smooth_Q[NUM_BANDS];
  Smooth *smooth_OnOff[NUM_BANDS];
  Vu *InputVu[NUM_CHANNELS];
  Vu *OutputVu[NUM_CHANNELS];
 
  //Port polling, new methode
  int port_samples;
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
  plugin_data->port_samples = 0;
  
  int i,ch;
  for(i=0; i<NUM_BANDS; i++)
  {
    plugin_data->filter[i] = FilterInit(s_rate);
    plugin_data->smooth_Gain[i] = SmoothInit(1.0/PORT_POLLING_PERIOD, SMOOTH_GAIN_STEP_PER_SECOND);
    plugin_data->smooth_Freq[i] = SmoothInit(1.0/PORT_POLLING_PERIOD, SMOOTH_FREQ_STEP_PER_SECOND);
    plugin_data->smooth_Q[i] = SmoothInit(1.0/PORT_POLLING_PERIOD, SMOOTH_Q_STEP_PER_SECOND);
    plugin_data->smooth_OnOff[i] = SmoothInit(1.0/PORT_POLLING_PERIOD, SMOOTH_ON_STEP_PER_SECOND);
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
  
  ///printf("intantiateEQ Return\n\r");
  return (LV2_Handle)plugin_data;
}

//New runEQ() methode, trying to reduce DSP LOAD
static void runEQ_v2(LV2_Handle instance, uint32_t sample_count)
{
  EQ *plugin_data = (EQ *)instance;

  //Get values of control ports
  const int iBypass = *(plugin_data->fBypass) > 0.0f ? 1 : 0;  
  const float fInGain = dB2Lin(*(plugin_data->fInGain));
  const float fOutGain = dB2Lin(*(plugin_data->fOutGain));
  static int bd, pos; //loop index
  static float  sampleL; //Current processing sample left signal
  #if NUM_CHANNELS == 2
  static float sampleR; //Current processing sampleright signal
  #endif
  

///TODO: He descobert ke fer copies de dades en plugin_data a variables locals es en realitat mes lent que  treballar directamente amb plugin_data
//TODO: de forma que mes et val revisar el polling de ports.... esta mal!!!! 

    //for ( bd = 0; bd<NUM_BANDS; bd++)
    //{
      ///TODO::: Aixo esta generant DENORMALS!!!!!!!!!
      //calcCoefs(plugin_data->filter[plugin_data->port_samples], 
	//	0.2f*dB2Lin(*(plugin_data->fBandGain[plugin_data->port_samples])) + 0.8f * plugin_data->filter[plugin_data->port_samples]->gain, 
	//	0.2f*(*(plugin_data->fBandFreq[plugin_data->port_samples])) + 0.8f * plugin_data->filter[plugin_data->port_samples]->freq, 
	//	0.2f*(*(plugin_data->fBandParam[plugin_data->port_samples])) + 0.8f * plugin_data->filter[plugin_data->port_samples]->q, 
	//	(int)(*(plugin_data->fBandType[plugin_data->port_samples])), 
	//	0.2f*(*(plugin_data->fBandEnabled[plugin_data->port_samples])) + 0.8f * plugin_data->filter[plugin_data->port_samples]->enable);
      bd = plugin_data->port_samples;
      calcCoefs(plugin_data->filter[plugin_data->port_samples],
		dB2Lin(*(plugin_data->fBandGain[plugin_data->port_samples])),
		*(plugin_data->fBandFreq[plugin_data->port_samples]),
		*(plugin_data->fBandParam[plugin_data->port_samples]),
		(int)(*(plugin_data->fBandType[plugin_data->port_samples])),
		*(plugin_data->fBandEnabled[plugin_data->port_samples]));
    //}
  
  //Process band change
  //bd++;
  //bd %= NUM_BANDS;
  //plugin_data->port_samples = bd;
  plugin_data->port_samples++;
  plugin_data->port_samples %= NUM_BANDS;
  
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
      sampleL *= EQ_INPUT_GAIN;
      
      #if NUM_CHANNELS == 2
      //The input amplifier
      sampleR *= fInGain;    
      //Update VU input sample
      SetSample(plugin_data->InputVu[1], sampleR);
      //Apply gain to the sample to move it to de-normalized state
      sampleR *= EQ_INPUT_GAIN;
      #endif
      
      //EQ PROCESSOR
      for(bd = 0; bd<NUM_BANDS; bd++)
      {
	computeFilter(plugin_data->filter[bd], &plugin_data->buf[bd][0],&sampleL);
	#if NUM_CHANNELS == 2
	computeFilter(plugin_data->filter[bd], &plugin_data->buf[bd][1],&sampleR);
	#endif
      }
           
      //Apply gain to the sample to move it to de-normalized state
      sampleL *= EQ_OUTPUT_GAIN;
      //The output amplifier
      sampleL *= fOutGain;
      //Update VU output sample
      SetSample(plugin_data->OutputVu[0], sampleL);
      
      #if NUM_CHANNELS == 2
      //Apply gain to the sample to move it to de-normalized state
      sampleR *= EQ_OUTPUT_GAIN;
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
  eqDescriptor->run = runEQ_v2;
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
