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

#ifndef BAND_CTL_H
  #define BAND_CTL_H
#include "eqbutton.h"
#include "pixmapcombo.h"
#include "../dsp/filter.h"

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/alignment.h>

class BandCtl : public Gtk::VBox
{
  public:
    BandCtl(const int iBandNum,bool *bSemafor);
    
    virtual ~BandCtl();
    float getGain();
    float getFreq();
    float getQ();
    float getFilterType();
    bool getEnabled();
    
    void setGain(float fGain);
    void setFreq(float fFreq);
    void setQ(float fQ);
    void setFilterType(float fType);
    void setEnabled(bool bIsEnabled);
    
    //signal accessor: 
	//Parameters:
	//int -> BandNumber
	//int -> field(gain, freq, Q, type, ON/OFF)
	//Float -> value
    typedef sigc::signal<void, int, int, float> signal_ctlBandChanged;
    signal_ctlBandChanged signal_changed();
        
  protected:
    Gtk::Label m_BandLabel;
    Gtk::ToggleButton m_OnButton;
    PixMapCombo m_FilterSel;
    Gtk::Alignment m_ButtonAlign, m_ComboAlign;
    EQButton *m_Gain, *m_Freq, *m_Q;
    
    //Signal Handlers
    void onButtonClicked();
    void onComboChanged();
    void onGainChanged();
    void onFreqChanged();
    void onQChanged();
    
  private:
    //int m_iFilterType; //TODO: check this can be remove
	FilterType m_FilterType;
    int m_iBandNum;
    bool m_bBandIsEnabled;
    
    void configSensitive();
      
    //Band change signal
    signal_ctlBandChanged m_bandChangedSignal;

};
#endif

