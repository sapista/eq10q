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

#include <iostream>

#include <iomanip>

#include <cmath>

#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/alignment.h>

#include "constants.h"

#define GAIN_TYPE 0
#define FREQ_TYPE 1
#define Q_TYPE    2

#define ACCELERATION 15

class EQButton;

class CtlButton : public Gtk::Button{
  public:
    CtlButton(int type, float *value, float *f, EQButton *m_eqbutton);
    virtual ~CtlButton();
    void set_press();
    void set_depress();
    void set_button_number(float num);
    void set_freq_index(int index);
    virtual float get_freq_ptr();
  
    
  protected:
    virtual void on_button_depressed();
    virtual bool on_mouse_move(GdkEventMotion* event);
    
    
    virtual void set_value(int x, int y);
  
  private:  
    bool press,x_direction, first_time;
    int  act_val, ant_val, filter_type, ptr, acumula;
    float  *mouse_value, *f_ptr; //ptr es l'index de la taula, el fet k sigui float es un trukillu pel calcul de l'acceleracio
    EQButton *eqbutton_ref_ptr;
};

class EQButton : public Gtk::VBox{
  public:
    EQButton(int type, float *f, sigc::slot<void> m_slot, int *semafor);
    virtual ~EQButton();
    virtual void set_value(float val);
    virtual float get_value();
    virtual void set_freq_ptr(float index);
    virtual void hide_spin();
    virtual void set_spin_number();
   
    
  protected:
    Gtk::Alignment button_align;
    Gtk::SpinButton text_entry;
    CtlButton *ctlbutton;
    

    virtual bool on_button_double_clicked(GdkEventButton* event);
    virtual void on_enter_pressed();
    virtual void on_spin_change();
    //Funtion per sobreescriure en segons tipus de butto

    
  private:
    int filter_type;
    float value, *f_ptr; //punter a on guardem el valor
    int *stop;
};
