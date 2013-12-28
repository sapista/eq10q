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

#define BAR_SEPARATION 0.004
#define TEXT_OFFSET 17
#define MARGIN 3
#define BT_VU_MARGIN 0.02
#define SPACE_BETWEEN_CHANNELS 0.03
#define CHANNEL_WIDTH 8
#define MICROFADER_WIDTH 20
#define MICROFADER_HEIGHT 20
#define NUM_OF_BARS 80
#define TEXT_DB_SEPARATION 3.0
#define SCROLL_EVENT_PERCENT 0.02
#define WIDGET_HEIGHT 150

VUWidget::VUWidget(int iChannels, float fMin, float fMax, bool IsGainReduction, bool DrawThreshold) 
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
  m_end(new timeval[m_iChannels])
{
  m_fdBPerLed = (m_fMax - m_fMin)/((float)(NUM_OF_BARS));
  
  for (int i = 0; i < m_iChannels; i++)
  {
    m_fValues[i] = 0.0;
    m_fPeaks[i] = 0.0;
  }
  
  int widget_witdh;
  m_Lmargin = (float)MARGIN/(float)(TEXT_OFFSET +  CHANNEL_WIDTH* m_iChannels + MICROFADER_WIDTH);
  if(m_bDrawThreshold)
  {
    widget_witdh = TEXT_OFFSET +  CHANNEL_WIDTH* m_iChannels + MICROFADER_WIDTH + 2*MARGIN;
    m_Rmargin = (float)(MICROFADER_WIDTH)/(float)(widget_witdh);
  }
  else
  {
    widget_witdh = TEXT_OFFSET +  CHANNEL_WIDTH* m_iChannels + 2*MARGIN;
    m_Rmargin = m_Lmargin;
  }
  set_size_request(widget_witdh, WIDGET_HEIGHT);
 
  m_fBarWidth = (1.0 - 2*BT_VU_MARGIN)/(float)(NUM_OF_BARS) - BAR_SEPARATION;
  m_fBarStep = BAR_SEPARATION + m_fBarWidth;
  m_RedBarsCount = m_YellowBarsCount = (int)(m_fMax/m_fdBPerLed);
  m_GreenBarsCount = NUM_OF_BARS - m_RedBarsCount - m_YellowBarsCount;
   
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
  delete [] fdBValue;
  delete [] fdBPeak;
}
  
void VUWidget::setValue(int iChannel, float fValue)
{ 
  long mtime, seconds, useconds;
  gettimeofday(&m_end[iChannel], NULL);
  
  seconds  = m_end[iChannel].tv_sec  - m_start[iChannel].tv_sec;
  useconds = m_end[iChannel].tv_usec - m_start[iChannel].tv_usec;
  mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

  m_fValues[iChannel] = fValue;
  if (m_fValues[iChannel] >= m_fPeaks[iChannel])
  {
    m_fPeaks[iChannel] = m_fValues[iChannel];
    gettimeofday(&m_start[iChannel] , NULL);
  }
  
  else if (mtime > PEAK_CLEAR_TIMEOUT)
  {
    m_fPeaks[iChannel] = 0.0;
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


bool VUWidget::on_expose_event(GdkEventExpose* event)
{
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
  {
  
    Gtk::Allocation allocation = get_allocation();
    width = allocation.get_width();
    height = allocation.get_height();

    //Compute number of bars for each zone
    fdBValue = new float[m_iChannels];
    fdBPeak = new float[m_iChannels];
    fTextOffset = TEXT_OFFSET/(float)width;
    
    fChannelWidth = (1 - fTextOffset - m_Lmargin -m_Rmargin)/(float)m_iChannels; //TODO aplica el Rmargin!!!
  
    //Translate input to dBu
    for(int i = 0; i<m_iChannels; i++)
    {
      if (m_fValues[i] > 0)
      {
	fdBValue[i] = 20*log10(m_fValues[i]);
      }
      else
      {
	fdBValue[i] = -60;
      }
      if (m_fPeaks[i] > 0)
      {
	fdBPeak[i] = 20*log10(m_fPeaks[i]);
      }
      else
      {
	fdBPeak[i] = -100;
      }
      fdBPeak[i] = fdBPeak[i] > m_fMax ? m_fMax : fdBPeak[i];
    }
    
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

    //Clip inside acording the expose event
    cr->rectangle(event->area.x, event->area.y, event->area.width, event->area.height);
    cr->clip();
    cr->set_source_rgb(BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
    cr->paint(); //Fill all with background color
    
    //Draw text with pango
    cr->save();
    Glib::RefPtr<Pango::Layout> pangoLayout = Pango::Layout::create(cr);
    Pango::FontDescription font_desc("sans 6.5");
    pangoLayout->set_font_description(font_desc);
    pangoLayout->set_alignment(Pango::ALIGN_RIGHT);

    cr->move_to(0.5,10);
    cr->set_source_rgba(0.9, 0.9, 1.0, 1.0);

    pangoLayout->update_from_cairo_context(cr);  //gets cairo cursor position

    for(float fdb = m_fMin; fdb <= m_fMax; fdb = fdb + TEXT_DB_SEPARATION)
    {
      std::stringstream ss;

      ss<<abs(round(fdb));
      
      float py = (fdb - m_fMin) / m_fdBPerLed;
      if(m_bIsGainReduction)
      {
	cr->move_to(3, py*height*m_fBarStep + height*0.005);
      }
      else
      {
	cr->move_to(3, height - py*height*m_fBarStep - height*0.035);
      }
      pangoLayout->set_text(ss.str());
      pangoLayout->show_in_cairo_context(cr);
      cr->stroke();     
    }
    cr->restore();
     
    //Draw LEDs
    cr->save();
    if(m_bIsGainReduction)
    {
      redraw_Gr(cr);
    }
    else
    {
      redraw_Normal(cr);
    }
    
     
    //Draw Threshold MicroFader
    
    if(m_bDrawThreshold)
    {
      cr->restore();  
      cr->set_source_rgba(0.8, 0.8, 0.8, 1.0);
      cr->set_line_width(1.0);
      double  m = ((double)(-height - 20))/(m_fMax - m_fMin); //TODO oju akets 20 son la mida del microfader
      double n = (double)(height - 20) - m_fMin*m;
    
      m_iThFaderPositon = (int)(m*m_ThFaderValue + n);
      cr->rectangle(width - MICROFADER_WIDTH,  m_iThFaderPositon +10, MICROFADER_WIDTH, 20); //TODO oju el 10 es la meitat del 20
      cr->stroke();
    }     
  }
  return true; 
}

inline void VUWidget::redraw_Normal(Cairo::RefPtr<Cairo::Context> cr)
{   
    cr->scale(width, height);
    cr->translate(0, 1);
    cr->set_line_width(m_fBarWidth);
    cr->set_line_cap(Cairo::LINE_CAP_ROUND);   
 
    for(int c = 0; c < m_iChannels; c++)
    {
      //draw active VU in green
      cr->set_source_rgba(0.0, 0.9, 0.3, 1.0);
      for(int i = 0; i< m_GreenBarsCount; i++)
      {
	if(fdBValue[c] >= (float)i*m_fdBPerLed + m_fMin)
	{
	  cr->move_to(m_Lmargin + fTextOffset + c*fChannelWidth + SPACE_BETWEEN_CHANNELS, -BT_VU_MARGIN -i*m_fBarStep - m_fBarWidth/2);
	  cr->line_to(m_Lmargin + fTextOffset + c*fChannelWidth + fChannelWidth - SPACE_BETWEEN_CHANNELS, -BT_VU_MARGIN -i*m_fBarStep - m_fBarWidth/2); 
	}
      }
      cr->stroke();
	  
      //draw inactive VU in green
      cr->set_source_rgba(0.0, 0.9, 0.3, 0.4);
      for(int i = 0; i< m_GreenBarsCount; i++)
      {
	if(fdBValue[c] < (float)i*m_fdBPerLed + m_fMin)
	{
	  cr->move_to(m_Lmargin + fTextOffset + c*fChannelWidth + SPACE_BETWEEN_CHANNELS, -BT_VU_MARGIN -i*m_fBarStep - m_fBarWidth/2);
	  cr->line_to(m_Lmargin + fTextOffset + c*fChannelWidth + fChannelWidth - SPACE_BETWEEN_CHANNELS, -BT_VU_MARGIN -i*m_fBarStep - m_fBarWidth/2);
	}
      }
      cr->stroke();
      
      //draw active VU in yellow
      cr->set_source_rgba(0.9, 0.9, 0.0, 1.0);
      for(int i = m_GreenBarsCount; i<m_GreenBarsCount + m_YellowBarsCount; i++)
      { 
	if(fdBValue[c] >= (float)i*m_fdBPerLed + m_fMin)
	{
	  cr->move_to(m_Lmargin + fTextOffset + c*fChannelWidth + SPACE_BETWEEN_CHANNELS, -BT_VU_MARGIN -i*m_fBarStep - m_fBarWidth/2);
	  cr->line_to(m_Lmargin + fTextOffset + c*fChannelWidth + fChannelWidth - SPACE_BETWEEN_CHANNELS, -BT_VU_MARGIN -i*m_fBarStep - m_fBarWidth/2);
	}
      }
      cr->stroke();
      
      //draw inactive VU in yellow
      cr->set_source_rgba(0.9, 0.9, 0.0, 0.4);
      for(int i = m_GreenBarsCount; i<m_GreenBarsCount + m_YellowBarsCount; i++)
      { 
	if(fdBValue[c] < (float)i*m_fdBPerLed + m_fMin)
	{
	  cr->move_to(m_Lmargin + fTextOffset + c*fChannelWidth + SPACE_BETWEEN_CHANNELS, -BT_VU_MARGIN -i*m_fBarStep - m_fBarWidth/2);
	  cr->line_to(m_Lmargin + fTextOffset + c*fChannelWidth + fChannelWidth - SPACE_BETWEEN_CHANNELS, -BT_VU_MARGIN -i*m_fBarStep - m_fBarWidth/2);
	}
      }
      cr->stroke();
      
      //draw active VU in red
      cr->set_source_rgba(0.9, 0.1, 0.0, 1.0);
      for(int i = m_GreenBarsCount + m_YellowBarsCount; i<m_GreenBarsCount + m_YellowBarsCount + m_RedBarsCount; i++)
      {
	if(fdBValue[c] >= (float)i*m_fdBPerLed + m_fMin)
	{
	  cr->move_to(m_Lmargin + fTextOffset + c*fChannelWidth + SPACE_BETWEEN_CHANNELS, -BT_VU_MARGIN -i*m_fBarStep - m_fBarWidth/2);
	  cr->line_to(m_Lmargin + fTextOffset + c*fChannelWidth + fChannelWidth - SPACE_BETWEEN_CHANNELS, -BT_VU_MARGIN -i*m_fBarStep - m_fBarWidth/2);
	}
      }
      cr->stroke();
      
      //draw inactive VU in red
      cr->set_source_rgba(0.9, 0.1, 0.0, 0.4);
      for(int i = m_GreenBarsCount + m_YellowBarsCount; i<m_GreenBarsCount + m_YellowBarsCount + m_RedBarsCount; i++)
      {
	if(fdBValue[c] < (float)i*m_fdBPerLed + m_fMin)
	{
	  cr->move_to(m_Lmargin + fTextOffset + c*fChannelWidth + SPACE_BETWEEN_CHANNELS, -BT_VU_MARGIN -i*m_fBarStep - m_fBarWidth/2);
	  cr->line_to(m_Lmargin + fTextOffset + c*fChannelWidth + fChannelWidth - SPACE_BETWEEN_CHANNELS, -BT_VU_MARGIN -i*m_fBarStep - m_fBarWidth/2);
	}
      }
      cr->stroke();
      
      //draw peak VU
      if ((fdBPeak[c] - m_fMin)/m_fdBPerLed < m_GreenBarsCount) cr->set_source_rgba(0.0, 0.9, 0.3, 1.0);
      else if((fdBPeak[c] - m_fMin)/m_fdBPerLed < m_GreenBarsCount + m_YellowBarsCount) cr->set_source_rgba(0.9, 0.9, 0.0, 1.0);
      else  cr->set_source_rgba(0.9, 0.1, 0.0, 1.0);
      cr->move_to(m_Lmargin + fTextOffset + c*fChannelWidth + SPACE_BETWEEN_CHANNELS, -BT_VU_MARGIN -(int)((fdBPeak[c]-m_fMin)/m_fdBPerLed)*m_fBarStep - m_fBarWidth/2);
      cr->line_to(m_Lmargin + fTextOffset + c*fChannelWidth + fChannelWidth - SPACE_BETWEEN_CHANNELS, -BT_VU_MARGIN -(int)((fdBPeak[c]-m_fMin)/m_fdBPerLed)*m_fBarStep - m_fBarWidth/2);
      cr->stroke();
    }   
    
    //draw a rectangle arround the VU Widget
    cr->set_source_rgb(0.22, 0.30, 0.53);

     //top horitzontal line
    cr->set_line_width(0.008);
    cr->move_to(0, 0); 
    cr->line_to(1, 0);
    cr->stroke();
    
    //right vertical line
    cr->set_line_width(0.04);
    cr->move_to(1, 0);
    cr->line_to(1, -1);
    cr->stroke();
    
    //bottom Horitzontal line
    cr->set_line_width(0.008);
    cr->move_to(1, -1);
    cr->line_to(0, -1);
    cr->stroke();
    
    //left Vertical line_to
    cr->set_line_width(0.04);
    cr->move_to(0, -1);
    cr->line_to(0, 0);
    cr->stroke();   
}

inline void VUWidget::redraw_Gr(Cairo::RefPtr<Cairo::Context> cr)
{
     cr->scale(width, height);
    cr->translate(0, 0);
    cr->set_line_width(m_fBarWidth);
    cr->set_line_cap(Cairo::LINE_CAP_ROUND);

    for(int c = 0; c < m_iChannels; c++)
    {
	//draw active VU in red
	cr->set_source_rgba(0.9, 0.1, 0.0, 1.0);
	for(int i = 0; i<NUM_OF_BARS; i++)
	{
	  if(fdBValue[c] >= (float)i*m_fdBPerLed + m_fMin)
	  {
	    cr->move_to(m_Lmargin + fTextOffset + c*fChannelWidth + SPACE_BETWEEN_CHANNELS, (BT_VU_MARGIN + i*m_fBarStep + m_fBarWidth/2));
	    cr->line_to(m_Lmargin + fTextOffset + c*fChannelWidth + fChannelWidth - SPACE_BETWEEN_CHANNELS, (BT_VU_MARGIN + i*m_fBarStep + m_fBarWidth/2));
	  }
	}
	cr->stroke();
	
	//draw inactive VU in red
	cr->set_source_rgba(0.9, 0.1, 0.0, 0.4);
	for(int i = 0; i<NUM_OF_BARS; i++)
	{
	  if(fdBValue[c] < (float)i*m_fdBPerLed + m_fMin)
	  {
	    cr->move_to(m_Lmargin + fTextOffset + c*fChannelWidth + SPACE_BETWEEN_CHANNELS, (BT_VU_MARGIN + i*m_fBarStep + m_fBarWidth/2));
	    cr->line_to(m_Lmargin + fTextOffset + c*fChannelWidth + fChannelWidth - SPACE_BETWEEN_CHANNELS, (BT_VU_MARGIN + i*m_fBarStep + m_fBarWidth/2));
	  }
	}
	cr->stroke();
	
	//draw peak VU
	cr->set_source_rgba(0.9, 0.1, 0.0, 1.0);
	cr->move_to(m_Lmargin + fTextOffset + c*fChannelWidth + SPACE_BETWEEN_CHANNELS, BT_VU_MARGIN +(int)((fdBPeak[c]-m_fMin)/m_fdBPerLed)*m_fBarStep + m_fBarWidth/2);
	cr->line_to(m_Lmargin + fTextOffset + c*fChannelWidth + fChannelWidth - SPACE_BETWEEN_CHANNELS, BT_VU_MARGIN +(int)((fdBPeak[c]-m_fMin)/m_fdBPerLed)*m_fBarStep + m_fBarWidth/2);
	cr->stroke();      
    }
      
    //draw a rectangle arround the VU Widget
    cr->set_source_rgb(0.22, 0.30, 0.53);

     //top horitzontal line
    cr->set_line_width(0.008);
    cr->move_to(0, 0); 
    cr->line_to(1, 0);
    cr->stroke();
    
    //right vertical line
    cr->set_line_width(0.04);
    cr->move_to(1, 0);
    cr->line_to(1, -1);
    cr->stroke();
    
    //bottom Horitzontal line
    cr->set_line_width(0.008);
    cr->move_to(1, -1);
    cr->line_to(0, -1);
    cr->stroke();
    
    //left Vertical line_to
    cr->set_line_width(0.04);
    cr->move_to(0, -1);
    cr->line_to(0, 0);
    cr->stroke();
    
}

void VUWidget::set_value_th(double value)
{
  m_ThFaderValue = value;
  m_ThFaderValue = m_ThFaderValue < m_fMin ? m_fMin : m_ThFaderValue;
  m_ThFaderValue = m_ThFaderValue > m_fMax ? m_fMax : m_ThFaderValue;
  redraw();
}

double VUWidget::get_value_th()
{
  return m_ThFaderValue;
}

//Mouse grab signal handlers
bool  VUWidget::on_button_press_event(GdkEventButton* event)
{
  //Act in case mouse pointer is inside faderwidget
  //Gtk::Allocation allocation = get_allocation(); //TODO en principi no ho fare servir
  //const int width = allocation.get_width(); //TODO en principi no ho fare servir
  int x,y;
  get_pointer(x,y);
  if( y > m_iThFaderPositon - 20 &&
      y < m_iThFaderPositon + 20)
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
    int yPixels;
    double m, n, fader_pos;
    Gtk::Allocation allocation = get_allocation();
    const int height = allocation.get_height();

    yPixels = event->y; //Offset fader icon to grab the center of the fader

    //Stoppers
    yPixels = yPixels < 0 ? 0 : yPixels;
    yPixels = yPixels > height - MICROFADER_HEIGHT ? height - MICROFADER_HEIGHT : yPixels;

    
    m = -(double)height/(m_fMax - m_fMin);
    n = (double)(height) - m_fMin*m;
    
    fader_pos = ((double)yPixels - n)/m;

    set_value_th(fader_pos);
    m_FaderChangedSignal.emit();
    
    
    //Relacio db pixels empleat en el pic
     -BT_VU_MARGIN -(int)((fdBPeak[c]-m_fMin)/m_fdBPerLed)*m_fBarStep - m_fBarWidth/2
    
    ///TODO remove this test
    std::cout<<"Fader Y= "<<fader_pos<<std::endl;
    
    return true;
}

 VUWidget::signal_FaderChanged  VUWidget::signal_changed()
 {
  return m_FaderChangedSignal;
 }