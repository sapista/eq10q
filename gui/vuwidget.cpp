/****************************************************************************
    
    vuwidget.cpp - simple VU meter
    
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

#include <iostream>

#include "vuwidget.hpp"


VUWidget::VUWidget(unsigned channels, float min) 
  : m_channels(channels),
    m_min(min),
    m_values(new float[m_channels]),
    m_peaks(new float[m_channels]),
    m_peak_connections(new sigc::connection[m_channels]) {
  
  for (unsigned c = 0; c < m_channels; ++c) {
    m_values[c] = 0.0;
    m_peaks[c] = 0.0;
  }
  
  set_size_request(4 + 12 * m_channels, 150);
  m_bg.set_rgb(10000, 10000, 15000);
  m_shadow.set_rgb(3000, 3000, 5000);
  m_light.set_rgb(30000, 30000, 30000);
  m_fg1.set_rgb(0, 65000, 45000);
  m_fg2.set_rgb(65000, 45000, 10000);
  m_fg3.set_rgb(65000, 0, 0);
  m_fg1b.set_rgb(8500, 16000, 26000);
  m_fg2b.set_rgb(16000, 14000, 12500);
  m_fg3b.set_rgb(16000, 8500, 12500);
  Glib::RefPtr<Gdk::Colormap> cmap = Gdk::Colormap::get_system();
  cmap->alloc_color(m_bg);
  cmap->alloc_color(m_shadow);
  cmap->alloc_color(m_light);
  cmap->alloc_color(m_fg1);
  cmap->alloc_color(m_fg2);
  cmap->alloc_color(m_fg3);
  cmap->alloc_color(m_fg1b);
  cmap->alloc_color(m_fg2b);
  cmap->alloc_color(m_fg3b);
}


VUWidget::~VUWidget() {
  delete [] m_values;
  delete [] m_peaks;
  delete [] m_peak_connections;
}

  
void VUWidget::set_value(unsigned channel, float value) {
  m_values[channel] = value;
  if (m_values[channel] > m_peaks[channel]) {
    m_peaks[channel] = m_values[channel];
    m_peak_connections[channel].disconnect();
    m_peak_connections[channel] = Glib::signal_timeout().
      connect(bind_return(bind(mem_fun(*this, &VUWidget::clear_peak), channel),
			  false), 3000);
  }
  queue_draw();
}
  

bool VUWidget::on_expose_event(GdkEventExpose* event) {
  Glib::RefPtr<Gdk::Window> win = get_window();
  Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(win);
  
  gc->set_foreground(m_bg);
  win->draw_rectangle(gc, true, 0, 0, get_width(), get_height());
  unsigned int n = (get_height() - 4) / 3;
  
  gc->set_foreground(m_light);
  win->draw_line(gc, 0, get_height() - 1, get_width() - 1, get_height() - 1);
  win->draw_line(gc, get_width() - 1, 0, get_width() - 1, get_height() - 1);
  gc->set_foreground(m_shadow);
  win->draw_line(gc, 0, 0, get_width(), 0);
  win->draw_line(gc, 0, 0, 0, get_height());
  
  for (unsigned c = 0; c < m_channels; ++c) {
    float mapped_value = map_to_log(m_values[c]);
    int x = 2 + c * ((get_width() - 3) / m_channels);
    int w = (get_width() - 3) / m_channels - 2;
    gc->set_foreground(m_fg1);
    int level = 1;
    bool active = true;
    for (unsigned i = 0; i < n; ++i) {
      if (mapped_value * 0.8 * n <= i) {
	active = false;
	if (level == 1)
	  gc->set_foreground(m_fg1b);
	else if (level == 2)
	  gc->set_foreground(m_fg2b);
	if (level == 3)
	  gc->set_foreground(m_fg3b);
      }
      if (level == 1 && 0.6 * n <= i) {
	if (active)
	  gc->set_foreground(m_fg2);
	else
	  gc->set_foreground(m_fg2b);
	level = 2;
      }
      if (level == 2 && 0.8 * n <= i) {
	if (active)
	  gc->set_foreground(m_fg3);
	else
	  gc->set_foreground(m_fg3b);
	level = 3;
      }
      win->draw_rectangle(gc, true, x, get_height() - (2 + 3 * i + 3), w, 2);
    }
    
    if (m_peaks[c] > 0) {
      float mapped_value = map_to_log(m_peaks[c]);
      unsigned i = mapped_value * 0.8 * n;
      if (i >= n)
	i = n - 1;
      if (mapped_value * 0.8 <= 0.6)
	gc->set_foreground(m_fg1);
      else if (mapped_value * 0.8 <= 0.8)
	gc->set_foreground(m_fg2);
      else
	gc->set_foreground(m_fg3);
      win->draw_rectangle(gc, true, x, get_height() - (2 + 3 * i + 3), w, 2);
    }
  }
}


void VUWidget::clear_peak(unsigned channel) {
  m_peaks[channel] = 0.0;
  queue_draw();
}

