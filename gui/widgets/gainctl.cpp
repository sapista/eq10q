/***************************************************************************
 *   Copyright (C) 2009 by Pere Ràfols Soler                               *
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

#include "gainctl.h"
#include "guiconstants.h"

GainCtl::GainCtl(const Glib::ustring sTitle, int iNumOfChannel, double Fader_dBMax, double Fader_dBMin, const char* bundlePath):
m_iNumOfChannels(iNumOfChannel)
{

  set_label(sTitle);
  m_GainFader = Gtk::manage(new FaderWidget(Fader_dBMax, Fader_dBMin, bundlePath));
  m_VuMeter = Gtk::manage(new VUWidget(iNumOfChannel));
  m_HBox.pack_start(*m_GainFader);
  m_HBox.pack_start(*m_VuMeter);
  m_HBox.set_spacing(0);
  m_HBox.set_homogeneous(false);
  m_HBox.show();
  m_GainFader->show();
  m_VuMeter->show();
  add(m_HBox);
  show();
  
  m_GainFader->signal_changed().connect(sigc::mem_fun(*this, &GainCtl::onGainChanged));
}

void GainCtl::setGain(float fValue){
  m_GainFader->set_value((double) fValue);
}

void GainCtl::setVu(int channel, float fValue)
{
  m_VuMeter->setValue(channel, fValue);
}

float GainCtl::getGain(){
  return (float)m_GainFader->get_value();
}

GainCtl::signal_GainChanged GainCtl::signal_changed()
{
  return m_GainChangedSignal;
}

void GainCtl::onGainChanged()
{
  m_GainChangedSignal.emit();
}

GainCtl::~GainCtl()
{
  delete m_GainFader;
  delete m_VuMeter;
}
