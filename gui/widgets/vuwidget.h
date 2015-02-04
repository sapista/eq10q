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

#ifndef VU_WIDGET_H
#define VU_WIDGET_H

#include <cmath>
#include <string>
#include <vector>
#include <gtkmm/drawingarea.h>

#include <sys/time.h>
#define PEAK_CLEAR_TIMEOUT 2000

class VUWidget : public Gtk::DrawingArea
{
  public:
    VUWidget(int iChannels, float fMin, float fMax, bool IsGainReduction = false, bool DrawThreshold = false);
    ~VUWidget();
    void setValue(int iChannel, float fValue);
  
    //Data accessors
    void set_value_th(double value);
    double get_value_th();
    
    //signal accessor:
    typedef sigc::signal<void> signal_FaderChanged;
    signal_FaderChanged signal_changed();
    
protected:
  //Override default signal handler:
  virtual bool on_expose_event(GdkEventExpose* event);
  void clearPeak(int uChannel);
  void redraw();
  
  //Mouse grab signal handlers
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_scrollwheel_event(GdkEventScroll* event);
  virtual bool on_mouse_motion_event(GdkEventMotion* event);
  
  int m_iChannels;
  float m_fMin; //Min representable value in dB
  float m_fMax; //Max representable value in dB
  bool m_bIsGainReduction;
  bool bMotionIsConnected;
  float* m_fValues;
  float* m_fPeaks;
  
  float m_ThFaderValue;
  int m_iThFaderPositon;
  bool m_bDrawThreshold;
  //sigc::connection* m_peak_connections;
private:  
    struct timeval *m_start; //Array of timeval start, on for each channel
    struct timeval *m_end; //Array of timeval end, on for each channel
    
    int width;
    int height;
     
    sigc::connection m_motion_connection;
    
    //Fader change signal
    signal_FaderChanged m_FaderChangedSignal;
    
    //dB to pixels convertion function
    double dB2Pixels(double dB_in);
};
#endif
