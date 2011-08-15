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
