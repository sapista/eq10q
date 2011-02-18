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

EQButton::EQButton(int type, float *f, sigc::slot<void> m_slot, int *semafor):
button_align(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0.0, 0.0){
  value=0;
  f_ptr=f;
  stop=semafor;
  ctlbutton=Gtk::manage(new CtlButton(type, &value, f_ptr, this)); //Aki el isX indica si llegirem valors de X
  
  text_entry.set_numeric(true); //nomes permet numeros
  filter_type = type;
  
  switch (filter_type){
    case GAIN_TYPE:
      text_entry.set_range(GAIN_MIN, GAIN_MAX);
      text_entry.set_digits(1);
    break;
    
    case FREQ_TYPE:
      text_entry.set_range(FREQ_MIN, FREQ_MAX);
      text_entry.set_digits(1);
    break;
    
    case Q_TYPE:
      text_entry.set_range(PEAK_Q_MIN, PEAK_Q_MAX);
      text_entry.set_digits(2);
    break;
  }
  
  text_entry.set_increments(0.1, 1.0);
  
  
  

  if(filter_type == FREQ_TYPE){
    set_size_request(70,25);
    ctlbutton->set_size_request(70,25);
    text_entry.set_size_request(70,25);
  }

  else {
    set_size_request(45,25);
    ctlbutton->set_size_request(50,25);
    text_entry.set_size_request(50,25);
  }
  
  button_align.add(*ctlbutton);
  pack_start(text_entry,Gtk::PACK_EXPAND_PADDING);
  pack_start(button_align,Gtk::PACK_EXPAND_PADDING);
  
  text_entry.hide();
  ctlbutton->show();
  button_align.show();
  
  ctlbutton->signal_button_press_event().connect(sigc::mem_fun(*this, &EQButton::on_button_double_clicked),false);
  text_entry.signal_activate().connect(sigc::mem_fun(*this, &EQButton::on_enter_pressed));	
  text_entry.signal_value_changed().connect(m_slot);
  text_entry.signal_value_changed().connect(sigc::mem_fun(*this, &EQButton::on_spin_change));

}

EQButton::~EQButton(){

}

void EQButton::set_value(float val){
  int isota = 0, isobre = NUM_POINTS-1;
  bool OUT = false;
  value = val;
  //Limitem el valor dins del rang segons el tipus de filtre
    switch(filter_type){
    case GAIN_TYPE:
      if (value > GAIN_MAX) value = GAIN_MAX;
      else if (value < GAIN_MIN) value = GAIN_MIN;
    break;
    
    case FREQ_TYPE:
      for(int i = 0; i< NUM_POINTS && !OUT; i++){
        if(f_ptr[i] <= value ){
          isota = i;
        }
        else OUT = true;
       }
        OUT = false;
        
      for(int i = NUM_POINTS-1; i>=0 && !OUT; i--){
        if(f_ptr[i] >= value ){
          isobre = i;
        }
        else OUT = true;
      }
      
      if((value - f_ptr[isota]) > (f_ptr[isobre] - value)){
        value = f_ptr[isobre];
        ctlbutton->set_freq_index(isobre);
       }
       
       else {
        value = f_ptr[isota];
        ctlbutton->set_freq_index(isota);
        }
    break;
    
    case Q_TYPE:
      if (value > PEAK_Q_MAX) value = PEAK_Q_MAX;
      else if (value < PEAK_Q_MIN) value = PEAK_Q_MIN;
    break;
   }
   ctlbutton->set_button_number(value);

   //text_entry.set_value(value);
   set_spin_number();
//std::cout<<"EQButton::set_value(float val)"<<"  Type: "<<filter_type<<" VAL: "<<val<<std::endl;
}

float EQButton::get_value(){
//std::cout<<"EQButton::get_value()"<<"  Type: "<<filter_type<<" VALUE: "<<value<<std::endl;
  if(filter_type != FREQ_TYPE)return value;
  else return ctlbutton->get_freq_ptr();

}

void EQButton::set_freq_ptr(float index){
  //ctlbutton->set_freq_index((int) index);
  set_value(f_ptr[(int) index]);
}

bool EQButton::on_button_double_clicked(GdkEventButton* event){
 if(*stop == 0){
   
      if((event->type == GDK_2BUTTON_PRESS) && (event->button == 1)){
       *stop = 1;
      ctlbutton->hide();
      ctlbutton->set_depress();
      text_entry.set_value((double)value);
      text_entry.show();
      text_entry.grab_focus();
      }
 
    else if(event->button == 1){
      ctlbutton->set_press();
    }
    
  }
 return true;
}

void EQButton::on_enter_pressed(){
  float aux=value;
  //float val = (float)text_entry.get_value();
  //set_value(val);
  ctlbutton->show();
  text_entry.hide();
  *stop = 0;
  
  /*El SLOT te un problema amb la tecla enter, el cas es ke
  En premer enter no senvia al valor actual capturat per value
  sino el valor anterior k ha pillat el spinbutton.
  Amb la seguent instruccio forzem el spin a moures a un valor
  k segur k es diferent k el seu valor actual, aixi la connexio 
  capura el valor anterio (k es el k ens interesa)  osigui "value"
  al final fem un set_value per restaurar la resta de witgets i variables
  amb el valor correcte de value 
  */
  text_entry.set_value((double)(value*1.1+0.1));
  set_value(aux);
  
}

void EQButton::hide_spin(){ //Amaga els SPIN Buttons
  text_entry.hide();
  ctlbutton->show();
}

void EQButton::on_spin_change(){
  float val = (float)text_entry.get_value();
  set_value(val);
}

void EQButton::set_spin_number(){
  text_entry.set_value((double)value);
}