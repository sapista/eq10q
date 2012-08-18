/***************************************************************************
 *   Copyright (C) 2012 by Pere Ràfols Soler                               *
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
#include <iomanip>
#include <cstring>

#include "colors.h"
#include "faderwidget.h"

FaderWidget::FaderWidget(double dMax, double dMin, const char *bundlePath)
  :m_max(dMax), m_min(dMin), m_value(0), bMotionIsConnected(false), m_bundlePath(bundlePath)
{
  m_image_surface_ptr = Cairo::ImageSurface::create_from_png (m_bundlePath + "/" + std::string(FADER_ICON_FILE));
  set_size_request(2*m_image_surface_ptr->get_width()+4*FADER_MARGIN, FADER_INITAL_HIGHT);
  
  //Connect mouse signals
  add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK | Gdk::SCROLL_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &FaderWidget::on_button_press_event),true);
  signal_button_release_event().connect(sigc::mem_fun(*this, &FaderWidget::on_button_release_event),true);
  signal_scroll_event().connect(sigc::mem_fun(*this, &FaderWidget::on_scrollwheel_event),true);
}
    
FaderWidget::~FaderWidget()
{
}


void FaderWidget::set_value(double value)
{
  m_value = value;
  m_value = m_value < m_min ? m_min : m_value;
  m_value = m_value > m_max ? m_max : m_value;
  redraw();
}

double FaderWidget::get_value()
{
  return m_value;
}

void FaderWidget::set_range(double max, double min)
{
  m_max = max;
  m_min = min;
  redraw();
}

double FaderWidget::get_max()
{
  return m_max;
}

double FaderWidget::get_min()
{
  return m_min;
}

void FaderWidget::redraw()
{
  Glib::RefPtr<Gdk::Window> win = get_window();
  if(win)
  {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(), get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
}

//Override default signal handler:
bool FaderWidget::on_expose_event(GdkEventExpose* event)
{
  double m, n;
  
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
  {
  
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    m = ((double)(FADER_MARGIN - (height - FADER_MARGIN - m_image_surface_ptr->get_height())))/(m_max - m_min);
    n = (double)(height - FADER_MARGIN - m_image_surface_ptr->get_height()) - m_min*m;
    
    yFaderPosition = (int)(m*m_value + n);
    
      ///std::cout<<"FaderWidget m = "<<m<<"\t n = "<<n<<"\t y = "<<yFaderPosition<<std::endl;
    
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

    //Draw fader backgroud rectangle and paint it
    cr->save();
    cr->set_source_rgb(BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
    cr->paint(); //Fill all with background color
    cr->restore();
    
    
    //Draw fader backgroud line
    cr->save();
    cr->set_source_rgb(0.8, 0.8, 0.9);
    cr->set_line_width(3);
    cr->move_to(width/2, FADER_MARGIN + m_image_surface_ptr->get_height()/2); 
    cr->line_to(width/2, height - FADER_MARGIN - m_image_surface_ptr->get_height()/2);
    cr->stroke();
    cr->restore();
    
    //Draw fader dB scale
    int yBarPosition;
    
    //Draw thin lines for each dB
    cr->save();
    cr->set_source_rgb(1, 1, 1);
    cr->set_line_width(0.5);
    for (double i = m_max; i >= m_min; i--)  //The var step size is one dBu
    {
      yBarPosition = (int)(m*i + n) +  m_image_surface_ptr->get_height()/2;
      cr->move_to(width/2 - m_image_surface_ptr->get_width()/3 - FADER_MARGIN, yBarPosition); 
      cr->line_to(width/2 - FADER_MARGIN, yBarPosition);
      cr->move_to(width/2 + FADER_MARGIN, yBarPosition); 
      cr->line_to(width/2 + m_image_surface_ptr->get_width()/3 + FADER_MARGIN, yBarPosition);
    }
    cr->stroke();
    cr->restore();
    
    
    //Draw text with pango
    cr->save();
    cr->set_source_rgb(1, 1, 1);
    Glib::RefPtr<Pango::Layout> pangoLayout = Pango::Layout::create(cr);
    Pango::FontDescription font_desc("sans 7");
    pangoLayout->set_font_description(font_desc);
    pangoLayout->set_alignment(Pango::ALIGN_RIGHT);
    ///pangoLayout->update_from_cairo_context(cr);  //gets cairo cursor position
    for (double i = m_max; i >= m_min; i -= m_max/2)  //The var step size is the half of max value because I like this way ;-)
    {
      std::stringstream ss;
      yBarPosition = (int)(m*i + n) +  m_image_surface_ptr->get_height()/2;
      ss<< std::setprecision(3) << i;
      cr->move_to(width/2 - m_image_surface_ptr->get_width()/2 - 3*FADER_MARGIN, yBarPosition - 14);
      pangoLayout->set_text(ss.str());
      pangoLayout->show_in_cairo_context(cr);
      cr->stroke();  
    }
    cr->restore();
   
    //Draw strong lines with labels
    cr->save();
    cr->set_source_rgb(1, 1, 1);
    cr->set_line_width(1.5);
    for (double i = m_max; i >= m_min; i -= m_max/2)  //The var step size is the hlaf of max because I like this way
    {
      yBarPosition = (int)(m*i + n) +  m_image_surface_ptr->get_height()/2;
      cr->move_to(width/2 - m_image_surface_ptr->get_width() + FADER_MARGIN, yBarPosition); 
      cr->line_to(width/2 - FADER_MARGIN, yBarPosition);
      cr->move_to(width/2 + FADER_MARGIN, yBarPosition); 
      cr->line_to(width/2 + m_image_surface_ptr->get_width() - FADER_MARGIN, yBarPosition);
    }
    cr->stroke();
    cr->restore();
    
    //Draw the fader icon
    cr->save();
    cr->set_source (m_image_surface_ptr, width/2 - m_image_surface_ptr->get_width()/2, yFaderPosition);
    cr->rectangle (width/2 - m_image_surface_ptr->get_width()/2, yFaderPosition, m_image_surface_ptr->get_width()+1,  m_image_surface_ptr->get_height()+1);
    cr->clip();
    cr->paint();
    cr->restore();
  }
  return true;  
}


//Mouse grab signal handlers
bool  FaderWidget::on_button_press_event(GdkEventButton* event)
{
  //Act in case mouse pointer is inside faderwidget
  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();
  int x,y;
  get_pointer(x,y);
  if( x > width/2 - m_image_surface_ptr->get_width()/2 &&
      x < width/2 + m_image_surface_ptr->get_width()/2 &&
      y > yFaderPosition &&
      y < yFaderPosition + m_image_surface_ptr->get_height())
  {

    if (!bMotionIsConnected)
    {
      m_motion_connection = signal_motion_notify_event().connect(sigc::mem_fun(*this, &FaderWidget::on_mouse_motion_event),true);
      bMotionIsConnected = true;
    }
  }
}

bool  FaderWidget::on_button_release_event(GdkEventButton* event)
{
  m_motion_connection.disconnect();
  bMotionIsConnected = false;
}

bool  FaderWidget::on_scrollwheel_event(GdkEventScroll* event)
{
  double increment;
  
  increment =  SCROLL_EVENT_PERCENT*(m_max - m_min);
  if (event->direction == GDK_SCROLL_UP) 
  { 
    // up code
    set_value (m_value + increment);

  } 
  else if (event->direction == GDK_SCROLL_DOWN) 
  { 
    // down code 
    set_value(m_value - increment);
  }
  m_FaderChangedSignal.emit();
}

bool  FaderWidget::on_mouse_motion_event(GdkEventMotion* event)
{
    int yPixels;
    double m, n;
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    yPixels = event->y - m_image_surface_ptr->get_height()/2; //Offset fader icon to grab the center of the fader

    //Stoppers
    yPixels = yPixels < FADER_MARGIN ? FADER_MARGIN : yPixels;
    yPixels = yPixels > height - FADER_MARGIN - m_image_surface_ptr->get_height() ? height - FADER_MARGIN - m_image_surface_ptr->get_height() : yPixels;

    
    m = ((double)(FADER_MARGIN - (height - FADER_MARGIN - m_image_surface_ptr->get_height())))/(m_max - m_min);
    n = (double)(height - FADER_MARGIN - m_image_surface_ptr->get_height()) - m_min*m;
    
    set_value(((double)yPixels - n)/m);
    m_FaderChangedSignal.emit();
}

 FaderWidget::signal_FaderChanged  FaderWidget::signal_changed()
 {
  return m_FaderChangedSignal;
 }