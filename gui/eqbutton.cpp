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
 
#include "eqbutton.h"

EQButton::EQButton(int iType, float *fPtr, sigc::slot<void> slot, int *iSemafor):
m_ButtonAlign(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0.0, 0.0){
  m_fValue=0;
  m_ptr_f=fPtr;
  m_iStop=iSemafor;
  
  ///TODO: Un cop mes!!! Pero que collons esta fent el punter a objecte de nivell superior!?!¿!?!¿?!
  m_ptr_CtlButton=Gtk::manage(new CtlButton(type, &value, f_ptr, this)); //Aki el isX indica si llegirem valors de X
  
  m_TextEntry.set_numeric(true); //nomes permet numeros
  m_iFilterType = iType;
  
  switch (m_iFilterType){
    case GAIN_TYPE:
      m_TextEntry.set_range(GAIN_MIN, GAIN_MAX); ///TODO: Aquest limits han de sortir d'algun lloc, el mes logic seria usar el fitxer de config del LV2 *.ttl
      m_TextEntry.set_digits(1);
    break;
    
    case FREQ_TYPE:
      m_TextEntry.set_range(FREQ_MIN, FREQ_MAX); ///TODO: Aquest limits han de sortir d'algun lloc, el mes logic seria usar el fitxer de config del LV2 *.ttl
      m_TextEntry.set_digits(1);
    break;
    
    case Q_TYPE:
      m_TextEntry.set_range(PEAK_Q_MIN, PEAK_Q_MAX); ///TODO: Aquest limits han de sortir d'algun lloc, el mes logic seria usar el fitxer de config del LV2 *.ttl
      m_TextEntry.set_digits(2);
    break;
  }
  
  m_TextEntry.set_increments(0.1, 1.0);
  

  if(m_iFilterType == FREQ_TYPE){
    set_size_request(70,25);
    m_ptr_CtlButton->set_size_request(70,25); ///TODO: Un cop mes el putu punter que no se quin sentit te!!!!!!!!!!!!!!!!!!!
    m_TextEntry.set_size_request(70,25);
  }

  else {
    set_size_request(45,25);
    m_ptr_CtlButton->set_size_request(50,25); ///TODO: Un cop mes el putu punter que no se quin sentit te!!!!!!!!!!!!!!!!!!!
    m_TextEntry.set_size_request(50,25);
  }
  
  m_ButtonAlign.add(*m_ptr_CtlButton); ///TODO: Un cop mes el putu punter que no se quin sentit te!!!!!!!!!!!!!!!!!!!
  pack_start(m_TextEntry,Gtk::PACK_EXPAND_PADDING);
  pack_start(m_ButtonAlign,Gtk::PACK_EXPAND_PADDING);
  
  m_TextEntry.hide();
  m_ptr_CtlButton->show(); ///TODO: Un cop mes el putu punter que no se quin sentit te!!!!!!!!!!!!!!!!!!!
  m_ButtonAlign.show();
  
  m_ptr_CtlButton->signal_button_press_event().connect(sigc::mem_fun(*this, &EQButton::onButtonDoubleClicked),false); ///TODO: Un cop mes el putu punter que no se quin sentit te!!!!!!!!!!!!!!!!!!!
  m_TextEntry.signal_activate().connect(sigc::mem_fun(*this, &EQButton::onEnterPressed));	
  m_TextEntry.signal_value_changed().connect(slot); ///TODO: Buscar una explicacio a perque cal passar el punter a slot des de fora!
  m_TextEntry.signal_value_changed().connect(sigc::mem_fun(*this, &EQButton::onSpinChange));

}

EQButton::~EQButton(){

}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> I'm working here
void EQButton::set_value(float val){
  int isota = 0, isobre = NUM_POINTS-1;
  bool OUT = false;
  value = val;
  //Limitem el valor dins del rang segons el tipus de filtre
    switch(m_iFilterType){
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
        m_ptr_CtlButton->set_freq_index(isobre);
       }
       
       else {
        value = f_ptr[isota];
        m_ptr_CtlButton->set_freq_index(isota);
        }
    break;
    
    case Q_TYPE:
      if (value > PEAK_Q_MAX) value = PEAK_Q_MAX;
      else if (value < PEAK_Q_MIN) value = PEAK_Q_MIN;
    break;
   }
   m_ptr_CtlButton->set_button_number(value);

   //m_TextEntry.set_value(value);
   set_spin_number();
//std::cout<<"EQButton::set_value(float val)"<<"  Type: "<<m_iFilterType<<" VAL: "<<val<<std::endl;
}

float EQButton::get_value(){
//std::cout<<"EQButton::get_value()"<<"  Type: "<<m_iFilterType<<" VALUE: "<<value<<std::endl;
  if(m_iFilterType != FREQ_TYPE)return value;
  else return m_ptr_CtlButton->get_freq_ptr();

}

void EQButton::set_freq_ptr(float index){
  //m_ptr_CtlButton->set_freq_index((int) index);
  set_value(f_ptr[(int) index]);
}

bool EQButton::on_button_double_clicked(GdkEventButton* event){
 if(*stop == 0){
   
      if((event->type == GDK_2BUTTON_PRESS) && (event->button == 1)){
       *stop = 1;
      m_ptr_CtlButton->hide();
      m_ptr_CtlButton->set_depress();
      m_TextEntry.set_value((double)value);
      m_TextEntry.show();
      m_TextEntry.grab_focus();
      }
 
    else if(event->button == 1){
      m_ptr_CtlButton->set_press();
    }
    
  }
 return true;
}

void EQButton::on_enter_pressed(){
  float aux=value;
  //float val = (float)m_TextEntry.get_value();
  //set_value(val);
  m_ptr_CtlButton->show();
  m_TextEntry.hide();
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
  m_TextEntry.set_value((double)(value*1.1+0.1));
  set_value(aux);
  
}

void EQButton::hide_spin(){ //Amaga els SPIN Buttons
  m_TextEntry.hide();
  m_ptr_CtlButton->show();
}

void EQButton::on_spin_change(){
  float val = (float)m_TextEntry.get_value();
  set_value(val);
}

void EQButton::set_spin_number(){
  m_TextEntry.set_value((double)value);
}