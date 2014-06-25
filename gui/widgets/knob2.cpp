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
#include "knob2.h"
#include "colors.h"
#include <gdkmm.h>//For the function Gdk::Cairo::set_source_pixbuf()
//#include <gdkmm/general.h> //Switched back to gdkmm.h for cairo portability problems


#define PI 3.1416
#define KNOB_RADIUS 0.3
#define SCROLL_EVENT_PERCENT 0.005
#define MOUSE_EVENT_PERCENT 0.008
#define KNOB_CENTER_X 0.5
#define KNOB_CENTER_Y 0.4
#define CURSOR_LENGHT 0.25
#define CURSOR_TRIANGLE_BASE 0.03

KnobWidget2::KnobWidget2(float fMin, float fMax, std::string sLabel, std::string sUnits, const char *bundlePath, bool bIsFreqType)
 :KnobWidget(fMin, fMax, sLabel, sUnits, bIsFreqType),
 m_bundlePath(bundlePath)
{
   m_image_ptr =  Gdk::Pixbuf::create_from_file(m_bundlePath + "/" + std::string(KNOB_ICON_FILE));

  // Detect transparent colors for loaded image
  Cairo::Format format = Cairo::FORMAT_RGB24;
  if (m_image_ptr->get_has_alpha())
  {
      format = Cairo::FORMAT_ARGB32;
  }
  
  // Create a new ImageSurface
  m_image_surface_ptr = Cairo::ImageSurface::create  (format, m_image_ptr->get_width(), m_image_ptr->get_height());
  
  // Create the new Context for the ImageSurface
  m_image_context_ptr = Cairo::Context::create (m_image_surface_ptr);     
  
  // Draw the image on the new Context
  Gdk::Cairo::set_source_pixbuf (m_image_context_ptr, m_image_ptr, 0.0, 0.0);
  m_image_context_ptr->paint();
  //set_size_request(2*m_image_surface_ptr->get_width()+4*FADER_MARGIN, FADER_INITAL_HIGHT);
  
}

//Drawing Knob
bool KnobWidget2::on_expose_event(GdkEventExpose* event)
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
    ss.precision(2);
    
    if(m_FreqKnob && m_Value >= 1000.0)
    {   
      ss<<std::fixed<<m_Value/1000.0<<" k"<<m_Units;
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
    cr->set_source_rgba(0.2,0.6,0.6,1.0);
    cr->set_line_width(0.10);
    cr->arc(KNOB_CENTER_X, KNOB_CENTER_Y, KNOB_RADIUS, 0.74 * PI, 0.26 * PI);
    cr->stroke();
    
    //Draw circle background black
    cr->set_source_rgba(0.1,0.1,0.2,0.9);
    cr->set_line_width(0.07);
    cr->arc(KNOB_CENTER_X, KNOB_CENTER_Y, KNOB_RADIUS, 0.75 * PI, 0.25 * PI); 
    cr->stroke();
    
    //Calc konb angle (pos)
    double pos;
    if(m_FreqKnob)
    {
        double m = (1.48*PI)/log10(m_fMax/m_fMin);
        double n = 0.76*PI;
        pos = m*log10(m_Value/m_fMin) + n;
    }
    else
    {
      double m = (1.48*PI)/(m_fMax-m_fMin);
      double n = 0.76*PI  - m*m_fMin;
      pos = m*m_Value + n;  
    }
    
    //Draw colored circle
    cr->set_source_rgba(1.0,0.5,0.0,1.0);
    cr->set_line_width(0.05);
    cr->arc(KNOB_CENTER_X, KNOB_CENTER_Y, KNOB_RADIUS, 0.76 * PI, pos); 
    cr->stroke();
    
    cr->set_source_rgba(0.6,0.3,0.0,1.0);
    cr->set_line_width(0.03);
    cr->arc(KNOB_CENTER_X, KNOB_CENTER_Y, KNOB_RADIUS, 0.76 * PI, pos); 
    cr->stroke();
    cr->restore();
    
    //Draw knob and rotate
    cr->save();
    cr->translate(width/2 + KNOB_X_CALIBRATION, height/2 + KNOB_Y_CALIBRATION);
    cr->rotate(pos + KNOB_R_CALIBRATION);
    //cr->rectangle(-0.15, -0.15, 0.3, 0.3);
    //cr->fill();
    //cr->restore();
    
    //Draw the knob icon
    cr->set_source (m_image_surface_ptr, -m_image_surface_ptr->get_width()/2, -m_image_surface_ptr->get_height()/2);
    cr->rectangle (-m_image_surface_ptr->get_width()/2, -m_image_surface_ptr->get_height()/2, m_image_surface_ptr->get_width(),  m_image_surface_ptr->get_height());
    cr->clip();
    cr->paint();
    cr->restore();
  
  }
  return true;
}