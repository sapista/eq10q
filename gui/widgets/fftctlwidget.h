/***************************************************************************
 *   Copyright (C) 2015 by Pere Ràfols Soler                               *
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


#ifndef FFT_WIDGET_H
  #define FFT_WIDGET_H

#include <gtkmm/drawingarea.h>

class  FFTWidget : public Gtk::DrawingArea
{
  public:
    FFTWidget(double min, double max);
    virtual ~FFTWidget();
    void set_value(double val);
    double get_value();
    bool get_active();
    bool get_isSpectrogram();
    
    //signal accessor:
    typedef sigc::signal<void> signal_Changed;
    signal_Changed signal_changed();
    
    typedef sigc::signal<void> signal_Changed_btnClicked;
    signal_Changed_btnClicked signal_clicked();
    
  protected:
    //Override default signal handler:
    virtual bool on_expose_event(GdkEventExpose* event);
    
    //Mouse grab signal handlers
    virtual bool on_button_press_event(GdkEventButton* event);
    virtual bool on_button_release_event(GdkEventButton* event);
    virtual bool on_scrollwheel_event(GdkEventScroll* event);
    virtual bool on_mouse_motion_event(GdkEventMotion* event);
    
    void redraw();    
  
  private:
    double m_value, m_max, m_min;
    bool m_bEnabled, m_bSlider_Focus, m_bSlider_Press, m_bBtn_Foucs, m_bBtn_Press, m_bIsSpectrogram;
    int width, height;
    double Val2Pixels(double val);
    double Pixels2Val(double px);
    
    //Fader change signal
    signal_Changed m_ChangedSignal;
    
    //Button click signal
    signal_Changed_btnClicked m_ClickSignal;
  
};
#endif