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
#include <gtkmm/image.h>

#include <cmath>

#include <gtkmm/window.h>

#include "gui/band_ctl.h"
#include "eq_type.h"

using namespace sigc;


class EqMainWindow : public Gtk::Window {
  public:
    EqMainWindow();
    virtual ~EqMainWindow();


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

    void AB_change_params(bool toA); //toA = true => guardem B i carreguem A
    void on_button_A();
    void on_button_B();
    void on_button_FLAT();
    void flat();

};

#endif
