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

//LV2 UI header
#include <slv2/lv2_ui.h>

#include "bandctl.h"
#include "gainctl.h"
#include "eqparams.h"
//#include "eq_type.h"  ///TODO: Estudiar aixo no cal, crec.


using namespace sigc;

class EqMainWindow : public Gtk::Box {
  public:
    EqMainWindow(int iAudioChannels, int iNumBands, const char *uri);
    virtual ~EqMainWindow();
    
    
    ///TODO: comprobar si tot aixo es necessari en realitat--------------------------------------------
    //Input control ports handlers
    void setBypass(bool bBypass);
    void setInputGain(float Gain);
    void setOutputGain(float Gain);
    void setInputVuLevel(int Channel, float Level);
    void setOutputVuLevel(int Channel, float Level);
    void setBandGain(int Band, float Gain);
    void setBandFreq(int Band, float Freq);
    void setBandQ(int Band, float Q);
    void setBandType(int Band, float Type);
    void setBandEnabled(int Band, bool Enabled);
    
    //Output control ports handlers (signals eimted by this widget)
    typedef sigc::signal<void, bool> signal_BypassChanged;
    typedef sigc::signal<void, float> signal_ControlGainChanged;
    typedef sigc::signal<void, int, float> signal_BandParamChanged;
    typedef sigc::signal<void, int, int> signal_BandTypeChanged;
    typedef sigc::signal<void, int, bool> signal_BandEnabledChanged;
    
    signal_BypassChanged signal_Bypass_Changed();
    signal_ControlGainChanged signal_InputGain_Changed();
    signal_ControlGainChanged signal_OutputGain_Changed();
    signal_BandParamChanged signal_BandGain_Changed();
    signal_BandParamChanged signal_BandFreq_Changed();
    signal_BandParamChanged signal_BandQ_Changed();
    signal_BandTypeChanged signal_BandType_Changed();
    signal_BandEnabledChanged signal_BandEnabled_Changed();
    ///------------------------------------------------------------------TODO: fins aki
    
    
    // Informing GUI about changes in the control ports
    void gui_port_event(LV2UI_Handle ui, uint32_t port_index, uint32_t buffer_size, uint32_t format, const void * buffer)
    {
        float val;
        val = * static_cast<const float*>(buffer);

        // Checking if params are the same as specified in the LV2 documentation
        if (format != 0) {
            return;
        }
        if (buffer_size != 4) {
            return;
        }

        // Updating values for GUI changes
        switch (port_index) {
	  case 1:
		///TODO: completar aqui amb escriptura a tots els ports
            break;
	  default:
            return;
        }
    }

    LV2UI_Controller controller;
    LV2UI_Write_Function write_function;

  protected:
    EqParams *m_AParams, *m_BParams, *m_CurParams;
    BandCtl **m_BandCtlArray; 
    GainCtl *m_InGain, *m_OutGain;
    Gtk::HBox m_BandBox, m_ABFlatBox, m_GainEqBox;
    Gtk::VBox m_CurveBypassBandsBox, m_MainBox;
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
    void onInputGainChange();
    void onOutputGainChange();
    //void onCurveChange(); //TODO: no se com ha de ser en handle de la curve
    
  private:
    const int m_iNumOfChannels;
    const int m_iNumOfBands;
    bool m_bMutex;
    std::string m_pluginUri;
    
    ///TODO: comprobar si tot aixo es necessari en realitat--------------------------------------------
    //LV2 Ports Signals
    signal_BypassChanged m_BypassChangedSignal;
    signal_ControlGainChanged m_InputGainChangedSignal;
    signal_ControlGainChanged m_OutputGainChangedSignal;
    signal_BandParamChanged m_BandGainChangedSignal;
    signal_BandParamChanged m_BandFreqChangedSignal;
    signal_BandParamChanged m_BandQChangedSignal;
    signal_BandTypeChanged m_BandTypeChangedSignal;
    signal_BandEnabledChanged m_BandEnabledChangedSignal;
    ///TODO: Fins aki----------------------------------------------------------------------------------------
};

#endif
