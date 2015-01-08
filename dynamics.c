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
This file is the implementation of the DYN plugin
This plugin is inside the Sapista Plugins Bundle
This file implements functionalities for diferent dynamic plugins
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lv2.h"
#include "dsp/db.h"
#include "dsp/fastmath.h"
#include "dsp/vu.h"
#include "dsp/filter.h"

#define @Plugin_Is_Dynamics_Compressor@
#define USE_EQ10Q_FAST_MATH
#define NUM_CHANNELS @Dyn_Channels_Count@


#define DYN_URI @Dyn_Uri@
#define PORT_OUTPUT_L 0
#define PORT_INPUT_L 1
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
#define PORT_KNEE 13

#ifdef PLUGIN_IS_COMPRESSOR
#define PORT_OUTPUT_R 14
#define PORT_INPUT_R 15
#else
#define PORT_OUTPUT_R 13
#define PORT_INPUT_R 14
#endif

typedef struct {
  //Plugin ports
  float *key_listen;
  float *threshold;
  float *attack;
  float *hold_makeup; //Hold for gate makeup for compressor/expander
  float *decay;
  float *range_ratio; //range for gate ratio for compressor/expander
  float *output[NUM_CHANNELS];
  float *gainreduction; 
  const float *input[NUM_CHANNELS];
  float *hpffreq;
  float *lpffreq;
  float *ingain;
  float *fVuIn;
  #ifdef PLUGIN_IS_COMPRESSOR
  float *knee;
  #endif
  
  //Plugin Internal data
  float sample_rate;
  float g;
  int hold_count;
  Vu *InputVu[1];
  float noise;
  Filter *LPF_fil, *HPF_fil;
  Buffers LPF_buf, HPF_buf;
  float *LutLog10;
} Dynamics;

static void cleanupDyn(LV2_Handle instance)
{
  Dynamics *plugin = (Dynamics *)instance;
  free(plugin->LutLog10);
  VuClean(plugin->InputVu[0]);
  FilterClean(plugin->HPF_fil);
  FilterClean(plugin->LPF_fil);
  free(instance);
}

static void connectPortDyn(LV2_Handle instance, uint32_t port, void *data)
{
  Dynamics *plugin = (Dynamics *)instance;
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
	    plugin->hold_makeup = (float*)data;
	    break;
    case PORT_DECAY:
	    plugin->decay = (float*)data;
	    break;
    case PORT_RANGE:
	    plugin->range_ratio = (float*)data;
	    break;
    case PORT_INPUT_L:
	    plugin->input[0] = (const float*)data;
	    break;
    case PORT_OUTPUT_L:
	    plugin->output[0] = (float*)data;
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

    #ifdef PLUGIN_IS_COMPRESSOR
    case PORT_KNEE:
	    plugin->knee = (float*)data;
	    break;
    #endif
	    
    #if NUM_CHANNELS == 2
    case PORT_INPUT_R:
	    plugin->input[1] = (const float*)data;
	    break;
    case PORT_OUTPUT_R:
	    plugin->output[1] = (float*)data;
	    break;
    #endif
  } 
}

static LV2_Handle instantiateDyn(const LV2_Descriptor *descriptor, double s_rate, const char *path, const LV2_Feature *const * features)
{
  Dynamics *plugin_data = (Dynamics *)malloc(sizeof(Dynamics));  
  plugin_data->LutLog10 = GenerateLog10LUT();
  plugin_data->sample_rate = s_rate;
  plugin_data->hold_count = 1000000;
  plugin_data->g = 0.0f;
  plugin_data->InputVu[0] = VuInit(s_rate);
  plugin_data->noise = 0.01; //the noise to get the GR VU workin in GUI
  plugin_data->HPF_fil = FilterInit(s_rate);
  plugin_data->LPF_fil = FilterInit(s_rate);
  flushBuffers(&plugin_data->LPF_buf);
  flushBuffers(&plugin_data->HPF_buf);
  return (LV2_Handle)plugin_data;
}

#define DENORMAL_TO_ZERO_FLOAT(x) if (fabs(x) < (1e-30)) x = 0.f; //Min float is 1.1754943e-38
static void runDyn(LV2_Handle instance, uint32_t sample_count)
{
  Dynamics *plugin_data = (Dynamics *)instance;
   
  const float attack = *(plugin_data->attack);
  const float decay = *(plugin_data->decay);
  const float hpffreq = *(plugin_data->hpffreq);
  const float lpffreq = *(plugin_data->lpffreq);
  const float KeyListen = *(plugin_data->key_listen);
  const float InputGain = dB2Lin(*(plugin_data->ingain));
  float gr_meter = 1.0f;
  
  //Read ports (gate)
  #ifdef PLUGIN_IS_GATE
  float input_detector;
  const float range = *(plugin_data->range_ratio);
  const float hold = *(plugin_data->hold_makeup);
  const float threshold = Fast_dB2Lin10(*(plugin_data->threshold));
  #endif
  
  //Read ports (compressor/expander)
  #ifdef PLUGIN_IS_COMPRESSOR
  const float threshold = *(plugin_data->threshold);
  const float ratio =  *(plugin_data->range_ratio);
  const float makeup = dB2Lin(*(plugin_data->hold_makeup));
  const float knee = *(plugin_data->knee);
  #endif
  
  //Plguin data
  float sample_rate = plugin_data->sample_rate;
  float g = plugin_data->g;
  int hold_count = plugin_data->hold_count;

  //Processor vars (only for gate)
  #ifdef PLUGIN_IS_GATE
  const float range_lin = pow(10, range * 0.05); //Range constant, I can not use Fast_dB2Lin10 because the Taylor aprox starts clipping at -67 dB and range is -90 dB to 0 dB
  const int hold_max = (int)round(hold * sample_rate * 0.001f);
  #endif
  
  //Processor vars (only COMPRESSOR)
  #ifdef PLUGIN_IS_COMPRESSOR
  float x_dB, y_dB;
  float knee_range;
  #endif
  
  //Processor vars  common
  const float ac = exp(-6.0f/(attack * sample_rate * 0.001f)); //Attack constant
  const float dc = exp(-1.0f/(decay * sample_rate * 0.001f)); //Decay constant

  float gain_reduction = 0.0f;
  float input_filtered = 0.0f;
  double dToFiltersChain = 0.0;
  float input_preL;
  #if NUM_CHANNELS == 2
  float input_preR;
  #endif
  for (uint32_t i = 0; i < sample_count; ++i) 
  {
    //Compute filter coeficients
    if(hpffreq != plugin_data->HPF_fil->freq)
    {
      calcCoefs(plugin_data->HPF_fil, 0.0, hpffreq, 0.75, F_HPF_ORDER_2, 1.0);
    }
    if(lpffreq != plugin_data->LPF_fil->freq)
    {
      calcCoefs(plugin_data->LPF_fil, 0.0, lpffreq, 0.75, F_LPF_ORDER_2, 1.0);
    }

    //Input gain
    input_preL =  plugin_data->input[0][i] * InputGain;
    dToFiltersChain = (double)input_preL;
    #if NUM_CHANNELS == 2
    input_preR = plugin_data->input[1][i] * InputGain;
    dToFiltersChain = (double)(input_preL + input_preR)*0.5f;
    #endif
    
    //Sample to Input Vumeter
    SetSample(plugin_data->InputVu[0], (float)dToFiltersChain);   

    //Apply Filters
    computeFilter(plugin_data->LPF_fil, &plugin_data->LPF_buf, &dToFiltersChain);
    computeFilter(plugin_data->HPF_fil, &plugin_data->HPF_buf, &dToFiltersChain);
    input_filtered = (float)dToFiltersChain;
    
    //===================== GATE CODE ================================
    #ifdef PLUGIN_IS_GATE
    input_detector = fabs(input_filtered); //This is the detector signal filtered
    //Threshold
    hold_count = input_detector > threshold ? 0 : hold_count;
    if(hold_count < hold_max)
    {
      //Dynamics Open
      g = 1-(1-g)*ac;
      hold_count++;
    }
    else
    {
      //Dynamics Close
      g = g*dc;
    }
    DENORMAL_TO_ZERO_FLOAT(g);
    gain_reduction = range_lin * (1 - g) + g;
    gr_meter=gain_reduction < gr_meter ? gain_reduction : gr_meter;
    #endif
    //===================== END OF GATE CODE =========================
    
    //=================== COMPRESSOR CODE ============================
    #ifdef PLUGIN_IS_COMPRESSOR   
       
    //Thresholding and gain computer
#ifdef USE_EQ10Q_FAST_MATH
    x_dB = 20.0f * fastLog10((int*)(&input_filtered), plugin_data->LutLog10);	
#else
    x_dB = 20.0f*log10(fabs(input_filtered) + 0.00001f); //Add -100dB constant to avoid zero crozing
#endif
    knee_range = 2.0f*(x_dB - threshold);
    
    if (knee_range < -knee)
    {
      //Under Threshold
      y_dB = x_dB; 
    }
    else if(knee_range > knee )
    {
      //Over Threshold
      y_dB = threshold + (x_dB - threshold)/ratio; 
    }
    else
    {
      //On Knee
      y_dB = x_dB + ((1.0f/ratio -1.0f)*(x_dB - threshold + knee/2.0f)*(x_dB - threshold + knee/2.0f))/(2.0f*knee);
    }

    //Linear gain computing
#ifdef USE_EQ10Q_FAST_MATH
    gain_reduction = Fast_dB2Lin8(y_dB - x_dB);
#else
    gain_reduction = pow(10.0f, 0.05f*(y_dB - x_dB));
#endif
        
    //Ballistics and peak detector
    if(gain_reduction > g)
    {
      //Compressor OFF
      gain_reduction = 1.0f - (1.0f - g)*dc;
    }
    else
    {
      //Compressor ON
      gain_reduction = gain_reduction  - (gain_reduction - g)*ac;
    }
    g = gain_reduction;
    gr_meter = gain_reduction < gr_meter ? gain_reduction : gr_meter;
    gain_reduction *= makeup;    
    #endif
    //=================== END OF COMPRESSOR CODE ======================
    
    plugin_data->output[0][i] = input_filtered*(KeyListen) + input_preL*gain_reduction*(1-KeyListen);
    #if NUM_CHANNELS == 2
    plugin_data->output[1][i] = input_filtered*(KeyListen) + input_preR*gain_reduction*(1-KeyListen);
    #endif

  }
  plugin_data->g = g;
  plugin_data->hold_count = hold_count;
  plugin_data->noise *= -1.0;
  *(plugin_data->gainreduction) = 1.0/gr_meter + plugin_data->noise; // + ((float)(rand() % 100)/100.0);; //OK esta en lineal
  *(plugin_data->fVuIn) = ComputeVu(plugin_data->InputVu[0], sample_count);
}

static const LV2_Descriptor dynDescriptor = {
  DYN_URI,
  instantiateDyn,
  connectPortDyn,
  NULL,
  runDyn,
  NULL,
  cleanupDyn,
  NULL
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor *lv2_descriptor(uint32_t index)
{
  switch (index) {
  case 0:
    return &dynDescriptor;
  default:
    return NULL;
  }
}
