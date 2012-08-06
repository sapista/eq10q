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

#ifndef FADER_WIDGET_H
  #define FADER_WIDGET_H

#include <gtkmm/drawingarea.h>

#define FADER_ICON_FILE "knobs/fader_white.png"
#define FADER_INITAL_HIGHT 250
#define FADER_MARGIN 5

class  FaderWidget : public Gtk::DrawingArea
{
  public:
    FaderWidget(double dMax, double dMin);
    virtual ~FaderWidget();
    
    //Data accessors
    void set_value(double value);
    double get_value();
    
    void set_range(double max, double min);
    double get_max();
    double get_min();
    
  protected:
      //Override default signal handler:
      virtual bool on_expose_event(GdkEventExpose* event);
      void redraw();
      
  private:
      double m_value, m_max, m_min;
      Cairo::RefPtr<Cairo::ImageSurface> m_image_surface_ptr;

};
#endif