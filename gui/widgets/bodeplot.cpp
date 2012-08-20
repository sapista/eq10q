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

#include "bodeplot.h"
#include <cmath>

PlotEQCurve::PlotEQCurve(int iNumOfBands)
:m_TotalBandsCount(iNumOfBands)
{
   //Calc the number of points
   m_NumOfPoints = floor(log10(MAX_FREQ/MIN_FREQ)*NUM_POINTS_PER_DECADE);
    

  //Allocate memory for filter data strcuts
  m_filters = new FilterBandParams*[m_TotalBandsCount];
  for (int i = 0; i<m_TotalBandsCount; i++)
  {
    m_filters[i] = new FilterBandParams;
  }
  
  //Allocate memory for f and pixels arrays
  f = new double[m_NumOfPoints];
  xPixels = new int[m_NumOfPoints];
  
  //Allocate memory for Y axes arrays
  main_y = new double[m_NumOfPoints];
  
  band_y = new double*[m_TotalBandsCount];
  for (int i = 0; i<m_TotalBandsCount; i++)
  {
    band_y[i] = new double[m_NumOfPoints];
  }

  
  //TODO incialitzar vector de frequencies ho tens apuntat en un paperet!!!!!!!!! al boloso!!!!!

}

PlotEQCurve::~PlotEQCurve()
{
  //Delete filter structs
  for (int i = 0; i<m_TotalBandsCount; i++)
  {
    delete m_filters[i];
  }
  delete m_filters;
  
  //Delete freq and pixels pointers
  delete f;
  delete xPixels;
  
  //Delete Y array pointers
  delete main_y; 
  for (int i = 0; i<m_TotalBandsCount; i++)
  {
    delete band_y[i];
  }
  delete band_y;
  
}

void PlotEQCurve::ComputeFilter(int bd_ix)
{
  //Restem a la main curve la corba de la banda que s'actualitza
  for(int i = 0; i<m_NumOfPoints; i++){
    main_y[i] = main_y[i] - band_y[bd_ix][i];
  }
  
  //Calculem els valors de la banda actualitzada
  if (m_filters[bd_ix]->bIsOn)
  {
    switch(m_filters[bd_ix]->fType){    
      case LPF_ORDER_1:
	CalcBand_lpf_order1(bd_ix);
      break;
      
      case LPF_ORDER_2:
	CalcBand_lpf_order2(bd_ix);
      break;
      
      case LPF_ORDER_3:
	CalcBand_lpf_order3(bd_ix);
      break;
      
      case LPF_ORDER_4:
	CalcBand_lpf_order4(bd_ix);
      break;
      
      case HPF_ORDER_1:
	CalcBand_hpf_order1(bd_ix);
      break;
      
      case HPF_ORDER_2:
	CalcBand_hpf_order2(bd_ix);
      break;
      
      case HPF_ORDER_3:
	CalcBand_hpf_order3(bd_ix);
      break;
      
      case HPF_ORDER_4:
	CalcBand_hpf_order4(bd_ix);
      break;
      
      case LOW_SHELF:
	CalcBand_low_shelv(bd_ix);
      break;
	
      case HIGH_SHELF:
	CalcBand_high_shelv(bd_ix);
      break;
      
      case PEAK:
	CalcBand_peak(bd_ix);
      break;
      
      case NOTCH:
	CalcBand_notch(bd_ix);
      break;
    }
  }
  else
  {
    CalcBand_filter_off(bd_ix);
  }
  //sumem els nous valors de la banda actualitzada  a la main curve
  for(int i = 0; i<m_NumOfPoints; i++)
  {
    main_y[i] = main_y[i] + band_y[bd_ix][i];
  }
}

//===============================DATA ACCESORS===================================================================
void  PlotEQCurve::setBandGain(int bd_ix, float newGain)
{
  m_filters[bd_ix]->Gain = newGain;
  ComputeFilter(bd_ix);
  redraw();
}

void  PlotEQCurve::setBandFreq(int bd_ix, float newFreq)
{
  m_filters[bd_ix]->Freq = newFreq;
  ComputeFilter(bd_ix);
  redraw();
}

void  PlotEQCurve::setBandQ(int bd_ix, float newQ)
{
  m_filters[bd_ix]->Q = newQ;
  ComputeFilter(bd_ix);
  redraw();
}

void  PlotEQCurve::setBandType(int bd_ix, int newType) ///TODO: are you shure that this is an integer type???
{
  m_filters[bd_ix]->fType = int2FilterType(newType);
  ComputeFilter(bd_ix);
  redraw();
}

void  PlotEQCurve::setBandEnable(int bd_ix, bool bIsEnabled)
{
  m_filters[bd_ix]->bIsOn = bIsEnabled;
  ComputeFilter(bd_ix);
  redraw();  
}
    
void PlotEQCurve::setBypass(bool bypass)
{
///TODO canviar totes les curves a color gris....
  if(bypass)
  {
    //TODO set all colors to gray
  }
  else
  {
    //TODO set colors to color bands
  }
  redraw();
}

//==========================SIGNAL SLOTS===========================================================
//Mouse grab signal handlers
bool PlotEQCurve::on_button_press_event(GdkEventButton* event)
{
  //TODO: check if is over some control pointer
  //connect signal for mouse move
}

bool PlotEQCurve::on_button_release_event(GdkEventButton* event)
{
  //TODO: if was previously connected... maybe I need a bool flag... disonnect
}

bool PlotEQCurve::on_scrollwheel_event(GdkEventScroll* event)
{
  //TODO: if is over some control point, edit the Q setting for de corresponding band and redraw
  
}

bool PlotEQCurve::on_mouse_motion_event(GdkEventMotion* event)
{
  //TODO: recompute curve on current band and redraw... maybe I need another var in this clas to grab the selected band index...
}

//Override default signal handler:
bool PlotEQCurve::on_expose_event(GdkEventExpose* event)
{
  
}

void PlotEQCurve::redraw()
{
  //TODO: copy from other DrawingArea widget
}

int PlotEQCurve::dB2Pixels(double db)
{
 ///TODO:
}

PlotEQCurve::signal_BandChanged PlotEQCurve::signal_changed()
{
  return m_BandChangedSignal;
}
