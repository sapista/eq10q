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

#include <gtkmm/rc.h>
#include "eqwindow.h"
#include "guiconstants.h"
#include "colors.h"
#include "setwidgetcolors.h"

//Constructor
EqMainWindow::EqMainWindow(int iAudioChannels, int iNumBands, const char *uri, const char *bundlePath)
  :m_FlatButton(" Flat "),
  m_AButton(" A "),
  m_BButton(" B "),
  m_BypassButton(" Bypass "),
  m_iNumOfChannels(iAudioChannels),
  m_iNumOfBands(iNumBands),
  m_bMutex(false),
  m_pluginUri(uri),
  m_bundlePath(bundlePath)
{
///TODO: Molt xapusero!
//   image_logo_top_top("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_top_top.png"),
//   image_logo_top("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_top.png"),
//   image_logo_center("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_center.png"),
//   image_logo_bottom("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_bottom.png"),
//   image_logo_bottom_bottom("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_bottom_bottom.png")


//Set Custom style
//Glib::RefPtr<Gtk::RCStyle> thisWidgetStyle = get_modifier_style();
//thisWidgetStyle->set_font(Pango::FontDescription::FontDescription("Monospace 15"));



  
 //Buttons A,B i Flat
  //m_AButton.set_size_request(25,23);
  //m_BButton.set_size_request(25,23);
  m_AButton.set_active(true);
  m_BButton.set_active(false);
  //m_FlatButton.set_size_request(40,23); 
  //m_BypassButton.set_size_request(55,23); 
  
  m_ButtonAAlign.add(m_AButton);
  m_ButtonBAlign.add(m_BButton);
  m_BypassAlign.add(m_BypassButton);
  m_ButtonAAlign.set(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0.0, 0.0);
  m_ButtonBAlign.set(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0.0, 0.0);
  m_BypassAlign.set(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0.0, 0.0);
  m_FlatAlign.add(m_FlatButton);
  m_FlatAlign.set(Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER,0.0, 0.0);


  
  m_InGain = Gtk::manage(new GainCtl("In Gain", m_iNumOfChannels, 6, -20, m_bundlePath.c_str())); ///TODO: Get gain min max from ttl file
  m_OutGain = Gtk::manage(new GainCtl("Out Gain", m_iNumOfChannels, 6, -20, m_bundlePath.c_str())); ///TODO: Get gain min max from ttl file
  m_Bode = Gtk::manage(new PlotEQCurve(m_iNumOfBands));

  m_BandBox.set_spacing(0);
  m_BandBox.set_homogeneous(true);
  m_BandCtlArray = (BandCtl**)malloc(sizeof(BandCtl*)*m_iNumOfBands);
  
  for (int i = 0; i< m_iNumOfBands; i++)
  {
    m_BandCtlArray[i] = Gtk::manage(new BandCtl(i, &m_bMutex, m_bundlePath.c_str()));
    m_BandBox.pack_start(*m_BandCtlArray[i], Gtk::PACK_SHRINK);
    m_BandCtlArray[i] -> signal_changed().connect( sigc::mem_fun(*this, &EqMainWindow::onBandChange));
  }

  //Bode plot layout
  m_PlotFrame.add(*m_Bode);
  m_PlotFrame.set_label("EQ Curve");

  //Box layout
  m_ABFlatBox.set_homogeneous(false);
  m_ABFlatBox.pack_start(m_BypassAlign,Gtk::PACK_SHRINK);
  m_ABFlatBox.pack_start(m_ButtonAAlign,Gtk::PACK_SHRINK);
  m_ABFlatBox.pack_start(m_ButtonBAlign,Gtk::PACK_SHRINK);
  //m_ABFlatBox.pack_start(image_logo_center); //TODO: the center image
  m_ABFlatBox.pack_start(m_FlatAlign);
  
  
  m_CurveBypassBandsBox.pack_start(m_PlotFrame ,Gtk::PACK_SHRINK);
  m_CurveBypassBandsBox.pack_start(m_ABFlatBox ,Gtk::PACK_SHRINK);
  m_CurveBypassBandsBox.pack_start(m_BandBox ,Gtk::PACK_SHRINK);

  m_GainEqBox.pack_start(*m_InGain, Gtk::PACK_SHRINK);
  m_GainEqBox.pack_start(m_CurveBypassBandsBox, Gtk::PACK_SHRINK);
  m_GainEqBox.pack_start(*m_OutGain, Gtk::PACK_SHRINK);
  m_GainEqBox.set_spacing(2);
   
  //m_MainBox.pack_start(image_logo_top); ///TODO: les imatges....
  //m_MainBox.pack_start(PRESETS Widget,Gtk::PACK_EXPAND_PADDING); ///TODO: add presets widget
  //m_MainBox.pack_start(image_logo_bottom); //TODO: imatges
  m_MainBox.pack_start(m_GainEqBox);
  //m_MainBox.pack_start(image_logo_bottom_bottom); //TODO: imatges
  m_MainBox.set_spacing(0);


//TODO: imatges
//   image_logo_bottom.show();
//   image_logo_top.show();
//   image_logo_center.show();
 
  add(m_MainBox);
  
  //Add some tooltips
  m_AButton.set_tooltip_text("Switch to curve A");
  m_BButton.set_tooltip_text("Switch to curve B");
  m_BypassButton.set_tooltip_text("Bypass the equalizer");
  m_FlatButton.set_tooltip_text("Reset all values to default");
  m_InGain->set_tooltip_text("Adjust the input gain");
  m_OutGain->set_tooltip_text("Adjust the output gain");

  //connect signals
  m_BypassButton.signal_toggled().connect(mem_fun(*this, &EqMainWindow::onButtonBypass));
  m_AButton.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::onButtonA));
  m_BButton.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::onButtonB));
  m_FlatButton.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::onButtonFlat));
  m_InGain->signal_changed().connect( sigc::mem_fun(*this, &EqMainWindow::onInputGainChange));
  m_OutGain->signal_changed().connect( sigc::mem_fun(*this, &EqMainWindow::onOutputGainChange));
  m_Bode->signal_changed().connect(sigc::mem_fun(*this, &EqMainWindow::onCurveChange));
  signal_realize().connect( sigc::mem_fun(*this, &EqMainWindow::onRealize));
  
  //Load the EQ Parameters objects, the params for A curve will be loaded by host acording previous session plugin state
  m_AParams = new EqParams(m_iNumOfBands);
  m_BParams = new EqParams(m_iNumOfBands);
  m_AParams->loadFromTtlFile(m_pluginUri.c_str());
  m_BParams->loadFromTtlFile(m_pluginUri.c_str());  
  m_CurParams = m_AParams;
  

  //Set cutom theme color:
  Gdk::Color m_WinBgColor;
  SetWidgetColors m_WidgetColors;

  //Set Main widget Background
  m_WinBgColor.set_rgb(GDK_COLOR_MACRO( BACKGROUND_R ), GDK_COLOR_MACRO( BACKGROUND_G ), GDK_COLOR_MACRO( BACKGROUND_B ));
  modify_bg(Gtk::STATE_NORMAL, m_WinBgColor);
  
  m_WidgetColors.setGenericWidgetColors(&m_PlotFrame);
  m_WidgetColors.setGenericWidgetColors(m_PlotFrame.get_label_widget());
  m_WidgetColors.setGenericWidgetColors(m_InGain);
  m_WidgetColors.setGenericWidgetColors(m_InGain->get_label_widget());
  m_WidgetColors.setGenericWidgetColors(m_OutGain);
  m_WidgetColors.setGenericWidgetColors(m_OutGain->get_label_widget());
  m_WidgetColors.setButtonColors(&m_AButton);
  m_WidgetColors.setButtonColors(&m_BButton);
  m_WidgetColors.setButtonColors(&m_FlatButton);
  m_WidgetColors.setButtonColors(&m_BypassButton);
  
  //Set buttons font type
  m_BypassButton.modify_font(Pango::FontDescription::FontDescription("Monospace 8"));
  m_AButton.modify_font(Pango::FontDescription::FontDescription("Monospace 8"));
  m_BButton.modify_font(Pango::FontDescription::FontDescription("Monospace 8"));
  m_FlatButton.modify_font(Pango::FontDescription::FontDescription("Monospace 8"));

  //Set pixmap objects TODO:
  //void modify_bg_pixmap (StateType state, const Glib::ustring& pixmap_name)
  

/*
    unsetenv ("GTK_RC_FILES");
  Gtk::RC rc ("/home/sapista/build/LV2/GtkRCTest/gtkrc");
  //RC::reset_styles (Gtk::Settings::get_default());
  //rc.reset_styles(Gtk::Settings::get_default());
  gtk_rc_reset_styles (gtk_settings_get_default());
  */
  



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

void EqMainWindow::onRealize()
{
  Gtk::Window* toplevel = dynamic_cast<Gtk::Window *>(this->get_toplevel()); 
  toplevel->set_resizable(false);  
}


void EqMainWindow::changeAB(EqParams *toBeCurrent)
{ 
  m_CurParams = toBeCurrent;
  
  //Reload All data
  m_InGain->setGain(m_CurParams->getInputGain());
  m_InGainValue = m_InGain->getGain();
  m_OutGain->setGain(m_CurParams->getOutputGain());
  m_OutGainValue = m_OutGain->getGain();
  
   //Write to LV2 port
   write_function(controller, EQ_INGAIN, sizeof(float), 0, &m_InGainValue);
   write_function(controller, EQ_OUTGAIN, sizeof(float), 0, &m_OutGainValue);
  
  //Reset the Curve Plot
  m_Bode->resetCurve();
  m_Bode->redraw();
   
  for(int i = 0; i < m_iNumOfBands; i++)
  {
    //TODO: crec  el setEnabled no esta funcionant be, es pot veure en fer canvi de A/B -> bug real vist el 09/08/2012 P.Rafols
    m_BandCtlArray[i]->setEnabled(m_CurParams->getBandEnabled(i));
    m_BandCtlArray[i]->setFilterType(m_CurParams->getBandType(i));
    m_BandCtlArray[i]->setFreq(m_CurParams->getBandFreq(i));
    m_BandCtlArray[i]->setGain(m_CurParams->getBandGain(i));
    m_BandCtlArray[i]->setQ(m_CurParams->getBandQ(i));
    
    /**
    m_Bode->setBandEnable(i, m_CurParams->getBandEnabled(i));
    m_Bode->setBandType(i, m_CurParams->getBandType(i));
    m_Bode->setBandFreq(i, m_CurParams->getBandFreq(i));
    m_Bode->setBandGain(i, m_CurParams->getBandGain(i));
    m_Bode->setBandQ(i, m_CurParams->getBandQ(i));
    **/
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
  #ifdef PRINT_DEBUG_INFO
    std::cout<<"onButtonBypass... ";
  #endif
  m_Bode->setBypass(m_BypassButton.get_active());
  if (m_BypassButton.get_active())
  {
    m_bypassValue = 1;
  }
  else
  {
    m_bypassValue = 0;
  }
  write_function(controller, EQ_BYPASS, sizeof(float), 0, &m_bypassValue);
  m_BypassChangedSignal.emit(m_BypassButton.get_active());

  #ifdef PRINT_DEBUG_INFO
    std::cout<<"Return"<<std::cout;
  #endif
}

void EqMainWindow::onBandChange(int iBand, int iField, float fValue)
{

  #ifdef PRINT_DEBUG_INFO
    std::cout<<"onBandChange...  Band = "<<iBand<<" Field = "<<iField;
  #endif
  
  switch(iField)
  {
    case GAIN_TYPE: 
      write_function(controller, iBand + PORT_OFFSET + 2*m_iNumOfChannels, sizeof(float), 0, &fValue);
      m_CurParams->setBandGain(iBand, fValue);
      m_Bode->setBandGain(iBand, fValue);
      m_BandGainChangedSignal.emit(iBand, fValue);
      break;
      
    case FREQ_TYPE:
      write_function(controller, iBand + PORT_OFFSET + 2*m_iNumOfChannels + m_iNumOfBands, sizeof(float), 0, &fValue);
      m_CurParams->setBandFreq(iBand, fValue);
      m_Bode->setBandFreq(iBand, fValue);
      m_BandFreqChangedSignal.emit(iBand, fValue);
      break;
      
    case Q_TYPE:
      write_function(controller, iBand + PORT_OFFSET + 2*m_iNumOfChannels + 2*m_iNumOfBands, sizeof(float), 0, &fValue);
      m_CurParams->setBandQ(iBand, fValue);
      m_Bode->setBandQ(iBand, fValue);
      m_BandQChangedSignal.emit(iBand, fValue);
      break;
      
    case FILTER_TYPE:
      write_function(controller, iBand + PORT_OFFSET + 2*m_iNumOfChannels + 3*m_iNumOfBands, sizeof(float), 0, &fValue);
      m_CurParams->setBandType(iBand, (int) fValue);
      m_Bode->setBandType(iBand, (int) fValue);
      m_BandTypeChangedSignal.emit(iBand, (int)fValue);
      break;
      
    case ONOFF_TYPE:
      write_function(controller, iBand + PORT_OFFSET + 2*m_iNumOfChannels + 4*m_iNumOfBands, sizeof(float), 0, &fValue);
      m_CurParams->setBandEnabled(iBand, (fValue > 0.5)); 
      m_Bode->setBandEnable(iBand, (fValue > 0.5));
      m_BandEnabledChangedSignal.emit(iBand, (fValue > 0.5));
      break;  
  }
  
  #ifdef PRINT_DEBUG_INFO
    std::cout<<"Return"<<std::endl;
  #endif
}

void EqMainWindow::onInputGainChange()
{

  #ifdef PRINT_DEBUG_INFO
    std::cout<<"onInputGainChange... ";
  #endif
  
  //Save data Change
  m_InGainValue = m_InGain->getGain();
  m_CurParams->setInputGain(m_InGainValue);
  
  //Write to LV2 port
   write_function(controller, EQ_INGAIN, sizeof(float), 0, &m_InGainValue);
  
  //Emit signal
  m_InputGainChangedSignal.emit(m_InGain->getGain());
  
  #ifdef PRINT_DEBUG_INFO
    std::cout<<"Return"<<std::cout;
  #endif
}

void EqMainWindow::onOutputGainChange()
{
  #ifdef PRINT_DEBUG_INFO
    std::cout<<"onOutputGainChange... ";
  #endif
  
  //Save data Change
  m_OutGainValue = m_OutGain->getGain();
  m_CurParams->setOutputGain(m_OutGainValue);
  
  //Write to LV2 port
  write_function(controller, EQ_OUTGAIN, sizeof(float), 0, &m_OutGainValue);
  
  //Emit signal
  m_OutputGainChangedSignal.emit(m_OutGain->getGain());
  
  #ifdef PRINT_DEBUG_INFO
    std::cout<<"Return"<<std::cout;
  #endif
}

void EqMainWindow::onCurveChange(int band_ix, float Gain, float Freq, float Q)
{
  m_BandCtlArray[band_ix]->setGain(Gain);
  m_BandCtlArray[band_ix]->setFreq(Freq);
  m_BandCtlArray[band_ix]->setQ(Q);
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

// LV2 Input Control ports handlers
void EqMainWindow::setBypass(bool bBypass)
{
    m_BypassButton.set_active(bBypass);
    m_Bode->setBypass(bBypass);
}

void EqMainWindow::setInputGain(float Gain)
{
  m_InGain->setGain(Gain);
}

void EqMainWindow::setOutputGain(float Gain)
{
  m_OutGain->setGain(Gain);
}

void EqMainWindow::setInputVuLevel(int Channel, float Level)
{
  m_InGain->setVu(Channel,Level);
}

void EqMainWindow::setOutputVuLevel(int Channel, float Level)
{
  m_OutGain->setVu(Channel, Level);
}

void EqMainWindow::setBandGain(int Band, float Gain)
{
  m_BandCtlArray[Band]->setGain(Gain);
  ///m_Bode->setBandGain(Band, Gain);
}

void EqMainWindow::setBandFreq(int Band, float Freq)
{
  m_BandCtlArray[Band]->setFreq(Freq);
  ///m_Bode->setBandFreq(Band, Freq);
}

void EqMainWindow::setBandQ(int Band, float Q)
{
  m_BandCtlArray[Band]->setQ(Q);
  ///m_Bode->setBandQ(Band, Q);
}

void EqMainWindow::setBandType(int Band, float Type)
{
  m_BandCtlArray[Band]->setFilterType(Type);
  ///m_Bode->setBandType(Band, (int)Type);
}

void EqMainWindow::setBandEnabled(int Band, bool Enabled)
{
  m_BandCtlArray[Band]->setEnabled(Enabled);
  ///m_Bode->setBandEnable(Band, Enabled);
}

//LV2 Output port signals
EqMainWindow::signal_BypassChanged EqMainWindow::signal_Bypass_Changed()
{
  return m_BypassChangedSignal;
}

EqMainWindow::signal_ControlGainChanged EqMainWindow::signal_InputGain_Changed()
{
  return m_InputGainChangedSignal;
}

EqMainWindow::signal_ControlGainChanged EqMainWindow::signal_OutputGain_Changed()
{
  return m_OutputGainChangedSignal;
}

EqMainWindow::signal_BandParamChanged EqMainWindow::signal_BandGain_Changed()
{
  return m_BandGainChangedSignal;
}

EqMainWindow::signal_BandParamChanged EqMainWindow::signal_BandFreq_Changed()
{
  return m_BandFreqChangedSignal;
}

EqMainWindow::signal_BandParamChanged EqMainWindow::signal_BandQ_Changed()
{
  return m_BandQChangedSignal;
}

EqMainWindow::signal_BandTypeChanged EqMainWindow::signal_BandType_Changed()
{
  return m_BandTypeChangedSignal;
}

EqMainWindow::signal_BandEnabledChanged EqMainWindow::signal_BandEnabled_Changed()
{
  return m_BandEnabledChangedSignal;
}