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

#include "band_ctl.h"

BandCtl::BandCtl( const int iBandNum, int *semafor):
m_ButtonAlign(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0.0, 0.0),
m_ComboAlign(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0.0, 0.0),
m_iBandNum(iBandNum)
{
  Glib::ustring sText;
 
  m_Gain = Gtk::manage(new EQButton(GAIN_TYPE, semafor));
  m_Freq = Gtk::manage(new EQButton(FREQ_TYPE, semafor));
  m_Q = Gtk::manage(new EQButton(Q_TYPE, semafor));

  sText = Glib::ustring::compose("Band %1", m_iBandNum);
  m_BandLabel.set_label(sText);

  pack_start(m_BandLabel, Gtk::PACK_SHRINK );
  pack_start(m_ComboAlign, Gtk::PACK_EXPAND_PADDING );
  pack_start(*m_Gain, Gtk::PACK_EXPAND_PADDING );
  pack_start(*m_Freq, Gtk::PACK_EXPAND_PADDING );
  pack_start(*m_Q, Gtk::PACK_EXPAND_PADDING );
  pack_start(m_ButtonAlign, Gtk::PACK_EXPAND_PADDING );

  m_OnButton.set_size_request(35,-1);
  m_ButtonAlign.add(m_OnButton);
  
  set_spacing(2);
  set_homogeneous(false);
  set_size_request(80,-1);

  m_FilterSel.set_size_request(60,-1);
  m_ComboAlign.add(m_FilterSel);
  
  m_BandLabel.show();
  m_OnButton.show();
  m_FilterSel.show();
  m_Gain->show();
  m_Freq->show();
  m_Q->show();
  m_ButtonAlign.show();
  show();

  m_OnButton.set_label("ON");
  m_OnButton.signal_clicked().connect(sigc::mem_fun(*this, &BandCtl::onButtonClicked));
   m_FilterSel.signal_changed().connect(sigc::mem_fun(*this, &BandCtl::onComboChanged));
 
   ///TODO: k es aixo que esta comentat
   //posa els buttons com hagin d'estar al principi
  //config_sensitive();
  //vic_de_set=false;

  ///TODO: Com comuniquem aquest objecte amb l'exterior?
  //connexions externes
  //m_filter_sel.signal_changed().connect(type_slot);
  //m_on_button.signal_clicked().connect(type_slot);

  
}

//Data accesors
float BandCtl::getGain(){
  return m_Gain->getValue();
}

float BandCtl::getFreq(){
  return m_Freq->getValue();
}

float BandCtl::getQ(){
  return m_Q->getValue();
}

float BandCtl::getFilterType(){
  return (float)m_iFilterType;
}

bool BandCtl::getEnabled()
{
  return m_bBandIsEnabled;
}

void BandCtl::setGain(float fGain){
  m_Gain->setValue(fGain);
}

void BandCtl::setFreq(float fFreq){
  m_Freq->setValue(fFreq);
}

void BandCtl::setQ(float fQ){
  m_Q->setValue(fQ);
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>I'm here
void BandCtl::setFilterType(float fType){
  m_iFilterType = (int)fType;
  m_FilterSel.set_active(m_iFilterType);
  
  ///vic_de_set=true; ///TODO: k es aixo??????

///TODO: FilterType no te res a veure amb si esta ON o OFF
  if( filter_type == 0)m_on_button.set_active(false);
  else{
    m_on_button.set_active(true);
    }

  //if(m_on_button.get_active())m_filter_sel.set_active(filter_type); //Modificat per evitar k kuan apaguem el butto es perdi la select <<<<<<<
  m_filter_sel.set_active(filter_type);
  vic_de_set=false;
  config_sensitive();

}

void BandCtl::setEnabled(bool bIsEnabled)
{
  m_bBandIsEnabled = bIsEnabled;
}

//GTK signal Handlers
void BandCtl::onButtonClicked(){
  config_type(); ///TODO: pq esta en una funcio aillada???

  if(m_on_button.get_active() && ant_filter != FILTER_OFF){
    m_filter_sel.set_active(ant_filter);
  }
}

void BandCtl::onComboChanged(){
config_type(); ///TODO: pq esta en una funcio aillada???
}


///TODO: What is this????
void BandCtl::configType(){
  if(filter_type != FILTER_OFF) ant_filter = filter_type;

  if(m_on_button.get_active()) {
    if(!vic_de_set)filter_type=m_filter_sel.get_active_row_number();
   }
  else filter_type=0;

  config_sensitive();
}

void BandCtl::configSensitive(){
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

