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
#include "pixmapcombo.h"

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/togglebutton.h>
//#include <gtkmm/comboboxtext.h>
#include <gtkmm/scale.h>
#include <gtkmm/alignment.h>


class BandCtl : public Gtk::VBox{
  public:
    BandCtl(); //constructor cutre
    BandCtl( float *freq, const int band_num,
            sigc::slot<void> gain_slot,
            sigc::slot<void> freq_slot,
            sigc::slot<void> Q_slot,
            sigc::slot<void> type_slot, int *semafor
            );
    virtual ~BandCtl();
    float get_gain();
    float get_freq();
    float get_Q();
    float get_filter_type();
    void set_gain(float g);
    void set_freq(float f);
    void set_freq_direct(float f);
    void set_Q(float q);
    void set_filter_type(float t);
    void hide_spins();
    
  protected:
    Gtk::Label band_label;
    Gtk::ToggleButton m_on_button;
    //Gtk::ComboBoxText m_filter_sel;
    PixMapCombo m_filter_sel;
    Gtk::Alignment button_align, combo_align;
    EQButton *m_gain, *m_freq, *m_Q;
    
    void on_button_clicked();
    void on_combo_changed();
    void config_type();
    void config_sensitive();
    void reset_Q(float q);
    
    
  private:
    int filter_type, ant_filter;
    float *f;
    bool vic_de_set;
};

class  GainCtl : public Gtk::VBox{
  public:
    GainCtl(const Glib::ustring title, sigc::slot<void> m_slot);
    virtual ~GainCtl();
    void set_gain(float g);
    float get_gain();
  
  protected:
    Gtk::VScale gain_scale;
    Gtk::Label gain_label;

};