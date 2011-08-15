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

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/scale.h>
#include <gtkmm/alignment.h>

#define  LPF_ORDER_1 0
#define  LPF_ORDER_2 1
#define  LPF_ORDER_3 2
#define  LPF_ORDER_4 3
#define  HPF_ORDER_1 4
#define  HPF_ORDER_2 5
#define  HPF_ORDER_3 6
#define  HPF_ORDER_4 7
#define  LOW_SHELF   8
#define  HIGH_SHELF  9
#define  PEAK        10
#define  NOTCH       11

//Q default values ///TODO: Aixo ha de venir del .ttl
#define HPF_LPF_Q_DEFAULT 0.7
#define NOTCH_Q_DEFAULT 2
#define HIGH_LOW_SHELF_Q_DEFAULT  0.7
#define PEAK_Q_DEFAULT 2

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
    typedef sigc::signal<int, int, float> ctlBand_changed;
    ctlBand_changed signal_changed(); ///TODO: Definir aixo en el CPP i col·locar tots  els emtit() necessaris
        
  protected:
    Gtk::Label m_BandLabel;
    Gtk::ToggleButton m_OnButton;
    PixMapCombo m_FilterSel;
    Gtk::Alignment m_ButtonAlign, m_ComboAlign;
    EQButton *m_Gain, *m_Freq, *m_Q;
    
    void onButtonClicked();
    void onComboChanged();
    void configSensitive();   
    
  private:
    int m_iFilterType;
    int m_iBandNum;
    bool m_bBandIsEnabled;
    
    //Band change signal
    ctlBand_changed m_bandChangedSignal;
    

};
#endif

