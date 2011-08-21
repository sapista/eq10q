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
  SLV2Value lv2Uri = slv2_value_new_uri(lv2World, uri);
  SLV2Plugins lv2Plugins = slv2_world_get_all_plugins(lv2World);
  
  //TODO: falla aki en carregar el plguin des de la URI s'obte null
  SLV2Plugin lv2ThisPlugin = slv2_plugins_get_by_uri(lv2Plugins, lv2Uri);
  
  //Load Data from plugin
  SLV2Value lv2Symbol;
  SLV2Value min, max, def;
  float fMin, fMax, fDef;
  SLV2Port port;
  
  lv2Symbol = slv2_value_new_string(lv2World, "input_gain");
  port = slv2_plugin_get_port_by_symbol(lv2ThisPlugin, lv2Symbol);
  slv2_port_get_range(lv2ThisPlugin, port, &def, &min, &max);
  fDef = slv2_value_as_float(def);
  fMin = slv2_value_as_float(min);
  fMax = slv2_value_as_float(max);
  
  //Free all
  slv2_value_free(lv2Uri);
  slv2_value_free(lv2Symbol);
  slv2_value_free(min);
  slv2_value_free(max);
  slv2_value_free(def);
  slv2_plugins_free(lv2World, lv2Plugins);
  slv2_world_free(lv2World);
}

