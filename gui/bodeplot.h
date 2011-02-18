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


#include <iostream>

#include <plotmm/plot.h>
#include <plotmm/scalediv.h>
#include <plotmm/curve.h>
#include <plotmm/symbol.h>
#include <plotmm/paint.h>

#include "constants.h"

#define PLOT_WIDTH 960
#define PLOT_HEIGHT 230


#ifndef  MAIN_WINDOW_WIDGET
  #define MAIN_WINDOW_WIDGET
  class main_window;
#endif

/**********************Classe per fer plots*****************************/
class PlotEQCurve : public PlotMM::Plot
{
public:
  PlotEQCurve(main_window *m_ptr, void (*f_ptr) (main_window *myptr, int b_ix, float g, float f));
  virtual ~PlotEQCurve();
  virtual void DrawPeakCurve(int band, double G, double F, double lQ, int Ftype);
  virtual void on_button_press(int a, int b, GdkEventButton* event);
  virtual void on_button_release(int a, int b, GdkEventButton* event);
  virtual void on_mouse_move(int a, int b, GdkEventMotion* event);
  virtual int CalcFg(double &f, double &g);
  virtual void Set_Bypass(bool bypass);
  
protected:
 bool mouse_press; //flag pel mouse
 double f[NUM_POINTS];
 void canvicoordenades(double &freq, double &gain);
 double y_vs_gain, x_vs_freq; //pendents pel canvi de coordenades
 int BandSet;
 
 //Vector de corbes
 double main_y[NUM_POINTS], band_y[NUM_OF_FILTERS][NUM_POINTS];
 
 //Cada Banda del EQ
 //EQBandCtlPack **FrameBand;

 //Gdk::Color m_colors;

 Glib::RefPtr<PlotMM::Curve> ZeroCurve;
 Glib::RefPtr<PlotMM::Curve> freq_grid[26], gain_grid[6] ;
 //Glib::RefPtr<PlotMM::Curve> peak_Curve;
 Glib::RefPtr<PlotMM::Curve> peak_Point[NUM_OF_FILTERS]; 
 Glib::RefPtr<PlotMM::Curve> Master_Curve; 
 
 //Funcions de calcul de corves
 void CalcBand_filter_off(int bd_ix);
 void CalcBand_lpf_order1(int bd_ix, double freq);
 void CalcBand_lpf_order2(int bd_ix, double freq, double Q);
 void CalcBand_lpf_order3(int bd_ix, double freq, double Q);
 void CalcBand_lpf_order4(int bd_ix, double freq, double Q);
 void CalcBand_hpf_order1(int bd_ix, double freq);
 void CalcBand_hpf_order2(int bd_ix, double freq, double Q);
 void CalcBand_hpf_order3(int bd_ix, double freq, double Q);
 void CalcBand_hpf_order4(int bd_ix, double freq, double Q);
 void CalcBand_low_shelv(int bd_ix, double gain, double freq, double Q);
 void CalcBand_high_shelv(int bd_ix, double gain, double freq, double Q);
 void CalcBand_peak(int bd_ix, double gain, double freq, double Q);
 void CalcBand_notch(int bd_ix, double freq, double Q);

  //Function ptr to main
 void (*external_ptr) (main_window *ptr, int b_ix, float g, float f);

  //punter a main_window
  main_window *main_win_ptr;
  
};
/*****************************************************************************/
