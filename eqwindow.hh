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

#ifndef EQ_MAIN_WIN_H
  #define EQ_MAIN_WIN_H

#include <iostream>

#include <gtkmm/box.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/alignment.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/button.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/fixed.h>
//#include <gtkmm/container.h>
#include <gtkmm/image.h>

#include <cmath>

//#include <gtkmm/window.h>
#include <lv2gui.hpp>

#include "band_ctl.h"
#include "bodeplot.h"
#include "vuwidget.hpp"
#include "templatewidget.h"
#include "types.h"

using namespace sigc;

class main_window : public LV2::GUI<main_window, LV2::Presets<false> >{ //LV2 STYLE

//class main_window : public Gtk::Window { //Class per disseny de GUI en Kdevelop
  public:
    //main_window(); //Creacio GUI Kdevelop
    main_window(const std::string& URI); //LV2 Style
    virtual ~main_window();
    
    void set_gain_freq(int b_ix, float g, float f);
    void set_all_params(int b_ix, int type, float g, float f, float Q);
    void get_all_params(int band_id, BandParams &m_params);

    void port_event(uint32_t port, uint32_t buffer_size, uint32_t format, const void* buffer) {
      float param;
      param=*static_cast<const float*>(buffer);
      //std::cout<<"PORT = "<<port<<" ADJUST = "<<param<<std::endl;
    
      switch(port){
        case 2: //bypass
          if(param < 0.5) bypass_button.set_active(false);
          else bypass_button.set_active(true);
        break;

        case 3: //input gain
          in_gain->set_gain(param);
          break;
  
        case 4: //out gain
         out_gain->set_gain(param);
        break;

       //Filter Type
        case 5: case 9: case 13: case 17: case 21:
        case 25: case 29: case 33: case 37: case 41:
          m_banda[(port-FIRST_FILTER_PORT_INDEX)/4]->set_filter_type(param);
        break;

        //Filter Gain
        case 6: case 10: case 14: case 18: case 22:
        case 26: case 30: case 34: case 38: case 42:
          m_banda[(port-FIRST_FILTER_PORT_INDEX)/4]->set_gain(param);
        break;

        //Filter Freq
        case 7: case 11: case 15: case 19: case 23:
        case 27: case 31: case 35: case 39: case 43:
          m_banda[(port-FIRST_FILTER_PORT_INDEX)/4]->set_freq(param);
        break;

        //Filter Q
        case 8: case 12: case 16: case 20: case 24:
        case 28: case 32: case 36: case 40: case 44:
          m_banda[(port-FIRST_FILTER_PORT_INDEX)/4]->set_Q(param);
        break;

        case 45:
          in_vu.set_value(0,param);
        break;

        case 46:
          out_vu.set_value(0,param);
        break;
      }//FI Switch
  }


  protected:
    BandCtl *m_banda[10]; 
    GainCtl *in_gain, *out_gain;
    Gtk::HBox band_box, gain_box, ABFlat_box;
    Gtk::VBox gain_bypass_box, main_box;
    Gtk::ToggleButton bypass_button, dummy_box, A_Button, B_Button;
    Gtk::HButtonBox buttons_ABF;
    Gtk::Alignment Flat_align, AB_align, buttonA_align, buttonB_align;
    Gtk::Button Flat_Button;
    Gtk::Fixed plot_fix;
    Gtk::Image image_logo_top_top, image_logo_top, image_logo_center, image_logo_bottom, image_logo_bottom_bottom;

    //el plot
    PlotEQCurve *my_plot;

    //Els Vumeters
    VUWidget out_vu, in_vu;

    //Els presets
    TemplateWidget  *m_presets;
    
    float get_bypass();
    float get_in_gain();
    float get_out_gain();
    
    //OJU s'han de posar a float pel plugin real
    float get_band_type(int band_id);
    float get_band_gain(int band_id);
    float get_band_freq(int band_id);
    float get_band_Q(int band_id);
    
    
    void redraw_plot(int band, float g, float f, float q, float t);
    bool on_window_popup(GdkEventExpose* event);
 
    //Funcions per depurat
/*
    void bypass_control();
    void in_gain_control();
    void out_gain_control();
    void gain_control(int index);
    void freq_control(int index);
    void Q_control(int index);
    void type_control(int index);
  */  
    float f[NUM_POINTS];
    int stop;
    
    //Comparacio de corves A B
    BandParams band_params;
    
    BandParams paramsA[NUM_OF_FILTERS], paramsB[NUM_OF_FILTERS];
    void AB_change_params(bool toA); //toA = true => guardem B i carreguem A
    void on_button_A();
    void on_button_B();
    void on_button_FLAT();
    void flat();

    //Flag per evitar loop amb el window_popup
    bool flag_pop_resize;
};

//Uncomment for LV2
static int _ = main_window::register_class("http://sapistaplugin.com/eq/param/peaking/gui");

#endif
