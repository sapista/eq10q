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

#include "eqwindow.hh"

//Constructor
EqMainWindow::EqMainWindow()
  :Flat_Button("Flat"),
  A_Button("A"),
  B_Button("B"),

///TODO: Molt xapusero!
  image_logo_top_top("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_top_top.png"),
  image_logo_top("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_top.png"),
  image_logo_center("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_center.png"),
  image_logo_bottom("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_bottom.png"),
  image_logo_bottom_bottom("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_bottom_bottom.png")
{

  //Slots
  sigc::slot<void> bypass_slot, in_gain_slot, out_gain_slot;
  sigc::slot<void> gain_slot[NUM_BANDS], freq_slot[NUM_BANDS], Q_slot[NUM_BANDS], type_slot[NUM_BANDS];

 //Buttons A,B i Flat
  A_Button.set_size_request(40,-1);
  B_Button.set_size_request(40,-1);
  Flat_Button.set_size_request(60,-1); 
  bypass_button.set_size_request(60,-1); 
  
  buttonA_align.add(A_Button);
  buttonB_align.add(B_Button);

  buttonA_align.set(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0.0, 0.0);
  buttonB_align.set(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0.0, 0.0);
  
  buttonA_align.show();
  buttonB_align.show();

  buttons_ABF.set_homogeneous(false);
  
  buttons_ABF.pack_start(bypass_button,Gtk::PACK_EXPAND_PADDING);
  buttons_ABF.pack_start(buttonA_align,Gtk::PACK_EXPAND_PADDING);
  buttons_ABF.pack_start(buttonB_align,Gtk::PACK_EXPAND_PADDING);
  buttons_ABF.set_size_request(230,-1);
  buttons_ABF.set_spacing(2);
  
  Flat_align.add(Flat_Button);
  AB_align.add(buttons_ABF);
    
  Flat_align.set(Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER, 1.0, 0.0);
  AB_align.set(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 1.0, 0.0);
  
  ABFlat_box.pack_start(AB_align,Gtk::PACK_EXPAND_PADDING);
  ABFlat_box.pack_start(image_logo_center);
  ABFlat_box.pack_start(Flat_align,Gtk::PACK_EXPAND_PADDING);
  ABFlat_box.set_spacing(10);

  //connects
  //dummy_box.signal_expose_event().connect(mem_fun(*this, &EqMainWindow::on_window_popup), false);
  //dummy_box.set_sensitive(false);

  bypass_button.signal_toggled().connect(bypass_slot);
  bypass_button.set_label("Bypass");

  in_gain = Gtk::manage(new GainCtl("In", in_gain_slot));
  gain_box.pack_start(*in_gain, Gtk::PACK_SHRINK);
  
  out_gain = Gtk::manage(new GainCtl("Out", out_gain_slot));
  gain_box.pack_start(*out_gain, Gtk::PACK_SHRINK);

  gain_box.set_homogeneous(true);
  gain_box.set_spacing(2);
  gain_box.show();
  in_gain->show();
  out_gain->show();
  
  gain_bypass_box.pack_start(gain_box, Gtk::PACK_EXPAND_WIDGET);
  gain_bypass_box.set_homogeneous(false);
  gain_bypass_box.show();
  bypass_button.show();
  
  band_box.pack_start(gain_bypass_box,  Gtk::PACK_SHRINK);
  
  for (int i = 0; i< NUM_BANDS; i++){
    m_banda[i] = Gtk::manage(new BandCtl(i+1,
    gain_slot[i],
    freq_slot[i],
    Q_slot[i],
    type_slot[i],
    &stop
    ));
    band_box.pack_start(*m_banda[i], Gtk::PACK_SHRINK);
  }
   band_box.pack_start(dummy_box, Gtk::PACK_SHRINK); //afegim una caixa buida, serveix per invocar la fuincio expose
  band_box.set_spacing(4);
  band_box.set_homogeneous(false);


  band_box.show();


  main_box.pack_start(image_logo_top);
  main_box.pack_start(ABFlat_box,Gtk::PACK_EXPAND_PADDING);
  main_box.pack_start(image_logo_bottom);

  image_logo_bottom.show();
  image_logo_top.show();
  image_logo_center.show();

  ABFlat_box.show();
  Flat_Button.show();
  Flat_align.show();
  AB_align.show();
  buttons_ABF.show();
  A_Button.show();
  B_Button.show();
  ABFlat_box.show();
  
  main_box.pack_start(band_box);
  main_box.pack_start(image_logo_bottom_bottom);
  main_box.set_spacing(0);

  add(main_box);
  main_box.show();


  //SEnyals dels 3 botons A B FLAT
  A_Button.set_active(true);
  B_Button.set_active(false);
  A_Button.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::on_button_A));
  B_Button.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::on_button_B));
  Flat_Button.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::on_button_FLAT));
 
}


void EqMainWindow::AB_change_params(bool toA){
  
///Ho ignoro tot pq ara mateix no li veig cap sentit
/*for(int i=0; i<NUM_OF_FILTERS;i++){
    if(toA){
      paramsB[i].type=(int)m_banda[i]->get_filter_type();
      paramsB[i].gain=m_banda[i]->get_gain();
      paramsB[i].freq=m_banda[i]->get_freq();
      paramsB[i].Q=m_banda[i]->get_Q();
      
      m_banda[i]->set_filter_type((float)paramsA[i].type);
      m_banda[i]->set_gain(paramsA[i].gain);
      m_banda[i]->set_freq(paramsA[i].freq);
      m_banda[i]->set_Q(paramsA[i].Q);
      }
    
    else{
      paramsA[i].type=(int)m_banda[i]->get_filter_type();
      paramsA[i].gain=m_banda[i]->get_gain();
      paramsA[i].freq=m_banda[i]->get_freq();
      paramsA[i].Q=m_banda[i]->get_Q();
      
      m_banda[i]->set_filter_type((float)paramsB[i].type);
      m_banda[i]->set_gain(paramsB[i].gain);
      m_banda[i]->set_freq(paramsB[i].freq);
      m_banda[i]->set_Q(paramsB[i].Q);
    }
  
  
  }*/
}

void EqMainWindow::on_button_A(){
  if(A_Button.get_active()){
    B_Button.set_active(false);
    
///No crec que sigui la millor forma
    //guardem la corba B i Carregem la A
    //AB_change_params(true);
  }
  
  else B_Button.set_active(true);
  
}

void EqMainWindow::on_button_B(){
  if(B_Button.get_active()){
    A_Button.set_active(false);
    
///No crec que sigui la millor forma
    //guardem la corba A
    //AB_change_params(false);
    }
  else A_Button.set_active(true);
}


void EqMainWindow::on_button_FLAT(){
  
  //Popup a waring message
  Gtk::MessageDialog dialog((Gtk::Window&)(*this->get_toplevel()),"This will flat the EQ curve, are you sure?",
          false /* use_markup */, Gtk::MESSAGE_QUESTION,
          Gtk::BUTTONS_OK_CANCEL);

  int result = dialog.run();

  if(result == Gtk::RESPONSE_OK)flat();
}

void EqMainWindow::flat(){
///No em sembla elgant aquest flat
/*for(int i=0; i< NUM_OF_FILTERS; i++){
    paramsB[i].type=0;
    paramsB[i].gain=0;
    paramsB[i].freq=(i+1)*30-1;
    paramsB[i].Q=2;
    
    paramsA[i].type=0;
    paramsA[i].gain=0;
    paramsA[i].freq=(i+1)*30-1;
    paramsA[i].Q=2;

    m_banda[i]->set_filter_type((float)paramsA[i].type);
    m_banda[i]->set_gain(paramsA[i].gain);
    m_banda[i]->set_freq(paramsA[i].freq);
    m_banda[i]->set_Q(paramsA[i].Q);
 }*/
  
  in_gain->set_gain(0.0);
  out_gain->set_gain(0.0);

  A_Button.set_active(true);
      
}

EqMainWindow::~EqMainWindow(){

}
