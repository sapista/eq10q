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

#include <lv2plugin.hpp>

#include <cmath>
#include "constants.h"


typedef struct _Filter{
   float b0, b1, b2, a1, a2; //coefs de segon ordre
   float b1_0, b1_1, a1_1; //coefs de primer ordre
   float buffer[3];  //buffer de segon ordre
   float buffer1[2]; //buffer de primer ordre
   float buffer_extra[3]; //buffer extra per fer filtres de 4t ordre
   int filter_type; //ID del tipus de filtre defint a constants.h
   int filter_order;  //defineix l'ordre del filtre
   float gain, freq, Q; //parametres del filtre
}Filter;

using namespace LV2;


class ParamEQ : public Plugin<ParamEQ> {
public:
  
  ParamEQ(double rate)
     : Plugin<ParamEQ>( NUM_OF_LV2_PORTS),
    m_min(1.0 / 256),
    m_decay(exp(log(m_min) / (1 * rate))){

    in_values=0.0;
    out_values=0.0;

    fs=float(rate);
    iport=FIRST_FILTER_PORT_INDEX; //contra el primer ctl_port del filtre a llegir
    in_gain=1;
    out_gain=1;
    bypass=false;

  //dades de inicccccccccZccccccccccialitzacio
  float d1=log10(FREQ_MIN);
  float d2=log10(FREQ_MAX);
  for(int i=0;i<NUM_POINTS;i++){
    f[i]=pow(10,(d1+(i*(d2-d1))/(NUM_POINTS-1)));
  }

for(int i=0; i< NUM_OF_FILTERS; i++){
  filters[i].filter_type = FILTER_OFF; //tots els filtres disabled
  flush_buffers(i);
}

}//end of constructor

void flush_buffers(int filter_id);
void calc_coefs(int filter_ID,  int filter_type, int freq_ptr, float p2, float p3);
  
void run(uint32_t nframes) {
  //lectura dels ports
  float Gain, freq_ptr, local_Q, w;
  int filter_ID, f_type;

  filter_ID = (iport-FIRST_FILTER_PORT_INDEX)/4;
  f_type =  (int)*p(iport);
  Gain = *p(iport+1);
  freq_ptr = *p(iport+2);
  local_Q = *p(iport+3);

  if( f_type != FILTER_OFF){
  //si el port es diferent al vector de parametres
    if(filters[filter_ID].filter_type != f_type || 
      filters[filter_ID].gain != Gain ||
      filters[filter_ID].freq != freq_ptr ||
      filters[filter_ID].Q    != local_Q )
      { 
      calc_coefs(filter_ID,  f_type,(int)freq_ptr, Gain, local_Q);
      filters[filter_ID].gain = Gain;
      filters[filter_ID].freq = freq_ptr;
      filters[filter_ID].Q = local_Q;

      //obtenim l'ordre del filtre
      switch(f_type){
        case LPF_ORDER_1: case HPF_ORDER_1:
          filters[filter_ID].filter_order = 1;
        break;

       case LPF_ORDER_2: case HPF_ORDER_2:
       case LOW_SHELF: case HIGH_SHELF: 
       case PEAK: case NOTCH:
          filters[filter_ID].filter_order = 2;
        break;

        case LPF_ORDER_3: case HPF_ORDER_3:
          filters[filter_ID].filter_order = 3;
        break;

        case LPF_ORDER_4: case HPF_ORDER_4:
          filters[filter_ID].filter_order = 4;
        break;
       }//Final del switch 
    }
  }

  else if (filters[filter_ID].filter_type != f_type) flush_buffers(filter_ID); //netejo els buffer per no provocar un click la propera acivacio del filtre

  filters[filter_ID].filter_type = f_type;

  iport = iport + 4;

  if(iport == NUM_OF_LV2_PORTS-2){ //hem comprobat tots els ports de filtres
      in_gain = pow(10,(*p(3))/20);
      out_gain = pow(10,(*p(4))/20);
      bypass = *p(2) > 0 ? true : false;
      iport = FIRST_FILTER_PORT_INDEX;
  }
  //Fi de la lectura de ports

  //Bucle de processat de senyal
    for (uint32_t i = 0; i < nframes; ++i){

      w = p(1)[i]; //pilla la mostra del audio port

      if(!bypass){
      w=w*in_gain;

      //Actualitzar el Vu meter
      vu_value = std::abs(w);
      in_values = vu_value > in_values ? vu_value : in_values;



      for (int j=0; j < NUM_OF_FILTERS; j++){
 
          //Compute filter
        if(filters[j].filter_type != FILTER_OFF){ //si el filtre esta enabled 
          
          switch(filters[j].filter_order){ 
            //Processat 1r ordre
            case 1:
              //w(n)=x(n)-a1*w(n-1)
              filters[j].buffer1[0] = w-filters[j].a1_1*filters[j].buffer1[1];
              //y(n)=bo*w(n)+b1*w(n-1)
              w = filters[j].b1_0*filters[j].buffer1[0] + filters[j].b1_1*filters[j].buffer1[1];

              filters[j].buffer1[1] = filters[j].buffer1[0];
            break;

            //Processat 2n ordre
            case 2:
              //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
              filters[j].buffer[0] = w-filters[j].a1*filters[j].buffer[1]-filters[j].a2*filters[j].buffer[2];
              //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
              w = filters[j].b0*filters[j].buffer[0] + filters[j].b1*filters[j].buffer[1]+ filters[j].b2*filters[j].buffer[2];

              filters[j].buffer[2] = filters[j].buffer[1];
              filters[j].buffer[1] = filters[j].buffer[0];
            break;
          
            //Processat 3r ordre
            case 3:
              //w(n)=x(n)-a1*w(n-1)
              filters[j].buffer1[0] = w-filters[j].a1_1*filters[j].buffer1[1];
              //y(n)=bo*w(n)+b1*w(n-1)
              w = filters[j].b1_0*filters[j].buffer1[0] + filters[j].b1_1*filters[j].buffer1[1];

              filters[j].buffer1[1] = filters[j].buffer1[0];

              //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
              filters[j].buffer[0] = w-filters[j].a1*filters[j].buffer[1]-filters[j].a2*filters[j].buffer[2];
              //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
              w = filters[j].b0*filters[j].buffer[0] + filters[j].b1*filters[j].buffer[1]+ filters[j].b2*filters[j].buffer[2];

              filters[j].buffer[2] = filters[j].buffer[1];
              filters[j].buffer[1] = filters[j].buffer[0];
            
            break;
          
            //Processat 4t ordre
            case 4:
              //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
              filters[j].buffer[0] = w-filters[j].a1*filters[j].buffer[1]-filters[j].a2*filters[j].buffer[2];
              //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
              w = filters[j].b0*filters[j].buffer[0] + filters[j].b1*filters[j].buffer[1]+ filters[j].b2*filters[j].buffer[2];

              filters[j].buffer[2] = filters[j].buffer[1];
              filters[j].buffer[1] = filters[j].buffer[0];

              //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
              filters[j].buffer_extra[0] = w-filters[j].a1*filters[j].buffer_extra[1]-filters[j].a2*filters[j].buffer_extra[2];
              //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
              w = filters[j].b0*filters[j].buffer_extra[0] + filters[j].b1*filters[j].buffer_extra[1]+ filters[j].b2*filters[j].buffer_extra[2];

              filters[j].buffer_extra[2] = filters[j].buffer_extra[1];
              filters[j].buffer_extra[1] = filters[j].buffer_extra[0];
                
            break;
          }//END SWITCH ORDER
          }//END of if filter acived
      }

    w=w*out_gain; //fi de processat per una mostra enviem a la sortida
    p(0)[i]=w;
    
     } //Fi del NO-BYPASS

    else p(0)[i]=w; //BYPASS

    //Actualitzar el Vu meter
    vu_value = std::abs(w);
    out_values = vu_value > out_values ? vu_value : out_values;

    }// Fi de bulce mostres

  //Actualitzo port dels meters
  //IN meter
  *p(45) = in_values > m_min ? in_values : 0;
      if (in_values > m_min)
	in_values *= pow(m_decay, nframes);
      else
	in_values = 0.0;

  //OUT meter
  *p(46) = out_values > m_min ? out_values : 0;
      if (out_values > m_min)
	out_values *= pow(m_decay, nframes);
      else
	out_values = 0.0;


}//Fincal del RUN()

private:
  int iport;
  float fs, f[NUM_POINTS], in_gain, out_gain;
  bool bypass;
  Filter filters[NUM_OF_FILTERS];

  //PEl Vumeter
  float vu_value, in_values, out_values, m_min, m_decay;

};

void ParamEQ::calc_coefs(int filter_ID, int filter_type, int freq_ptr, float p2, float p3){ //p2 = GAIN p3 = Q
  //Compute filter coeficients
  float w0=2*PI*(f[freq_ptr]/fs);
  float alpha, A, b0, b1, b2, a0, a1, a2, b1_0, b1_1, a1_0, a1_1;

  switch(filter_type){
  
    case HPF_ORDER_1: 
      b1_0 = 2; //b0
      b1_1 = -2; //b1
      a1_0 = w0+2; //a0
      a1_1 = w0-2; //a1
      break;

    case HPF_ORDER_2:  case HPF_ORDER_4:
      alpha = sin(w0)/(2*p3); //p3 = Q

      b0 = (1 + cos(w0))/2; //b0
      b1 = -(1 + cos(w0)); //b1
      b2 = (1 + cos(w0))/2; //b2
      a0 = 1 + alpha; //a0
      a1 = -2*cos(w0); //a1
      a2 = 1 - alpha; //a2
      break;

    case HPF_ORDER_3: 
      alpha = sin(w0)/(2*p3); //p3 = Q
      b1_0 = 2; //b0
      b1_1 = -2; //b1
      a1_0 = w0+2; //a0
      a1_1 = w0-2; //a1
      b0 = (1 + cos(w0))/2; //b0
      b1 = -(1 + cos(w0)); //b1
      b2 = (1 + cos(w0))/2; //b2
      a0 = 1 + alpha; //a0
      a1 = -2*cos(w0); //a1
      a2 = 1 - alpha; //a2
      break;

    case LPF_ORDER_1: 

      b1_0 = w0; //b0
      b1_1 = w0; //b1
      a1_0 = w0+2; //a0
      a1_1 = w0-2; //a1
      break;

    case LPF_ORDER_2: case LPF_ORDER_4:
      alpha = sin(w0)/(2*p3); //p3 = Q

      b0 = (1 - cos(w0))/2; //b0
      b1 = 1 - cos(w0); //b1
      b2 = (1 - cos(w0))/2; //b2
      a0 = 1 + alpha; //a0
      a1 = -2*cos(w0); //a1
      a2 = 1 - alpha; //a2
      break;

    case LPF_ORDER_3:
      alpha = sin(w0)/(2*p3); //p3 = Q
      b1_0 = w0; //b0
      b1_1 = w0; //b1
      a1_0 = w0+2; //a0
      a1_1 = w0-2; //a1
      b0 = (1 - cos(w0))/2; //b0
      b1 = 1 - cos(w0); //b1
      b2 = (1 - cos(w0))/2; //b2
      a0 = 1 + alpha; //a0
      a1 = -2*cos(w0); //a1
      a2 = 1 - alpha; //a2
      break;

    case LOW_SHELF:
      A=pow(10,(p2/40));  //p2 = Gain
      alpha =sin(w0)/2 * (1/p3); //p3 = Q

      b0 = A*((A+1)-(A-1)*cos(w0)+2*sqrt(A)*alpha); //b0
      b1 = 2*A*((A-1)-(A+1)*cos(w0)); //b1
      b2 = A*((A+1)-(A-1)*cos(w0)-2*sqrt(A)*alpha); //b2
      a0 = (A+1) + (A-1)*cos(w0) + 2*sqrt(A)*alpha; //a0
      a1 = -2*((A-1) + (A+1)*cos(w0)); //a1
      a2 = (A+1) + (A-1)*cos(w0) - 2*sqrt(A)*alpha; //a2
      break;

    case HIGH_SHELF:
      A=pow(10,(p2/40)); // p2 = Gain
      alpha =sin(w0)/2 * (1/p3); //p3 = Q

      b0 = A*( (A+1) + (A-1)*cos(w0) + 2*sqrt(A)*alpha ); //b0
      b1 = -2*A*( (A-1) + (A+1)*cos(w0)); //b1
      b2 = A*( (A+1) + (A-1)*cos(w0) - 2*sqrt(A)*alpha ); //b2
      a0 = (A+1) - (A-1)*cos(w0) + 2*sqrt(A)*alpha; //a0
      a1 = 2*( (A-1) - (A+1)*cos(w0)); //a1
      a2 = (A+1) - (A-1)*cos(w0) - 2*sqrt(A)*alpha; //a2
      break;

    case PEAK:
      A=pow(10,(p2/40));  //p2 = Gain
      alpha = sin(w0)/(2*p3) ; //p3 = Q

      b0 = 1 + alpha*A; //b0
      b1 =  -2*cos(w0); //b1
      b2 =   1 - alpha*A; //b2
      a0 =   1 + alpha/A; //a0
      a1 =  -2*cos(w0); //a1
      a2 =   1 - alpha/A; //a2
      break;

    case NOTCH:
      alpha = sin(w0)/(2*p3) ; //p3 = Q

      b0 =  1; //b0
      b1 = -2*cos(w0); //b1
      b2 =  1; //b2
      a0 =  1 + alpha; //a0
      a1 = -2*cos(w0); //a1
      a2 =  1 - alpha; //a2
      break;
 } //End of switch

  //Normalice coeficients to a0=1
  b0 = b0/a0; //b0
  b1 =b1/a0; //b1
  b2 =b2/a0; //b2
  a1 =a1/a0; //a1
  a2 =a2/a0; //a2

  b1_0 = b1_0/a1_0;
  b1_1 = b1_1/a1_0;
  a1_1 = a1_1/a1_0;


  filters[filter_ID].b0 = b0;
  filters[filter_ID].b1 =b1;
  filters[filter_ID].b2 =b2;
  filters[filter_ID].a1 =a1;
  filters[filter_ID].a2 =a2;

  filters[filter_ID].b1_0 =b1_0;
  filters[filter_ID].b1_1 =b1_1;
  filters[filter_ID].a1_1 =a1_1;
  
}

void ParamEQ::flush_buffers(int filter_id){
    for(int j=0; j<3; j++){ //neteja els buffers
      filters[filter_id].buffer_extra[j]=0;
      filters[filter_id].buffer[j]=0;
      if(j<2)filters[filter_id].buffer1[j]=0;
    }
}

static int _ = ParamEQ::register_class("http://sapistaplugin.com/eq/param/peaking");
