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

#include "gui/band_ctl.h"

BandCtl::BandCtl(){
  //empty constructor
}

//True constructor
BandCtl::BandCtl( const int band_num,
  sigc::slot<void> gain_slot,
  sigc::slot<void> freq_slot,
  sigc::slot<void> Q_slot,
  sigc::slot<void> type_slot, int *semafor
   ):
button_align(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0.0, 0.0),
combo_align(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0.0, 0.0){
  Glib::ustring text;
 
  m_gain = Gtk::manage(new EQButton(GAIN_TYPE, f, gain_slot, semafor));
  m_freq = Gtk::manage(new EQButton(FREQ_TYPE, f, freq_slot, semafor));
  m_Q = Gtk::manage(new EQButton(Q_TYPE, f, Q_slot, semafor));

  text = Glib::ustring::compose("Band %1", band_num);
  band_label.set_label(text);

  pack_start(band_label, Gtk::PACK_SHRINK );
  pack_start(combo_align, Gtk::PACK_EXPAND_PADDING );
  pack_start(*m_gain, Gtk::PACK_EXPAND_PADDING );
  pack_start(*m_freq, Gtk::PACK_EXPAND_PADDING );
  pack_start(*m_Q, Gtk::PACK_EXPAND_PADDING );
  pack_start(button_align, Gtk::PACK_EXPAND_PADDING );

  m_on_button.set_size_request(35,-1);
  button_align.add(m_on_button);
  
  set_spacing(2);
  set_homogeneous(false);
  set_size_request(80,-1);

  m_filter_sel.set_size_request(60,-1);
  combo_align.add(m_filter_sel);
  
  band_label.show();
  m_on_button.show();
  m_filter_sel.show();
  m_gain->show();
  m_freq->show();
  m_Q->show();
  button_align.show();
  show();

  m_on_button.set_label("ON");
  m_on_button.signal_clicked().connect(sigc::mem_fun(*this, &BandCtl::on_button_clicked));
 
  m_filter_sel.signal_changed().connect(sigc::mem_fun(*this, &BandCtl::on_combo_changed));
  //posa els buttons com hagin d'estar al principi
  //config_sensitive();

  vic_de_set=false;

  //connexions externes
  m_filter_sel.signal_changed().connect(type_slot);
  m_on_button.signal_clicked().connect(type_slot);

  
}

//Data accesors
float BandCtl::getGain(){
  return m_gain->get_value();
}

float BandCtl::getFreq(){
  return m_freq->get_value();
}

float BandCtl::getQ(){
  return m_Q->get_value();
}

float BandCtl::getFilterType(){
  return (float)filter_type;
}


//Funcions de salvar parametres
void BandCtl::set_gain(float g){
  m_gain->set_value(g);
}

void BandCtl::set_freq(float f){
  m_freq->set_freq_ptr(f);
}

void BandCtl::set_freq_direct(float f){
  m_freq->set_value(f);
}

void BandCtl::set_Q(float q){
  m_Q->set_value(q);
}

void BandCtl::set_filter_type(float t){
  filter_type = (int)t;
  vic_de_set=true;

  if( filter_type == 0)m_on_button.set_active(false);
  else{
    m_on_button.set_active(true);
    }

  //if(m_on_button.get_active())m_filter_sel.set_active(filter_type); //Modificat per evitar k kuan apaguem el butto es perdi la select <<<<<<<
  m_filter_sel.set_active(filter_type);
  vic_de_set=false;
  config_sensitive();

}

void BandCtl::on_button_clicked(){
  config_type();

  if(m_on_button.get_active() && ant_filter != FILTER_OFF){
    m_filter_sel.set_active(ant_filter);
  }
}

void BandCtl::on_combo_changed(){
config_type();
}

void BandCtl::config_type(){
  if(filter_type != FILTER_OFF) ant_filter = filter_type;

  if(m_on_button.get_active()) {
    if(!vic_de_set)filter_type=m_filter_sel.get_active_row_number();
   }
  else filter_type=0;

  config_sensitive();
}

void BandCtl::config_sensitive(){
  switch(filter_type){
    case FILTER_OFF:
       m_Q->set_sensitive(false);
       m_gain->set_sensitive(false);
       m_freq->set_sensitive(false);
    break;
    
    case LPF_ORDER_1:
    case HPF_ORDER_1:
      m_Q->set_sensitive(false);
      m_gain->set_sensitive(false);
      m_freq->set_sensitive(true);

      set_gain(0.0);
    break;
    
    case LPF_ORDER_2:
    case LPF_ORDER_3:
    case LPF_ORDER_4:
    case HPF_ORDER_2:
    case HPF_ORDER_3:
    case HPF_ORDER_4:
      reset_Q(HPF_LPF_Q_DEFAULT);
      goto hpf_lpf;
    case NOTCH:
      reset_Q(NOTCH_Q_DEFAULT);

hpf_lpf:
      m_gain->set_sensitive(false);
      m_Q->set_sensitive(true);
      m_freq->set_sensitive(true);
      set_gain(0.0);
    break;

    case LOW_SHELF:
    case HIGH_SHELF:
      reset_Q(HIGH_LOW_SHELF_Q_DEFAULT);
      goto shelvings;
    case PEAK:
     reset_Q(PEAK_Q_DEFAULT);
shelvings:
      m_gain->set_sensitive(true);
      m_Q->set_sensitive(true);
      m_freq->set_sensitive(true);
    break;
   }

}

void  BandCtl::reset_Q(float q){
  if(filter_type != ant_filter){
    set_Q(q);
  }
}

void BandCtl::hide_spins(){
  m_gain->hide_spin();
  m_freq->hide_spin();
  m_Q->hide_spin();
}

BandCtl::~BandCtl(){

}

///////////////////////////////////////////////////////

GainCtl::GainCtl(const Glib::ustring title, sigc::slot<void> m_slot){
  gain_scale.set_digits(1);
  gain_scale.set_draw_value(true);
  gain_scale.set_value_pos(Gtk::POS_TOP);
  gain_scale.set_inverted(true);
  gain_scale.set_range(GAIN_MIN, GAIN_MAX);
  gain_scale.set_value(GAIN_DEFAULT);
  gain_scale.signal_value_changed().connect(m_slot);

  
  gain_label.set_label(title);
  
  pack_start(gain_label);
  pack_start(gain_scale);
  
  set_spacing(2);
  set_homogeneous(false);
  
  gain_scale.set_size_request(40,100); 

  gain_label.show();
  gain_scale.show();
  show();
}

void GainCtl::set_gain(float g){
  gain_scale.set_value((double) g);
}

float GainCtl::get_gain(){
  return (float)gain_scale.get_value();
}

GainCtl::~GainCtl(){

}

