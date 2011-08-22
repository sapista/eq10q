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
#include <string>

#include <gtkmm/box.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/alignment.h>
#include <gtkmm/button.h>
#include <gtkmm/messagedialog.h>
//#include <gtkmm/fixed.h> //Amb el nou plot potser no caldara
#include <gtkmm/image.h>

#include <cmath>

#include "gui/bandctl.h"
#include "gui/gainctl.h"
#include "gui/eqparams.h"
//#include "eq_type.h"  ///TODO: Estudiar aixo no cal, crec.


using namespace sigc;

class EqMainWindow : public Gtk::Box {
  public:
    EqMainWindow(int iAudioChannels, int iNumBands, const char *uri);
    virtual ~EqMainWindow();

  protected:
    EqParams *m_AParams, *m_BParams, *m_CurParams;
    BandCtl **m_BandCtlArray; 
    GainCtl *m_InGain, *m_OutGain;
    Gtk::HBox m_BandBox, m_GainBox, m_ABFlatBox;
    Gtk::VBox m_GainBypassBox, m_MainBox;
    Gtk::ToggleButton m_BypassButton, m_AButton, m_BButton;
    Gtk::Alignment m_FlatAlign, m_ABAlign, m_ButtonAAlign, m_ButtonBAlign, m_BypassAlign;
    Gtk::Button m_FlatButton;
    ///Gtk::Fixed m_PlotFix; ///TODO: amb el nou sistema de plot potser no caldra
    
    ///TODO: Estudiar la millor forma de carrgar les imatges
    //Gtk::Image image_logo_top_top, image_logo_top, image_logo_center, image_logo_bottom, image_logo_bottom_bottom;

    void loadEqParams();
    void changeAB(EqParams *toBeCurrent);
    
    //Signal Handlers
    void onButtonA();
    void onButtonB();
    void onButtonFlat();
    void onButtonBypass();
    void onBandChange(int iBand, int iField, float fValue);
    void onGainChange(bool bIn, float fGain);
    //void onCurveChange(); //TODO: no se com ha de ser en handle de la curve
    
  private:
    const int m_iNumOfChannels;
    const int m_iNumOfBands;
    bool m_bMutex;
    std::string m_pluginUri;
};

#endif
