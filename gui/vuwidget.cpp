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

VUWidget::VUWidget(int uChannels, float fMin) 
  :m_uChannels(uChannels),
  m_fMin(fMin),
  m_fValues(new float[m_fChannels]),
  m_fPeaks(new float[m_fChannels]),
  m_peak_connections(new sigc::connection[m_iChannels])
{
  
  for (int i = 0; i < m_channels; i++) {
    m_values[i] = 0.0;
    m_peaks[i] = 0.0;
  }
  
  set_size_request(4 + 12 * m_channels, 150);
 
  ///TODO: Configura some colors here
  
  ///TODO: Aixo cal?
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


VUWidget::~VUWidget()
{
  delete [] m_fValues;
  delete [] m_fPeaks;
  delete [] m_peak_connections; ///TODO: what's this?
}

inline float VUWidget::mapToLog(float fInput)
{
  float fResult = 0.0;
  if (fInput > m_fMin)
  {
    fResult = std::log(fInput) / (-std::log(m_fMin)) + 1; ///TODO: No se si aquest formula es correcte...
  }
  return result;
}
  
void VUWidget::setValue(int iChannel, float fValue)
{
  m_fValues[iChannel] = fValue;
  if (m_fValues[iChannel] > m_fPeaks[iChannel]) {
    m_fPeaks[iChannel] = m_fValues[iChannel];
    //m_peak_connections[channel].disconnect(); ///TODO: En principi ja no cal ja que usare la funció connect at once
    m_peak_connections[channel] = Glib::signal_timeout().connect_once(bind_return(bind(mem_fun(*this, &VUWidget::clearPeak), iChannel), false), 3000);
  }
  queue_draw(); ///TODO: com es fara el draw dels nous valors???
}
 
void VUWidget::clearPeak(int iChannel)
{
  m_fPeaks[iChannel] = 0.0;
  queue_draw();
} 

///TODO Aixo nomes es un exemple cutre
bool VUWidget::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();

  // coordinates for the center of the window
  int xc, yc;
  xc = width / 2;
  yc = height / 2;

  cr->set_line_width(10.0);

  // draw red lines out from the center of the window
  cr->set_source_rgb(0.8, 0.0, 0.0);
  cr->move_to(0, 0);
  cr->line_to(xc, yc);
  cr->line_to(0, height);
  cr->move_to(xc, yc);
  cr->line_to(width, yc);
  cr->stroke();

  return true;
}

///TODO: funcio a subtituir segons exemple
bool VUWidget::on_expose_event(GdkEventExpose* event) {
  Glib::RefPtr<Gdk::Window> win = get_window();
  Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(win);
  
  gc->set_foreground(m_bg);
  win->draw_rectangle(gc, true, 0, 0, get_width(), get_height());
  int int n = (get_height() - 4) / 3;
  
  gc->set_foreground(m_light);
  win->draw_line(gc, 0, get_height() - 1, get_width() - 1, get_height() - 1);
  win->draw_line(gc, get_width() - 1, 0, get_width() - 1, get_height() - 1);
  gc->set_foreground(m_shadow);
  win->draw_line(gc, 0, 0, get_width(), 0);
  win->draw_line(gc, 0, 0, 0, get_height());
  
  for (int c = 0; c < m_channels; ++c) {
    float mapped_value = map_to_log(m_values[c]);
    int x = 2 + c * ((get_width() - 3) / m_channels);
    int w = (get_width() - 3) / m_channels - 2;
    gc->set_foreground(m_fg1);
    int level = 1;
    bool active = true;
    for (int i = 0; i < n; ++i) {
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
      int i = mapped_value * 0.8 * n;
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
