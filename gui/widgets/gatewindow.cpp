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

#include <stdlib.h>
#include <iostream>

#include <cmath> //TODO: si no u necessites eliminau!!!
#include <cstring>
#include <gtkmm/window.h>
#include "gatewindow.h"

GateMainWindow::GateMainWindow(const char *uri, const char *bundlePath)
  :m_pluginUri(uri),
  m_bundlePath(bundlePath)
{
  m_ThresholdFader = Gtk::manage(new FaderWidget(6.0, -20.0, bundlePath)); //TODO posa els limits que toca akests son per probar VU
  m_InputVu = Gtk::manage(new VUWidget(1, -24.0, 6.0,false, true));
  m_GainReductionVu = Gtk::manage(new VUWidget(1, -24.0, 6.0, true));
  m_VuBox.pack_start(*m_ThresholdFader);
  m_VuBox.pack_start(*m_InputVu);
  m_VuBox.pack_start(*m_GainReductionVu);
  m_VuBox.show_all_children();
  m_VuBox.show();
  add(m_VuBox);
  
  //Connect signals
  //m_ThresholdFader->signal_changed().connect(sigc::mem_fun(*this, &GateMainWindow::onThresholdChange));
  m_InputVu->signal_changed().connect(sigc::mem_fun(*this, &GateMainWindow::onThresholdChange));
}

GateMainWindow::~GateMainWindow()
{
  delete m_InputVu;
  delete m_GainReductionVu;
  delete m_ThresholdFader;
}

void GateMainWindow::onRealize()
{
  Gtk::Window* toplevel = dynamic_cast<Gtk::Window *>(this->get_toplevel()); 
  toplevel->set_resizable(false);  
}

void GateMainWindow::onThresholdChange()
{
 //TODO: to implement event handler this is just a simple test
  
  //double th = m_ThresholdFader->get_value();
  float th = m_InputVu->get_value_th();
  m_InputVu->setValue(0, pow(10.0, (th) *0.05));
  m_GainReductionVu->setValue(0, (float)(pow(10.0, th*0.05)));
}
