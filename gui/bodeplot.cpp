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

///////////////////////////////Membres de PlotEQCurve////////////////////
/************************Constructor*************************************/
PlotEQCurve::PlotEQCurve(main_window *m_ptr, void (*f_ptr) (main_window *myptr, int b_ix, float g, float f))
{
  //preparem el punter a taula de punters
  //FrameBand = new EQBandCtlPack *FrameBand[NUM_PEAK_BANDS];
  external_ptr = f_ptr;

  //Inicialitza punter a objecte principal
  main_win_ptr = m_ptr;
    
  set_size_request(PLOT_WIDTH+45,PLOT_HEIGHT+57); //No tocar aquest limits de pixels, son els marges
  scale(PlotMM::AXIS_LEFT)->set_enabled(true);
  scale(PlotMM::AXIS_RIGHT)->set_enabled(false);
  scale(PlotMM::AXIS_BOTTOM)->set_enabled(true);
  scale(PlotMM::AXIS_TOP)->set_enabled(false);
  
  scale(PlotMM::AXIS_BOTTOM)->set_autoscale(false);
  scale(PlotMM::AXIS_LEFT)->set_autoscale(false);
  
  scale(PlotMM::AXIS_BOTTOM)->set_range(FREQ_MIN,FREQ_MAX, true);
  scale(PlotMM::AXIS_LEFT)->set_range(GAIN_MIN,GAIN_MAX, false);
    
  label(PlotMM::AXIS_BOTTOM)->set_text("Fequency (Hz)");
  label(PlotMM::AXIS_LEFT)->set_text("Gain (dB)");
  label(PlotMM::AXIS_BOTTOM)->set_enabled(true);
  label(PlotMM::AXIS_LEFT)->set_enabled(true);
    
  /* set a plot title and some axis labels */
  title()->set_text("Eq Curve");
  title()->set_enabled(true);

  /* create some named curves with different colors and symbols */
  ZeroCurve= Glib::RefPtr<PlotMM::Curve>(new PlotMM::Curve("c0"));
  ZeroCurve->paint()->set_pen_color(Gdk::Color("white"));

  //Dibuixem una graella a base de plots a lo bestia
  int ii=30, jj=0;
  double ff[2], yy[2];
  
  while(ii <= 10000){
    ff[0]=ii;
    ff[1]=ii;
    yy[0]=GAIN_MIN;
    yy[1]=GAIN_MAX;
    freq_grid[jj]= Glib::RefPtr<PlotMM::Curve>(new PlotMM::Curve("f_grid"));
    freq_grid[jj]->paint()->set_pen_color(Gdk::Color("gray50"));
    freq_grid[jj]->set_data(ff,yy,2);
    add_curve(freq_grid[jj]);
    if(ii<100) ii=ii+10;
    else if(ii >=100 && ii<1000) ii=ii+100;
    else if(ii >= 1000) ii=ii+1000;
    jj++;
  }
  
   ii=-15, jj=0;
   ff[2], yy[2];
  
  while(jj < 6){
    ff[0]=FREQ_MIN;
    ff[1]=FREQ_MAX;
    yy[0]=ii;
    yy[1]=ii;
    gain_grid[jj]= Glib::RefPtr<PlotMM::Curve>(new PlotMM::Curve("g_grid"));
    gain_grid[jj]->paint()->set_pen_color(Gdk::Color("gray50"));
    gain_grid[jj]->set_data(ff,yy,2);
    add_curve(gain_grid[jj]);
    ii=ii+5;
    if(ii==0)ii=5;
    jj++;
  }

  //dades de inizialitzacio
  double d1=log10(FREQ_MIN);
  double d2=log10(FREQ_MAX);
  for(int i=0;i<NUM_POINTS;i++){
    main_y[i]=0;
    f[i]=pow(10,(d1+(i*(d2-d1))/(NUM_POINTS-1)));
    for(int j=0; j<NUM_OF_FILTERS; j++)band_y[j][i]=0;
  }
  

  for(int i=0; i<NUM_OF_FILTERS; i++){
    
    peak_Point[i]= Glib::RefPtr<PlotMM::Curve>(new PlotMM::Curve("peak_point")); //el punt
    
     //preparem el punt
    peak_Point[i]->symbol()->set_style(PlotMM::SYMBOL_ELLIPSE); //posa un simbol a cada punt
    peak_Point[i]->symbol()->set_size(8); //mida dels simbols a 10
    peak_Point[i]->set_curve_style(PlotMM::CURVE_NONE); //no dibuixis la corva, nomes simbols
    peak_Point[i]->symbol()->paint()->set_pen_color(Gdk::Color("white")); 
    
  }
  

  //colors dels simbols 
  //mirar fitxer /etc/X11/rgb.txt per veure colors possibles
  peak_Point[0]->symbol()->paint()->set_brush_color(Gdk::Color("brown")); 
  peak_Point[1]->symbol()->paint()->set_brush_color(Gdk::Color("yellow")); 
  peak_Point[2]->symbol()->paint()->set_brush_color(Gdk::Color("orange")); 
  peak_Point[3]->symbol()->paint()->set_brush_color(Gdk::Color("salmon3")); 
  peak_Point[4]->symbol()->paint()->set_brush_color(Gdk::Color("magenta1"));
  peak_Point[5]->symbol()->paint()->set_brush_color(Gdk::Color("red")); 
  peak_Point[6]->symbol()->paint()->set_brush_color(Gdk::Color("purple2")); 
  peak_Point[7]->symbol()->paint()->set_brush_color(Gdk::Color("blue")); 
  peak_Point[8]->symbol()->paint()->set_brush_color(Gdk::Color("LightBlue2")); 
  peak_Point[9]->symbol()->paint()->set_brush_color(Gdk::Color("green"));
  
    
  
  //Construim la corba principal, suma de les altres
  //m_colors.set_rgb_p(0.7,0.1,0.9);
  Master_Curve= Glib::RefPtr<PlotMM::Curve>(new PlotMM::Curve("master_curve")); //la corba
  Master_Curve->paint()->set_brush_color(Gdk::Color("royal blue"));
  Master_Curve->paint()->set_pen_color(Gdk::Color("royal blue"));
  add_curve(Master_Curve);
  Master_Curve->set_data(f,main_y,NUM_POINTS);
  Master_Curve->set_baseline(0.0);
  
  //Constrium corva del zero
  double f[2]={FREQ_MIN,FREQ_MAX}, y[2]={0,0};
  ZeroCurve->set_data(f,y,2); //dibuixem la corva de eix ZERO
  add_curve(ZeroCurve);
  
  for(int i=0; i<NUM_OF_FILTERS; i++)add_curve(peak_Point[i]);
  
  mouse_press=false;
  signal_plot_mouse_press().connect( sigc::mem_fun(*this, &PlotEQCurve::on_button_press));
  signal_plot_mouse_release().connect( sigc::mem_fun(*this, &PlotEQCurve::on_button_release));
  signal_plot_mouse_move().connect( sigc::mem_fun(*this, &PlotEQCurve::on_mouse_move));
 
  //precalculs canvis de coordenades
  y_vs_gain=(((double)(GAIN_MAX-GAIN_MIN))/(double)PLOT_HEIGHT);
  x_vs_freq=log10(((double)FREQ_MAX)/((double)FREQ_MIN))/(double)PLOT_WIDTH;
}
/***********************************************************************/


/**********************************Destructor************************/
PlotEQCurve::~PlotEQCurve()
{
}
/********************************************************************/

int PlotEQCurve::CalcFg(double &f, double &g){
//petita correccio de posicio xapusera
g=g+5;
f=f+5;

if(f>PLOT_WIDTH)f=PLOT_WIDTH;
    if(f<0.0)f=0.0;
    if(g>PLOT_HEIGHT)g=PLOT_HEIGHT;
    if(g<0.0)g=0.0;
    
    canvicoordenades(f, g);
    
    for(int i=NUM_OF_FILTERS-1; i>=0; i--){
      if( (f > peak_Point[i]->x(0)*0.85 && f< peak_Point[i]->x(0)*1.15) && //avans 0.95 i 1.05
          (g > peak_Point[i]->y(0)-0.9 && g< peak_Point[i]->y(0)+0.9)){    //avans 0.7 i 0.7
           return i;
          }
      }
  return -1;
}

/**********************Handler button press***********************************/
void PlotEQCurve::on_button_press(int freq, int gain, GdkEventButton* event){
  
  double f, g;
  f=(double)freq;
  g=(double)gain;
  BandSet=CalcFg(f,g);
  if(BandSet>=0) mouse_press=true;
  
}
/********************************************************************************

/**********************Handler mouse motion***********************************/
void PlotEQCurve::on_mouse_move(int freq, int gain, GdkEventMotion* event){
  if(mouse_press) {
    double f, g;
    f=(double)freq;
    g=(double)gain;
    
    CalcFg(f,g);
    
    //main_p->set_gain_freq(BandSet, (float)g, (float) f);
    //FrameBand[BandSet]->set_band_param(g, f);
    external_ptr(main_win_ptr, BandSet, (float) g, (float) f);
    
    /////////////////////////////////////////////////////////////////////////////<<<<<<<<<<<<<<<<<<<<<<<
    //////////////////////////////////
    //AKI ES ON ENVIEM LES NOVES DADES A CARREGAR EN EL FILTRE CORRESPONENT

    }
}
/********************************************************************************

/******************Handler Button Releas*******************************/
void PlotEQCurve::on_button_release(int freq, int gain, GdkEventButton* event){
  mouse_press=false;
}
/***********************************************************************/

void PlotEQCurve::canvicoordenades(double &freq, double &gain){

  gain=GAIN_MAX-y_vs_gain*gain;
  freq=pow(10,log10((double)FREQ_MIN)+x_vs_freq*freq);
}


/************************Funcio dibuix de bode**********************/
void PlotEQCurve::DrawPeakCurve(int band, double G, double F, double lQ, int Ftype){
  double gain, freq, Q;
  int Band_ID, Filter_type;
  
  Band_ID=band;
  gain=G;
  freq=F;
  Q=lQ;
  Filter_type=Ftype;
  
  //Comprovar
  /*
  std::cout<<"\nPlotEQCurve::DrawPeakCurve"<<std::endl;
  std::cout<<"Band ID = "<<Band_ID<<std::endl;
  std::cout<<"Filter Type = "<<Filter_type<<std::endl;
  std::cout<<"Gain = "<<gain<<std::endl;
  std::cout<<"Freq = "<<freq<<std::endl;
  std::cout<<"Q = "<<Q<<std::endl;*/
 
  //Restem a la main curve la corba de la banda que s'actualitza
  for(int i = 0; i<NUM_POINTS; i++){
    main_y[i] = main_y[i] - band_y[Band_ID][i];
  }
  
  //Calculem els valors de la banda actualitzada
  switch(Filter_type){
    case FILTER_OFF:
      CalcBand_filter_off(Band_ID);
    break;
    
    case LPF_ORDER_1:
      CalcBand_lpf_order1(Band_ID, freq);
    break;
    
    case LPF_ORDER_2:
      CalcBand_lpf_order2(Band_ID, freq, Q);
    break;
    
    case LPF_ORDER_3:
      CalcBand_lpf_order3(Band_ID, freq, Q);
    break;
    
    case LPF_ORDER_4:
      CalcBand_lpf_order4(Band_ID, freq, Q);
    break;
    
    case HPF_ORDER_1:
      CalcBand_hpf_order1(Band_ID, freq);
    break;
    
    case HPF_ORDER_2:
      CalcBand_hpf_order2(Band_ID, freq, Q);
    break;
    
    case HPF_ORDER_3:
      CalcBand_hpf_order3(Band_ID, freq, Q);
    break;
    
    case HPF_ORDER_4:
      CalcBand_hpf_order4(Band_ID, freq, Q);
    break;
    
    case LOW_SHELF:
      CalcBand_low_shelv(Band_ID, gain, freq, Q);
    break;
      
    case HIGH_SHELF:
      CalcBand_high_shelv(Band_ID, gain, freq, Q);
    break;
    
    case PEAK:
      CalcBand_peak(Band_ID, gain, freq, Q);
    break;
    
    case NOTCH:
      CalcBand_notch(Band_ID, freq, Q);
    break;
  }
  
  //sumem els nous valors de la banda actualitzada  a la main curve
    for(int i = 0; i<NUM_POINTS; i++){
    main_y[i] = main_y[i] + band_y[Band_ID][i];
  }
  
  //Dibuixem totes les corbes
  Master_Curve->set_data(f,main_y,NUM_POINTS);
  replot();

  peak_Point[Band_ID]->set_data(&freq,&gain,1);
  replot();
  //std::cout<<"Entrat a DrawPeakCurve()"<<std::endl;
}

 void PlotEQCurve::CalcBand_filter_off(int bd_ix){
   for(int i=0; i<NUM_POINTS; i++) band_y[bd_ix][i]=0;
 }
 
 void PlotEQCurve::CalcBand_lpf_order1(int bd_ix, double freq){
   double w, w2, Im, den;
   
   //Precalculables
   double wo=2*PI*freq;
   double wo2=wo*wo;
   double Re2=wo2*wo2;

  for(int i=0; i<NUM_POINTS; i++){
    w=2*PI*f[i]; 
    w2=w*w;

    Im=-w*wo;
    den=wo2+w2;

    band_y[bd_ix][i]=(double)20*log10(sqrt((Re2)+(Im*Im))/den);
    }
 }
 
 void PlotEQCurve::CalcBand_lpf_order2(int bd_ix, double freq, double Q){
  double w, w2, wo2w22, betha, Re, Im, den;

  //Precalculables
  double wo=2*PI*freq;
  double wo2=wo*wo;
  double wo4=wo2*wo2;
  double Q2=Q*Q;
  double wo3Q=-(wo*wo2)/Q;


  for(int i=0; i<NUM_POINTS; i++){
    w=2*PI*f[i]; 
    w2=w*w;

    wo2w22=wo2-w2;
    wo2w22=wo2w22*wo2w22;
    betha=wo2*w2;

    Re=wo4-betha;
    Im=wo3Q*w;
    den=wo2w22+(betha/Q2);

    band_y[bd_ix][i]=(double)20*log10(sqrt((Re*Re)+(Im*Im))/den);
  }
 }
 
 void PlotEQCurve::CalcBand_lpf_order3(int bd_ix, double freq, double Q){
  double aux_band[NUM_POINTS];
  CalcBand_lpf_order1(bd_ix, freq);
  
  for(int i=0; i<NUM_POINTS; i++)aux_band[i]=band_y[bd_ix][i];
  
  CalcBand_lpf_order2( bd_ix, freq, Q);
  
  for(int i=0; i<NUM_POINTS; i++) band_y[bd_ix][i]=band_y[bd_ix][i]+aux_band[i];  
 }
 
 void PlotEQCurve::CalcBand_lpf_order4(int bd_ix, double freq, double Q){
  double aux_band[NUM_POINTS];
  CalcBand_lpf_order2( bd_ix, freq, Q);
  
  for(int i=0; i<NUM_POINTS; i++)aux_band[i]=band_y[bd_ix][i];
  
  CalcBand_lpf_order2( bd_ix, freq, Q);
  
  for(int i=0; i<NUM_POINTS; i++) band_y[bd_ix][i]=band_y[bd_ix][i]+aux_band[i];  
 }
 
 void PlotEQCurve::CalcBand_hpf_order1(int bd_ix, double freq){
  double w, w2, Im, den;
  
  //Precalculables
  double wo=2*PI*freq;
  double wo2=wo*wo;

  for(int i=0; i<NUM_POINTS; i++){
    w=2*PI*f[i]; 
    w2=w*w;

    Im=w*wo;
    den=wo2+w2;

    band_y[bd_ix][i]=(double)20*log10(sqrt((w2*w2)+(Im*Im))/den);
    }
 }
 
 void PlotEQCurve::CalcBand_hpf_order2(int bd_ix, double freq, double Q){
  double w, w2, wo2w22, betha, Re, Im, den;
  
  //Precalculables
  double wo=2*PI*freq;
  double wo2=wo*wo;
  double Q2=Q*Q;
  double woQ=wo/Q;

  for(int i=0; i<NUM_POINTS; i++){
    w=2*PI*f[i]; 
    w2=w*w;

    wo2w22=wo2-w2;
    wo2w22=wo2w22*wo2w22;
    betha=wo2*w2;

    Re=(w2*w2)-betha;
    Im=woQ*w*w2;
    den=wo2w22+(betha/Q2);

    band_y[bd_ix][i]=(double)20*log10(sqrt((Re*Re)+(Im*Im))/den);
    }
 }
 
 void PlotEQCurve::CalcBand_hpf_order3(int bd_ix, double freq, double Q){
  double aux_band[NUM_POINTS];
  CalcBand_hpf_order1(bd_ix, freq);
  
  for(int i=0; i<NUM_POINTS; i++)aux_band[i]=band_y[bd_ix][i];
  
  CalcBand_hpf_order2( bd_ix, freq, Q);
  
  for(int i=0; i<NUM_POINTS; i++) band_y[bd_ix][i]=band_y[bd_ix][i]+aux_band[i];  
 }
 
 void PlotEQCurve::CalcBand_hpf_order4(int bd_ix, double freq, double Q){
  double aux_band[NUM_POINTS];
  CalcBand_hpf_order2( bd_ix, freq, Q);
  
  for(int i=0; i<NUM_POINTS; i++)aux_band[i]=band_y[bd_ix][i];
  
  CalcBand_hpf_order2( bd_ix, freq, Q);
  
  for(int i=0; i<NUM_POINTS; i++) band_y[bd_ix][i]=band_y[bd_ix][i]+aux_band[i];
 }
 
 void PlotEQCurve::CalcBand_low_shelv(int bd_ix, double gain, double freq, double Q){
  double w, w2, Re, Im, den;

  //Precalculables
  double wo=2*PI*freq;
  double A=pow(10,(gain/40));
  double wo2=wo*wo;
  double wo4=wo2*wo2;
  double Q2=Q*Q;
  double A2=A*A;
  double AQ2=A/Q2;
  double wo2AQ2_A2_1=(AQ2-A2-1)*wo2;
  double ArAQ=(1-A)*((A*sqrt(A))/Q);
  double wo3=wo2*wo;
  double AQ2wo2=AQ2*wo2;

  for(int i=0; i<NUM_POINTS; i++){
    w=2*PI*f[i]; 
    w2=w*w;

    Re=A*(A*(wo4+w2*w2)+w2*wo2AQ2_A2_1);
    Im= ArAQ*(wo3*w+wo*w2*w);
    den=wo2-A*w2;
    den=den*den;
    den=den+AQ2wo2*w2;

    band_y[bd_ix][i]=(double)20*log10(sqrt((Re*Re)+(Im*Im))/den);
    }
 }
 
 void PlotEQCurve::CalcBand_high_shelv(int bd_ix, double gain, double freq, double Q){
  double w, w2, Re, Im, den;
  
  //Precalculables
  double wo=2*PI*freq; 
  double A=pow(10,(gain/40));
  double wo2=wo*wo;
  double wo4=wo2*wo2;
  double Q2=Q*Q;
  double A2=A*A;
  double AQ2=A/Q2;
  double wo2AQ2_A2_1=(AQ2-A2-1)*wo2;
  double ArAQ=(1-A)*((A*sqrt(A))/Q);
  double wo3=wo2*wo;
  double AQ2wo2=AQ2*wo2;

  for(int i=0; i<NUM_POINTS; i++){
    w=2*PI*f[i]; 
    w2=w*w;

    Re=A*(A*(wo4+w2*w2)+w2*wo2AQ2_A2_1);
    Im= ArAQ*(wo3*w+wo*w2*w);
    den=A*wo2-w2;
    den=den*den;
    den=den+AQ2wo2*w2;

    band_y[bd_ix][i]=(double)20*log10(sqrt((Re*Re)+(Im*Im))/den);
    }
 }
 
 void PlotEQCurve::CalcBand_peak(int bd_ix, double gain, double freq, double Q){
  double w, w2, wo2w22, betha, Re, Im, den;

  //Precalculables
  double wo=2*PI*freq; 
  double A=pow(10,(gain/40));
  double A2=A*A;
  double wo2=wo*wo;
  double wo3=wo2*wo;
  double Q2=Q*Q;
  double wo2Q2=wo2/Q2;
  double gamma=(A2-1)/(A*Q);

//std::cout<<"\nPlotEQCurve::CalcBand_peak"<<std::endl;

  for(int i=0; i<NUM_POINTS; i++){
    w=2*PI*f[i]; 
    w2=w*w;

    wo2w22=wo2-w2;
    wo2w22=wo2w22*wo2w22;
    betha=wo2Q2*w2;

    Re=wo2w22+betha;
    Im=gamma*((wo3*w)-(wo*w2*w));
    den=wo2w22+(betha/A2);

    band_y[bd_ix][i]=(double)20*log10(sqrt((Re*Re)+(Im*Im))/den);
    //std::cout<<"i = "<<i<<"\tY = "<<band_y[bd_ix][i]<<std::endl;
    }
    
    
 }
 
 void PlotEQCurve::CalcBand_notch(int bd_ix, double freq, double Q){
  double w, w2, Re, Im, den;

  //Precalculables
  double wo=2*PI*freq; 
  double wo2=wo*wo;
  double wo4=wo2*wo2;
  double Q2=Q*Q;
  double wo3=wo2*wo;
  double doswo2=2*wo2;
  double woQ=wo/Q;
  double wo2Q2=wo2/Q2;

  for(int i=0; i<NUM_POINTS; i++){
    w=2*PI*f[i]; 
    w2=w*w;

    Re=wo4+w2*w2-doswo2*w2;
    Im=woQ*w*(w2-wo2);
    den=wo2-w2;
    den=den*den;
    den=den+wo2Q2*w2;

    band_y[bd_ix][i]=(double)20*log10(sqrt((Re*Re)+(Im*Im))/den);
    }
 }
/*****************************************************************/

void PlotEQCurve::Set_Bypass(bool bypass){

  if(bypass){
      Master_Curve->paint()->set_brush_color(Gdk::Color("gray50"));
      Master_Curve->paint()->set_pen_color(Gdk::Color("gray50"));
      }
  else {
    Master_Curve->paint()->set_brush_color(Gdk::Color("royal blue"));
    Master_Curve->paint()->set_pen_color(Gdk::Color("royal blue"));
    }
  replot();
}