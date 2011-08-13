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
#include <gtkmm/scale.h>
#include <gtkmm/alignment.h>


class BandCtl : public Gtk::VBox
{
  public:
    BandCtl( const int iBandNum,int *semafor); ///TODO: Aixo hauria de canviar, sistema senyal-slot
    
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
    void hide_spins(); ///TODO: pq volem aixo???
    
  protected:
    Gtk::Label m_BandLabel;
    Gtk::ToggleButton m_OnButton;
    PixMapCombo m_FilterSel;
    Gtk::Alignment m_ButtonAlign, m_ComboAlign;
    EQButton *m_Gain, *m_Freq, *m_Q;
    
    void onButtonClicked();
    void onComboChanged();
    void configType(); ///TODO: pq serveix?
    void configSensitive();///TODO: Es per activar/desactivar els botons que no apliquen a un tipus de filtre
    void reset_Q(float q);///TODO: pq serveix?
    
    
  private:
    int m_iFilterType;
    int m_iBandNum;
    bool m_bBandIsEnabled;

};



///TODO: GainCtl Ha d'estar en un altre fitxer separat
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