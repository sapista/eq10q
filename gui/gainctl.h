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

#ifndef GAIN_CTL_H
  #define GAIN_CTL_H

#include <gtkmm/box.h>
#include <gtkmm/scale.h>
#include <gtkmm/label.h>

class  GainCtl : public Gtk::VBox{
  public:
    GainCtl(const Glib::ustring sTitle, int iNumOfChannels, bool bTrueIfIn);
    virtual ~GainCtl();
    void setGain(float fValue);
    float getGain();
     
    //signal accessor:
      //Parameters:
      //bool: true if is the Input Gain control, false if Out
      //flot: the value of gain
    typedef sigc::signal<void, bool, float> signal_GainChanged;
    signal_GainChanged signal_changed();
    
  protected:
    Gtk::VScale m_GainScale;
    Gtk::Label m_GainLabel;
    const int m_iNumOfChannels;
    const bool m_bTrueIfIn;
    
    //Signal Handler
    void onGainChanged();
    
    //Gain change signal
    signal_GainChanged m_GainChangedSignal;

};
#endif