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

#include "colors.h"
#include "vuwidget.h"

#define TEXT_OFFSET 12
#define MARGIN 6.5
#define CHANNEL_WIDTH 9
#define MICROFADER_WIDTH 30
#define TEXT_DB_SEPARATION 3.0
#define SCROLL_EVENT_PERCENT 0.02
#define WIDGET_HEIGHT 150
#define TOP_OFFSET 24

VUWidget::VUWidget(int iChannels, float fMin, float fMax, std::string title, bool IsGainReduction, bool DrawThreshold) 
  :m_iChannels(iChannels),
  m_fMin(fMin),
  m_fMax(fMax),
  m_bIsGainReduction(IsGainReduction),
  bMotionIsConnected(false),
  m_fValues(new float[m_iChannels]),
  m_fPeaks(new float[m_iChannels]),
  m_ThFaderValue(0.0),
  m_iThFaderPositon(0),
  m_bDrawThreshold(DrawThreshold),
  m_start(new timeval[m_iChannels]),
  m_end(new timeval[m_iChannels]),
  m_Title(title)
{
  
  for (int i = 0; i < m_iChannels; i++)
  {
    m_fValues[i] = -100.0;
    m_fPeaks[i] = -100.0;
  }
  
  int widget_witdh;
  if(m_bDrawThreshold)
  {   
    widget_witdh = MARGIN + TEXT_OFFSET +  (MARGIN + CHANNEL_WIDTH)* m_iChannels + MICROFADER_WIDTH/2 + MARGIN + 2;
  }
  else
  {
    widget_witdh = MARGIN + TEXT_OFFSET +  (MARGIN + CHANNEL_WIDTH)* m_iChannels;
  }
  set_size_request(widget_witdh, WIDGET_HEIGHT);
 
   
  //Initialize peak time counters
  for (int i = 0; i < m_iChannels; i++)
  {
    gettimeofday(&m_start[i], NULL);
    gettimeofday(&m_end[i], NULL);
  } 

  //The micro fader for threshold
  add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK | Gdk::SCROLL_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &VUWidget::on_button_press_event),true);
  signal_button_release_event().connect(sigc::mem_fun(*this, &VUWidget::on_button_release_event),true);
  signal_scroll_event().connect(sigc::mem_fun(*this, &VUWidget::on_scrollwheel_event),true);
}

VUWidget::~VUWidget()
{
  delete [] m_fValues;
  delete [] m_fPeaks;
  delete [] m_start;
  delete [] m_end;
}
  
void VUWidget::setValue(int iChannel, float fValue)
{ 
  long mtime, seconds, useconds;
  gettimeofday(&m_end[iChannel], NULL);
  
  seconds  = m_end[iChannel].tv_sec  - m_start[iChannel].tv_sec;
  useconds = m_end[iChannel].tv_usec - m_start[iChannel].tv_usec;
  mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
  
  if (fValue > 0)
  {
     m_fValues[iChannel] = 20.0*log10(fValue);
  }
  else
  {
     m_fValues[iChannel] = -100.0;
  }
  
  //Clip max
  m_fValues[iChannel] =  m_fValues[iChannel] > m_fMax ? m_fMax :  m_fValues[iChannel];
  
  if (m_fValues[iChannel] >= m_fPeaks[iChannel])
  {
    m_fPeaks[iChannel] = m_fValues[iChannel];
    gettimeofday(&m_start[iChannel] , NULL);
  }
  
  else if (mtime > PEAK_CLEAR_TIMEOUT)
  {
    m_fPeaks[iChannel] = -100.0;
  }
  redraw();
}

void VUWidget::clearPeak(int iChannel)
{
  m_fPeaks[iChannel] = 0.0;
} 

void VUWidget::redraw()
{
  Glib::RefPtr<Gdk::Window> win = get_window();
  if(win)
  {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(), get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
}

double VUWidget::dB2Pixels(double dB_in)
{
  double m, n;
  if(m_bIsGainReduction)
  {   
    m = ((double)(height - 3.0*MARGIN - TOP_OFFSET))/(m_fMax - m_fMin);
    n = (double)(MARGIN + TOP_OFFSET) - m_fMin*m;
  }
  else
  {
    m = ((double)(3.0*MARGIN + TOP_OFFSET - height ))/(m_fMax - m_fMin);
    n = (double)(height - 2.0*MARGIN) - m_fMin*m;
  }
  return m*dB_in + n;
}


bool VUWidget::on_expose_event(GdkEventExpose* event)
{
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
  {
  
    Gtk::Allocation allocation = get_allocation();
    width = allocation.get_width();
    height = allocation.get_height();

    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

    //Paint background
    cr->save();
    cr->set_source_rgb(BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
    cr->paint(); //Fill all with background color
    cr->restore();
    
    //Draw text with pango
    cr->save();
    Glib::RefPtr<Pango::Layout> pangoLayout = Pango::Layout::create(cr);
    Pango::FontDescription font_desc("mono 9px");
    pangoLayout->set_font_description(font_desc);
    cr->set_source_rgba(0.9, 0.9, 0.9, 0.5);
    
    cr->move_to(MARGIN + TEXT_OFFSET - 3, TOP_OFFSET/2);//4 is to get the text centered in VU
    pangoLayout->set_text(m_Title.c_str());
    pangoLayout->set_width(Pango::SCALE * (CHANNEL_WIDTH * m_iChannels + (m_iChannels - 1)*MARGIN));
    pangoLayout->set_alignment(Pango::ALIGN_CENTER);
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();  
    
    for(float fdb = m_fMin; fdb <= m_fMax; fdb = fdb + TEXT_DB_SEPARATION)
    {
      std::stringstream ss;
      ss<<abs(round(fdb));
      cr->move_to(MARGIN, dB2Pixels(fdb) - 4);//4 is to get the text centered in VU
      pangoLayout->set_text(ss.str());
      pangoLayout->set_width(Pango::SCALE * (TEXT_OFFSET - MARGIN));
      pangoLayout->set_alignment(Pango::ALIGN_RIGHT);
      pangoLayout->show_in_cairo_context(cr);
      cr->stroke();     
    }
    cr->restore();
    
    //Draw VU rectangle
    double radius = height / 100.0;
    double degrees = M_PI / 180.0;
    for(int i = 0; i < m_iChannels; i++)
    {
      cr->save();         
      cr->begin_new_sub_path();
      cr->arc (MARGIN + TEXT_OFFSET + CHANNEL_WIDTH + i*(MARGIN + CHANNEL_WIDTH + 0.5) - radius, MARGIN + TOP_OFFSET - 4 + radius, radius, -90 * degrees, 0 * degrees);
      cr->arc (MARGIN + TEXT_OFFSET + CHANNEL_WIDTH + i*(MARGIN + CHANNEL_WIDTH + 0.5) - radius, height - 1 - MARGIN - radius, radius, 0 * degrees, 90 * degrees);
      cr->arc (MARGIN + TEXT_OFFSET + i*(MARGIN + CHANNEL_WIDTH + 0.5) + radius, height - 1 - MARGIN - radius, radius, 90 * degrees, 180 * degrees);
      cr->arc (MARGIN + TEXT_OFFSET + i*(MARGIN + CHANNEL_WIDTH + 0.5) + radius, MARGIN + TOP_OFFSET - 4 + radius, radius, 180 * degrees, 270 * degrees);
      cr->close_path();
      cr->set_source_rgb(0.15, 0.15, 0.15);
      cr->fill_preserve();
      cr->set_line_width(1.0);
      cr->set_source_rgb(0.5, 0.5, 0.5);
      cr->stroke();
      cr->restore();
    }
    
    //New draw value
    Cairo::RefPtr<Cairo::LinearGradient> bkg_gradient_ptr;

    //Draw the VU
    for(int i = 0; i < m_iChannels; i++)
    {
      cr->save();
      cr->set_line_width(CHANNEL_WIDTH - 4);
      cr->set_line_cap(Cairo::LINE_CAP_ROUND);
      bkg_gradient_ptr = Cairo::LinearGradient::create(MARGIN + TEXT_OFFSET + CHANNEL_WIDTH/2.0 + i*(MARGIN + CHANNEL_WIDTH + 0.5), dB2Pixels(m_fMin), MARGIN + TEXT_OFFSET + CHANNEL_WIDTH/2.0 + i*(MARGIN + CHANNEL_WIDTH + 0.5), dB2Pixels(m_fMax));   
      if(m_bIsGainReduction)
      {
        bkg_gradient_ptr->add_color_stop_rgba (0.0, 1.0, 0.5, 0.0, 0.0); 
        bkg_gradient_ptr->add_color_stop_rgba (0.01, 1.0, 0.5, 0.0, 1.0); 
        bkg_gradient_ptr->add_color_stop_rgba (1.0, 1.0, 0.0, 0.0, 1.0); 
      }
      else
      {
        bkg_gradient_ptr->add_color_stop_rgba (0.0, 0.0, 1.0, 0.0, 0.0); 
        bkg_gradient_ptr->add_color_stop_rgba (0.01, 0.0, 1.0, 0.0, 1.0); 
        bkg_gradient_ptr->add_color_stop_rgba (0.5, 1.0, 1.0, 0.0, 1.0); 
        bkg_gradient_ptr->add_color_stop_rgba (1.0, 1.0, 0.0, 0.0, 1.0); 
      }
      cr->set_source(bkg_gradient_ptr);
            
      //The VU
      if(m_fValues[i] >= m_fMin)
      {
        cr->move_to(MARGIN + TEXT_OFFSET + CHANNEL_WIDTH/2.0 + i*(MARGIN + CHANNEL_WIDTH + 0.5), dB2Pixels(m_fMin));
        cr->line_to(MARGIN + TEXT_OFFSET + CHANNEL_WIDTH/2.0 + i*(MARGIN + CHANNEL_WIDTH + 0.5), dB2Pixels(m_fValues[i]));
        cr->stroke();
      }
      
      //The peak
      if(m_fPeaks[i] >= m_fMin)
      {
        cr->move_to(MARGIN + TEXT_OFFSET + CHANNEL_WIDTH/2.0 + i*(MARGIN + CHANNEL_WIDTH + 0.5), dB2Pixels(m_fPeaks[i]));
        cr->line_to(MARGIN + TEXT_OFFSET + CHANNEL_WIDTH/2.0 + i*(MARGIN + CHANNEL_WIDTH + 0.5), dB2Pixels(m_fPeaks[i]));
        cr->stroke();
        cr->restore();
      }
    }
           
    //Draw some horitzontal lines to show dB scale over Vu
    cr->save();
    cr->set_line_width(1.0);
    cr->set_source_rgba(0.8, 0.8, 0.8, 0.4);
    for(float fdb = m_fMin; fdb <= m_fMax; fdb = fdb + TEXT_DB_SEPARATION)
    {
      cr->move_to(MARGIN + TEXT_OFFSET - 2, round(dB2Pixels(fdb)) + 0.5);
      cr->line_to(MARGIN + TEXT_OFFSET + CHANNEL_WIDTH + (m_iChannels - 1 ) * (CHANNEL_WIDTH + MARGIN ) + 2, round(dB2Pixels(fdb)) + 0.5);
      cr->stroke();     
    }
    cr->restore();
    
    
    //Draw Threshold MicroFader  
    if(m_bDrawThreshold)
    {
      //Draw vertical line
      cr->save();     
      cr->move_to(width - MICROFADER_WIDTH/2 + 0.5, dB2Pixels(m_fMin + 2.0));      
      cr->line_to(width - MICROFADER_WIDTH/2 + 0.5, dB2Pixels(m_fMax - 2.0));
      
      cr->set_line_cap(Cairo::LINE_CAP_ROUND);
      cr->set_line_width(3.0);
      cr->set_source_rgba(0.7, 0.7, 0.7, 0.5);
      cr->stroke_preserve();
      cr->set_source_rgba(0.15, 0.15, 0.15, 1.0);
      cr->set_line_width(1);
      cr->stroke();
      
      
      cr->stroke();
      
      //Draw threshold text with pango
      Glib::RefPtr<Pango::Layout> pangoLayout_th = Pango::Layout::create(cr);
      Pango::FontDescription font_desc_th("sans bold 8px");
      font_desc_th.set_gravity(Pango::GRAVITY_EAST);
      pangoLayout_th->set_font_description(font_desc_th);
      pangoLayout_th->set_alignment(Pango::ALIGN_LEFT);
      cr->move_to(width - MICROFADER_WIDTH/2 - 10, height - MICROFADER_WIDTH/2 - 85);
      cr->set_source_rgba(0.9, 0.9, 0.9, 0.7);
      pangoLayout_th->update_from_cairo_context(cr);  //gets cairo cursor position
      pangoLayout_th->set_text("d\r\nl\r\no\r\nh\r\ns\r\ne\r\nr\r\nh\r\nT");
      pangoLayout_th->show_in_cairo_context(cr);
      cr->stroke();
           
      //Calc coords for mini fader
      m_iThFaderPositon = (int) dB2Pixels(m_ThFaderValue);
      
      //Draw the fader drop down shadow
      cr->save();    
      cr->translate(width - MICROFADER_WIDTH/2 + 2,  m_iThFaderPositon + 4);
      cr->scale(MICROFADER_WIDTH/2, MICROFADER_WIDTH/4);
      Cairo::RefPtr<Cairo::RadialGradient> bkg_gradient_rad_ptr = Cairo::RadialGradient::create(0, 0, 0, 0, 0, 1);
      bkg_gradient_rad_ptr->add_color_stop_rgba (0.3, 0.2, 0.2, 0.2, 1.0); 
      bkg_gradient_rad_ptr->add_color_stop_rgba (1.0, 0.1, 0.1, 0.2, 0.0); 
      cr->set_source(bkg_gradient_rad_ptr);  
      cr->arc(0.0, 0.0, 1.0, 0.0, 2.0*M_PI);
      cr->fill();
      cr->restore();
      
      //Draw Threshold fader
      cr->begin_new_sub_path();
      cr->arc(width - 2 - MICROFADER_WIDTH/4, m_iThFaderPositon + 0.5, MICROFADER_WIDTH/4,  -90 * degrees, 90 * degrees);
      cr->line_to( width - 2 - MICROFADER_WIDTH/2, m_iThFaderPositon + MICROFADER_WIDTH/4 + 0.5);
      cr->line_to( width - 2 - MICROFADER_WIDTH, m_iThFaderPositon + 0.5);
      cr->line_to( width - 2 - MICROFADER_WIDTH/2, m_iThFaderPositon - MICROFADER_WIDTH/4 + 0.5);
      cr->close_path();
      bkg_gradient_ptr = Cairo::LinearGradient::create(width - 2 - MICROFADER_WIDTH/2, m_iThFaderPositon - MICROFADER_WIDTH/4, width - 2 - MICROFADER_WIDTH/2, m_iThFaderPositon + MICROFADER_WIDTH/4);
      bkg_gradient_ptr->add_color_stop_rgba (0.3, 0.8, 0.8, 0.85, 1.0); 
      bkg_gradient_ptr->add_color_stop_rgba (1.0, 0.2, 0.2, 0.25, 1.0); 
      cr->set_source(bkg_gradient_ptr);
      cr->fill_preserve();   
      cr->set_source_rgba(0.1, 0.1, 0.1, 0.7);
      cr->set_line_width(1.0);
      cr->stroke();
      
      cr->move_to(width - 2 - 5*MICROFADER_WIDTH/8, m_iThFaderPositon + 0.5);
      cr->line_to(width - 4 - MICROFADER_WIDTH/8, m_iThFaderPositon + 0.5);
      cr->move_to(width - 2 - 5*MICROFADER_WIDTH/8, m_iThFaderPositon + 0.5 - 2);
      cr->line_to(width - 4 - MICROFADER_WIDTH/8, m_iThFaderPositon + 0.5 - 2);
      cr->move_to(width - 2 - 5*MICROFADER_WIDTH/8, m_iThFaderPositon + 0.5 + 2);
      cr->line_to(width - 4 - MICROFADER_WIDTH/8, m_iThFaderPositon + 0.5 + 2);
      cr->set_source_rgba(0.0, 0.0, 0.0, 0.2);
      cr->set_line_width(1.0);
      cr->stroke();
    }     
  }
  return true; 
}

void VUWidget::set_value_th(double value)
{
  m_ThFaderValue = value;
  m_ThFaderValue = m_ThFaderValue < m_fMin + 2.0 ? m_fMin + 2.0 : m_ThFaderValue;
  m_ThFaderValue = m_ThFaderValue > m_fMax - 2.0 ? m_fMax - 2.0 : m_ThFaderValue; //Limit threshols 2dB less than VU
  redraw();
}

double VUWidget::get_value_th()
{
  return m_ThFaderValue;
}

//Mouse grab signal handlers
bool  VUWidget::on_button_press_event(GdkEventButton* event)
{
  int x,y;
  get_pointer(x,y);
  if( y > m_iThFaderPositon - MICROFADER_WIDTH/2 &&
      y < m_iThFaderPositon + MICROFADER_WIDTH/2)
  {

    if (!bMotionIsConnected)
    {
      m_motion_connection = signal_motion_notify_event().connect(sigc::mem_fun(*this, &VUWidget::on_mouse_motion_event),true);
      bMotionIsConnected = true;
    }
  }
  return true;
}

bool  VUWidget::on_button_release_event(GdkEventButton* event)
{
  m_motion_connection.disconnect();
  bMotionIsConnected = false;
  return true;
}

bool  VUWidget::on_scrollwheel_event(GdkEventScroll* event)
{
  double increment;
  
  increment =  SCROLL_EVENT_PERCENT*(m_fMax - m_fMin);
  if (event->direction == GDK_SCROLL_UP) 
  { 
    // up code
    set_value_th(m_ThFaderValue + increment);

  } 
  else if (event->direction == GDK_SCROLL_DOWN) 
  { 
    // down code 
    set_value_th(m_ThFaderValue - increment);
  }
  m_FaderChangedSignal.emit();
  return true;
}

bool  VUWidget::on_mouse_motion_event(GdkEventMotion* event)
{
    double m = ((double)(3.0*MARGIN + TOP_OFFSET - height ))/(m_fMax - m_fMin);
    double n = (double)(height - 2.0*MARGIN) - m_fMin*m;
    double faderPos = (event->y - n)/m;
    set_value_th(faderPos);  
    m_FaderChangedSignal.emit();
    return true;
}

 VUWidget::signal_FaderChanged  VUWidget::signal_changed()
 {
  return m_FaderChangedSignal;
 }