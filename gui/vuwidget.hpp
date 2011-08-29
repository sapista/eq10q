/****************************************************************************
    
    vuwidget.hpp - simple VU meter
    
    Copyright (C) 2006-2007 Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301  USA

****************************************************************************/

#ifndef VUWIDGET_HPP
#define VUWIDGET_HPP

#include <cmath>
#include <string>
#include <vector>

//TODO: Pero quina animalada es aquest include de tota la gtkmm!
#include <gtkmm.h>


class VUWidget : public Gtk::DrawingArea {
public:
  
  VUWidget(unsigned channels, float min = 1.0 / 256);
  ~VUWidget();
  
  void set_value(unsigned channel, float value);
  
protected:
  
  bool on_expose_event(GdkEventExpose* event);
  
  void clear_peak(unsigned channel);
  
  float map_to_log(float input) {
    float result = 0;
    if (input > m_min) {
      float lmin = -std::log(m_min);
      result = std::log(input) / lmin + 1;
    }
    return result;
  }
  
  unsigned m_channels;
  float m_min;
  float* m_values;
  float* m_peaks;
  sigc::connection* m_peak_connections;
  
  Gdk::Color m_bg, m_fg1, m_fg2, m_fg3, m_fg1b, m_fg2b, m_fg3b, 
    m_shadow, m_light;

};


#endif
