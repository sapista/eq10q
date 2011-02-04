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
This file tryies to implement functionalities for a large numbers of equalizers
****************************************************************************/

//Enable this for debugin mode
//#define IS_DEBUG_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#include <lv2.h>

#include "eq.h"
#include "dsp/filter.h"
#include "dsp/vu.h"

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
  Vu *InputVu[NUM_CHANNELS];
  Vu *OutputVu[NUM_CHANNELS];
} EQ;

static void cleanupEQ(LV2_Handle instance)
{
  EQ *plugin = (EQ *)instance;
  int i;
  for(i=0; i<NUM_BANDS; i++)
    FilterClean(plugin->filter[i]);
  for(i=0; i<NUM_CHANNELS; i++)
  {
    VuClean(plugin->InputVu[i]);
    VuClean(plugin->OutputVu[i]);
  }
  free(instance);
}

static void connectPortEQ(LV2_Handle instance, uint32_t port, void *data)
{
#ifdef IS_DEBUG_
  printf("Starting connecting port %d... \n\r", port);
#endif
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
#ifdef IS_DEBUG_
  printf("Starting EQ1Q Mono instantce...\n\r");
#endif
  EQ *plugin_data = (EQ *)malloc(sizeof(EQ));
  int i;
  for(i=0; i<NUM_BANDS; i++)
  {
    plugin_data->filter[i] = FilterInit( s_rate);
  }

  for(i=0; i<NUM_CHANNELS; i++)
  {
    plugin_data->InputVu[i] = VuInit(s_rate);
    plugin_data->OutputVu[i] = VuInit(s_rate);
  }
#ifdef IS_DEBUG_
  printf("EQ1Q Mono instantiated OK\n\r");
#endif
  return (LV2_Handle)plugin_data;
}

//Amplifier definition
#define DB_CO(g) ((g) > -90.0f ? powf(10.0f, (g) * 0.05f) : 0.0f)

static void runEQ(LV2_Handle instance, uint32_t sample_count)
{
#ifdef IS_DEBUG_
  printf("EQ1Q running... \n\r");
#endif
  EQ *plugin_data = (EQ *)instance;

  //Get values of control ports
  const int iBypass = *(plugin_data->fBypass) > 0.0f ? 1 : 0;
  const float fInGain = DB_CO(*(plugin_data->fInGain));
  const float fOutGain = DB_CO(*(plugin_data->fOutGain));

  float fBandGain[NUM_BANDS];
  float fBandFreq[NUM_BANDS];
  float fBandParam[NUM_BANDS];
  int   iBandType[NUM_BANDS];
  int   iBandEnabled[NUM_BANDS];

  int i, j, pos; //loop index
  float sample; //actual processing sample

#if IS_DBUS == 1
//Filter params from DBUS
///@TODO com fem l'adaptador a DBUS??? Realment cal aixo???

#else
//Filter parameters from ports
  if(NUM_BANDS > 0)
  {
    for(i = 0; i<NUM_BANDS; i++)
    {
      fBandGain[i] = *(plugin_data->fBandGain[i]);
      fBandFreq[i] = *(plugin_data->fBandFreq[i]);
      fBandParam[i] = *(plugin_data->fBandParam[i]);
      iBandType[i] = (int)(*(plugin_data->fBandType[i]));
      iBandEnabled[i] = (int)(*(plugin_data->fBandEnabled[i]));
printf("iBandEnabled = %d\n\r",iBandEnabled[i]);
printf("iBandType = %d\n\r",iBandType[i]);
      if(checkBandChange(plugin_data->filter[i], fBandGain[i], fBandFreq[i], fBandParam[i], iBandType[i], iBandEnabled[i]))
      {
        setFilterParams(plugin_data->filter[i], fBandGain[i], fBandFreq[i], fBandParam[i], iBandType[i], iBandEnabled[i]);
        calcCoefs(plugin_data->filter[i]);
      }

    }
  }
#endif

  //Get the audio input and output buffers
  float *fInput[NUM_CHANNELS];
  float *fOutput[NUM_CHANNELS];

  for(i = 0; i<NUM_CHANNELS; i++)
  {
    fInput[i] = plugin_data->fInput[i];
    fOutput[i] = plugin_data->fOutput[i];
  }

  //STEP2: Compute the filter

  for (pos = 0; pos < sample_count; pos++) 
  {
    for(i = 0; i<NUM_CHANNELS; i++)
    { 
      sample = fInput[i][pos];
    
      //Process every band
      if(iBypass != 1)
      {
        //The input amplifier
        sample *= fInGain;
        
        //Update VU input sample
        SetSample(plugin_data->InputVu[i], sample);
      
        for(j = 0; j< NUM_BANDS; j++)
        {
          sample = computeFilter(plugin_data->filter[j], sample);
        }
        
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
      fOutput[i][pos] = sample;
    }
  }
  
  //Update VU ports
  for(i = 0; i<NUM_CHANNELS; i++)
  {
    *(plugin_data->fVuIn[i]) = ComputeVu(plugin_data->InputVu[i]);
    *(plugin_data->fVuOut[i]) = ComputeVu(plugin_data->OutputVu[i]);
  }
}

static void init()
{
#ifdef IS_DEBUG_
  printf("EQ1Q Mono init()...\n\r");
#endif
  eqDescriptor = (LV2_Descriptor *)malloc(sizeof(LV2_Descriptor));

  eqDescriptor->URI = EQ_URI;
  eqDescriptor->activate = NULL;
  eqDescriptor->cleanup = cleanupEQ;
  eqDescriptor->connect_port = connectPortEQ;
  eqDescriptor->deactivate = NULL;
  eqDescriptor->instantiate = instantiateEQ;
  eqDescriptor->run = runEQ;
  eqDescriptor->extension_data = NULL;
#ifdef IS_DEBUG_
  printf("EQ1Q Mono init() OK\n\r");
#endif
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
