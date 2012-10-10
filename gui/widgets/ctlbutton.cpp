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
  
  signal_button_press_event().connect(sigc::mem_fun(*this, &CtlButton::onButtonDoubleClicked),false);
  signal_released().connect( sigc::mem_fun(*this,&CtlButton::onButtonDepressed));
  signal_show().connect(sigc::mem_fun(*this,&CtlButton::onButtonRealize));
  signal_realize().connect(sigc::mem_fun(*this,&CtlButton::onButtonRealize));
  signal_state_changed().connect(sigc::mem_fun(*this,&CtlButton::onButtonStateChanged));
  add_events(Gdk::POINTER_MOTION_MASK);
  
  //Set font type
  btnLabel.modify_font(Pango::FontDescription::FontDescription("Monospace 8"));
  btnLabel.modify_fg(Gtk::STATE_ACTIVE, Gdk::Color::Color("#FFFFFF"));
  btnLabel.modify_fg(Gtk::STATE_INSENSITIVE, Gdk::Color::Color("#FFFFFF"));
  btnLabel.modify_fg(Gtk::STATE_NORMAL, Gdk::Color::Color("#FFFFFF"));
  btnLabel.modify_fg(Gtk::STATE_PRELIGHT, Gdk::Color::Color("#FFFFFF"));
  btnLabel.modify_fg(Gtk::STATE_SELECTED, Gdk::Color::Color("#FFFFFF")); 
  add(btnLabel);

  //Set custom theme style    
  set_style(m_WidgetColors.getPlainButtonStyle());
}

CtlButton::~CtlButton()
{

}

void CtlButton::onButtonRealize()
{
  ///TODO: remove this method and his connection to on_realize, now is don with Gtk::Style Object
  //Set Start Colors
  //m_WidgetColors.setButtonColors(this);
}

void CtlButton::onButtonStateChanged(Gtk::StateType previous_state)
{
  onButtonRealize();
}

bool CtlButton::onButtonDoubleClicked(GdkEventButton* event)
{
  if(event->button == 1)
  {
    if(event->type == GDK_2BUTTON_PRESS) //Double click on the 1st button
    {
      //Emit signal button click
      onButtonDepressed();
      m_doubleClickSignal.emit();
    }
    else
    {
      onButtonPressed();
    }
  }
  return true;
}

void CtlButton::onButtonPressed()
{ 
  m_iAntValue = 0;
  m_iActValue = 0;
  m_MouseSignal = signal_motion_notify_event().connect( sigc::mem_fun(*this, &CtlButton::onMouseMove),false);
  set_state(Gtk::STATE_ACTIVE);
}

void CtlButton::onButtonDepressed()
{
  m_MouseSignal.disconnect();
  m_iAntValue = 0;
  m_iActValue = 0;
  set_state(Gtk::STATE_NORMAL);
}

bool CtlButton::onMouseMove(GdkEventMotion* event)
{
  int x,y;
  get_pointer(x, y);
  setButtonNumber(computeValue(x,y));
  m_ctlButtonChangedSignal.emit();
  set_state(Gtk::STATE_ACTIVE);
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
      button_text = Glib::ustring::format(std::fixed, std::setprecision(1), m_fValue);
      break;

    case FREQ_TYPE:
      button_text = Glib::ustring::format(std::fixed, std::setprecision(0), m_fValue);
      break;

    case Q_TYPE:
      button_text = Glib::ustring::format(std::fixed, std::setprecision(2), m_fValue);
      break;
  }
 
  btnLabel.set_text(button_text);
}

float CtlButton::getButtonNumber()
{
  return m_fValue;
}

CtlButton::ctlButton_double_clicked CtlButton::signal_double_clicked()
{
  return m_doubleClickSignal;
}

CtlButton::ctlButton_changed CtlButton::signal_changed()
{
  return m_ctlButtonChangedSignal;
}
