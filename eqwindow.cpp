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
 //punter a objecte principal;

void gloabal_set_gain_freq(main_window *main_ptr, int b_ix, float g, float f){
  main_ptr->set_gain_freq(b_ix, g, f);
}

void global_set_all_params(main_window *main_ptr, int b_ix, int type, float g, float f, float Q){
  main_ptr->set_all_params(b_ix, type, g, f, Q);
}

void global_get_all_params(main_window *main_ptr, int band_id, BandParams &m_params){
  main_ptr->get_all_params(band_id, m_params);
}

//main_window::main_window()
main_window::main_window(const std::string& URI) //LV2 Style
:Flat_Button("Flat"),
  A_Button("A"),
  B_Button("B"),
  in_vu(1),
  out_vu(1),
  image_logo_top_top("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_top_top.png"),
  image_logo_top("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_top.png"),
  image_logo_center("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_center.png"),
  image_logo_bottom("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_bottom.png"),
  image_logo_bottom_bottom("/usr/local/lib/lv2/paramEQ-Rafols.lv2/logo_bottom_bottom.png")

  {

  stop = 0;

  flag_pop_resize = true;
    
  //Slots
  sigc::slot<void> bypass_slot, in_gain_slot, out_gain_slot;
  sigc::slot<void> gain_slot[10], freq_slot[10], Q_slot[10], type_slot[10];
  

  //Aket es el calcul del vector de freq
  float d1=log10(FREQ_MIN);
  float d2=log10(FREQ_MAX);
  for(int i=0;i<NUM_POINTS;i++){
    f[i]=pow(10,(d1+(i*(d2-d1))/(NUM_POINTS-1)));
  }
  
  //set_title("10 Bands Parametric EQ (by Pere Ràfols)");
  //set_resizable(false);

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
  //buttons_ABF.set_border_width(2);
  buttons_ABF.set_size_request(230,-1);
  buttons_ABF.set_spacing(2);
  
  Flat_align.add(Flat_Button);
  AB_align.add(buttons_ABF);
    
  Flat_align.set(Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER, 1.0, 0.0);
  AB_align.set(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 1.0, 0.0);
  
  //ABFlat_box.set_size_request(640,-1);
  ABFlat_box.pack_start(AB_align,Gtk::PACK_EXPAND_PADDING);
  ABFlat_box.pack_start(image_logo_center);
  ABFlat_box.pack_start(Flat_align,Gtk::PACK_EXPAND_PADDING);
  ABFlat_box.set_spacing(10);


/////////////////////////////////////////////////////////////////////////////////////////////
//SLOTS PER PORTS DE CONTROL DEL Plugin segons especificacio LV2 (desactivar per proves de GUI)
  //creem els Slots
   bypass_slot = compose(bind<0>(mem_fun(*this, &main_window::write_control), 2),
                mem_fun(*this, &main_window::get_bypass));


   in_gain_slot = compose(bind<0>(mem_fun(*this, &main_window::write_control), 3),
                mem_fun(*this, &main_window::get_in_gain));

   out_gain_slot = compose(bind<0>(mem_fun(*this, &main_window::write_control), 4),
                mem_fun(*this, &main_window::get_out_gain));

  for(int i = 0; i< 10; i++){
     type_slot[i] = compose(bind<0>(mem_fun(*this, &main_window::write_control), 
                    i*4+FIRST_FILTER_PORT_INDEX),
                    bind<0>(mem_fun(*this, &main_window::get_band_type),i));

    gain_slot[i] = compose(bind<0>(mem_fun(*this, &main_window::write_control), 
                    i*4+FIRST_FILTER_PORT_INDEX+1),
                    bind<0>(mem_fun(*this, &main_window::get_band_gain),i));

    freq_slot[i] = compose(bind<0>(mem_fun(*this, &main_window::write_control), 
                    i*4+FIRST_FILTER_PORT_INDEX+2),
                    bind<0>(mem_fun(*this, &main_window::get_band_freq),i));

    Q_slot[i] = compose(bind<0>(mem_fun(*this, &main_window::write_control), 
                    i*4+FIRST_FILTER_PORT_INDEX+3),
                    bind<0>(mem_fun(*this, &main_window::get_band_Q),i));
  }
/////////////////////////////////////////////////////////////////////////////////////////////////

/*/////////////////////////////////////////////////////////////////////////////////////////////
//SLOTS PER PROBES DE LA GUI I EL PLOT(desactivar per el plugin!!!)
  //creem els Slots

  for(int i = 0; i< 10; i++){
    type_slot[i] = bind<0>(mem_fun(*this, &main_window::get_band_type),i);

    gain_slot[i] = bind<0>(mem_fun(*this, &main_window::get_band_gain),i);

    freq_slot[i] = bind<0>(mem_fun(*this, &main_window::get_band_freq),i);

    Q_slot[i] = bind<0>(mem_fun(*this, &main_window::get_band_Q),i);
  }
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
*/

//SLOTS DE DEPURAT///////////////////////////////////////////////////////////////
/*  
  bypass_slot = sigc::mem_fun(*this, &main_window::bypass_control);
  in_gain_slot = sigc::mem_fun(*this, &main_window::in_gain_control);
  out_gain_slot = sigc::mem_fun(*this, &main_window::out_gain_control);
  
  for(int i = 0; i< 10; i++){
    gain_slot[i] = sigc::bind<int>(sigc::mem_fun(*this, &main_window::gain_control), i);
    freq_slot[i] = sigc::bind<int>(sigc::mem_fun(*this, &main_window::freq_control), i);
    Q_slot[i] = sigc::bind<int>(sigc::mem_fun(*this, &main_window::Q_control), i);
    type_slot[i] = sigc::bind<int>(sigc::mem_fun(*this, &main_window::type_control), i);
  }
*/////////////////////////////////////////////////////////////////////////////////

  //connects

  //bypass_button.signal_grab_focus().connect(mem_fun(*this, &main_window::on_window_popup));
  //signal_realize().connect(mem_fun(*this, &main_window::on_window_popup));
  dummy_box.signal_expose_event().connect(mem_fun(*this, &main_window::on_window_popup), false);
  dummy_box.set_sensitive(false);

  bypass_button.signal_toggled().connect(bypass_slot);
  bypass_button.set_label("Bypass");

  in_gain = Gtk::manage(new GainCtl("In", in_gain_slot));
  gain_box.pack_start(*in_gain, Gtk::PACK_SHRINK);
  gain_box.pack_start(in_vu,  Gtk::PACK_SHRINK);
  
  out_gain = Gtk::manage(new GainCtl("Out", out_gain_slot));
  gain_box.pack_start(*out_gain, Gtk::PACK_SHRINK);
  gain_box.pack_start(out_vu,  Gtk::PACK_SHRINK);
  
  gain_box.set_homogeneous(true);
  gain_box.set_spacing(2);
  gain_box.show();
  in_gain->show();
  out_gain->show();
  
  //gain_bypass_box.pack_start(bypass_button, Gtk::PACK_SHRINK);
  gain_bypass_box.pack_start(gain_box, Gtk::PACK_EXPAND_WIDGET);
  gain_bypass_box.set_homogeneous(false);
  gain_bypass_box.show();
  bypass_button.show();
  
  band_box.pack_start(gain_bypass_box,  Gtk::PACK_SHRINK);
  
  for (int i = 0; i< NUM_OF_FILTERS; i++){
    m_banda[i] = Gtk::manage(new BandCtl(f, i+1,
    gain_slot[i], //comencem a passar els slots
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
  

  //Afegim el Plot
  my_plot=Gtk::manage(new PlotEQCurve(this, &gloabal_set_gain_freq));

  /*for(int i=0; i<NUM_OF_FILTERS; i++){
    float lg, lf, lq, lt;
    lg = m_banda[i]->get_gain();
    lf = m_banda[i]->get_freq();
    lq = m_banda[i]->get_Q();
    lt = m_banda[i]->get_filter_type();

    //redraw_plot(i, lg, f[(int)lf], lq, lt);
    //m_banda[i]->set_Q(2); ////////////////////////<<<<<<<<<<<<<<<<<<<<<<FORA PER LV2
    }*/

  //Afegim els presets
  m_presets = Gtk::manage(new TemplateWidget(this, &global_set_all_params, &global_get_all_params));
  
  main_box.pack_start(image_logo_top_top);
  main_box.pack_start(*m_presets);
  m_presets->show();

  plot_fix.put(*my_plot, 0.0, 0.0);
  main_box.pack_start(plot_fix);
  plot_fix.show();

  //main_box.pack_start(*my_plot);
  my_plot->show();

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

  //incicialitzar estrcutures de parametres
  //flat();

  //SEnyals dels 3 botons A B FLAT
  A_Button.set_active(true);
  B_Button.set_active(false);
  A_Button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_button_A));
  B_Button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_button_B));
  Flat_Button.signal_clicked().connect( sigc::mem_fun(*this, &main_window::on_button_FLAT));


   for(int i=0; i< NUM_OF_FILTERS; i++){
    paramsB[i].type=0;
    paramsB[i].gain=0;
    paramsB[i].freq=(i+1)*30-1;
    paramsB[i].Q=2;
    }

  //Posem-hi una imatge de fons
  //band_box.modify_bg_pixmap(Gtk::STATE_ACTIVE,"my_eq_bak.png");

 /*
 Glib::RefPtr<Gdk::Window> my_window = get_parent_window();
std::cout<<"Parent = "<<my_window<<std::endl;
 my_window->set_background(Gdk::Color("orange"));
*/
 
}


float main_window::get_bypass(){
  bool bypass = bypass_button.get_active();
  my_plot->Set_Bypass(bypass);
  return (float)bypass;
}

float main_window::get_in_gain(){
  return in_gain->get_gain();
}

float main_window::get_out_gain(){
  return out_gain->get_gain();
}


//Tornar a posar els float com a return
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
float main_window::get_band_type(int band_id){
  float lg, lf, lq, lt;
  lg = m_banda[band_id]->get_gain();
  lf = m_banda[band_id]->get_freq();
  lq = m_banda[band_id]->get_Q();
  lt = m_banda[band_id]->get_filter_type();
  if(lq == 0) lq = 2;
  redraw_plot(band_id, lg, f[(int)lf], lq, lt);

  return lt;
}

float main_window::get_band_gain(int band_id){
  float lg, lf, lq, lt;
  lg = m_banda[band_id]->get_gain();
  lf = m_banda[band_id]->get_freq();
  lq = m_banda[band_id]->get_Q();
  lt = m_banda[band_id]->get_filter_type();
  if(lq == 0) lq = 2;
  redraw_plot(band_id, lg, f[(int)lf], lq, lt);
  
  return lg;
}

float main_window::get_band_freq(int band_id){
  float lg, lf, lq, lt;
  lg = m_banda[band_id]->get_gain();
  lf = m_banda[band_id]->get_freq();
  lq = m_banda[band_id]->get_Q();
  lt = m_banda[band_id]->get_filter_type();
  if(lq == 0) lq = 2;
  redraw_plot(band_id, lg, f[(int)lf], lq, lt);

  return lf;
}

float main_window::get_band_Q(int band_id){
  float lg, lf, lq, lt;
  lg = m_banda[band_id]->get_gain();
  lf = m_banda[band_id]->get_freq();
  lq = m_banda[band_id]->get_Q();
  lt = m_banda[band_id]->get_filter_type();
  if(lq == 0) lq = 2;
  redraw_plot(band_id, lg, f[(int)lf], lq, lt);
  
  return lq;
}


void main_window::redraw_plot(int band, float g, float f, float q, float t){
my_plot->DrawPeakCurve(band, (double)g, (double)f, (double)q, (int)t);

}
///////Funcions de depurat
/*
void main_window::bypass_control(){
  std::cout<<"Bypass = "<<get_bypass()<<std::endl;
  
}

void main_window::in_gain_control(){
  std::cout<<"InGain = "<<in_gain->get_gain()<<std::endl;
}

void main_window::out_gain_control(){
  std::cout<<"InGain = "<<out_gain->get_gain()<<std::endl;
}

void main_window::gain_control(int index){
  std::cout<<"Band "<<(index+1)<<" Gain = "<<m_banda[index]->get_gain()<<std::endl;
}
    
void main_window::freq_control(int index){
  std::cout<<"Band "<<(index+1)<<" Freq = "<<m_banda[index]->get_freq()<<std::endl;
}

void main_window::Q_control(int index){
  std::cout<<"Band "<<(index+1)<<" Q = "<<m_banda[index]->get_Q()<<std::endl;
}

void main_window::type_control(int index){
  std::cout<<"Band "<<(index+1)<<" Type = "<<m_banda[index]->get_filter_type()<<std::endl;
}
*//////////////////////////////////////////////////////

bool main_window::on_window_popup(GdkEventExpose* event){
  for (int i = 0; i< 10; i++){
    m_banda[i]->hide_spins();
  }

  m_presets->load_combo_list();

  if(flag_pop_resize){
    flag_pop_resize = false;
    Gtk::Window *this_win = &((Gtk::Window&)(*this->get_toplevel()));
    this_win->set_resizable(false);
    this_win->modify_bg_pixmap(Gtk::STATE_NORMAL , "/usr/local/lib/lv2/paramEQ-Rafols.lv2/EQ-10Q_bakc.png");
  }

  //Passu dels spins
  stop = 0;

  return true;
}

void main_window::set_gain_freq(int b_ix, float g, float f){
  int ft=(int)m_banda[b_ix]->get_filter_type();
  
  if( ft==LOW_SHELF || ft==HIGH_SHELF || ft==PEAK)m_banda[b_ix]->set_gain(g);
  if(ft != FILTER_OFF)m_banda[b_ix]->set_freq_direct(f); 
}

void main_window::set_all_params(int b_ix, int type, float g, float f, float Q){
  m_banda[b_ix]->set_filter_type((float)type);
  m_banda[b_ix]->set_gain(g);
  m_banda[b_ix]->set_freq(f); //modifica el freq PTR
  m_banda[b_ix]->set_Q(Q);
}

void main_window::get_all_params(int band_id, BandParams &m_params){
  m_params.type = (int) get_band_type(band_id);
  m_params.gain = get_band_gain(band_id);
  m_params.freq = get_band_freq(band_id);
  m_params.Q = get_band_Q(band_id);
}

void main_window::AB_change_params(bool toA){
  for(int i=0; i<NUM_OF_FILTERS;i++){
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
  
  
  }
}

void main_window::on_button_A(){
  if(A_Button.get_active()){
    B_Button.set_active(false);
    
    //guardem la corba B i Carregem la A
    AB_change_params(true);
  }
  
  else B_Button.set_active(true);
  
}

void main_window::on_button_B(){
  if(B_Button.get_active()){
    A_Button.set_active(false);
    
    //guardem la corba A
    AB_change_params(false);
    }
  else A_Button.set_active(true);
}


void main_window::on_button_FLAT(){
  
  //Generem un missatge
  Gtk::MessageDialog dialog((Gtk::Window&)(*this->get_toplevel()),"This will flat the EQ curve, are you sure?",
          false /* use_markup */, Gtk::MESSAGE_QUESTION,
          Gtk::BUTTONS_OK_CANCEL);
 
  int result = dialog.run();

  if(result == Gtk::RESPONSE_OK)flat();
  
}

void main_window::flat(){
   for(int i=0; i< NUM_OF_FILTERS; i++){
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
 }
  
  in_gain->set_gain(0.0);
  out_gain->set_gain(0.0);

  A_Button.set_active(true);
      
}

main_window::~main_window(){

}
