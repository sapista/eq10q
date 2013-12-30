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
#include <math.h>

#include "lv2.h"
#include "dsp/db.h"
#include "dsp/vu.h"
#include "dsp/filter.h"

#define GATE_URI "http://eq10q.sourceforge.net/gate"
#define PORT_OUTPUT 0
#define PORT_INPUT 1
#define PORT_KEY_LISTEN 2
#define PORT_THRESHOLD 3
#define PORT_ATACK 4
#define PORT_HOLD 5
#define PORT_DECAY 6
#define PORT_RANGE 7
#define PORT_HPFFREQ 8
#define PORT_LPFFREQ 9
#define PORT_GAIN 10
#define PORT_INVU 11
#define PORT_GAINREDUCTION 12

#define FILTER_INPUT_GAIN 10000.0
#define FILTER_OUTPUT_GAIN 0.0001

static LV2_Descriptor *gateDescriptor = NULL;

typedef struct {
  //Plugin ports
  float *key_listen;
  float *threshold;
  float *attack;
  float *hold;
  float *decay;
  float *range;
  float *output;
  float *gainreduction;
  const float *input;
  float *hpffreq;
  float *lpffreq;
  float *ingain;
  float *fVuIn;
  
  //Plugin Internal data
  float sample_rate;
  float g;
  int hold_count;
  Vu *InputVu[1];
  float noise;
  Filter *LPF_fil, *HPF_fil;
  Buffers LPF_buf, HPF_buf;
} Gate;

static void cleanupGate(LV2_Handle instance)
{
  Gate *plugin = (Gate *)instance;
  VuClean(plugin->InputVu[0]);
  FilterClean(plugin->HPF_fil);
  FilterClean(plugin->LPF_fil);
  free(instance);
}

static void connectPortGate(LV2_Handle instance, uint32_t port, void *data)
{
  Gate *plugin = (Gate *)instance;
  switch (port) {
    case PORT_KEY_LISTEN:
	    plugin->key_listen = (float*)data;
	    break;
    case PORT_THRESHOLD:
	    plugin->threshold = (float*)data;
	    break;
    case PORT_ATACK:
	    plugin->attack = (float*)data;
	    break;
    case PORT_HOLD:
	    plugin->hold = (float*)data;
	    break;
    case PORT_DECAY:
	    plugin->decay = (float*)data;
	    break;
    case PORT_RANGE:
	    plugin->range = (float*)data;
	    break;
    case PORT_INPUT:
	    plugin->input = (const float*)data;
	    break;
    case PORT_OUTPUT:
	    plugin->output = (float*)data;
	    break;
    case PORT_GAINREDUCTION:
	    plugin->gainreduction = (float*)data;
	    break;
    case PORT_HPFFREQ:
	    plugin->hpffreq = (float*)data;
	    break;
    case PORT_LPFFREQ:
	    plugin->lpffreq = (float*)data;
	    break;
    case PORT_GAIN:
	    plugin->ingain = (float*)data;
	    break;
    case PORT_INVU:
	    plugin->fVuIn=(float*)data;
	    break;
  } 
}

static LV2_Handle instantiateGate(const LV2_Descriptor *descriptor, double s_rate, const char *path, const LV2_Feature *const * features)
{
  Gate *plugin_data = (Gate *)malloc(sizeof(Gate));  
  plugin_data->sample_rate = s_rate;
  plugin_data->hold_count = 1000000; //TODO set to max int
  plugin_data->g = 0.0f;
  plugin_data->InputVu[0] = VuInit(s_rate);
  plugin_data->noise = 0.01; //the noise to get the GR VU workin in GUI
  plugin_data->HPF_fil = FilterInit(s_rate);
  plugin_data->LPF_fil = FilterInit(s_rate);
  flushBuffers(&plugin_data->LPF_buf);
  flushBuffers(&plugin_data->HPF_buf);
  return (LV2_Handle)plugin_data;
}

#define DENORMAL_TO_ZERO(x) if (fabs(x) < (1e-30)) x = 0.f; //Min float is 1.1754943e-38
static void runGate(LV2_Handle instance, uint32_t sample_count)
{
  Gate *plugin_data = (Gate *)instance;
  
  //Read ports
  float * const output = plugin_data->output;
  const float * const input = plugin_data->input;
  const float threshold = pow(10, *(plugin_data->threshold) * 0.05);
  const float attack = *(plugin_data->attack);
  const float hold = *(plugin_data->hold);
  const float decay = *(plugin_data->decay);
  const float range = *(plugin_data->range);
  const float hpffreq = *(plugin_data->hpffreq);
  const float lpffreq = *(plugin_data->lpffreq);
  const float KeyListen = *(plugin_data->key_listen);
  const float InputGain = dB2Lin(*(plugin_data->ingain));
  
  //Plguin data
  float sample_rate = plugin_data->sample_rate;
  float g = plugin_data->g;
  int hold_count = plugin_data->hold_count;

  //Processor vars
  const float range_lin = pow(10, range * 0.05); //Range constant
  const float ac = exp(-3.0f/(attack * sample_rate * 0.001f)); //Attack constant
  const float dc = exp(-3.0f/(decay * sample_rate * 0.001f)); //Decay constant
  const int hold_max = (int)round((attack + hold) * sample_rate * 0.001f);
  float gain_reduction = 0.0f;
  
  //Compute filter coeficients
  if(hpffreq != plugin_data->HPF_fil->freq)
  {
    calcCoefs(plugin_data->HPF_fil, 0.0, hpffreq, 0.75, F_HPF_ORDER_2, 1.0);
  }
  if(lpffreq != plugin_data->LPF_fil->freq)
  {
    calcCoefs(plugin_data->LPF_fil, 0.0, lpffreq, 0.75, F_LPF_ORDER_2, 1.0);
  }

  float input_filtered, input_detector;
  for (uint32_t i = 0; i < sample_count; ++i) 
  {

    // Counting input dB
    input_filtered = input[i] * InputGain;
    
    //Sample to Input Vumeter
    SetSample(plugin_data->InputVu[0], input_filtered);

    //Apply Filters
    input_filtered*= FILTER_INPUT_GAIN;
    computeFilter(plugin_data->LPF_fil, &plugin_data->LPF_buf, &input_filtered);
    computeFilter(plugin_data->HPF_fil, &plugin_data->HPF_buf, &input_filtered);
    input_filtered*=FILTER_OUTPUT_GAIN;
    
    input_detector = fabs(input_filtered);
    
    //Threshold
    hold_count = input_detector > threshold ? 0 : hold_count;
    
    if(hold_count < hold_max)
    {
      //Gate Open
      g = 1-(1-g)*ac;
      hold_count++;
    }
    else
    {
      //Gate Close
      g = g*dc;
    }
    DENORMAL_TO_ZERO(g);
    
    gain_reduction = range_lin * (1 - g) + g;
    output[i] = input_filtered*(KeyListen) + input[i]*gain_reduction*(1-KeyListen);

  }
  plugin_data->g = g;
  plugin_data->hold_count = hold_count;
  plugin_data->noise *= -1.0;
  *(plugin_data->gainreduction) = 1.0/gain_reduction + plugin_data->noise; // + ((float)(rand() % 100)/100.0);; //OK esta en lineal
  *(plugin_data->fVuIn) = ComputeVu(plugin_data->InputVu[0], sample_count);
}

static void init()
{
  gateDescriptor = (LV2_Descriptor *)malloc(sizeof(LV2_Descriptor));

  gateDescriptor->URI = GATE_URI;
  gateDescriptor->activate = NULL;
  gateDescriptor->cleanup = cleanupGate;
  gateDescriptor->connect_port = connectPortGate;
  gateDescriptor->deactivate = NULL;
  gateDescriptor->instantiate = instantiateGate;
  gateDescriptor->run = runGate;
  gateDescriptor->extension_data = NULL;
}

LV2_SYMBOL_EXPORT
const LV2_Descriptor *lv2_descriptor(uint32_t index)
{
  if (!gateDescriptor) init();

  switch (index) {
  case 0:
    return gateDescriptor;
  default:
    return NULL;
  }
}
