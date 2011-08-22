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
#include "eqwindow.h"
#include "gui/guiconstants.h"

//Constructor
EqMainWindow::EqMainWindow(int iAudioChannels, int iNumBands, const char *uri)
  :m_FlatButton("Flat"),
  m_AButton("A"),
  m_BButton("B"),
  m_BypassButton("Bypass"),
  m_iNumOfChannels(iAudioChannels),
  m_iNumOfBands(iNumBands),
  m_bMutex(false),
  m_pluginUri(uri)

///TODO: Molt xapusero!
//   image_logo_top_top("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_top_top.png"),
//   image_logo_top("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_top.png"),
//   image_logo_center("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_center.png"),
//   image_logo_bottom("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_bottom.png"),
//   image_logo_bottom_bottom("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_bottom_bottom.png")
{
 //Buttons A,B i Flat
  m_AButton.set_size_request(25,23);
  m_BButton.set_size_request(25,23);
  m_AButton.set_active(true);
  m_BButton.set_active(false);
  m_FlatButton.set_size_request(40,23); 
  m_BypassButton.set_size_request(55,23); 
  
  m_ButtonAAlign.add(m_AButton);
  m_ButtonBAlign.add(m_BButton);
  m_BypassAlign.add(m_BypassButton);
  m_ButtonAAlign.set(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0.0, 0.0);
  m_ButtonBAlign.set(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0.0, 0.0);
  m_BypassAlign.set(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0.0, 0.0);

  m_ABFlatBox.set_homogeneous(false);
  m_FlatAlign.add(m_FlatButton);
  m_FlatAlign.set(Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER,0.0, 0.0);
  m_ABFlatBox.pack_start(m_BypassAlign,Gtk::PACK_SHRINK);
  m_ABFlatBox.pack_start(m_ButtonAAlign,Gtk::PACK_SHRINK);
  m_ABFlatBox.pack_start(m_ButtonBAlign,Gtk::PACK_SHRINK);
  //m_ABFlatBox.pack_start(image_logo_center); //TODO: the center image
  m_ABFlatBox.pack_start(m_FlatAlign);

  
  m_InGain = Gtk::manage(new GainCtl("In", m_iNumOfChannels ,true));
  m_GainBox.pack_start(*m_InGain, Gtk::PACK_SHRINK);
  m_OutGain = Gtk::manage(new GainCtl("Out", m_iNumOfChannels, false));
  m_GainBox.pack_start(*m_OutGain, Gtk::PACK_SHRINK);
  m_GainBox.set_homogeneous(true);
  m_GainBox.set_spacing(2);
  m_GainBypassBox.pack_start(m_GainBox, Gtk::PACK_EXPAND_WIDGET);
  m_GainBypassBox.set_homogeneous(false);

  m_BandBox.pack_start(m_GainBypassBox,  Gtk::PACK_SHRINK);
  m_BandBox.set_spacing(4);
  m_BandBox.set_homogeneous(false);
  m_BandCtlArray = (BandCtl**)malloc(sizeof(BandCtl*)*m_iNumOfBands);
  
  for (int i = 0; i< m_iNumOfBands; i++)
  {
    m_BandCtlArray[i] = Gtk::manage(new BandCtl(i, &m_bMutex));
    m_BandBox.pack_start(*m_BandCtlArray[i], Gtk::PACK_SHRINK);
    m_BandCtlArray[i] -> signal_changed().connect( sigc::mem_fun(*this, &EqMainWindow::onBandChange));
  }

  //m_MainBox.pack_start(image_logo_top); ///TODO: les imatges....
  m_MainBox.pack_start(m_ABFlatBox,Gtk::PACK_EXPAND_PADDING);
  //m_MainBox.pack_start(image_logo_bottom); //TODO: imatges

//TODO: imatges
//   image_logo_bottom.show();
//   image_logo_top.show();
//   image_logo_center.show();
  m_ABFlatBox.show();
  m_FlatButton.show();
  m_FlatAlign.show();
  m_ABAlign.show();
  m_BypassAlign.show();
  m_AButton.show();
  m_BButton.show();
  m_GainBox.show();
  m_InGain->show();
  m_OutGain->show();
  m_GainBypassBox.show();
  m_BypassButton.show();
  m_BandBox.show();
  m_ButtonAAlign.show();
  m_ButtonBAlign.show();
  
  //Add some tooltips
  m_AButton.set_tooltip_text("Switch to curve A");
  m_BButton.set_tooltip_text("Switch to curve B");
  m_BypassButton.set_tooltip_text("Bypass the equalizer");
  m_FlatButton.set_tooltip_text("Reset all values to default");
  m_InGain->set_tooltip_text("Adjust the input gain");
  m_OutGain->set_tooltip_text("Adjust the output gain");
  
  m_MainBox.pack_start(m_BandBox);
  //m_MainBox.pack_start(image_logo_bottom_bottom); //TODO: imatges
  m_MainBox.set_spacing(0);
  add(m_MainBox);
  m_MainBox.show();
  
   //connect signals
  m_BypassButton.signal_toggled().connect(mem_fun(*this, &EqMainWindow::onButtonBypass));
  m_AButton.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::onButtonA));
  m_BButton.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::onButtonB));
  m_FlatButton.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::onButtonFlat));
  m_InGain->signal_changed().connect( sigc::mem_fun(*this, &EqMainWindow::onGainChange));
  m_OutGain->signal_changed().connect( sigc::mem_fun(*this, &EqMainWindow::onGainChange));
  
  //Load the EQ Parameters objects
  m_AParams = new EqParams(m_iNumOfBands);
  m_BParams = new EqParams(m_iNumOfBands);
  m_CurParams = m_AParams;
  m_BParams->loadFromTtlFile(m_pluginUri.c_str());
}

EqMainWindow::~EqMainWindow()
{
  delete m_AParams;
  delete m_BParams;
  delete m_InGain;
  delete m_OutGain;
  for(int i = 0; i < m_iNumOfBands; i++)
  {
    delete m_BandCtlArray[i];
  }
  free(m_BandCtlArray);
}

void EqMainWindow::changeAB(EqParams *toBeCurrent)
{
  m_CurParams = toBeCurrent;
  
  //Reload All data
  m_InGain->setGain(m_CurParams->getInputGain());
  m_OutGain->setGain(m_CurParams->getOutputGain());
  for(int i = 0; i < m_iNumOfBands; i++)
  {
    //TODO: crec que el setEnabled no esta funcionant be, es pot veure en fer canvi de A/B
    m_BandCtlArray[i]->setEnabled(m_CurParams->getBandEnabled(i));
    m_BandCtlArray[i]->setFilterType(m_CurParams->getBandType(i));
    m_BandCtlArray[i]->setFreq(m_CurParams->getBandFreq(i));
    m_BandCtlArray[i]->setGain(m_CurParams->getBandGain(i));
    m_BandCtlArray[i]->setQ(m_CurParams->getBandQ(i));
  }
}

void EqMainWindow::onButtonA()
{
  if(m_AButton.get_active())
  {
    changeAB(m_AParams);
    m_BButton.set_active(false);
  }
  else m_BButton.set_active(true);
}

void EqMainWindow::onButtonB()
{
  if(m_BButton.get_active()){
    changeAB(m_BParams);
    m_AButton.set_active(false);
    }
  else m_AButton.set_active(true);
}

void EqMainWindow::onButtonFlat()
{ 
  //Popup a waring message
  Gtk::MessageDialog dialog((Gtk::Window&)(*this->get_toplevel()),"This will flat the current curve, are you sure?",
          false /* use_markup */, Gtk::MESSAGE_QUESTION,
          Gtk::BUTTONS_OK_CANCEL);

  if(dialog.run() == Gtk::RESPONSE_OK)loadEqParams();
}

void EqMainWindow::onButtonBypass()
{
  //TODO: complete this method
}

void EqMainWindow::onBandChange(int iBand, int iField, float fValue)
{
  //TODO: complete this method
  
  //Save data change
  switch(iField)
  {
    case GAIN_TYPE: m_CurParams->setBandGain(iBand, fValue); break;
    case FREQ_TYPE: m_CurParams->setBandFreq(iBand, fValue); break;
    case Q_TYPE: m_CurParams->setBandQ(iBand, fValue); break;
    case FILTER_TYPE: m_CurParams->setBandType(iBand, (int) fValue);
    case ONOFF_TYPE: m_CurParams->setBandEnabled(iBand, (fValue > 0.5)); break;  
  }
}

void EqMainWindow::onGainChange(bool bIn, float fGain)
{
  //TODO: complete this method
  
  //Save data Change
  if(bIn) m_CurParams->setInputGain(fGain);
  else m_CurParams->setOutputGain(fGain);
}

void EqMainWindow::loadEqParams()
{
  
  ///TODO: Load from *.ttl file insted of global constants!
//   onGainChange(true, GAIN_DEFAULT);
//   onGainChange(false, GAIN_DEFAULT);
//   for(int i = 0; i< m_iNumOfBands; i++)
//   {
//     onBandChange(i, GAIN_TYPE, GAIN_DEFAULT);
//     onBandChange(i, FREQ_TYPE, FREQ_MIN);
//     onBandChange(i, Q_TYPE, PEAK_Q_DEFAULT);
//     onBandChange(i, FILTER_TYPE, PEAK);
//     onBandChange(i, ONOFF_TYPE, 0.0);
//   }

  //TODO: el punt ttl s'ha de modificar? esta el type de 1 a 12 i aqui el tinc de 0 a 11, com esta en el motor de audio?
  //TODO: estic provant amb una URI que funcioni, la URI ha d'estar fora!
  m_CurParams->loadFromTtlFile(m_pluginUri.c_str());
  
  changeAB(m_CurParams);
}