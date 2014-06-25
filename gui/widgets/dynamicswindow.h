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

#ifndef DYN_MAIN_WIN_H
  #define DYN_MAIN_WIN_H

#include <iostream>
#include <string>

#include <gtkmm/eventbox.h>
#include <gtkmm/alignment.h>
#include <gtkmm/box.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/button.h>
#include <gtkmm/frame.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

#include <cmath>

//LV2 UI header
#include "../lv2_ui.h"

#include "vuwidget.h"
#include "knob2.h"

#define PORT_OUTPUT 0
#define PORT_INPUT 1
#define PORT_KEY_LISTEN 2
#define PORT_THRESHOLD 3
#define PORT_ATACK 4
#define PORT_HOLD 5
#define PORT_DECAY 6
#define PORT_RANGE 7
#define PORT_HPFFREQ 8
#define PORT_LPFFREQ 9
#define PORT_GAIN 10
#define PORT_INVU 11
#define PORT_GAINREDUCTION 12
#define PORT_KNEE 13

//Test print information, comment out for the final release
//#define PRINT_DEBUG_INFO

using namespace sigc;

class DynMainWindow : public Gtk::EventBox
{
  public:
    DynMainWindow(const char *uri, std::string bundlePath, std::string title, bool isCompressor);
    virtual ~DynMainWindow();   
    
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
	  case PORT_KEY_LISTEN:
	    m_KeyButton.set_active(data > 0.5);
	  break;
	  
	  case PORT_THRESHOLD:
	    m_InputVu->set_value_th(data);
	  break;
	  
	  case PORT_ATACK:
	    m_Attack->set_value(data);
	  break;
	  
	  case PORT_HOLD:
	    m_Hold_Makeup->set_value(data);
	  break;
	  
	  case PORT_DECAY:
	    m_Release->set_value(data);
	  break;
	  
	  case PORT_RANGE:
	    m_Range_Ratio->set_value(data);
	  break;
	  
	  case PORT_GAINREDUCTION:
	    m_GainReductionVu->setValue(0,data);
	  break;
	  
	  case PORT_HPFFREQ:
	    m_HPF->set_value(data);
	  break;
	  
	  case PORT_LPFFREQ:
	    m_LPF->set_value(data);
	  break;
	  
	  case PORT_GAIN:
	    m_InGainFader->set_value(data);
	  break;
	  
	  case PORT_INVU:
	    m_InputVu->setValue(0,data);
	  break;
	  
	  case PORT_KNEE:
	    m_Knee->set_value(data);
	  break;
	}       
        
	#ifdef PRINT_DEBUG_INFO	    
	  std::cout<<"\t--  Return OK"<<std::endl;
	#endif
	
    }

    LV2UI_Controller controller;
    LV2UI_Write_Function write_function;

  protected:
    VUWidget *m_InputVu;
    VUWidget *m_GainReductionVu; 
    KnobWidget2 *m_InGainFader;
    KnobWidget2 *m_Attack;
    KnobWidget2 *m_Hold_Makeup;
    KnobWidget2 *m_Release;
    KnobWidget2 *m_Range_Ratio;
    KnobWidget2 *m_Knee;
    KnobWidget2 *m_HPF;
    KnobWidget2 *m_LPF;
    Gtk::ToggleButton m_KeyButton;
    Gtk::Alignment m_ButtonAlign;
    Gtk::HBox m_VuBox;
    Gtk::VBox m_GattingBox, m_SideChainBox, m_TitleBox;
    Gtk::Frame m_GattingFrame, m_SideChainFrame, m_VuInFrame, m_VuGrFrame, m_TitleFrame;       
    Gtk::Alignment m_VuInAlign, m_VuGrAlign, m_MainWidgetAlign, m_TitleAlign;
    Gtk::Image *image_logo;
    Gtk::Label m_LTitle;
    
    //Signal Handlers
    void onGainChange();
    void onThresholdChange();
    void onRangeChange();
    void onAttackChange();
    void onHoldChange();
    void onReleaseChange();
    void onKneeChange();
    void onHPFChange();
    void onLPFChange();
    void onRealize();
    void onKeyListenChange();
    
  private:
    std::string m_pluginUri;
    std::string m_bundlePath;  
    bool m_bIsCompressor;
};

#endif
