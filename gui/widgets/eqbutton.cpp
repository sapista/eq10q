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

#include "guiconstants.h"
#include "eqbutton.h"
#include "setwidgetcolors.h"

EQButton::EQButton(int iType, bool *bSemafor):
m_ButtonAlign(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0.0, 0.0),
m_FilterType(int2FilterType(iType)),
m_bTextEntryMode(false),
m_bStop(bSemafor)
{
  *m_bStop = false;
  m_ptr_CtlButton = Gtk::manage(new CtlButton(iType));
  m_ptr_CtlButton->setButtonNumber(m_fValue);
  
  m_TextEntry.set_update_policy(Gtk::UPDATE_ALWAYS);
  
  switch ((int)m_FilterType)
  {
    case GAIN_TYPE:
      m_TextEntry.set_range(GAIN_MIN, GAIN_MAX);
      m_TextEntry.set_digits(1);
      m_TextEntry.set_increments(0.2, 0.5);
    break;
    
    case FREQ_TYPE:
      m_TextEntry.set_range(FREQ_MIN, FREQ_MAX);
    break;
    
    case Q_TYPE:
      m_TextEntry.set_range(PEAK_Q_MIN, PEAK_Q_MAX);
      m_TextEntry.set_digits(2);
      m_TextEntry.set_increments(0.1, 1.0);
    break;
  }
  
  //if(m_FilterType == FREQ_TYPE)
  //{   
    set_size_request(45,20);
    m_ptr_CtlButton->set_size_request(45,20);
    //m_TextEntry.set_size_request(45,20);
  /*}

  else 
  {   
    set_size_request(40, 20);
    m_ptr_CtlButton->set_size_request(40, 20);
    //m_TextEntry.set_size_request(40, 20);
  }
  */
  
  m_ButtonAlign.add(*m_ptr_CtlButton);
  pack_start(m_TextEntry,Gtk::PACK_EXPAND_PADDING);
  pack_start(m_ButtonAlign,Gtk::PACK_EXPAND_PADDING);
  
  m_ptr_CtlButton->signal_double_clicked().connect(sigc::mem_fun(*this, &EQButton::onButtonDoubleClicked));
  m_ptr_CtlButton->signal_changed().connect(sigc::mem_fun(*this, &EQButton::onCtlButtonChanged));
  m_TextEntry.signal_activate().connect(sigc::mem_fun(*this, &EQButton::onEnterPressed));
  
  //This signal is not connected because is not much usefull and enabling it introduces a bug thet crashed the plugin on automation!
  //m_TextEntry.signal_value_changed().connect(sigc::mem_fun(*this, &EQButton::onSpinChange));
 
  //Set property to start with textEntry hide
  m_TextEntry.set_no_show_all();
  
  //Set Colors and fonts
  m_TextEntry.modify_font(Pango::FontDescription("Monospace 7"));
  SetWidgetColors m_WidgetColors;
  m_WidgetColors.setGenericWidgetColors(&m_TextEntry); 
  

}

EQButton::~EQButton()
{
  delete m_ptr_CtlButton;
}


void EQButton::setValue(float fVal)
{
  m_fValue = fVal;
  
  //Limit the value between rang acording filter type
  switch((int)m_FilterType)
  {
    case GAIN_TYPE:
      if (m_fValue > GAIN_MAX) m_fValue = GAIN_MAX;
      else if (m_fValue < GAIN_MIN) m_fValue = GAIN_MIN;
    break;
    
    case FREQ_TYPE:
	  if (m_fValue > FREQ_MAX) m_fValue = FREQ_MAX;
      else if (m_fValue < FREQ_MIN) m_fValue = FREQ_MIN;
    break;
    
    case Q_TYPE:
      if (m_fValue > PEAK_Q_MAX) m_fValue = PEAK_Q_MAX;
      else if (m_fValue < PEAK_Q_MIN) m_fValue = PEAK_Q_MIN;
    break;
  }
  m_ptr_CtlButton->setButtonNumber(m_fValue);
}

float EQButton::getValue()
{
  return m_fValue;
}

void EQButton::onButtonDoubleClicked()
{
  if(!*m_bStop)
  {
    m_EqButtonSpinState.emit(true);
    //Button change
    m_bTextEntryMode = true;
    *m_bStop = true;
    m_ptr_CtlButton->hide();
    
    ///TODO: this is old!!!!
    //setValue(m_ptr_CtlButton->getButtonNumber());
    m_TextEntry.set_value((double)m_fValue);
    
    m_TextEntry.show();
    m_TextEntry.grab_focus();
  }
}

void EQButton::onEnterPressed()
{
  m_fValue = (float)m_TextEntry.get_value();
  m_ptr_CtlButton->setButtonNumber(m_fValue);
  m_ptr_CtlButton->show();
  m_TextEntry.hide();
  *m_bStop = false;
  m_EqButtonSpinState.emit(false);
  m_EqButtonChangedSignal.emit();
}

void EQButton::onSpinChange()
{
  m_fValue = (float)m_TextEntry.get_value();
  m_EqButtonChangedSignal.emit();
  
  if((int)m_FilterType == FREQ_TYPE)
  {
    m_TextEntry.set_increments(m_fValue/10.0, m_fValue/5.0);
  }
}

void EQButton::onCtlButtonChanged()
{
  m_fValue = m_ptr_CtlButton->getButtonNumber();
  m_EqButtonChangedSignal.emit();
}

EQButton::signal_EqButtonChanged EQButton::signal_changed()
{
  return m_EqButtonChangedSignal;
}

EQButton::signal_EqButtonSpinState EQButton::spinState_changed()
{
  return m_EqButtonSpinState;
}
