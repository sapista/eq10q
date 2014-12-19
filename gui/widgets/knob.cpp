/***************************************************************************
 *   Copyright (C) 2011 by Pere Ràfols Soler                               *
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

#include <iostream>
#include <cmath>
#include <cstdio>
#include "knob.h"
#include "colors.h"

#define PI 3.1416
#define KNOB_RADIUS 0.3
#define SCROLL_EVENT_PERCENT 0.005
#define MOUSE_EVENT_PERCENT 0.008
#define KNOB_CENTER_X 0.5
#define KNOB_CENTER_Y 0.4
#define CURSOR_LENGHT 0.25
#define CURSOR_TRIANGLE_BASE 0.03

KnobWidget::KnobWidget(float fMin, float fMax, std::string sLabel, std::string sUnits, int iType):
  m_fMin(fMin),
  m_fMax(fMax),
  m_Value(fMin),
  m_Label(sLabel),
  m_Units(sUnits),
  m_TypeKnob(iType),
  mouse_move_ant(0)
{
  add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK | Gdk::SCROLL_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &KnobWidget::on_button_press_event),true);
  signal_button_release_event().connect(sigc::mem_fun(*this, &KnobWidget::on_button_release_event),true);
  signal_scroll_event().connect(sigc::mem_fun(*this, &KnobWidget::on_scrollwheel_event),true);
}

KnobWidget::~KnobWidget()
{

}

KnobWidget::signal_KnobChanged KnobWidget::signal_changed()
{
  return m_KnobChangedSignal;
}


void KnobWidget::set_value(float fValue)
{
  m_Value = fValue;
  m_Value = m_Value < m_fMin ? m_fMin : m_Value;
  m_Value = m_Value > m_fMax ? m_fMax : m_Value;
  redraw();
}

double KnobWidget::get_value()
{
  return m_Value;
}

void KnobWidget::redraw()
{
  Glib::RefPtr<Gdk::Window> win = get_window();
  if(win)
  {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(), get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
}


//Mouse events
bool KnobWidget::on_button_press_event(GdkEventButton* event)
{
  int x,y;
  get_pointer(x,y);
  if( x > 10 &&
      x < height -10 &&
      y > 10 &&
      y < height - 10)
  {
    mouse_move_ant = y;
    if (!bMotionIsConnected)
    {
      m_motion_connection = signal_motion_notify_event().connect(sigc::mem_fun(*this, &KnobWidget::on_mouse_motion_event),true);
      bMotionIsConnected = true;
    }
  }
  return true;
}

bool KnobWidget::on_button_release_event(GdkEventButton* event)
{
  m_motion_connection.disconnect();
  bMotionIsConnected = false;
  return true;
}

bool KnobWidget::on_mouse_motion_event(GdkEventMotion* event)
{
  double  increment;
  
  switch(m_TypeKnob)
  {
    case  KNOB_TYPE_FREQ:
      increment =  MOUSE_EVENT_PERCENT*(m_fMax - m_fMin)*0.0002*m_Value;
      break;
      
    case KNOB_TYPE_LIN:
      increment =  MOUSE_EVENT_PERCENT*(m_fMax - m_fMin);
      break;
      
    case KNOB_TYPE_TIME:
      increment =  MOUSE_EVENT_PERCENT*5.0*(m_Value + 1.0);
      break;  
  }

  int yPixels = event->y;
  
  if(yPixels - mouse_move_ant < 0)
  {
    //Move up
    set_value(m_Value + increment*(abs(yPixels - mouse_move_ant)));
  }
  
  if(yPixels - mouse_move_ant > 0)
  {
    //Move down
    set_value(m_Value - increment*(abs(yPixels - mouse_move_ant)));
  }
  mouse_move_ant = yPixels;
  m_KnobChangedSignal.emit();
  return true;
}

bool KnobWidget::on_scrollwheel_event(GdkEventScroll* event)
{
  double  increment;
  switch(m_TypeKnob)
  {
    case  KNOB_TYPE_FREQ:
      increment =  SCROLL_EVENT_PERCENT*(m_fMax - m_fMin)*0.0001*m_Value;
      break;
      
    case KNOB_TYPE_LIN:
       increment =  SCROLL_EVENT_PERCENT*(m_fMax - m_fMin);
      break;
      
    case KNOB_TYPE_TIME:
      increment =  SCROLL_EVENT_PERCENT*5.0*(m_Value + 1.0);
      break;  
  }
  
  if (event->direction == GDK_SCROLL_UP) 
  { 
    // up code
    set_value(m_Value + increment);

  } 
  else if (event->direction == GDK_SCROLL_DOWN) 
  { 
    // down code 
    set_value(m_Value - increment);
  }
  m_KnobChangedSignal.emit();
  return true;
}

bool KnobWidget::on_expose_event(GdkEventExpose* event)
{
   Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
  {
  
    Gtk::Allocation allocation = get_allocation();
    width = allocation.get_width();
    height = allocation.get_height();
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

    //Clip inside acording the expose event
    cr->rectangle(event->area.x, event->area.y, event->area.width, event->area.height);
    cr->clip();
    cr->set_source_rgb(BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
    cr->paint(); //Fill all with background color
    
    //Set text
    Glib::RefPtr<Pango::Layout> pangoLayout = Pango::Layout::create(cr);
    Pango::FontDescription font_desc("sans 9px");
    pangoLayout->set_font_description(font_desc);

    cr->move_to(0, height - 22);
    cr->set_source_rgba(0.9, 0.9, 0.9, 1.0);
    pangoLayout->update_from_cairo_context(cr);  //gets cairo cursor position
    pangoLayout->set_text(m_Label);
    pangoLayout->set_width(Pango::SCALE * width);
    pangoLayout->set_alignment(Pango::ALIGN_CENTER);
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();
    
    cr->move_to(0, height - 10);
    cr->set_source_rgba(0.9, 0.9, 0.9, 1.0);
    pangoLayout->update_from_cairo_context(cr);  //gets cairo cursor position
    std::stringstream ss;
    ss.precision(1);
    
    if(m_TypeKnob == KNOB_TYPE_FREQ && m_Value >= 1000.0)
    {   
      ss<<std::fixed<<m_Value/1000.0<<" k"<<m_Units;
    }
    else if(m_TypeKnob == KNOB_TYPE_TIME && m_Value >= 1000.0)
    {   
      ss<<std::fixed<<m_Value/1000.0<<" s";
    }
    else
    {
      ss<<std::fixed<<m_Value<<" "<<m_Units;
    }
    pangoLayout->set_text(ss.str());
    pangoLayout->set_width(Pango::SCALE * width);
    pangoLayout->set_alignment(Pango::ALIGN_CENTER);
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();
    cr->save();
    
    //Draw circle background white
    cr->scale(width,height);
    cr->set_source_rgba(0.8,0.8,0.8,1.0);
    cr->set_line_width(0.10);
    cr->arc(KNOB_CENTER_X, KNOB_CENTER_Y, KNOB_RADIUS, 0.74 * PI, 0.26 * PI); 
    cr->stroke();
    
    //Draw circle background black
    cr->set_source_rgba(0.0,0.0,0.0,1.0);
    cr->set_line_width(0.07);
    cr->arc(KNOB_CENTER_X, KNOB_CENTER_Y, KNOB_RADIUS, 0.75 * PI, 0.25 * PI); 
    cr->stroke();
    

    //Calc konb angle (pos)
    double pos, m, n;
    switch(m_TypeKnob)
    {
      case KNOB_TYPE_FREQ:
      case KNOB_TYPE_TIME:
        m = (1.48*PI)/log10(m_fMax/m_fMin);
        n = 0.76*PI;
        pos = m*log10(m_Value/m_fMin) + n;
        break;
        
      case KNOB_TYPE_LIN:
        m = (1.48*PI)/(m_fMax-m_fMin);
        n = 0.76*PI  - m*m_fMin;
        pos = m*m_Value + n; 
        break;
    }
    
    //Draw colored circle
    cr->set_source_rgba(0.0,0.8,0.2,1.0);
    cr->set_line_width(0.05);
    cr->arc(KNOB_CENTER_X, KNOB_CENTER_Y, KNOB_RADIUS, 0.76 * PI, pos); 
    cr->stroke();
   
    //Draw Knob Cursor
    double vx = (KNOB_RADIUS - 0.02) * cos(pos);
    double vy = (KNOB_RADIUS - 0.02) * sin(pos);
    
    //point to cursor
    double pc_x = vx + KNOB_CENTER_X;
    double pc_y = vy + KNOB_CENTER_Y;
    
    //Cursor midpoint
    double  vpx, vpy, mod;
    vpx = vx;
    vpy = vy;
    mod = sqrt(vpx*vpx + vpy*vpy);
    vpx /= mod;
    vpy /= mod;
    vpx *= CURSOR_LENGHT;
    vpy *= CURSOR_LENGHT;
    double pm_x = pc_x - vpx;
    double pm_y = pc_y - vpy;
    
    //point a triangle 1
    vpx = -vy;
    vpy = vx;
    mod = sqrt(vpx*vpx + vpy*vpy);
    vpx /= mod;
    vpy /= mod;
    vpx *= CURSOR_TRIANGLE_BASE;
    vpy *= CURSOR_TRIANGLE_BASE;
    double p1_x = pm_x + vpx;
    double p1_y = pm_y + vpy;
    
    //point a triangle 2
    vpx = vy;
    vpy = -vx;
    mod = sqrt(vpx*vpx + vpy*vpy);
    vpx /= mod;
    vpy /= mod;
    vpx *= CURSOR_TRIANGLE_BASE;
    vpy *= CURSOR_TRIANGLE_BASE;
    double p2_x = pm_x + vpx;
    double p2_y = pm_y + vpy;
    
    cr->set_source_rgba(0.8,0.8,0.8,1.0);
    cr->move_to(pc_x, pc_y);
    cr->line_to(p1_x, p1_y);
    cr->line_to(p2_x, p2_y);
    cr->line_to(pc_x, pc_y);
    cr->fill();

    cr->restore();
  
  }
  return true;
}
