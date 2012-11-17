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

#include <gtkmm/eventbox.h>
#include <gtkmm/alignment.h>
#include <gtkmm/box.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/alignment.h>
#include <gtkmm/button.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/frame.h>
#include <gtkmm/image.h>

#include <cmath>

//LV2 UI header
#include "../lv2_ui.h"

#include "bandctl.h"
#include "gainctl.h"
#include "eqparams.h"
#include "bodeplot.h"

//Include eq definition
#include "../eq_defines.h"

#define IMAGE_LOGO_PATH "icons/logoeq10q.png"
#define TIMER_VALUE_MS 100

//Test print information, comment out for the final release
//#define PRINT_DEBUG_INFO

using namespace sigc;

class EqMainWindow : public Gtk::EventBox
{
  public:
    EqMainWindow(int iAudioChannels, int iNumBands, const char *uri, const char *bundlePath);
    virtual ~EqMainWindow();
    
    ///TODO: comprobar si tot aixo es necessari en realitat----------- En teoria he de invocar la LV2 WriteFunction directament
    /***************************************************************
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
    *******************************************************************/
    ///------------------------------------------------------------------TODO: fins aki
    
    
    // Informing GUI about changes in the control ports
    void gui_port_event(LV2UI_Handle ui, uint32_t port, uint32_t buffer_size, uint32_t format, const void * buffer)
    {
      float data = * static_cast<const float*>(buffer);
      
      
      #ifdef PRINT_DEBUG_INFO
	std::cout<<"gui_port_event Entring....... "<<std::endl;
      #endif
      
        // Checking if params are the same as specified in the LV2 documentation
        if (format != 0) {
	    #ifdef PRINT_DEBUG_INFO
	      std::cout<<"\t-- Return Format != 0"<<std::endl;
	    #endif
            return;
        }
        if (buffer_size != 4) {
	    #ifdef PRINT_DEBUG_INFO
	      std::cout<<"\t-- Return buffer_size != 4"<<std::endl;
	    #endif  
            return;
        }

        // Updating values in GUI ========================================================
	switch (port)
	{
	  case EQ_BYPASS:
	    m_bypassValue = data > 0.5 ? 1 : 0;
	    m_port_event_Bypass = true; //Marck port even boolean
	    ///m_BypassButton.set_active(data > 0.5);
	    #ifdef PRINT_DEBUG_INFO
	      std::cout<<"\t-- BYPASS"<<std::endl;
	    #endif  
	  break;

	  case EQ_INGAIN:
	    //m_InGainValue = data;
	    m_CurParams->setInputGain(data);
	    m_port_event_InGain = true; //Marck port even boolean
	    ///m_InGain->setGain(data);
	    
	    #ifdef PRINT_DEBUG_INFO
	      std::cout<<"\t-- Input Gain"<<std::endl;
	    #endif  
	  break;

	  case EQ_OUTGAIN:
	    //m_OutGainValue = data;
	    m_CurParams->setOutputGain(data);
	    m_port_event_OutGain = true; //Marck port even boolean
	    ///m_OutGain->setGain(data);
	    #ifdef PRINT_DEBUG_INFO
	      std::cout<<"\t-- Out Gain"<<std::endl;
	    #endif  
	  break;

	  default:
	    //Connect BandGain ports
	    if((int)port >= (PORT_OFFSET + 2*m_iNumOfChannels) && (int)port < (PORT_OFFSET + 2*m_iNumOfChannels + m_iNumOfBands))
	    {
	      ///m_BandCtlArray[(int)port - PORT_OFFSET - 2*m_iNumOfChannels]->setGain(data);
	      ///m_Bode->setBandGain((int)port - PORT_OFFSET - 2*m_iNumOfChannels, data);
	      m_CurParams->setBandGain((int)port - PORT_OFFSET - 2*m_iNumOfChannels, data);
	      m_port_event_Curve = true; //Marck port even boolean
	      #ifdef PRINT_DEBUG_INFO
		std::cout<<"\t-- Band Gain"<<std::endl;
	      #endif  
	    }

	    //Connect BandFreq ports
	    else if((int)port >= (PORT_OFFSET + 2*m_iNumOfChannels + m_iNumOfBands) && (int)port < (PORT_OFFSET + 2*m_iNumOfChannels + 2*m_iNumOfBands))
	    {
	      ///m_BandCtlArray[(int)port - PORT_OFFSET - 2*m_iNumOfChannels - m_iNumOfBands]->setFreq(data);
	      ///m_Bode->setBandFreq((int)port - PORT_OFFSET - 2*m_iNumOfChannels - m_iNumOfBands, data);
	      m_CurParams->setBandFreq((int)port - PORT_OFFSET - 2*m_iNumOfChannels - m_iNumOfBands, data);
	      m_port_event_Curve = true; //Marck port even boolean
	      #ifdef PRINT_DEBUG_INFO
		std::cout<<"\t-- Band Freq"<<std::endl;
	      #endif  
	    }

	    //Connect BandParam ports
	    else if((int)port >= (PORT_OFFSET + 2*m_iNumOfChannels + 2*m_iNumOfBands) && (int)port < (PORT_OFFSET + 2*m_iNumOfChannels + 3*m_iNumOfBands))
	    {
	      ///m_BandCtlArray[(int)port - PORT_OFFSET - 2*m_iNumOfChannels - 2*m_iNumOfBands]->setQ(data);
	      ///m_Bode->setBandQ((int)port - PORT_OFFSET - 2*m_iNumOfChannels - 2*m_iNumOfBands, data);
	      m_CurParams->setBandQ((int)port - PORT_OFFSET - 2*m_iNumOfChannels - 2*m_iNumOfBands, data);
	      m_port_event_Curve = true; //Marck port even boolean
	      #ifdef PRINT_DEBUG_INFO
		std::cout<<"\t-- Band Q"<<std::endl;
	      #endif  
	    }

	    //Connect BandType ports
	    else if((int)port >= (PORT_OFFSET + 2*m_iNumOfChannels + 3*m_iNumOfBands) && (int)port < (PORT_OFFSET + 2*m_iNumOfChannels + 4*m_iNumOfBands))
	    {
	      ///m_BandCtlArray[(int)port - PORT_OFFSET - 2*m_iNumOfChannels - 3*m_iNumOfBands]->setFilterType(data);
	      ///m_Bode->setBandType((int)port - PORT_OFFSET - 2*m_iNumOfChannels - 3*m_iNumOfBands, data);
	      m_CurParams->setBandType((int)port - PORT_OFFSET - 2*m_iNumOfChannels - 3*m_iNumOfBands, (int)data);
	      m_port_event_Curve = true; //Marck port even boolean
	      #ifdef PRINT_DEBUG_INFO
		std::cout<<"\t-- Band Type"<<std::endl;
	      #endif  
	    }

	    //Connect BandEnabled ports
	    else if((int)port >= (PORT_OFFSET + 2*m_iNumOfChannels + 4*m_iNumOfBands) && (int)port < (PORT_OFFSET + 2*m_iNumOfChannels + 5*m_iNumOfBands))
	    {
	      ///m_BandCtlArray[(int)port - PORT_OFFSET - 2*m_iNumOfChannels - 4*m_iNumOfBands]->setEnabled(data > 0.5);
	      ///m_Bode->setBandEnable((int)port - PORT_OFFSET - 2*m_iNumOfChannels - 4*m_iNumOfBands, data > 0.5);
	      m_CurParams->setBandEnabled((int)port - PORT_OFFSET - 2*m_iNumOfChannels - 4*m_iNumOfBands, data > 0.5);
	      m_port_event_Curve = true; //Marck port even boolean
	      #ifdef PRINT_DEBUG_INFO
		std::cout<<"\t-- Band Enabled"<<std::endl;
	      #endif  
	    }

	    //Connect VuInput ports
	    else if((int)port >= (PORT_OFFSET + 2*m_iNumOfChannels + 5*m_iNumOfBands) && (int)port < (PORT_OFFSET + 2*m_iNumOfChannels + 5*m_iNumOfBands + m_iNumOfChannels))
	    {
	      m_InGain->setVu((int)port - PORT_OFFSET - 2*m_iNumOfChannels - 5*m_iNumOfBands,data);
	      #ifdef PRINT_DEBUG_INFO
		std::cout<<"\t-- Vu input"<<std::endl;
	      #endif  
	    }

	    //Connect VuOutput ports
	    else if((int)port >= (PORT_OFFSET + 2*m_iNumOfChannels + 5*m_iNumOfBands + m_iNumOfChannels) && (int)port < (PORT_OFFSET + 2*m_iNumOfChannels + 5*m_iNumOfBands + 2*m_iNumOfChannels))
	    {
	      m_OutGain->setVu((int)port - PORT_OFFSET - 2*m_iNumOfChannels - 5*m_iNumOfBands - m_iNumOfChannels, data);
	      #ifdef PRINT_DEBUG_INFO
		std::cout<<"\t-- Vu output"<<std::endl;
	      #endif  
	    }
	    
	    //
	    else
	    {
	      #ifdef PRINT_DEBUG_INFO	    
		std::cout<<"\t--  Return port index is out of range"<<std::endl;
	      #endif
	      return;
	    }
	  break;
	}       
        
	#ifdef PRINT_DEBUG_INFO	    
	  std::cout<<"\t--  Return OK"<<std::endl;
	#endif
	
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
    Gtk::Alignment m_FlatAlign, m_ABAlign, m_ButtonAAlign, m_ButtonBAlign, m_BypassAlign, m_LoadAlign, m_SaveAlign;
    Gtk::Button m_FlatButton, m_SaveButton, m_LoadButton;
    Gtk::Frame m_PlotFrame;
    Gtk::Alignment m_MainWidgetAlign;
    PlotEQCurve *m_Bode;
    Gtk::Image *image_logo_center;

    void loadEqParams();
    void changeAB(EqParams *toBeCurrent);
    void saveToFile();
    void loadFromFile();
    
    //Signal Handlers
    void onButtonA();
    void onButtonB();
    void onButtonFlat();
    void onButtonBypass();
    void onBandChange(int iBand, int iField, float fValue);
    void onInputGainChange();
    void onOutputGainChange();
    void onRealize();
    void onCurveChange(int band_ix, float Gain, float Freq, float Q);
    void onCurveBandEnable(int band_ix, bool IsEnabled);
    bool on_timeout();
    
  private:
    float m_bypassValue;
    
    //TODO this vars are redundant with m_curParams!!! REMOVE IT!!!
    //float m_InGainValue, m_OutGainValue;
    
    const int m_iNumOfChannels;
    const int m_iNumOfBands;
    bool m_bMutex, m_port_event_InGain, m_port_event_OutGain, m_port_event_Curve, m_port_event_Bypass, m_DisableBypassEvent;
    std::string m_pluginUri;
    std::string m_bundlePath;
    
    ///TODO: comprobar si tot aixo es necessari en realitat--------------------------------------------
    /*************************************************************************
    //LV2 Ports Signals
    signal_BypassChanged m_BypassChangedSignal;
    signal_ControlGainChanged m_InputGainChangedSignal;
    signal_ControlGainChanged m_OutputGainChangedSignal;
    signal_BandParamChanged m_BandGainChangedSignal;
    signal_BandParamChanged m_BandFreqChangedSignal;
    signal_BandParamChanged m_BandQChangedSignal;
    signal_BandTypeChanged m_BandTypeChangedSignal;
    signal_BandEnabledChanged m_BandEnabledChangedSignal;
    ***************************************************************************/
    ///TODO: Fins aki----------------------------------------------------------------------------------------
    
};

#endif
