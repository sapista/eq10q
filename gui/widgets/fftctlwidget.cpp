/***************************************************************************
 *   Copyright (C) 2015 by Pere Ràfols Soler                               *
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
 
#include "fftctlwidget.h"
#include "colors.h"
#include "toggle_button.h" //To draw the LED using a static function
 
#define MARGIN 12
#define BUTTON_HEIGHT 10
#define MIN_WIDTH 60
#define SLIDER_DIAMITER 14
#define FFT_BTN_RADIUS 3
#define SCROLL_EVENT_PERCENT 0.02

FFTWidget::FFTWidget(double min, double max):
m_max(max), m_min(min), m_bEnabled(false), m_bSlider_Focus(false), m_bSlider_Press(false), m_bBtn_Foucs(false), m_bBtn_Press(false), m_bIsSpectrogram(false)
{
  m_value = 0.5*(m_max - m_min) + m_min; //Start with the middle value
  
  set_size_request(MIN_WIDTH, 3*MARGIN + BUTTON_HEIGHT + SLIDER_DIAMITER + MIN_WIDTH);
  
  //Connect mouse signals
  add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK | Gdk::SCROLL_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &FFTWidget::on_button_press_event),true);
  signal_button_release_event().connect(sigc::mem_fun(*this, &FFTWidget::on_button_release_event),true);
  signal_scroll_event().connect(sigc::mem_fun(*this, &FFTWidget::on_scrollwheel_event),true);
  signal_motion_notify_event().connect(sigc::mem_fun(*this, &FFTWidget::on_mouse_motion_event),true);
}

FFTWidget::~FFTWidget()
{

}

void FFTWidget::set_value(double val)
{
  m_value = val;
  m_value = m_value > m_max ? m_max : m_value;
  m_value = m_value < m_min ? m_min : m_value;
  redraw();
}

double FFTWidget::get_value()
{
  return m_value;
}

bool FFTWidget::get_active()
{
  return m_bEnabled;
}

bool FFTWidget::get_isSpectrogram()
{
  return m_bIsSpectrogram;
}


FFTWidget::signal_Changed FFTWidget::signal_changed()
{
  return m_ChangedSignal;
}

FFTWidget::signal_Changed_btnClicked FFTWidget::signal_clicked()
{
  return m_ClickSignal;
}


void FFTWidget::redraw()
{
  Glib::RefPtr<Gdk::Window> win = get_window();
  if(win)
  {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(), get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
}

bool FFTWidget::on_button_press_event(GdkEventButton* event)
{
  if(event->button == 1)
  {
    //Click on slider
    if(m_bSlider_Focus)
    {
      m_bSlider_Press = true;
      redraw();
    }
    
    //Click on button enable
    if(m_bBtn_Foucs)
    {
      m_bBtn_Press = true;
      if(m_bEnabled)
      {
        if(m_bIsSpectrogram)
        {
          m_bEnabled = false;
          m_bIsSpectrogram = false;
        }
        else
        {
          m_bIsSpectrogram = true;
        }
      }
      else
      {
        m_bEnabled = true;
      }
      m_ClickSignal.emit();
      redraw();
    }
  }
  return true;
}

bool FFTWidget::on_button_release_event(GdkEventButton* event)
{
  m_bSlider_Press = false;
  m_bBtn_Press = false;
  redraw();
  return true;
}

#include <iostream>
bool FFTWidget::on_mouse_motion_event(GdkEventMotion* event)
{
  m_bSlider_Focus = false;
  m_bBtn_Foucs = false;
  
  if(m_bSlider_Press)
  {
    m_bSlider_Focus = true;
    set_value(Pixels2Val(event->y));
    m_ChangedSignal.emit();
  }
  else if( (event->x > (2*width/3 - SLIDER_DIAMITER/2)) &&
           (event->x < (2*width/3 + SLIDER_DIAMITER/2)) &&
           (event->y > (Val2Pixels(m_value) - SLIDER_DIAMITER/2)) && 
           (event->y < (Val2Pixels(m_value) + SLIDER_DIAMITER/2)) )
  {
    m_bSlider_Focus = true; 
  }
  else if( (event->x > (MARGIN)) &&
           (event->x < (MARGIN + 4*FFT_BTN_RADIUS)) &&     
           (event->y > (MARGIN)) &&  
           (event->y < (MARGIN + 4*FFT_BTN_RADIUS)) )
  {
    m_bBtn_Foucs = true;
  }
  
  redraw();
  return true;
}

bool FFTWidget::on_scrollwheel_event(GdkEventScroll* event)
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
   m_ChangedSignal.emit();
  return true;
}

double FFTWidget::Val2Pixels(double val)
{
  double m = (double)(3.0*MARGIN + 2*SLIDER_DIAMITER + BUTTON_HEIGHT - height)/(m_max - m_min);
  double n = (double)(height - SLIDER_DIAMITER - MARGIN) - m*m_min;
  return m*val + n;
}

double FFTWidget::Pixels2Val(double px)
{
  double m = (double)(3.0*MARGIN + 2*SLIDER_DIAMITER + BUTTON_HEIGHT - height)/(m_max - m_min);
  double n = (double)(height - SLIDER_DIAMITER - MARGIN) - m*m_min;
  return (px - n)/m;
}


bool FFTWidget::on_expose_event(GdkEventExpose* event)
{
   Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
  {
    Gtk::Allocation allocation = get_allocation();
    width = allocation.get_width();
    height = allocation.get_height();
    
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();


    //Draw fader backgroud rectangle and paint it
    cr->save();
    cr->set_source_rgb(BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
    cr->paint(); //Fill all with background color
    cr->restore();
    
    
    //Draw a border to all widget
    cr->save();         
    cr->begin_new_sub_path();
    cr->arc( 5.5, 5.5, 4, M_PI, -0.5*M_PI);
    cr->arc( width - 5.5, 5.5, 4, -0.5*M_PI, 0);
    cr->arc( width - 5.5, height - 5.5, 4, 0, 0.5*M_PI);
    cr->arc( 5.5, height - 5.5, 4, 0.5*M_PI, M_PI);
    cr->close_path();  
    Cairo::RefPtr<Cairo::LinearGradient> bkg_gradientLin_ptr = Cairo::LinearGradient::create(width/2, 2, width/2, height - 2);   
    bkg_gradientLin_ptr->add_color_stop_rgba (0.0, 0.1, 0.2, 0.2, 0.1 );
    bkg_gradientLin_ptr->add_color_stop_rgba (0.7, 0.4, 0.4, 0.4, 0.3 ); 
    
    cr->set_source(bkg_gradientLin_ptr);
    cr->fill_preserve();
    cr->set_source_rgb(0.5, 0.5, 0.5); 
    cr->set_line_width(1.0);
    cr->stroke(); 
    cr->restore();
       
    //Draw fader backgroud line
    cr->save();
    cr->set_line_cap(Cairo::LINE_CAP_ROUND);
    cr->move_to(round(2*width/3) + 0.5, height - SLIDER_DIAMITER - MARGIN); 
    cr->line_to(round(2*width/3) + 0.5, 2*MARGIN + SLIDER_DIAMITER + BUTTON_HEIGHT);   
    cr->set_source_rgba(0.7, 0.7, 0.7, 0.7);
    cr->set_line_width(3);
    cr->stroke_preserve();
    cr->set_source_rgba(0.15, 0.15, 0.15, 1.0);
    cr->set_line_width(2);
    cr->stroke();
    cr->restore();
    
    Cairo::RefPtr<Cairo::RadialGradient> bkg_gradient_ptr;
    
    //Slider = control ball + shadow
    cr->save();
    cr->translate(round(2*width/3) + 0.5, round(Val2Pixels(m_value)) + 0.5);
    
    //Drop down shadow
    cr->save();
    cr->translate(3, 3);
    cr->arc( 0.0, 0.0, SLIDER_DIAMITER/2.0, 0.0, 2.0*M_PI);   
    bkg_gradient_ptr = Cairo::RadialGradient::create(0, 0, 0, -2, -2, SLIDER_DIAMITER/2);
    bkg_gradient_ptr->add_color_stop_rgba (0.5, 0.2, 0.2, 0.2, 1.0); 
    bkg_gradient_ptr->add_color_stop_rgba (1.0, 0.1, 0.1, 0.2, 0.0); 
    cr->set_source(bkg_gradient_ptr);  
    cr->fill();
    cr->restore();
    
    //Control ball
    cr->arc( 0.0, 0.0, SLIDER_DIAMITER/2.0, 0.0, 2.0*M_PI);   
    bkg_gradient_ptr = Cairo::RadialGradient::create(-2, -2, 0, -2, -2, SLIDER_DIAMITER/2);
    bkg_gradient_ptr->add_color_stop_rgba (0.2, 0.5, 0.5, 0.7, 1.0); 
    bkg_gradient_ptr->add_color_stop_rgba (1.0, 0.9, 0.9, 1.0, 0.9); 
    cr->set_source(bkg_gradient_ptr);  
    cr->fill_preserve();
    if(m_bSlider_Focus)
    {
      cr->set_source_rgba(0.0, 1.0, 1.0, 0.1);
      cr->fill_preserve();
    }
    cr->set_line_width(1.0);
    cr->set_source_rgba(0.1, 0.1, 0.1, 1.0);
    cr->stroke();
    cr->restore();
    
    //Draw a nice triangle at left side of slider
    cr->save();   
    cr->begin_new_sub_path();
    cr->arc(width/3 + 0.5,  2*MARGIN + 2*SLIDER_DIAMITER + BUTTON_HEIGHT + 0.5, 3, M_PI, 0.0);
    cr->line_to(width/3 + 0.5 + 1, height - SLIDER_DIAMITER - MARGIN + 0.5);
    cr->line_to(width/3 + 0.5 - 1, height - SLIDER_DIAMITER - MARGIN + 0.5);
    cr->close_path();
    cr->set_source_rgba(0.6, 0.7, 0.8, 0.3);
    cr->fill_preserve();
    cr->set_line_width(1.0);
    cr->set_source_rgba(0.2, 0.2, 0.2, 0.9);
    cr->stroke();
    cr->restore();
   
    double red = 0.8;
    double green = 0.8;
    double blue = 0.5;
    if(m_bIsSpectrogram)
    {
      green = 0.0;
      blue = 0.2;
    }
    ToggleButton::drawLedBtn(cr, m_bBtn_Foucs, m_bEnabled, "FFT", MARGIN, FFT_BTN_RADIUS, red, green, blue);
  }
  return true;
}
