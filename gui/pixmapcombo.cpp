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

#include "pixmapcombo.h"

PixMapCombo::PixMapCombo(){
  //Create the tree model
  m_refTreeModel = Gtk::ListStore::create(m_Columns);
  set_model(m_refTreeModel);
  
  //Enganxar una imatge al pixbuf
  Glib::RefPtr<Gdk::Pixbuf> m_refTopixbuf= Gdk::Pixbuf::create_from_file(RUTA_OFF);
  
  //Fill the ComboBox Tree Model
  Gtk::TreeModel::Row row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;
  
  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(RUTA_LPF1);
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;
  
  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(RUTA_LPF2);
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;

  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(RUTA_LPF3);
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;

  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(RUTA_LPF4);
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;

  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(RUTA_HPF1);
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;
  
  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(RUTA_HPF2);
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;

  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(RUTA_HPF3);
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;

  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(RUTA_HPF4);
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;

  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(RUTA_LOSHELF);
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;
  
  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(RUTA_HISHELF);
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;
  
  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(RUTA_PEAK);
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;
  
  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(RUTA_NOTCH);
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;  
  
   //add to combo
   pack_start(m_Columns.m_col_pix);
   
   
   
   show_all_children();

}

PixMapCombo::~PixMapCombo(){

}