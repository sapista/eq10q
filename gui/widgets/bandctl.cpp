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

#include  <gtkmm/label.h>
#include "guiconstants.h"
#include "bandctl.h"
#include "setwidgetcolors.h"

BandCtl::BandCtl( const int iBandNum, bool *bSemafor, const char* bundlepath):
m_ButtonAlign(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0.0, 0.0),
m_ComboAlign(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0.0, 0.0),
m_iBandNum(iBandNum),
m_bBandIsEnabled(false),
m_DisableComboEvent(false),
m_DisableButtonEvent(false)
{
  m_FilterSel = Gtk::manage(new PixMapCombo(bundlepath));
  m_Gain = Gtk::manage(new EQButton(GAIN_TYPE, bSemafor));
  m_Freq = Gtk::manage(new EQButton(FREQ_TYPE, bSemafor));
  m_Q = Gtk::manage(new EQButton(Q_TYPE, bSemafor));

  Glib::ustring sText = Glib::ustring::compose("Band %1", m_iBandNum + 1);
  m_FrameLabel.set_use_markup(true);
  m_FrameLabel.set_markup( "<span font_weight=\"bold\" font_family=\"Monospace\">" + sText + "</span>");
  set_label_widget(m_FrameLabel);

  m_VBox.pack_start(m_ComboAlign, Gtk::PACK_EXPAND_PADDING );
  m_VBox.pack_start(*m_Gain, Gtk::PACK_EXPAND_PADDING );
  m_VBox.pack_start(*m_Freq, Gtk::PACK_EXPAND_PADDING );
  m_VBox.pack_start(*m_Q, Gtk::PACK_EXPAND_PADDING );
  m_VBox.pack_start(m_ButtonAlign, Gtk::PACK_EXPAND_PADDING );

  m_OnButton.set_size_request(30,20);
  m_ButtonAlign.add(m_OnButton);
  
  m_VBox.set_spacing(0);
  m_VBox.set_homogeneous(false);
  m_VBox.set_size_request(-1,120);

  m_FilterSel->set_size_request(55,25);
  m_ComboAlign.add(*m_FilterSel);
  add(m_VBox);
  set_shadow_type(Gtk::SHADOW_OUT);
  
  m_ComboAlign.show();
  m_OnButton.show();
  m_FilterSel->show();
  m_Gain->show();
  m_Freq->show();
  m_Q->show();
  m_ButtonAlign.show();
  show();
  
  //Add some tooltips
  m_ComboAlign.set_tooltip_text("Select the filter type for this band.");
  m_Gain->set_tooltip_text("Press and drag to adjust gain.\nAlso you can double click to enter value.");
  m_Freq->set_tooltip_text("Press and drag to adjust frequency.\nAlso you can double click to enter value.");
  m_Q->set_tooltip_text("Press and drag to adjust Q.\nAlso you can double click to enter value.");
  m_OnButton.set_tooltip_text("Enable/Disable this band");

  m_Gain->spinState_changed().connect(sigc::mem_fun(*this, &BandCtl::onSpinStateChanged));
  m_Freq->spinState_changed().connect(sigc::mem_fun(*this, &BandCtl::onSpinStateChanged));
  m_Q->spinState_changed().connect(sigc::mem_fun(*this, &BandCtl::onSpinStateChanged));
  m_OnButton.signal_clicked().connect(sigc::mem_fun(*this, &BandCtl::onButtonClicked));
  m_FilterSel->signal_changed().connect(sigc::mem_fun(*this, &BandCtl::onComboChanged));
  m_Gain->signal_changed().connect(sigc::mem_fun(*this, &BandCtl::onGainChanged));
  m_Freq->signal_changed().connect(sigc::mem_fun(*this, &BandCtl::onFreqChanged));
  m_Q->signal_changed().connect(sigc::mem_fun(*this, &BandCtl::onQChanged));
  signal_realize().connect(sigc::mem_fun(*this, &BandCtl::onThisWidgetRealize));
  
  //Set ON Button font type and ON text
  btnLabel.modify_font(Pango::FontDescription("Monospace 9"));
  m_OnButton.add(btnLabel);
  btnLabel.set_text("ON");
  
  //Set Colors
  SetWidgetColors m_WidgetColors;
  m_WidgetColors.setButtonColors(&m_OnButton);
  m_WidgetColors.setGenericWidgetColors(m_FilterSel); ///TODO: axio no funciona, els colors del combo no canvien
}


BandCtl::~BandCtl(){
  delete m_Gain;
  delete m_Freq;
  delete m_Q;
  delete m_FilterSel;
}

void BandCtl::onThisWidgetRealize()
{
  //Set Colors
  SetWidgetColors m_WidgetColors;
  m_WidgetColors.setBandFrameColor(this, m_iBandNum);
}


//Data accesors
float BandCtl::getGain(){
  return m_Gain->getValue();
}

float BandCtl::getFreq(){
  return m_Freq->getValue();
}

float BandCtl::getQ(){
  return m_Q->getValue();
}

float BandCtl::getFilterType(){
  return (float)m_FilterType;
}

bool BandCtl::getEnabled()
{
  return m_bBandIsEnabled;
}

void BandCtl::setGain(float fGain){
  m_Gain->setValue(fGain);
}

void BandCtl::setFreq(float fFreq){
  m_Freq->setValue(fFreq);
}

void BandCtl::setQ(float fQ){
  m_Q->setValue(fQ);
}

void BandCtl::setFilterType(float fType, bool DisableEvent)
{
  m_DisableComboEvent = DisableEvent && ((int)fType != (int) m_FilterSel->get_active_row_number() + 1); //Disable only if event will happend
  m_FilterSel->set_active((int)fType - 1);
}

void BandCtl::setEnabled(bool bIsEnabled, bool DisableEvent)
{
  m_DisableButtonEvent = DisableEvent && (bIsEnabled != m_bBandIsEnabled); //Disable only if event will happend
  m_OnButton.set_active(bIsEnabled);
  m_bBandIsEnabled = bIsEnabled;
}

//GTK signal Handlers
void BandCtl::onSpinStateChanged(bool SpinState)
{
  m_OnButton.set_sensitive(!SpinState);
}


void BandCtl::onButtonClicked()
{ 
  m_bBandIsEnabled = m_OnButton.get_active();
  configSensitive();
  float fIsOn = 0;
  if(m_bBandIsEnabled) fIsOn = 1;
  
  if (m_DisableButtonEvent)
  {
    m_DisableButtonEvent = false;
  }
  else
  {
    m_bandChangedSignal.emit(m_iBandNum, ONOFF_TYPE, fIsOn);
  }
}

void BandCtl::onComboChanged()
{ 
  m_FilterType = int2FilterType(m_FilterSel->get_active_row_number() + 1);
  configSensitive();
  
  //Config default Q for each kind of filter
  switch((int)m_FilterType)
  {
    case LPF_ORDER_2:
    case LPF_ORDER_3:
    case LPF_ORDER_4:
    case HPF_ORDER_2:
    case HPF_ORDER_3:
    case HPF_ORDER_4:
      setQ(HPF_LPF_Q_DEFAULT);
      m_bandChangedSignal.emit(m_iBandNum, Q_TYPE, getQ());
    break;
    
    case NOTCH:
      setQ(NOTCH_Q_DEFAULT);
      m_bandChangedSignal.emit(m_iBandNum, Q_TYPE, getQ());
    break;

    case LOW_SHELF:
    case HIGH_SHELF:
      setQ(HIGH_LOW_SHELF_Q_DEFAULT);
      m_bandChangedSignal.emit(m_iBandNum, Q_TYPE, getQ());
      m_bandChangedSignal.emit(m_iBandNum, GAIN_TYPE, getGain());
    break;
    
    case PEAK:
      setQ(PEAK_Q_DEFAULT);
      m_bandChangedSignal.emit(m_iBandNum, Q_TYPE, getQ());
      m_bandChangedSignal.emit(m_iBandNum, GAIN_TYPE, getGain());
    break;
  }
  
  if (m_DisableComboEvent)
  {
    m_DisableComboEvent = false;
  }
  else
  {
    m_bandChangedSignal.emit(m_iBandNum, FILTER_TYPE, (float)m_FilterType);
  }
}

void BandCtl::onGainChanged()
{
  m_bandChangedSignal.emit(m_iBandNum, GAIN_TYPE, getGain());
}

void BandCtl::onFreqChanged()
{
  m_bandChangedSignal.emit(m_iBandNum, FREQ_TYPE, getFreq());
}

void BandCtl::onQChanged()
{
  m_bandChangedSignal.emit(m_iBandNum, Q_TYPE, getQ());
}

void BandCtl::configSensitive()
{
  if(!m_bBandIsEnabled)
  {
    m_Q->set_sensitive(false);
    m_Gain->set_sensitive(false);
    m_Freq->set_sensitive(false);
    m_FilterSel->set_sensitive(false);
  }
  else
  {
    m_FilterSel->set_sensitive(true);
    switch((int)m_FilterType)
    {
      case LPF_ORDER_1:
      case HPF_ORDER_1:
	m_Q->set_sensitive(false);
	m_Gain->set_sensitive(false);
	m_Freq->set_sensitive(true);
      break;
      
      case LPF_ORDER_2:
      case LPF_ORDER_3:
      case LPF_ORDER_4:
      case HPF_ORDER_2:
      case HPF_ORDER_3:
      case HPF_ORDER_4:
      case NOTCH:
	m_Gain->set_sensitive(false);
	m_Q->set_sensitive(true);
	m_Freq->set_sensitive(true);
      break;

      case LOW_SHELF:
      case HIGH_SHELF:
      case PEAK:
	m_Gain->set_sensitive(true);
	m_Q->set_sensitive(true);
	m_Freq->set_sensitive(true);
      break;
    }
  }
}

BandCtl::signal_ctlBandChanged BandCtl::signal_changed()
{
  return m_bandChangedSignal;
}
