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
#include <gtkmm/frame.h>

#include "vuwidget.h"
#include "faderwidget.h"

class  GainCtl : public Gtk::Frame{
  public:
    GainCtl(const Glib::ustring sTitle, int iNumOfChannels, double Fader_dBMax, double Fader_dBMin, const char* bundlePath);
    virtual ~GainCtl();
    void setGain(float fValue);
    void setVu(int channel, float fValue);
    float getGain();
     
    //signal accessor:
      //Parameters:
      //bool: true if is the Input Gain control, false if Out
      //flot: the value of gain
    typedef sigc::signal<void> signal_GainChanged;
    signal_GainChanged signal_changed();
    
  protected:
    FaderWidget *m_GainFader;
    VUWidget *m_VuMeter;
    Gtk::HBox m_HBox;
    const int m_iNumOfChannels;
    
    //Signal Handler
    void onGainChanged();
    
    //Gain change signal
    signal_GainChanged m_GainChangedSignal;
};
#endif