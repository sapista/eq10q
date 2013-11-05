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

//#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lv2.h"
#include "dsp/db.h"

#define GATE_URI "http://eq10q.sourceforge.net/gate"
#define PORT_OUTPUT 0
#define PORT_INPUT 1
#define PORT_KEY_LISTEN 2
#define PORT_THRESHOLD 3
#define PORT_ATACK 4
#define PORT_HOLD 5
#define PORT_DECAY 6
#define PORT_RANGE 7
#define PORT_GAINREDUCTION 8
#define PORT_HPFFREQ 9
#define PORT_LPFFREQ 10

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
  
  //Plugin Internal data
  float sample_rate;
  float g;
  int hold_count;
} Gate;

static void cleanupGate(LV2_Handle instance)
{
  //TODO si uses Vumeters hauras d'usar els corresponents detructors
  //Gate *plugin = (Gate *)instance;
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
    case PORT_HPFFREQ:
	    plugin->hpffreq = (float*)data;
      break;
    case PORT_LPFFREQ:
	    plugin->lpffreq = (float*)data;
      break;
  } 
}

static LV2_Handle instantiateGate(const LV2_Descriptor *descriptor, double s_rate, const char *path, const LV2_Feature *const * features)
{
  Gate *plugin_data = (Gate *)malloc(sizeof(Gate));  
  plugin_data->sample_rate = s_rate;
  plugin_data->hold_count = 1000000; //TODO set to max int
  plugin_data->g = 0.0f;
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

  float input_filtered;
  for (uint32_t i = 0; i < sample_count; ++i) 
  {

    // Counting input dB
    input_filtered = input[i];

    //Apply Filters
    //TODO
    
    input_filtered = fabs(input_filtered);
    
    //Threshold
    hold_count = input_filtered > threshold ? 0 : hold_count;
    
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
    *plugin_data->gainreduction = -20.0f*log10(gain_reduction); //TODO deixu en lineal kuan montirs GUI
    //TODO afegir un VU meter per nivell entrada
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
