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

///TODO: remove this inculde
#include <iostream>

#include "faderwidget.h"

FaderWidget::FaderWidget(double dMax, double dMin)
  :m_max(dMax), m_min(dMin), m_value(0)
{
  m_image_surface_ptr = Cairo::ImageSurface::create_from_png (FADER_ICON_FILE);
  set_size_request(m_image_surface_ptr->get_width()+2*FADER_MARGIN, FADER_INITAL_HIGHT);

}
    
FaderWidget::~FaderWidget()
{
}


void FaderWidget::set_value(double value)
{
  m_value = value;
  redraw();
}

double FaderWidget::get_value()
{
  ///TODO: saturate value to range min max
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
  ///TODO: Copy-paste de Vuwidget a revisar
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
  int yFaderPosition;
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
    
      std::cout<<"FaderWidget m = "<<m<<"\t n = "<<n<<"\t y = "<<yFaderPosition<<std::endl;
    
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

    //Draw fader backgroud line
    cr->save();
    cr->set_source_rgb(0.1, 0.1, 0.1);
    cr->set_line_width(3);
    cr->move_to(width/2, FADER_MARGIN + m_image_surface_ptr->get_height()/2); 
    cr->line_to(width/2, height - FADER_MARGIN - m_image_surface_ptr->get_height()/2);
    cr->stroke();
    cr->restore();
    
    //Draw fader dB scale
    ///TODO:
   
    
    
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
