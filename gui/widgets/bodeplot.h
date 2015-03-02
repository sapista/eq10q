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
#include "../eq_defines.h"

#define NUM_POINTS_PER_DECADE 150
#define MIN_FREQ 18.0
#define MAX_FREQ 22000.0
#define GRID_VERTICAL_LINES 28
#define DB_GRID_RANGE 50.0
#define CURVE_MARGIN 8
#define CURVE_BORDER 1.5
#define CURVE_TEXT_OFFSET 18
#define PLOT_HIGHT 250
#define PLOT_WIDTH 300
#define SCROLL_EVENT_INCREMENT 0.3
#define AUTO_REFRESH_TIMEOUT_MS 20

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
    
    void reComputeRedrawAll();
    void resetCurve();
    void setBandParamsQuiet(int bd_ix, float newGain, float newFreq, float newQ, int newType, bool bIsEnabled);
    virtual void setBandGain(int bd_ix, float newGain);
    virtual void setBandFreq(int bd_ix, float newFreq);
    virtual void setBandQ(int bd_ix, float newQ);
    virtual void setBandType(int bd_ix, int newType);
    virtual void setBandEnable(int bd_ix, bool bIsEnabled);
    virtual void setBypass(bool bypass);
    virtual void setSampleRate(double samplerate);
    virtual void setFftData();
    virtual void setFftActive(bool active, bool isSpectrogram);
    virtual void setFftGain(double g);
    virtual void setFftHold(bool hold);
    virtual void glowBand(int band);
    virtual void unglowBands();
    
    //signal accessor:
    //Slot prototype: void on_band_changed(int band_ix, float Gain, float Freq, float Q);
    typedef sigc::signal<void, int, float, float, float> signal_BandChanged;
    signal_BandChanged signal_changed();
    
    //Slot prototype: void on_band_enabled(int band_ix, bool enabled);
    typedef sigc::signal<void, int, bool> signal_BandEnabled;
    signal_BandEnabled signal_enabled();
    
    //Slot prototype: void on_band_selected(int band_ix, bool enabled);
    typedef sigc::signal<void, int> signal_BandSelected;
    signal_BandSelected signal_selected();
    
    //Slot prototype: void on_band_selected(int band_ix, bool enabled);
    typedef sigc::signal<void> signal_BandUnselected;
    signal_BandUnselected signal_unselected();
    
    //Public fft data to allow copying directly by atom
    double *fft_raw_data;
    
  protected:    
      //Mouse grab signal handlers
      virtual bool on_button_press_event(GdkEventButton* event);
      virtual bool on_button_release_event(GdkEventButton* event);
      virtual bool on_scrollwheel_event(GdkEventScroll* event);
      virtual bool on_mouse_motion_event(GdkEventMotion* event);
      virtual bool on_timeout();
      virtual bool on_mouse_leave_widget(GdkEventCrossing* event);
      virtual void redraw();
  
      //Override default signal handler:
      virtual bool on_expose_event(GdkEventExpose* event);
    
  private:
    int width, height; 
    int m_TotalBandsCount;
    int m_NumOfPoints;
    bool m_Bypass;
    int m_iBandSel;
    bool bMotionIsConnected;
    bool bBandFocus;
    int iRedrawByTimer;
    bool bIsFirstRun;
    double SampleRate;
    bool m_FftActive;
    
    //To hadle mouse mouve events
    sigc::connection m_motion_connection;
    
    //Store filters data
    FilterBandParams **m_filters;  //This pointer is initialized by construcor to an array of total num of bands
    
    //X axes LUT tables
    int xPixels_Grid[GRID_VERTICAL_LINES]; //Pixels used to draw the grind in logspace
    double *f; //This pointer is initialized by construcor to an array of total num of points acording min/max freq define
    int *xPixels; //This pointer is initialized by construcor to an array of total num of points, each item is the pixel space transaltion of corresponding freq
    
    //Curve vector for Y axes in dB units
    double *main_y; //This pointer is initialized by construcor to an array of total num of points
    double **band_y;  //This pointer is initialized by construcor to an array acording the format band_y[bd_ix][num_points]
    
    //FFT vectors
    double *xPixels_fft;
    double *fft_pink_noise;
    double *fft_plot;
    double *fft_gradient_LUT;
    double *fft_ant_data;
    double fft_gain;
    float *fft_log_lut;
    bool m_bIsSpectrogram, m_bFftHold;
    Cairo::RefPtr<Cairo::ImageSurface> m_fft_surface_ptr;    
    
    //Bode change signal
    signal_BandChanged m_BandChangedSignal;
    signal_BandEnabled m_BandEnabledSignal;
    signal_BandSelected m_BandSelectedSignal;
    signal_BandUnselected m_BandUnselectedSignal;
    
    //Method to initialize base vectors xPixels_Grind, f, xPixels
    void initBaseVectors();
    
    //Function for dB to pixels convertion
    double dB2Pixels(double db);
    
    //Function for Hz to pixels convertion
    double freq2Pixels(double f);
    
    //Function for pixels to dB convertion
    double Pixels2dB(double pixels);
    
    //Function for pixels to Hz convertion
    double Pixels2freq(double pixels);
    
    //Compute a filter points
    void ComputeFilter(int bd_ix); 
    
    //Curve math functions   
    void CalcBand_DigitalFilter(int bd_ix);
    
};
#endif