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

#ifndef CTL_BUTTON_H
	#define CTL_BUTTON_H
	
#include <iostream>
#include <iomanip>
#include <cmath>

///TODO: comprobar que tots aquest include son necessaris
#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/alignment.h>

#include "eqbutton.h"

#define GAIN_TYPE 0
#define FREQ_TYPE 1
#define Q_TYPE    2

#define ACCELERATION 15

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

#endif