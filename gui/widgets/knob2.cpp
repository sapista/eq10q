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
#define KNOB_RADIUS 0.4 //0.3
#define SCROLL_EVENT_PERCENT 0.005
#define MOUSE_EVENT_PERCENT 0.008
#define KNOB_CENTER_X 0.5
#define KNOB_CENTER_Y 0.5 //0.4
#define TEXT_SIZE 22
#define KNOB_R_CALIBRATION 0.93

KnobWidget2::KnobWidget2(float fMin, float fMax, std::string sLabel, std::string sUnits, const char *knobIconPath, int iType, bool snap2ZerodB )
 :KnobWidget(fMin, fMax, sLabel, sUnits, iType, snap2ZerodB ),
 m_knobIconPath(knobIconPath)
{
   m_image_ptr =  Gdk::Pixbuf::create_from_file(m_knobIconPath);

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
  
  //Size Request acording knob image
  set_size_request((int)(1.5*(double)(m_image_ptr->get_width())), TEXT_SIZE + (int)(1.5*(double)(m_image_ptr->get_height())));
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

    cr->move_to(0, height - TEXT_SIZE);
    cr->set_source_rgba(0.9, 0.9, 0.9, 1.0);
    pangoLayout->update_from_cairo_context(cr);  //gets cairo cursor position
    pangoLayout->set_text(m_Label);
    pangoLayout->set_width(Pango::SCALE * width);
    pangoLayout->set_alignment(Pango::ALIGN_CENTER);
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();
    
    cr->move_to(0, height - TEXT_SIZE/2);
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
    
    //Scale to 1
    cr->rectangle(0, 0, width,height - TEXT_SIZE);
    cr->clip();
    cr->scale(width,height - TEXT_SIZE);
    
    //Draw Background gradient full circle
    Cairo::RefPtr<Cairo::RadialGradient> bkg_gradient_ptr = Cairo::RadialGradient::create(
        KNOB_CENTER_X, KNOB_CENTER_Y, KNOB_RADIUS - 0.08, KNOB_CENTER_X, KNOB_CENTER_Y, KNOB_RADIUS + 0.1);
    
    bkg_gradient_ptr->add_color_stop_rgba (0, 0.0, 0.8, 0.3, 0.2);  
    bkg_gradient_ptr->add_color_stop_rgba (1, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 0.1);
 
    cr->set_source(  bkg_gradient_ptr);                       
    cr->set_line_width(0.8);
    cr->arc(KNOB_CENTER_X, KNOB_CENTER_Y, 0.2, 0.0, 2.0 * PI); 
    cr->stroke();
    
    //Draw colored circle
    Cairo::RefPtr<Cairo::RadialGradient> rad_gradient_ptr = Cairo::RadialGradient::create(
        KNOB_CENTER_X, KNOB_CENTER_Y, KNOB_RADIUS - 0.08, KNOB_CENTER_X, KNOB_CENTER_Y, KNOB_RADIUS + 0.1);
    
    rad_gradient_ptr->add_color_stop_rgba (0, 0.0, 1.0, 0.0, 0.8);  
    rad_gradient_ptr->add_color_stop_rgba (1, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 0.1);
 
    cr->set_source(  rad_gradient_ptr);                       
    cr->set_line_width(0.2);
    cr->arc(KNOB_CENTER_X, KNOB_CENTER_Y, KNOB_RADIUS + 0.04, 0.76 * PI, pos); 
    cr->stroke();
        
    //Draw color circle frame
    cr->set_source_rgba(BACKGROUND_R + 0.4, BACKGROUND_G + 0.4, BACKGROUND_B + 0.4, 1.0);
    cr->set_line_width(1.0/width);
    //cr->set_line_width(0.01);
    cr->arc(KNOB_CENTER_X, KNOB_CENTER_Y, KNOB_RADIUS + 0.04, 0.76 * PI, 0.24 * PI);
    cr->arc(KNOB_CENTER_X, KNOB_CENTER_Y, KNOB_RADIUS - 0.06, 0.24 * PI, 2.76*PI);
    cr->close_path();
    cr->stroke();
    
    cr->set_source_rgba(0.0, 0.6, 0.6, 0.1);
    cr->set_line_width(0.1);
    std::valarray< double > dashes(2);
    dashes[0] = 0.01;//1.0/width;
    dashes[1] = 0.02;//4.0/width;
    cr->set_dash (dashes, 0.5);
    cr->arc(KNOB_CENTER_X, KNOB_CENTER_Y, KNOB_RADIUS - 0.01, 0.76 * PI, 0.24 * PI);
    cr->stroke();
    
    
    cr->restore();
    
    //Draw knob and rotate
    cr->save();
    cr->translate(width/2, (height-TEXT_SIZE)/2);
    cr->rotate(pos + KNOB_R_CALIBRATION);
    
    //Draw the knob icon
    cr->set_source (m_image_surface_ptr, -m_image_surface_ptr->get_width()/2, -m_image_surface_ptr->get_height()/2);
    cr->rectangle (-m_image_surface_ptr->get_width()/2, -m_image_surface_ptr->get_height()/2, m_image_surface_ptr->get_width(),  m_image_surface_ptr->get_height());
    cr->clip();
    cr->paint();
    cr->restore();
  
  }
  return true;
}