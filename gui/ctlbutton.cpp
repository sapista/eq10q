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

#include "ctlbutton.h"

CtlButton::CtlButton(int type, float *value, float *f, EQButton *m_eqbutton):
act_val(0), ant_val(0), ptr(0), acumula(0), eqbutton_ref_ptr(m_eqbutton)
{
  filter_type=type;
  if(filter_type == GAIN_TYPE) x_direction=false;
  else x_direction=true;
  
  f_ptr=f;
  press=false;

  mouse_value=value; //un lloc on deixar el k es captura del mouse
  
  /*//Posem alguns valors per defecte
  switch(filter_type){
    case GAIN_TYPE:
      *mouse_value=GAIN_DEFAULT;
    break;
    
    case FREQ_TYPE:
      *mouse_value=f_ptr[PEAK_FREQ_PTR_DEFAULT];
      ptr= PEAK_FREQ_PTR_DEFAULT;
    break;
    
    case Q_TYPE:
      *mouse_value=PEAK_Q_DEFAULT;
    break;
  }
  */
  
  set_button_number(*mouse_value);
  

  signal_released().connect( sigc::mem_fun(*this,&CtlButton::on_button_depressed));
  
  signal_motion_notify_event().connect( sigc::mem_fun(*this, &CtlButton::on_mouse_move),false);
  
  add_events(Gdk::POINTER_MOTION_MASK); 
}

CtlButton::~CtlButton(){

}


void CtlButton::set_press(){
  press=true;
  first_time=true;
}

void CtlButton::on_button_depressed(){
  press=false;
  ant_val=0;
  act_val=0;
}

void CtlButton::set_depress(){
  press=false;
}

bool CtlButton::on_mouse_move(GdkEventMotion* event){
  if(press){
    int x,y;
    get_pointer(x, y);
    set_value(x,y);
   }
  return true;
}

void CtlButton::set_value(int x, int y){
  ant_val=act_val;

  if(x_direction) act_val=x;
  else act_val=(-1)*y;
  if (first_time){
    if(act_val>0) ant_val=act_val-1;
    else if(act_val<0) ant_val=act_val+1;
    first_time=false;
  }

  switch(filter_type){
    case GAIN_TYPE:
       *mouse_value =  *mouse_value + (float)(act_val-ant_val)/ACCELERATION;
      if (*mouse_value > GAIN_MAX) *mouse_value = GAIN_MAX;
      else if (*mouse_value < GAIN_MIN) *mouse_value = GAIN_MIN;
    break;
    
    case FREQ_TYPE:
      acumula=acumula+ (act_val-ant_val);
      if (acumula >= 6) ptr++;
      else if (acumula <= -6) ptr--;
      acumula = acumula %6;
      if (ptr >= NUM_POINTS) ptr = NUM_POINTS -1;
      else if (ptr < 0) ptr = 0;
      *mouse_value = f_ptr[ptr];
    break;
    
    case Q_TYPE:
      *mouse_value =  *mouse_value + (float)(act_val-ant_val)/ACCELERATION;
      if (*mouse_value > PEAK_Q_MAX) *mouse_value = PEAK_Q_MAX;
      else if (*mouse_value < PEAK_Q_MIN) *mouse_value = PEAK_Q_MIN;
    break;
   }
   
  //set_button_number(*mouse_value);
  eqbutton_ref_ptr->set_value(*mouse_value);
  eqbutton_ref_ptr->set_spin_number();
  //std::cout<<"val: "<<*mouse_value<<std::endl;

  
}

void CtlButton::set_button_number(float num){
  Glib::ustring button_text;
  
  switch(filter_type){
    case GAIN_TYPE:
    case FREQ_TYPE:
    button_text = Glib::ustring::format(std::fixed, std::setprecision(1), num);
    break;

    case Q_TYPE:
    button_text = Glib::ustring::format(std::fixed, std::setprecision(2), num);
    break;
    }
  set_label(button_text);
  
}

void CtlButton::set_freq_index(int index){
  ptr = index;
}

float CtlButton::get_freq_ptr(){
  return  (float)ptr;
}
