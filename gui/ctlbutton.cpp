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

#include "ctlbutton.h"

CtlButton::CtlButton(int iType):
/*m_bPress(false), */ m_iActValue(0), m_iAntValue(0), m_fValue(0.0), ///TODO test del sigc block, si txuta eliminar el m_bPress
{
  m_iFilterType = iType;
  if(m_iFilterType == GAIN_TYPE) m_bIsXDirection = false;
  else m_bIsXDirection = true;
  
  signal_pressed().connect( sigc::mem_fun(*this,&CtlButton::onButtonPressed));
  signal_released().connect( sigc::mem_fun(*this,&CtlButton::onButtonDepressed));
  signal_motion_notify_event().connect( sigc::mem_fun(*this, &CtlButton::onMouseMove),false);
  signal_motion_notify_event().block(); ///TODO: Test aixo
  
  add_events(Gdk::POINTER_MOTION_MASK); 
}

CtlButton::~CtlButton()
{

}

void CtlButton::onButtonPressed()
{
  //m_bPress = true;
  signal_motion_notify_event().unblock();///TODO: Test aixo
  
  m_bFirstTime = true;
}
void CtlButton::onButtonDepressed()
{
  //m_bPress=false;
  signal_motion_notify_event().block();///TODO: Test aixo
  
  m_iAntValue=0;
  m_iActValue=0;
}

///TODO: verificar que les funcions setPress()  setDepress() no fan falta
/*void CtlButton::setPress()
{
  m_bPress = true;
  m_bFirstTime = true;
}

void CtlButton::setDepress()
{
  m_bPress=false;
}*/

bool CtlButton::onMouseMove(GdkEventMotion* event)
{
  int x,y;
  //if(m_bPress) ///TODO: Test aixo, si el block() unlock() txuta eliminar definitivament aquest if 
  //{
    get_pointer(x, y);
	setButtonNumber(computeValue(x,y));
  //}
   
  return true;
}

float CtlButton::computeValue(int x, int y)
{
  float fValue;
  m_iAntValue=m_iActValue;

  if(m_bIsXDirection) m_iActValue = x;
  else m_iActValue =( -1)*y;
  
  if (m_bFirstTime) ///TODO: Investigar que fa aquest bool
  {
    if(m_iActValue > 0) m_iAntValue = m_iActValue - 1;
    else if(m_iActValue<0) m_iAntValue = m_iActValue + 1;
    m_bFirstTime = false;
  }

  fValue =  fValue + (float)(m_iActValue-m_iAntValue)/ACCELERATION;

///TODO: els limits MAX i MIN han de venir del fitxer *.TTL   propi de LV2
  switch(m_iFilterType)
  {
    case GAIN_TYPE:
      if (fValue > GAIN_MAX) fValue = GAIN_MAX;
      else if (fValue < GAIN_MIN) fValue = GAIN_MIN;
    break;
    
    case FREQ_TYPE:
      if (fValue > GAIN_MAX) fValue = FREQ_MAX;
      else if (fValue < GAIN_MIN) fValue = FREQ_MIN;
    break;
    
    case Q_TYPE:
      if (fValue > PEAK_Q_MAX) fValue = PEAK_Q_MAX;
      else if (fValue < PEAK_Q_MIN) fValue = PEAK_Q_MIN;
    break;
  }
   
   ///TODO: en el següent TODO s'explica la solucio correcte
  //setButtonNumber(m_fValue);
  ///eqbutton_ref_m_iPtr->setValue(m_fValue);
  ///eqbutton_ref_m_iPtr->set_spin_number();
  
  
  ///TODO: CTLButton es objecte de nivell inferior a EQButton i per tant no pot contenir un punter a EQButton ja que aquest no existeix en el seu context.
	///  La solució es un callback, serà que CTLButton llenci un nou event en aquest punts. Aquest event sera capturat per un slot de EQButton per invocar a
	/// setValue(m_fValue); i set_spin_number();
	///El valor m_ptr_fMouseValue no se com passar-lo via event pots el mes facil es que es fagi un getButtonNumber des de EQbutton

  
  //std::cout<<"val: "<<m_fValue<<std::endl;
  
  return fValue;
}

void CtlButton::setButtonNumber(float fNum)
{
  m_fValue = fNum;
  Glib::ustring button_text;
  
  switch(m_iFilterType)
  {
    case GAIN_TYPE:
    case FREQ_TYPE:
    button_text = Glib::ustring::format(std::fixed, std::setprecision(1), m_fValue);
    break;

    case Q_TYPE:
    button_text = Glib::ustring::format(std::fixed, std::setprecision(2), m_fValue);
    break;
  }
  set_label(button_text);
}

float CtlButton::getButtonNumber()
{
  return m_fValue;
}