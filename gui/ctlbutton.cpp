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
#include <cmath>
#include <iostream>
#include <iomanip>

CtlButton::CtlButton(int iType):
m_iActValue(0), m_iAntValue(0), m_fValue(0.0)
{
  m_iFilterType = iType;
  if(m_iFilterType == GAIN_TYPE) m_bIsXDirection = false;
  else m_bIsXDirection = true;
  
  signal_pressed().connect( sigc::mem_fun(*this,&CtlButton::onButtonPressed));
  signal_released().connect( sigc::mem_fun(*this,&CtlButton::onButtonDepressed));
   
  add_events(Gdk::POINTER_MOTION_MASK); 
}

CtlButton::~CtlButton()
{

}

void CtlButton::onButtonPressed()
{
  m_iAntValue = 0;
  m_iActValue = 0;
  m_MouseSignal = signal_motion_notify_event().connect( sigc::mem_fun(*this, &CtlButton::onMouseMove),false);
}

void CtlButton::onButtonDepressed()
{
  m_MouseSignal.disconnect();
  m_iAntValue = 0;
  m_iActValue = 0;
}

bool CtlButton::onMouseMove(GdkEventMotion* event)
{
  int x,y;
  get_pointer(x, y);
  setButtonNumber(computeValue(x,y));
  return true;
}

float CtlButton::computeValue(int x, int y)
{

  float fValue = m_fValue;
  m_iAntValue = m_iActValue;

  if(m_bIsXDirection) m_iActValue = x;
  else m_iActValue =( -1)*y;
  
  if(m_iFilterType != FREQ_TYPE)
  { 
    //Gain and Q variation
    fValue =  fValue + (float)(m_iActValue-m_iAntValue)/ACCELERATION;
  }
  else
  {
    //Freq variaton is logarithimc
    fValue = fValue + (fValue/7)*((float)(m_iActValue-m_iAntValue)/ACCELERATION);
  }

  switch(m_iFilterType)
  {
    case GAIN_TYPE:
      if (fValue > GAIN_MAX) fValue = GAIN_MAX;
      else if (fValue < GAIN_MIN) fValue = GAIN_MIN;
    break;
    
    case FREQ_TYPE:
      if (fValue > FREQ_MAX) fValue = FREQ_MAX;
      else if (fValue < FREQ_MIN) fValue = FREQ_MIN;
    break;
    
    case Q_TYPE:
      if (fValue > PEAK_Q_MAX) fValue = PEAK_Q_MAX;
      else if (fValue < PEAK_Q_MIN) fValue = PEAK_Q_MIN;
    break;
  }
   
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