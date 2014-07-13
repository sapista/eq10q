/***************************************************************************
 *   Copyright (C) 2011 by Pere Rafols Soler                               *
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

#ifndef KNOB_WIDGET2_H
#define KNOB_WIDGET2_H

#include "knob.h"
#include <gdkmm/pixbuf.h>
#include <glibmm/refptr.h>
#include <cairomm/surface.h>

#define KNOB_ICON_FILE "knobs/knob2_35px.png"
#define KNOB_X_CALIBRATION 1
#define KNOB_Y_CALIBRATION -6
#define KNOB_R_CALIBRATION 0.9

class KnobWidget2 : public KnobWidget
{
  public:
  KnobWidget2(float fMin, float fMax, std::string sLabel, std::string sUnits, const char *bundlePath, bool bIsFreqType = false);
  
  protected:
    //Override default signal handler:
    virtual bool on_expose_event(GdkEventExpose* event);
    
  private:
    std::string m_bundlePath;
    Cairo::RefPtr<Cairo::ImageSurface> m_image_surface_ptr;
    Glib::RefPtr<Gdk::Pixbuf> m_image_ptr;
    Cairo::RefPtr< Cairo::Context> m_image_context_ptr;
};
#endif