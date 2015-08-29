/***************************************************************************
 *   Copyright (C) 2015 by Pere Ràfols Soler                               *
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

#include <cstring>
#include "midsidewindow.h"
#include "guiconstants.h"
#include "setwidgetcolors.h"

#define KNOB_ICON_FILE "/knobs/knob2_32px.png"
#define KNOB_SIZE_X 75
#define KNOB_SIZE_Y 72
#define WIDGET_BORDER 3

MidSideMainWindow::MidSideMainWindow(const char* uri, std::string bundlePath, bool isLR2MS)
:m_pluginUri(uri),
  m_bundlePath(bundlePath),
  m_bisLR2MS(isLR2MS)
{
  std::string sIn1, sIn2, sOut1, sOut2;
  if(m_bisLR2MS)
  {
    sIn1 = "L";
    sIn2 = "R";
    sOut1 = "M";
    sOut2 = "S";
  }
  else
  {
    sIn1 = "M";
    sIn2 = "S";
    sOut1 = "L";
    sOut2 = "R";
  }
  
  m_InGain1 = Gtk::manage(new KnobWidget2(-20.0, 20.0, sIn1 + " Gain", "dB", (m_bundlePath + KNOB_ICON_FILE).c_str(), KNOB_TYPE_LIN, true ));
  m_InGain2 = Gtk::manage(new KnobWidget2(-20.0, 20.0, sIn2 + " Gain", "dB", (m_bundlePath + KNOB_ICON_FILE).c_str(), KNOB_TYPE_LIN, true ));
  m_OutGain1 = Gtk::manage(new KnobWidget2(-20.0, 20.0, sOut1 + " Gain", "dB", (m_bundlePath + KNOB_ICON_FILE).c_str(), KNOB_TYPE_LIN, true ));
  m_OutGain2 = Gtk::manage(new KnobWidget2(-20.0, 20.0, sOut2 + " Gain", "dB", (m_bundlePath + KNOB_ICON_FILE).c_str(), KNOB_TYPE_LIN, true ));
  
  m_InSolo1.set_label(sIn1 + " solo");
  m_InSolo2.set_label(sIn2 + " solo");
  m_OutSolo1.set_label(sOut1 + " solo");
  m_OutSolo2.set_label(sOut2 + " solo");
  
  m_InputVu = Gtk::manage(new VUWidget(2, -48.0, 6.0, "In"));
  m_OutputVu = Gtk::manage(new VUWidget(2, -48.0, 6.0, "Out"));
  
  m_VInBox.pack_start( *m_InGain1 ,Gtk::PACK_EXPAND_PADDING);
  m_VInBox.pack_start( m_InSolo1 );
  m_VInBox.pack_start( *m_InGain2 ,Gtk::PACK_EXPAND_PADDING);
  m_VInBox.pack_start( m_InSolo2 );
  
  m_VOutBox.pack_start( *m_OutGain1 ,Gtk::PACK_EXPAND_PADDING);
  m_VOutBox.pack_start( m_OutSolo1 );
  m_VOutBox.pack_start( *m_OutGain2 ,Gtk::PACK_EXPAND_PADDING);
  m_VOutBox.pack_start( m_OutSolo2 );
  
  m_HTopBox.pack_start( *m_InputVu);
  m_HTopBox.pack_start( m_VInBox , Gtk::PACK_EXPAND_PADDING );
  //m_HTopBox.pack_start( *image_flowChart ); //TODO add the flow chart diagram
  m_HTopBox.pack_start( m_VOutBox , Gtk::PACK_EXPAND_PADDING);
  m_HTopBox.pack_start( *m_OutputVu );
  
  show_all();
  add(m_HTopBox);
  set_size_request(500, 400);
  
  //Signal handlers
  m_InGain1->signal_changed().connect(sigc::mem_fun(*this, &MidSideMainWindow::onInGain1Change));
  m_InGain2->signal_changed().connect(sigc::mem_fun(*this, &MidSideMainWindow::onInGain2Change));
  m_OutGain1->signal_changed().connect(sigc::mem_fun(*this, &MidSideMainWindow::onOutGain1Change));
  m_OutGain2->signal_changed().connect(sigc::mem_fun(*this, &MidSideMainWindow::onOutGain2Change));
  m_InSolo1.signal_clicked().connect(sigc::mem_fun(*this, &MidSideMainWindow::onInSolo1Change));
  m_InSolo2.signal_clicked().connect(sigc::mem_fun(*this, &MidSideMainWindow::onInSolo2Change));
  m_OutSolo1.signal_clicked().connect(sigc::mem_fun(*this, &MidSideMainWindow::onOutSolo1Change));
  m_OutSolo2.signal_clicked().connect(sigc::mem_fun(*this, &MidSideMainWindow::onOutSolo2Change));
}

MidSideMainWindow::~MidSideMainWindow()
{
 delete m_InGain1;
 delete m_InGain2;
 delete m_OutGain1;
 delete m_OutGain2;
 delete m_InputVu;
 delete m_OutputVu;
 //delete image_flowChart; //TODO
}

void MidSideMainWindow::onInGain1Change()
{
  //Write to LV2 port
  float aux;
  aux = m_InGain1->get_value();
  write_function(controller, PORT_GAIN_IN_1, sizeof(float), 0, &aux);
}

void MidSideMainWindow::onInGain2Change()
{
  //Write to LV2 port
  float aux;
  aux = m_InGain2->get_value();
  write_function(controller, PORT_GAIN_IN_2, sizeof(float), 0, &aux);
}

void MidSideMainWindow::onOutGain1Change()
{
  //Write to LV2 port
  float aux;
  aux = m_OutGain1->get_value();
  write_function(controller, PORT_GAIN_OUT_1, sizeof(float), 0, &aux);
}

void MidSideMainWindow::onOutGain2Change()
{
  //Write to LV2 port
  float aux;
  aux = m_OutGain2->get_value();
  write_function(controller, PORT_GAIN_OUT_2, sizeof(float), 0, &aux);
}

void MidSideMainWindow::onInSolo1Change()
{
  if(m_InSolo1.get_active())
  {
    resetSoloState();
    m_InSolo1.set_active(true);
  }
  
  //Write to LV2 port the new value of this solo button
  float aux;
  aux = m_InSolo1.get_active() ? 1.0 : 0.0;
  write_function(controller, PORT_SOLO_IN_1, sizeof(float), 0, &aux);
}

void MidSideMainWindow::onInSolo2Change()
{
  if(m_InSolo2.get_active())
  {
    resetSoloState();
    m_InSolo2.set_active(true);
  }
  
  //Write to LV2 port the new value of this solo button
  float aux;
  aux = m_InSolo2.get_active() ? 1.0 : 0.0;
  write_function(controller, PORT_SOLO_IN_2, sizeof(float), 0, &aux);
}

void MidSideMainWindow::onOutSolo1Change()
{
  if(m_OutSolo1.get_active())
  {
    resetSoloState();
    m_OutSolo1.set_active(true);
  }
  
  //Write to LV2 port the new value of this solo button
  float aux;
  aux = m_OutSolo1.get_active() ? 1.0 : 0.0;
  write_function(controller, PORT_SOLO_OUT_1, sizeof(float), 0, &aux);
}

void MidSideMainWindow::onOutSolo2Change()
{
  if(m_OutSolo2.get_active())
  {
    resetSoloState();
    m_OutSolo2.set_active(true);
  }
  
  //Write to LV2 port the new value of this solo button
  float aux;
  aux = m_OutSolo2.get_active() ? 1.0 : 0.0;
  write_function(controller, PORT_SOLO_OUT_2, sizeof(float), 0, &aux);
}

void MidSideMainWindow::resetSoloState()
{
  m_InSolo1.set_active(false);
  m_InSolo2.set_active(false);
  m_OutSolo1.set_active(false);
  m_OutSolo2.set_active(false);
    
  //Write to LV2 ports the null solo state
  float aux;
  aux = 0.0;
  write_function(controller, PORT_SOLO_IN_1, sizeof(float), 0, &aux);
  write_function(controller, PORT_SOLO_IN_2, sizeof(float), 0, &aux);
  write_function(controller, PORT_SOLO_OUT_1, sizeof(float), 0, &aux);
  write_function(controller, PORT_SOLO_OUT_2, sizeof(float), 0, &aux);
}
