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
#define PI 3.1416

inline void PlotEQCurve::CalcBand_filter_off(int bd_ix)
{
  for(int i=0; i<m_NumOfPoints; i++)
  {
    band_y[bd_ix][i]=0;
  }
}
 
inline void PlotEQCurve::CalcBand_lpf_order1(int bd_ix)
{
  double w, w2, Im, den;

  //Precalculables
  double wo=2*PI*m_filters[bd_ix]->Freq;
  double wo2=wo*wo;
  double Re2=wo2*wo2;

  for(int i=0; i<m_NumOfPoints; i++)
  {
    w=2*PI*f[i]; 
    w2=w*w;

    Im=-w*wo;
    den=wo2+w2;

    band_y[bd_ix][i]=(double)20*log10(sqrt((Re2)+(Im*Im))/den);
  }
}
 
inline void PlotEQCurve::CalcBand_lpf_order2(int bd_ix)
{
  double w, w2, wo2w22, betha, Re, Im, den;
  double Q = m_filters[bd_ix]->Q;

  //Precalculables
  double wo=2*PI*m_filters[bd_ix]->Freq;
  double wo2=wo*wo;
  double wo4=wo2*wo2;
  double Q2=Q*Q;
  double wo3Q=-(wo*wo2)/Q;


  for(int i=0; i<m_NumOfPoints; i++)
  {
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
 
inline void PlotEQCurve::CalcBand_lpf_order3(int bd_ix)
{
  double aux_band[m_NumOfPoints];
  CalcBand_lpf_order1(bd_ix);

  for(int i=0; i<m_NumOfPoints; i++)
  {
    aux_band[i]=band_y[bd_ix][i];
  }

  CalcBand_lpf_order2( bd_ix);

  for(int i=0; i<m_NumOfPoints; i++)
  {  
    band_y[bd_ix][i]=band_y[bd_ix][i]+aux_band[i];  
  }
}
 
inline void PlotEQCurve::CalcBand_lpf_order4(int bd_ix)
{
  /*
  double aux_band[m_NumOfPoints];
  CalcBand_lpf_order2( bd_ix);

  for(int i=0; i<m_NumOfPoints; i++)
  {
    aux_band[i]=band_y[bd_ix][i];
  }

  CalcBand_lpf_order2( bd_ix);

  for(int i=0; i<m_NumOfPoints; i++)
  {
    band_y[bd_ix][i]=band_y[bd_ix][i]+aux_band[i];  
  }*/
  
  // TEST Testing faster implementation
  CalcBand_lpf_order2( bd_ix);
  for(int i=0; i<m_NumOfPoints; i++)
  {
    band_y[bd_ix][i]= 2*band_y[bd_ix][i];  
  }
}
 
inline void PlotEQCurve::CalcBand_hpf_order1(int bd_ix)
{
  double w, w2, Im, den;

  //Precalculables
  double wo=2*PI*m_filters[bd_ix]->Freq;
  double wo2=wo*wo;

  for(int i=0; i<m_NumOfPoints; i++)
  {
    w=2*PI*f[i]; 
    w2=w*w;

    Im=w*wo;
    den=wo2+w2;

    band_y[bd_ix][i]=(double)20*log10(sqrt((w2*w2)+(Im*Im))/den);
  }
}
 
inline void PlotEQCurve::CalcBand_hpf_order2(int bd_ix)
{
  double w, w2, wo2w22, betha, Re, Im, den;
  double Q = m_filters[bd_ix]->Q;

  //Precalculables
  double wo=2*PI*m_filters[bd_ix]->Freq;
  double wo2=wo*wo;
  double Q2=Q*Q;
  double woQ=wo/Q;

  for(int i=0; i<m_NumOfPoints; i++)
  {
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
 
inline void PlotEQCurve::CalcBand_hpf_order3(int bd_ix)
{
  double aux_band[m_NumOfPoints];
  CalcBand_hpf_order1(bd_ix);

  for(int i=0; i<m_NumOfPoints; i++)
  {
    aux_band[i]=band_y[bd_ix][i];
  }

  CalcBand_hpf_order2( bd_ix);

  for(int i=0; i<m_NumOfPoints; i++)
  {
    band_y[bd_ix][i]=band_y[bd_ix][i]+aux_band[i];  
  }
}
 
inline void PlotEQCurve::CalcBand_hpf_order4(int bd_ix)
{
  /*
  double aux_band[m_NumOfPoints];
  CalcBand_hpf_order2( bd_ix, freq, Q);

  for(int i=0; i<m_NumOfPoints; i++)
  {
    aux_band[i]=band_y[bd_ix][i];
  }

  CalcBand_hpf_order2( bd_ix, freq, Q);

  for(int i=0; i<m_NumOfPoints; i++)
  {
    band_y[bd_ix][i]=band_y[bd_ix][i]+aux_band[i];
  }
  */
  
  // TEST Testing a faster implementation
  
  CalcBand_hpf_order2( bd_ix);

  for(int i=0; i<m_NumOfPoints; i++)
  {
    band_y[bd_ix][i]=2*band_y[bd_ix][i];
  }
}
 
inline void PlotEQCurve::CalcBand_low_shelv(int bd_ix)
{
  double w, w2, Re, Im, den;
  double Q = m_filters[bd_ix]->Q;
  
  //Precalculables
  double wo=2*PI*m_filters[bd_ix]->Freq;
  double A=pow(10,((m_filters[bd_ix]->Gain)/40));
  double wo2=wo*wo;
  double wo4=wo2*wo2;
  double Q2=Q*Q;
  double A2=A*A;
  double AQ2=A/Q2;
  double wo2AQ2_A2_1=(AQ2-A2-1)*wo2;
  double ArAQ=(1-A)*((A*sqrt(A))/Q);
  double wo3=wo2*wo;
  double AQ2wo2=AQ2*wo2;

  for(int i=0; i<m_NumOfPoints; i++)
  {
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
 
inline void PlotEQCurve::CalcBand_high_shelv(int bd_ix)
{
  double w, w2, Re, Im, den;
  double Q = m_filters[bd_ix]->Q;
  
  //Precalculables
  double wo=2*PI*m_filters[bd_ix]->Freq; 
  double A=pow(10,((m_filters[bd_ix]->Gain)/40));
  double wo2=wo*wo;
  double wo4=wo2*wo2;
  double Q2=Q*Q;
  double A2=A*A;
  double AQ2=A/Q2;
  double wo2AQ2_A2_1=(AQ2-A2-1)*wo2;
  double ArAQ=(1-A)*((A*sqrt(A))/Q);
  double wo3=wo2*wo;
  double AQ2wo2=AQ2*wo2;

  for(int i=0; i<m_NumOfPoints; i++)
  {
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
 
inline void PlotEQCurve::CalcBand_peak(int bd_ix)
{
  double w, w2, wo2w22, betha, Re, Im, den;
  double Q = m_filters[bd_ix]->Q;
  
  //Precalculables
  double wo=2*PI*m_filters[bd_ix]->Freq; 
  double A=pow(10,((m_filters[bd_ix]->Gain)/40));
  double A2=A*A;
  double wo2=wo*wo;
  double wo3=wo2*wo;
  double Q2=Q*Q;
  double wo2Q2=wo2/Q2;
  double gamma=(A2-1)/(A*Q);

  //std::cout<<"\nPlotEQCurve::CalcBand_peak"<<std::endl;

  for(int i=0; i<m_NumOfPoints; i++)
  {
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
 
inline void PlotEQCurve::CalcBand_notch(int bd_ix)
{
  double w, w2, Re, Im, den;
  double Q = m_filters[bd_ix]->Q;
  
  //Precalculables
  double wo=2*PI*m_filters[bd_ix]->Freq;
  double wo2=wo*wo;
  double wo4=wo2*wo2;
  double Q2=Q*Q;
  double wo3=wo2*wo; ///TODO: Variable sense utilitzar!!!!! OJU verifica be el notch! si va be em carrego akesta linea
  double doswo2=2*wo2;
  double woQ=wo/Q;
  double wo2Q2=wo2/Q2;

  for(int i=0; i<m_NumOfPoints; i++)
  {
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