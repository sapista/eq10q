/***************************************************************************
 *   Copyright (C) 2011 by Pere Rafols Soler                               *
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

#ifndef KNOB_WIDGET_H
#define KNOB_WIDGET_H

#include <cmath>
#include <string>
#include <vector>
#include <gtkmm/drawingarea.h>

#define KNOB_TYPE_LIN 0
#define KNOB_TYPE_FREQ 1
#define KNOB_TYPE_TIME 2

class KnobWidget : public Gtk::DrawingArea
{
  public:
    KnobWidget(float fMin, float fMax, std::string sLabel, std::string sUnits, int iType = KNOB_TYPE_LIN);
    ~KnobWidget();
    void set_value(float fValue);
    double get_value();
    
    //signal accessor:
    typedef sigc::signal<void> signal_KnobChanged;
    signal_KnobChanged signal_changed();
    
protected:
  //Override default signal handler:
  virtual bool on_expose_event(GdkEventExpose* event);
  void redraw();
  
  //Mouse grab signal handlers
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_scrollwheel_event(GdkEventScroll* event);
  virtual bool on_mouse_motion_event(GdkEventMotion* event);
  
  float m_fMin; //Min representable value
  float m_fMax; //Max representable value
  bool bMotionIsConnected;
  float m_Value;
  std::string m_Label;
  std::string m_Units;
  int m_TypeKnob;

  int width;
  int height;
  int mouse_move_ant;
  sigc::connection m_motion_connection;
  
  //Fader change signal
  signal_KnobChanged m_KnobChangedSignal;
};
#endif
