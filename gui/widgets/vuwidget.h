/***************************************************************************
 *   Copyright (C) 2011 by Pere R�fols Soler                               *
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

 ///TODO: Recordar que part del codi es de Lars Lutermans O algu aixi!!!!!!
#ifndef VU_WIDGET_H
#define VU_WIDGET_H

#include <cmath>
#include <string>
#include <vector>
#include <gtkmm/drawingarea.h>

class VUWidget : public Gtk::DrawingArea
{
  public:
    VUWidget(int iChannels, float fMin = 1.0 / 256);
    ~VUWidget();
    void setValue(int iChannel, float fValue);
  
protected:
  //Override default signal handler:
  virtual bool on_expose_event(GdkEventExpose* event);
  void onTimeout();
  void clearPeak(int uChannel);
  void redraw();
  
  int m_iChannels;
  float m_fMin;
  float* m_fValues;
  float* m_fPeaks;
  float m_fBarWidth;
  float m_fBarStep;
  //sigc::connection* m_peak_connections;
private:
    void pangoLayout();
};
#endif
