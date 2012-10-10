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
#include "colors.h"
#include "guiconstants.h"
#include "bodecalc.cpp"
#include <cmath>

#include <iostream>
#include <iomanip>
#include <cstring>

PlotEQCurve::PlotEQCurve(int iNumOfBands)
:m_TotalBandsCount(iNumOfBands), m_Bypass(false), bMotionIsConnected(false),
height(PLOT_HIGHT), width(PLOT_WIDTH)
{
   //Calc the number of points
   m_NumOfPoints = floor(log10(MAX_FREQ/MIN_FREQ)*NUM_POINTS_PER_DECADE) + 1;
    
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
  resetCurve();

  set_size_request(width, height);
  
  //Connect mouse signals
  add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK | Gdk::SCROLL_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &PlotEQCurve::on_button_press_event),true);
  signal_button_release_event().connect(sigc::mem_fun(*this, &PlotEQCurve::on_button_release_event),true);
  signal_scroll_event().connect(sigc::mem_fun(*this, &PlotEQCurve::on_scrollwheel_event),true);
  
  //Initialize the base vectors for the first widget size
  initBaseVectors();
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

void PlotEQCurve::on_realize()
{
  
  Gtk::Widget::on_realize();
  
  Gtk::Allocation allocation = get_allocation();
  width = allocation.get_width();
  height = allocation.get_height();
  initBaseVectors();
  reComputeRedrawAll();
}

void PlotEQCurve::initBaseVectors()
{
  //Initialize freq vector and pixels
  for(int i=0; i < m_NumOfPoints; i++)
  {
    f[i] = MIN_FREQ * pow(10, ((double)i)/((double)NUM_POINTS_PER_DECADE));
    xPixels[i] = freq2Pixels(f[i]);
  }
  
  //Initalize the grid
  const double f_grid[GRID_VERTICAL_LINES] = {20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0,
			100.0, 200.0, 300.0, 400.0, 500.0, 600.0, 700.0, 800.0, 900.0,
			1000.0, 2000.0, 3000.0, 4000.0, 5000.0, 6000.0, 7000.0, 8000.0, 9000.0,
			10000.0, 20000.0};
  for(int i=0; i < GRID_VERTICAL_LINES; i++)
  {
    xPixels_Grid[i] = freq2Pixels(f_grid[i]);
  }
}


void PlotEQCurve::reComputeRedrawAll()
{
  for(int i = 0; i < m_TotalBandsCount; i++)
  {
    ComputeFilter(i);
  }
  redraw();
}


void PlotEQCurve::resetCurve()
{
  for(int i = 0; i < m_NumOfPoints; i++)
  {
    main_y[i] = 0.0;
  }
  
  for (int i = 0; i<m_TotalBandsCount; i++)
  {
    m_filters[i]->bIsOn = false;
    m_filters[i]->Freq = 20.0;
    m_filters[i]->fType = PEAK;
    m_filters[i]->Gain = 0.0;
    m_filters[i]->Q = 2.0;
    
    //Reset band_y to zero
    for(int j = 0; j < m_NumOfPoints; j++)
    {
      band_y[i][j] = 0.0;
    }
  }
}

void PlotEQCurve::ComputeFilter(int bd_ix)
{
  /*
  //Restem a la main curve la corba de la banda que s'actualitza
  if (m_filters[bd_ix]->bIsOn)
  {
    for(int i = 0; i<m_NumOfPoints; i++)
    {
      main_y[i] = main_y[i] - band_y[bd_ix][i];
    }
  }
  */
  
  //Calculem els valors de la banda actualitzada
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
  
  //Reset main_y
  for (int j = 0; j < m_NumOfPoints; j++)
  {
    main_y[j] = 0.0;
  }
  
  //Compute Shape
  for(int i = 0; i < m_TotalBandsCount; i++)
  {
    if(m_filters[i]->bIsOn)
    {
      for (int j = 0; j < m_NumOfPoints; j++)
      {
	main_y[j] = main_y[j] + band_y[i][j];
      }
    }
  }
  
  /*
  //sumem els nous valors de la banda actualitzada  a la main curve
  if (m_filters[bd_ix]->bIsOn)
  {
    for(int i = 0; i<m_NumOfPoints; i++)
    {
      main_y[i] = main_y[i] + band_y[bd_ix][i];
    }
  }
  */
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

void  PlotEQCurve::setBandType(int bd_ix, int newType)
{
  m_filters[bd_ix]->fType = int2FilterType(newType);
  ComputeFilter(bd_ix);
  redraw();
}

void  PlotEQCurve::setBandEnable(int bd_ix, bool bIsEnabled)
{
  m_filters[bd_ix]->bIsOn = bIsEnabled;
  
  //Restem a la main curve la corba de la banda que es desactiva
  if (!bIsEnabled)
  {
    for(int i = 0; i<m_NumOfPoints; i++)
    {
      main_y[i] = main_y[i] - band_y[bd_ix][i];
    }
  }
  
  //Restaura la main curve
  else 
  {
    for(int i = 0; i<m_NumOfPoints; i++)
    {
      main_y[i] = main_y[i] + band_y[bd_ix][i];
    }
  }
  ComputeFilter(bd_ix);
  redraw();
}
    
void PlotEQCurve::setBypass(bool bypass)
{
  m_Bypass = bypass;
  redraw();
}

//==========================SIGNAL SLOTS===========================================================
//Mouse grab signal handlers
bool PlotEQCurve::on_button_press_event(GdkEventButton* event)
{
  //Check if is over some control pointer
  for(int i = 0; i < m_TotalBandsCount; i++)
  {
    if( m_filters[i]->bIsOn &&
	event->x > freq2Pixels(m_filters[i]->Freq) - 8 &&
	event->x < freq2Pixels(m_filters[i]->Freq) + 8 &&
	event->y > dB2Pixels(m_filters[i]->Gain) - 8 &&
	event->y < dB2Pixels(m_filters[i]->Gain) + 8 )
    {
      m_iBandSel = i;
      if (!bMotionIsConnected)
      {
	m_motion_connection = signal_motion_notify_event().connect(sigc::mem_fun(*this, &PlotEQCurve::on_mouse_motion_event),true);
	bMotionIsConnected = true;
      }
      break;
    }
  }
  
  return true;
}

bool PlotEQCurve::on_button_release_event(GdkEventButton* event)
{
  m_motion_connection.disconnect();
  bMotionIsConnected = false;
  
  return true;
}

bool PlotEQCurve::on_scrollwheel_event(GdkEventScroll* event)
{
    //Check if is over some control pointer
  for(int i = 0; i < m_TotalBandsCount; i++)
  {
    if( m_filters[i]->bIsOn &&
	event->x > freq2Pixels(m_filters[i]->Freq) - 5 &&
	event->x < freq2Pixels(m_filters[i]->Freq) + 5 &&
	event->y > dB2Pixels(m_filters[i]->Gain) - 5 &&
	event->y < dB2Pixels(m_filters[i]->Gain) + 5 )
    {
      if (event->direction == GDK_SCROLL_UP) 
      { 
	// up code
	m_filters[i]->Q += SCROLL_EVENT_INCREMENT*m_filters[i]->Q;
	m_filters[i]->Q = m_filters[i]->Q > PEAK_Q_MAX ? PEAK_Q_MAX : m_filters[i]->Q;
      } 
      else if (event->direction == GDK_SCROLL_DOWN) 
      { 
	// down code 
	m_filters[i]->Q -= SCROLL_EVENT_INCREMENT*m_filters[i]->Q;
	m_filters[i]->Q = m_filters[i]->Q < PEAK_Q_MIN ? PEAK_Q_MIN : m_filters[i]->Q;
      }
      ComputeFilter(m_iBandSel);
      redraw();  
        
      // emit the signal
      m_BandChangedSignal.emit( i, m_filters[i]->Gain, m_filters[i]->Freq, m_filters[i]->Q);
      break;
    }
  }
  
  return true;
}

bool PlotEQCurve::on_mouse_motion_event(GdkEventMotion* event)
{
  //Recompute curve on current band and redraw
  m_filters[m_iBandSel]->Freq = Pixels2freq(event->x);
  m_filters[m_iBandSel]->Freq = m_filters[m_iBandSel]->Freq > FREQ_MAX ? FREQ_MAX : m_filters[m_iBandSel]->Freq;
  m_filters[m_iBandSel]->Freq = m_filters[m_iBandSel]->Freq < FREQ_MIN ? FREQ_MIN : m_filters[m_iBandSel]->Freq;
  
  if (m_filters[m_iBandSel]->fType == PEAK ||
      m_filters[m_iBandSel]->fType == HIGH_SHELF ||
      m_filters[m_iBandSel]->fType == LOW_SHELF )
  {
    m_filters[m_iBandSel]->Gain = Pixels2dB(event->y);
    m_filters[m_iBandSel]->Gain = m_filters[m_iBandSel]->Gain > GAIN_MAX ? GAIN_MAX : m_filters[m_iBandSel]->Gain;
    m_filters[m_iBandSel]->Gain = m_filters[m_iBandSel]->Gain < GAIN_MIN ? GAIN_MIN : m_filters[m_iBandSel]->Gain;
  }
  
  else
  {
    m_filters[m_iBandSel]->Gain = 0.0;
  }
  
  ComputeFilter(m_iBandSel);
  redraw();  

  // emit the signal
  m_BandChangedSignal.emit(m_iBandSel, m_filters[m_iBandSel]->Gain, m_filters[m_iBandSel]->Freq, m_filters[m_iBandSel]->Q);

  return true;
}

//Override default signal handler:
bool PlotEQCurve::on_expose_event(GdkEventExpose* event)
{
 
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
  {
  
    Gtk::Allocation allocation = get_allocation();
    width = allocation.get_width();
    height = allocation.get_height();
    
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

    //Paint backgroud
    cr->save();
    cr->set_source_rgb(BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
    cr->paint(); //Fill all with background color
    cr->restore();
    
    //Draw the grid
    cr->save();
    cr->set_source_rgb(0, 0.3, 0.4);
    cr->set_line_width(1);
    for(int i = 0; i < GRID_VERTICAL_LINES; i++)
    {
      cr->move_to(xPixels_Grid[i] + 0.5, CURVE_MARGIN); 
      cr->line_to(xPixels_Grid[i] + 0.5, height - CURVE_MARGIN - CURVE_TEXT_OFFSET);
      cr->stroke();
    }
    
    for(int i = -DB_GRID_RANGE/2; i <= DB_GRID_RANGE/2; i+=5)
    {
      cr->move_to(CURVE_MARGIN + CURVE_TEXT_OFFSET, dB2Pixels(i) + 0.5);
      cr->line_to(width - CURVE_MARGIN, dB2Pixels(i) + 0.5);
      cr->stroke();
    }
    cr->restore();
        
    
    //Draw text with pango to grid
    cr->save();
    cr->set_source_rgb(0, 0.3, 0.4);
    Glib::RefPtr<Pango::Layout> pangoLayout = Pango::Layout::create(cr);
    Pango::FontDescription font_desc("sans 7");
    pangoLayout->set_font_description(font_desc);
    pangoLayout->set_alignment(Pango::ALIGN_RIGHT);
    
    //Db Scale
    for(int i = -DB_GRID_RANGE/2; i <= DB_GRID_RANGE/2; i+=10)
    {
      std::stringstream ss;
      ss<< std::setprecision(2) << i;
      cr->move_to(CURVE_MARGIN, dB2Pixels(i) - 3.5);
      pangoLayout->set_text(ss.str());
      pangoLayout->show_in_cairo_context(cr);
      cr->stroke();  
    }
    
    //Hz scale 20 Hz
    cr->move_to( xPixels_Grid[0] + 5, height - CURVE_MARGIN - CURVE_TEXT_OFFSET + 3.5);
    pangoLayout->set_text("20");
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke(); 
    
    //Hz scale 50 Hz
    cr->move_to( xPixels_Grid[3] - 5, height - CURVE_MARGIN - CURVE_TEXT_OFFSET  + 3.5);
    pangoLayout->set_text("50");
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();
    
    //Hz scale 100 Hz
    cr->move_to( xPixels_Grid[8] - 10, height - CURVE_MARGIN - CURVE_TEXT_OFFSET  + 3.5);
    pangoLayout->set_text("100");
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();
    
    //Hz scale 200 Hz
    cr->move_to( xPixels_Grid[9] - 10, height - CURVE_MARGIN - CURVE_TEXT_OFFSET  + 3.5);
    pangoLayout->set_text("200");
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();
    
    //Hz scale 500 Hz
    cr->move_to( xPixels_Grid[12] - 10, height - CURVE_MARGIN - CURVE_TEXT_OFFSET  + 3.5);
    pangoLayout->set_text("500");
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();
    
    //Hz scale 1 KHz
    cr->move_to( xPixels_Grid[17] - 5, height - CURVE_MARGIN - CURVE_TEXT_OFFSET  + 3.5);
    pangoLayout->set_text("1k");
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();
    
    //Hz scale 2 KHz
    cr->move_to( xPixels_Grid[18] - 5, height - CURVE_MARGIN - CURVE_TEXT_OFFSET + 3.5);
    pangoLayout->set_text("2k");
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();
    
    //Hz scale 5 KHz
    cr->move_to( xPixels_Grid[21] - 5, height - CURVE_MARGIN - CURVE_TEXT_OFFSET + 3.5);
    pangoLayout->set_text("5k");
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();
    
    //Hz scale 10 KHz
    cr->move_to( xPixels_Grid[26] - 5, height - CURVE_MARGIN - CURVE_TEXT_OFFSET + 3.5);
    pangoLayout->set_text("10k");
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();

    //Hz scale 20 KHz
    cr->move_to( xPixels_Grid[27] - 18, height - CURVE_MARGIN - CURVE_TEXT_OFFSET + 3.5);
    pangoLayout->set_text("20k");
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();
    cr->restore();
  
    //Draw the curve for each band
    double ydB;   
    for(int i = 0; i < m_TotalBandsCount; i++) //for each band
    {
      if(m_filters[i]->bIsOn and !m_Bypass) //If band is enabled and not bypass
      {
	//Draw curve area in band color
	cr->save();
	Gdk::Color color(bandColorLUT[i]);
	cr->set_source_rgba(color.get_red_p(), color.get_green_p(), color.get_blue_p(), 0.3);
	cr->move_to(CURVE_MARGIN + CURVE_TEXT_OFFSET + 0.5, dB2Pixels(0.0) + 0.5);
	for (int j = 0; j < m_NumOfPoints; j++)
	{
	  ydB = band_y[i][j] > DB_GRID_RANGE/2 ? DB_GRID_RANGE/2 : band_y[i][j];
	  ydB = ydB < -DB_GRID_RANGE/2 ? -DB_GRID_RANGE/2 : ydB;
	  cr->line_to(xPixels[j] + 0.5, dB2Pixels(ydB) + 0.5);
	}
	cr->line_to(width - CURVE_MARGIN + 0.5, dB2Pixels(0.0) + 0.5);
	cr->line_to(CURVE_MARGIN + CURVE_TEXT_OFFSET + 0.5, dB2Pixels(0.0) + 0.5);
	cr->fill();
	cr->restore();
      }
      
      else //band is disabled
      {
	//Draw curve in gary color
	cr->save();
	cr->set_source_rgba(1, 1, 1, 0.3);
	cr->move_to(CURVE_MARGIN + CURVE_TEXT_OFFSET + 0.5, dB2Pixels(0.0) + 0.5);
	for (int j = 0; j < m_NumOfPoints; j++)
	{
	  ydB = band_y[i][j] > DB_GRID_RANGE/2 ? DB_GRID_RANGE/2 : band_y[i][j];
	  ydB = ydB < -DB_GRID_RANGE/2 ? -DB_GRID_RANGE/2 : ydB;
	  cr->line_to(xPixels[j] + 0.5, dB2Pixels(ydB) + 0.5);
	}
	cr->line_to(width - CURVE_MARGIN + 0.5, dB2Pixels(0.0) + 0.5);
	cr->line_to(CURVE_MARGIN + CURVE_TEXT_OFFSET + 0.5, dB2Pixels(0.0) + 0.5);
	cr->fill();
	cr->restore();
      }
    
    }

    if (!m_Bypass)
    {
      //Draw the main curve
      cr->save();
      cr->set_source_rgb(1, 1, 1);
      cr->set_line_width(1);
      //cr->move_to(CURVE_MARGIN + CURVE_TEXT_OFFSET + 0.5, dB2Pixels(0.0) + 0.5);
      ydB = main_y[0] > DB_GRID_RANGE/2 ? DB_GRID_RANGE/2 : main_y[0];
      ydB = ydB < -DB_GRID_RANGE/2 ? -DB_GRID_RANGE/2 : ydB;
      cr->move_to(xPixels[0] + 0.5, dB2Pixels(ydB) + 0.5);
      for (int i = 1; i < m_NumOfPoints; i++)
      {
	ydB = main_y[i] > DB_GRID_RANGE/2 ? DB_GRID_RANGE/2 : main_y[i];
	ydB = ydB < -DB_GRID_RANGE/2 ? -DB_GRID_RANGE/2 : ydB;
	cr->line_to(xPixels[i] + 0.5, dB2Pixels(ydB) + 0.5);
      }
      cr->stroke();
      cr->restore();
      
      //Draw curve control ball
      double ball_x, ball_y;     
      for(int i = 0; i < m_TotalBandsCount; i++) //for each band
      {
	if(m_filters[i]->bIsOn) //If band is enabled
	{
	  ball_x = (double)freq2Pixels(m_filters[i]->Freq);
	  if( m_filters[i]->fType == PEAK || 
	      m_filters[i]->fType == LOW_SHELF ||
	      m_filters[i]->fType == HIGH_SHELF )
	  {
	    ball_y = (double)dB2Pixels(m_filters[i]->Gain);
	  }
	  else
	  {
	    ball_y =  (double)dB2Pixels(0.0);
	    m_filters[i]->Gain = 0.0;
	  }
	  
	  cr->save();
	  Gdk::Color color(bandColorLUT[i]);
	  cr->set_source_rgb(color.get_red_p(), color.get_green_p(), color.get_blue_p());
	  cr->arc(ball_x, ball_y, 5.0, 0.0, 6.28318530717958647693);
	  cr->fill();
	  cr->restore();
	  
	  cr->save();
	  cr->set_line_width(1);
	  cr->set_source_rgb(0.1,0.1,0.1);
	  cr->arc(ball_x, ball_y, 4.0, 0.0, 6.28318530717958647693);
	  cr->stroke();
	  cr->restore();
	}
      }
    }// end Bypass check
    
    //draw de outer grind box
    cr->save();
    cr->set_source_rgb(0, 0.3, 0.4);
    cr->set_line_width(1);
    cr->move_to(CURVE_MARGIN + CURVE_TEXT_OFFSET + 0.5, CURVE_MARGIN + 0.5); 
    cr->line_to(width - CURVE_MARGIN + 0.5, CURVE_MARGIN + 0.5);
    cr->line_to(width - CURVE_MARGIN + 0.5, height - CURVE_MARGIN - CURVE_TEXT_OFFSET + 0.5);
    cr->line_to(CURVE_MARGIN + CURVE_TEXT_OFFSET + 0.5 , height - CURVE_MARGIN - CURVE_TEXT_OFFSET + 0.5);
    cr->line_to(CURVE_MARGIN + CURVE_TEXT_OFFSET + 0.5, CURVE_MARGIN + 0.5); 
    cr->stroke();
    cr->restore();
    
  }
  return true;    
}

void PlotEQCurve::redraw()
{
  Glib::RefPtr<Gdk::Window> win = get_window();
  if(win)
  {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(), get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
}

int PlotEQCurve::dB2Pixels(double db)
{
  return (int)((((double)height)/2.0) - ((((double)height) - 2*CURVE_MARGIN - CURVE_TEXT_OFFSET)/DB_GRID_RANGE)*db - CURVE_TEXT_OFFSET/2);
}

int PlotEQCurve::freq2Pixels(double f)
{
  double px_decade = (((double)width) - 2*CURVE_MARGIN - CURVE_TEXT_OFFSET)/(log10(MAX_FREQ/MIN_FREQ));
  return (int)(px_decade*log10(f/MIN_FREQ) + CURVE_MARGIN + CURVE_TEXT_OFFSET);
}

double PlotEQCurve::Pixels2dB(int pixels)
{
  return DB_GRID_RANGE*((((double)height)-CURVE_TEXT_OFFSET-2*(double)pixels)/(2*((double)height) - 4*CURVE_MARGIN - 2*CURVE_TEXT_OFFSET));
}

double PlotEQCurve::Pixels2freq(int pixels)
{
  double px_decade = (((double)width) - 2*CURVE_MARGIN - CURVE_TEXT_OFFSET)/(log10(MAX_FREQ/MIN_FREQ));
  return MIN_FREQ*pow(10, (((double)pixels - CURVE_MARGIN- CURVE_TEXT_OFFSET)/(px_decade))); 
}


PlotEQCurve::signal_BandChanged PlotEQCurve::signal_changed()
{
  return m_BandChangedSignal;
}
