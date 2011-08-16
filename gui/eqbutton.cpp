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

EQButton::EQButton(int iType, bool *bSemafor):
m_ButtonAlign(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0.0, 0.0),
m_bStop(bSemafor),
m_iFilterType(iType)
{
  *m_bStop = false;
  m_ptr_CtlButton = Gtk::manage(new CtlButton(iType));
  m_ptr_CtlButton->setButtonNumber(m_fValue);
  
  m_TextEntry.set_numeric(true);
  
  switch (m_iFilterType)
  {
    case GAIN_TYPE:
      m_TextEntry.set_range(GAIN_MIN, GAIN_MAX); ///TODO: Aquest limits han de sortir d'algun lloc, el mes logic seria usar el fitxer de config del LV2 *.ttl
      m_TextEntry.set_digits(1);
    break;
    
    case FREQ_TYPE:
      m_TextEntry.set_range(FREQ_MIN, FREQ_MAX); ///TODO: Aquest limits han de sortir d'algun lloc, el mes logic seria usar el fitxer de config del LV2 *.ttl
      m_TextEntry.set_digits(1);
    break;
    
    case Q_TYPE:
      m_TextEntry.set_range(PEAK_Q_MIN, PEAK_Q_MAX); ///TODO: Aquest limits han de sortir d'algun lloc, el mes logic seria usar el fitxer de config del LV2 *.ttl
      m_TextEntry.set_digits(2);
    break;
  }
  m_TextEntry.set_increments(0.1, 1.0);
  
  if(m_iFilterType == FREQ_TYPE)
  {
    set_size_request(70,25);
    m_ptr_CtlButton->set_size_request(70,25);
    m_TextEntry.set_size_request(70,25);
  }

  else 
  {
    set_size_request(45,25);
    m_ptr_CtlButton->set_size_request(50,25);
    m_TextEntry.set_size_request(50,25);
  }
  
  m_ButtonAlign.add(*m_ptr_CtlButton);
  pack_start(m_TextEntry,Gtk::PACK_EXPAND_PADDING);
  pack_start(m_ButtonAlign,Gtk::PACK_EXPAND_PADDING);
  
  m_TextEntry.hide();
  m_ptr_CtlButton->show();
  m_ButtonAlign.show();
  
  m_ptr_CtlButton->signal_double_clicked().connect(sigc::mem_fun(*this, &EQButton::onButtonDoubleClicked));
  m_ptr_CtlButton->signal_changed().connect(sigc::mem_fun(*this, &EQButton::onCtlButtonChanged));
  m_TextEntry.signal_activate().connect(sigc::mem_fun(*this, &EQButton::onEnterPressed));
  m_TextEntry.signal_value_changed().connect(sigc::mem_fun(*this, &EQButton::onSpinChange));
}

EQButton::~EQButton()
{
  delete m_ptr_CtlButton;
}

void EQButton::setValue(float fVal)
{
  m_fValue = fVal;
  
  //Limit the value between rang acording filter type
  switch(m_iFilterType)
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
  
  m_TextEntry.set_value((double)m_fValue);
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
      *m_bStop = true;
      m_ptr_CtlButton->hide();
      setValue(m_ptr_CtlButton->getButtonNumber());
      m_TextEntry.show();
      m_TextEntry.grab_focus();
  }
}

void EQButton::onEnterPressed()
{
  m_ptr_CtlButton->setButtonNumber(m_fValue);
  m_ptr_CtlButton->show();
  m_TextEntry.hide();
  *m_bStop = false;
  m_EqButtonChangedSignal.emit();
}

void EQButton::onSpinChange()
{
  m_fValue = (float)m_TextEntry.get_value();
  m_EqButtonChangedSignal.emit();
}

void EQButton::onCtlButtonChanged()
{
  m_fValue = getButtonNumber();
  m_EqButtonChangedSignal.emit();
}

EQButton::signal_EqButtonChanged EQButton::signal_changed()
{
  return m_EqButtonChangedSignal;
}