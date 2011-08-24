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
#include "eqparams.h"
#include <stdlib.h>
#include <string>
#include <slv2/world.h>
#include <slv2/plugin.h>
#include <slv2/collections.h>
#include <slv2/value.h>
#include <slv2/types.h>

EqParams::EqParams(int iNumBands):
m_iNumberOfBands(iNumBands)
{
  m_ptr_BandArray = (EqBandStruct*)malloc(sizeof(EqBandStruct) * m_iNumberOfBands);
}

EqParams::~EqParams()
{
  free(m_ptr_BandArray);
}

bool EqParams::getBandEnabled(int iBand)
{
  return m_ptr_BandArray[iBand].bIsEnabled;
}

float EqParams::getBandFreq(int iBand)
{
  return m_ptr_BandArray[iBand].fFreq;
}

float EqParams::getBandGain(int iBand)
{
  return m_ptr_BandArray[iBand].fGain;
}

float EqParams::getBandQ(int iBand)
{
  return m_ptr_BandArray[iBand].fQ;
}

int EqParams::getBandType(int iBand)
{
  return m_ptr_BandArray[iBand].iType;
}

float EqParams::getInputGain()
{
  return m_fInGain;
}

float EqParams::getOutputGain()
{
  return m_fOutGain;
}

void EqParams::setBandEnabled(int iBand, bool bIsEnabled)
{
  m_ptr_BandArray[iBand].bIsEnabled = bIsEnabled;
}

void EqParams::setBandFreq(int iBand, float fFreq)
{
  m_ptr_BandArray[iBand].fFreq = fFreq;
}

void EqParams::setBandGain(int iBand, float fGain)
{
  m_ptr_BandArray[iBand].fGain = fGain;
}

void EqParams::setBandQ(int iBand, float fQ)
{
  m_ptr_BandArray[iBand].fQ = fQ;
}

void EqParams::setBandType(int iBand, int iType)
{
  m_ptr_BandArray[iBand].iType = iType;
}

void EqParams::setInputGain(float fInGain)
{
  m_fInGain = fInGain;
}

void EqParams::setOutputGain(float fOutGain)
{
  m_fOutGain = fOutGain;
}

void EqParams::loadFromTtlFile(const char *uri)
{
  //Load from ttl
  SLV2World lv2World = slv2_world_new();
  slv2_world_load_all(lv2World);
  SLV2Plugins lv2Plugins = slv2_world_get_all_plugins(lv2World);
  SLV2Plugin lv2ThisPlugin = slv2_plugins_get_by_uri(lv2Plugins, slv2_value_new_uri(lv2World, uri));
  
  //Load Data from plugin
  SLV2Value lv2Symbol;
  SLV2Value min, max, def;
  SLV2Port port;

  //Get InputGain
  lv2Symbol = slv2_value_new_string(lv2World, "input_gain");
  port = slv2_plugin_get_port_by_symbol(lv2ThisPlugin, lv2Symbol);
  slv2_port_get_range(lv2ThisPlugin, port, &def, &min, &max);
  m_fInGain = slv2_value_as_float(def);

  //Get OutputGain
  lv2Symbol = slv2_value_new_string(lv2World, "output_gain");
  port = slv2_plugin_get_port_by_symbol(lv2ThisPlugin, lv2Symbol);
  slv2_port_get_range(lv2ThisPlugin, port, &def, &min, &max);
  m_fOutGain = slv2_value_as_float(def);

  std::string sSymbol; 
  char buffer[50];
  for(int i = 0; i < m_iNumberOfBands; i ++)
  {
    //Get Band Gain
    sprintf (buffer, "filter%d_gain", i+1);
    sSymbol = buffer;
    lv2Symbol = slv2_value_new_string(lv2World, sSymbol.c_str());
    port = slv2_plugin_get_port_by_symbol(lv2ThisPlugin, lv2Symbol);
    slv2_port_get_range(lv2ThisPlugin, port, &def, &min, &max);
    m_ptr_BandArray[i].fGain = slv2_value_as_float(def);
    
    //Get Band Freq
    sprintf (buffer, "filter%d_freq", i+1);
    sSymbol = buffer;
    lv2Symbol = slv2_value_new_string(lv2World, sSymbol.c_str());
    port = slv2_plugin_get_port_by_symbol(lv2ThisPlugin, lv2Symbol);
    slv2_port_get_range(lv2ThisPlugin, port, &def, &min, &max);
    m_ptr_BandArray[i].fFreq = slv2_value_as_float(def);
    
    //Get Band Q
    sprintf (buffer, "filter%d_q", i+1);
    sSymbol = buffer;
    lv2Symbol = slv2_value_new_string(lv2World, sSymbol.c_str());
    port = slv2_plugin_get_port_by_symbol(lv2ThisPlugin, lv2Symbol);
    slv2_port_get_range(lv2ThisPlugin, port, &def, &min, &max);
    m_ptr_BandArray[i].fQ = slv2_value_as_float(def);
    
    //Get Band Type
    sprintf (buffer, "filter%d_type", i+1);
    sSymbol = buffer;
    lv2Symbol = slv2_value_new_string(lv2World, sSymbol.c_str());
    port = slv2_plugin_get_port_by_symbol(lv2ThisPlugin, lv2Symbol);
    slv2_port_get_range(lv2ThisPlugin, port, &def, &min, &max);
    m_ptr_BandArray[i].iType = (int) slv2_value_as_float(def);
    
    //Get Band Enabled
    sprintf (buffer, "filter%d_enable", i+1);
    sSymbol = buffer;
    lv2Symbol = slv2_value_new_string(lv2World, sSymbol.c_str());
    port = slv2_plugin_get_port_by_symbol(lv2ThisPlugin, lv2Symbol);
    slv2_port_get_range(lv2ThisPlugin, port, &def, &min, &max);
    m_ptr_BandArray[i].bIsEnabled = slv2_value_as_float(def) > 0.5;
  }
    
  //Free all
  slv2_value_free(lv2Symbol);
  slv2_value_free(min);
  slv2_value_free(max);
  slv2_value_free(def);
  slv2_plugins_free(lv2World, lv2Plugins);
  slv2_world_free(lv2World);
}

