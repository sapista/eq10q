/***************************************************************************
 *   Copyright (C) 2009 by Pere Ràfols Soler                               *
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

#ifndef PLOT_BODE_CURVE_H
  #define PLOT_BODE_CURVE_H

#include <iostream>
#include <gtkmm/drawingarea.h>
#include "filter.h"

#define NUM_POINTS_PER_DECADE 100
#define MIN_FREQ 20
#define MAX_FREQ 20000

typedef struct
{
  float Gain;
  float Freq;
  float Q;
  bool bIsOn;
  FilterType fType;
}FilterBandParams;

class PlotEQCurve : public Gtk::DrawingArea
{
  public:
    PlotEQCurve(int iNumOfBands);
    virtual ~PlotEQCurve();
    
    virtual void setBandGain(int bd_ix, float newGain);
    virtual void setBandFreq(int bd_ix, float newFreq);
    virtual void setBandQ(int bd_ix, float newQ);
    virtual void setBandType(int bd_ix, int newType); ///TODO: are you shure that this is an integer type???
    virtual void setBandEnable(int bd_ix, bool bIsEnabled);
    virtual void setBypass(bool bypass);
    
    //signal accessor:
    //Slot prototype: void on_band_changed(int band_ix, float Gain, float Freq, float Q);
    typedef sigc::signal<void, int, float, float, float> signal_BandChanged;
    signal_BandChanged signal_changed();
    
  protected:    
      //Mouse grab signal handlers
      virtual bool on_button_press_event(GdkEventButton* event);
      virtual bool on_button_release_event(GdkEventButton* event);
      virtual bool on_scrollwheel_event(GdkEventScroll* event);
      virtual bool on_mouse_motion_event(GdkEventMotion* event);
  
      //Override default signal handler:
      virtual bool on_expose_event(GdkEventExpose* event);
      void redraw();
    
  private:
    int m_TotalBandsCount;
    int m_NumOfPoints;
    
    //Store filters data
    FilterBandParams **m_filters;  //This pointer is initialized by construcor to an array of total num of bands
    
    //X axes LUT tables
    double *f; //This pointer is initialized by construcor to an array of total num of points acording min/max freq define
    int *xPixels; //This pointer is initialized by construcor to an array of total num of points, eacy item is the pixel space transaltion of corresponding freq
    
    //Curve vector for Y axes in dB units
    double *main_y; //This pointer is initialized by construcor to an array of total num of points
    double **band_y;  //This pointer is initialized by construcor to an array acording the format band_y[bd_ix][num_points]
  
    //Fader change signal
    signal_BandChanged m_BandChangedSignal;
    
    //Function for dB to pixels convertion
    int dB2Pixels(double db);
    
    //Compute a filter points
    void ComputeFilter(int bd_ix); //this methos implements a switch to call methods on bodecalc.cpp acording filter type
  
    ///TODO: Falta definir els objectes de dibuix, Cairo, Pango...
    
    //Curve math functions implemented in bodecalc.cpp
    void CalcBand_filter_off(int bd_ix);
    void CalcBand_lpf_order1(int bd_ix);
    void CalcBand_lpf_order2(int bd_ix);
    void CalcBand_lpf_order3(int bd_ix);
    void CalcBand_lpf_order4(int bd_ix);
    void CalcBand_hpf_order1(int bd_ix);
    void CalcBand_hpf_order2(int bd_ix);
    void CalcBand_hpf_order3(int bd_ix);
    void CalcBand_hpf_order4(int bd_ix);
    void CalcBand_low_shelv(int bd_ix);
    void CalcBand_high_shelv(int bd_ix);
    void CalcBand_peak(int bd_ix);
    void CalcBand_notch(int bd_ix);
    
};
#endif