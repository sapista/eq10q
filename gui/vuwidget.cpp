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
#include "vuwidget.h"
#include <cmath>
#include <cstdio>

#define BAR_SEPARATION 0.004
#define TEXT_OFFSET 15
#define CHANNEL_WIDTH 4
#define GREEN_BARS 20
#define YELLOW_BARS 5
#define RED_BARS 4

VUWidget::VUWidget(int iChannels, float fMin) 
  :m_iChannels(iChannels),
  m_fMin(fMin),
  m_fValues(new float[m_iChannels]),
  m_fPeaks(new float[m_iChannels])
{
  
  for (int i = 0; i < m_iChannels; i++) {
    m_fValues[i] = 0.0;
    m_fPeaks[i] = 0.0;
  }
  
  set_size_request(TEXT_OFFSET +  CHANNEL_WIDTH* m_iChannels, 150);
 
  m_fBarWidth = 1.0/(float)(GREEN_BARS+YELLOW_BARS+RED_BARS) - BAR_SEPARATION;
  m_fBarStep = BAR_SEPARATION + m_fBarWidth;

}

VUWidget::~VUWidget()
{
  delete [] m_fValues;
  delete [] m_fPeaks;
}
  
void VUWidget::setValue(int iChannel, float fValue)
{
  m_fValues[iChannel] = fValue;
  if (m_fValues[iChannel] > m_fPeaks[iChannel])
  {
    m_fPeaks[iChannel] = m_fValues[iChannel];
    Glib::signal_timeout().connect_once(sigc::mem_fun(*this, &VUWidget::onTimeout), 3000);
  }
  redraw();
}

void VUWidget::onTimeout()
{
  for(int i = 0; i < m_iChannels; i++)
  {
    clearPeak(i);
  }
}

void VUWidget::clearPeak(int iChannel)
{
  m_fPeaks[iChannel] = 0.0;
  redraw();
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
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    //Compute number of bars for each zone
    float fdBValue[m_iChannels];
    float fdBPeak[m_iChannels];
    int iActiveVu;
    float fTextOffset = TEXT_OFFSET/(float)width;
    float fTextHeight = TEXT_OFFSET/(float)height;
    float fChannelWidth = (1 - fTextOffset)/(float)m_iChannels;
  
    //Translate input to dBu
    for(int i = 0; i<m_iChannels; i++)
    {
      fdBValue[i] = 10*log10(m_fValues[i]);
      fdBPeak[i] = 10*log10(m_fPeaks[i]);
      fdBPeak[i] = fdBPeak[i] > 4.0 ? 4.0 : fdBPeak[i];
    }
    
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

    //Clip inside acording the expose event
    cr->rectangle(event->area.x, event->area.y, event->area.width, event->area.height);
    cr->clip();
    cr->scale(width, height);
    cr->translate(0, 1);
    cr->set_line_width(m_fBarWidth);
    cr->paint(); //Fill all with dark color
    

 
//      //Draw text to the Vu
//     cr->set_source_rgb(0.9, 0.9, 0.9);
//     cr->set_font_size(0.15);
//     for(int i = 0; i < GREEN_BARS + YELLOW_BARS + RED_BARS; i=i+4)
//     {
//       cr->save();
//       std::stringstream ss;
//       ss<<(i-24);
// //       cr->rectangle(0.0, -i*m_fBarStep - fTextHeight, fTextOffset, fTextHeight);
// //       cr->clip();
// //       //cr->set_source_rgb(0.9, 0.0, 0.0);
// //       //cr->paint();
//       cr->move_to(0.0,  -i*m_fBarStep - fTextHeight);
//       cr->show_text(ss.str());
//       cr->restore();
//     }
 
        
    for(int c; c < m_iChannels; c++)
    {
      //draw active VU in green
      cr->set_source_rgba(0.0, 0.9, 0.3, 1.0);
      for(int i = 0; i< GREEN_BARS; i++)
      {
	if(fdBValue[c] >= (float)i - 24)
	{
	  cr->move_to(fTextOffset + c*fChannelWidth, -i*m_fBarStep - m_fBarWidth/2);
	  cr->line_to(fTextOffset + c*fChannelWidth + fChannelWidth, -i*m_fBarStep - m_fBarWidth/2);
	}
      }
      cr->stroke();
	  
      //draw inactive VU in green
      cr->set_source_rgba(0.0, 0.9, 0.3, 0.4);
      for(int i = 0; i< GREEN_BARS; i++)
      {
	if(fdBValue[c] < (float)i - 24)
	{
	  cr->move_to(fTextOffset + c*fChannelWidth, -i*m_fBarStep - m_fBarWidth/2);
	  cr->line_to(fTextOffset + c*fChannelWidth + fChannelWidth, -i*m_fBarStep - m_fBarWidth/2);
	}
      }
      cr->stroke();
      
      //draw active VU in yellow
      cr->set_source_rgba(0.9, 0.9, 0.0, 1.0);
      for(int i = GREEN_BARS; i<GREEN_BARS + YELLOW_BARS; i++)
      { 
	if(fdBValue[c] >= (float)i - 24)
	{
	  cr->move_to(fTextOffset + c*fChannelWidth, -i*m_fBarStep - m_fBarWidth/2);
	  cr->line_to(fTextOffset + c*fChannelWidth + fChannelWidth, -i*m_fBarStep - m_fBarWidth/2);
	}
      }
      cr->stroke();
      
      //draw inactive VU in yellow
      cr->set_source_rgba(0.9, 0.9, 0.0, 0.4);
      for(int i = GREEN_BARS; i<GREEN_BARS + YELLOW_BARS; i++)
      { 
	if(fdBValue[c] < (float)i - 24)
	{
	  cr->move_to(fTextOffset + c*fChannelWidth, -i*m_fBarStep - m_fBarWidth/2);
	  cr->line_to(fTextOffset + c*fChannelWidth + fChannelWidth, -i*m_fBarStep - m_fBarWidth/2);
	}
      }
      cr->stroke();
      
      //draw active VU in red
      cr->set_source_rgba(0.9, 0.1, 0.0, 1.0);
      for(int i = GREEN_BARS + YELLOW_BARS; i<GREEN_BARS + YELLOW_BARS + RED_BARS; i++)
      {
	if(fdBValue[c] >= (float)i - 24)
	{
	  cr->move_to(fTextOffset + c*fChannelWidth, -i*m_fBarStep - m_fBarWidth/2);
	  cr->line_to(fTextOffset + c*fChannelWidth + fChannelWidth, -i*m_fBarStep - m_fBarWidth/2);
	}
      }
      cr->stroke();
      
      //draw inactive VU in red
      cr->set_source_rgba(0.9, 0.1, 0.0, 0.4);
      for(int i = GREEN_BARS + YELLOW_BARS; i<GREEN_BARS + YELLOW_BARS + RED_BARS; i++)
      {
	if(fdBValue[c] < (float)i - 24)
	{
	  cr->move_to(fTextOffset + c*fChannelWidth, -i*m_fBarStep - m_fBarWidth/2);
	  cr->line_to(fTextOffset + c*fChannelWidth + fChannelWidth, -i*m_fBarStep - m_fBarWidth/2);
	}
      }
      cr->stroke();
      
      //draw peak VU
      if (fdBPeak[c] + 24 < GREEN_BARS) cr->set_source_rgba(0.0, 0.9, 0.3, 1.0);
      else if(fdBPeak[c] + 24 < GREEN_BARS + YELLOW_BARS) cr->set_source_rgba(0.9, 0.9, 0.0, 1.0);
      else  cr->set_source_rgba(0.9, 0.1, 0.0, 1.0);
      cr->move_to(fTextOffset + c*fChannelWidth, -((int)fdBPeak[c]+24)*m_fBarStep - m_fBarWidth/2);
      cr->line_to(fTextOffset + c*fChannelWidth + fChannelWidth, -((int)fdBPeak[c]+24)*m_fBarStep - m_fBarWidth/2);
      cr->stroke();
    }
    //Draw text with pango
    Glib::RefPtr<Pango::Layout> pangoLayout = Pango::Layout::create(cr);
    cr->move_to(0.5,-0.5);
    pangoLayout->set_text("HOLA");
    pangoLayout->update_from_cairo_context(cr);  //gets cairo cursor position
    pangoLayout->add_to_cairo_context(cr);       //adds text to cairos stack of stuff to be drawn
    cr->stroke();                                //tells Cairo to render it's stack

  }
  return true;  
}