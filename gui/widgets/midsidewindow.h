/***************************************************************************
 *   Copyright (C) 2015 by Pere Ràfols Soler                               *
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

#ifndef MIDSIDE_MAIN_WIN_H
  #define MIDSIDE_MAIN_WIN_H

#include <iostream>
#include <string>

#include <gtkmm/box.h>
#include <gtkmm/image.h>

#include <cmath>

//LV2 UI header
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

#include "mainwidget.h"
#include "vuwidget.h"
#include "knob2.h"
#include "toggle_button.h"

#define PORT_AUDIO_IN_1 0
#define PORT_AUDIO_IN_2 1
#define PORT_AUDIO_OUT_1 2
#define PORT_AUDIO_OUT_2 3
#define PORT_GAIN_IN_1 4
#define PORT_GAIN_IN_2 5
#define PORT_GAIN_OUT_1 6
#define PORT_GAIN_OUT_2 7
#define PORT_SOLO_IN_1 8
#define PORT_SOLO_IN_2 9
#define PORT_SOLO_OUT_1 10
#define PORT_SOLO_OUT_2 11
#define PORT_VU_IN_1 12
#define PORT_VU_IN_2 13
#define PORT_VU_OUT_1 14
#define PORT_VU_OUT_2 15

using namespace sigc;

class MidSideMainWindow : public MainWidget
{
  public:
    MidSideMainWindow(const char *uri, std::string bundlePath, bool isLR2MS);
    virtual ~MidSideMainWindow();   
    
    // Informing GUI about changes in the control ports
    void gui_port_event(LV2UI_Handle ui, uint32_t port, uint32_t buffer_size, uint32_t format, const void * buffer)
    {
      float data = * static_cast<const float*>(buffer);
      
        // Checking if params are the same as specified in the LV2 documentation
        if (format != 0) {
            return;
        }
        if (buffer_size != 4) {
            return;
        }

        // Updating values in GUI ========================================================
        switch (port)
        {
	  case PORT_GAIN_IN_1:
		m_InGain1->set_value(data);
		break;
	  
	  case PORT_GAIN_IN_2:
		m_InGain2->set_value(data);
		break;

	  case PORT_GAIN_OUT_1:
		m_OutGain1->set_value(data);
		break;

	  case PORT_GAIN_OUT_2:
		m_OutGain2->set_value(data);
		break;	  
		
	  case PORT_SOLO_IN_1:
		m_InSolo1.set_active(data > 0.5);
		break;
		
	  case PORT_SOLO_IN_2:
		m_InSolo2.set_active(data > 0.5);
		break;
		
	  case PORT_SOLO_OUT_1:
		m_OutSolo1.set_active(data > 0.5);
		break;
	      
	  case PORT_SOLO_OUT_2:
		m_OutSolo2.set_active(data > 0.5);
		break;  
		
	  case PORT_VU_IN_1:
		m_InputVu->setValue(0,data);
		break;
		
	  case PORT_VU_IN_2:
		m_InputVu->setValue(1,data);
		break;
		
	  case PORT_VU_OUT_1:
		m_OutputVu->setValue(0,data);
		break;
		
	  case PORT_VU_OUT_2:
		m_OutputVu->setValue(1,data);
		break;
        }               
    }

    LV2UI_Controller controller;
    LV2UI_Write_Function write_function;

  protected:
    KnobWidget2 *m_InGain1, *m_InGain2, *m_OutGain1, *m_OutGain2;
    ToggleButton m_InSolo1, m_InSolo2, m_OutSolo1, m_OutSolo2;
    VUWidget *m_InputVu, *m_OutputVu;;
    Gtk::HBox m_HTopBox;
    Gtk::VBox m_VInBox, m_VOutBox;
    Gtk::Image *image_flowChart;
        
    //Signal Handlers
    void onInGain1Change();
    void onInGain2Change();
    void onOutGain1Change();
    void onOutGain2Change();
    void onInSolo1Change();
    void onInSolo2Change();
    void onOutSolo1Change();
    void onOutSolo2Change();
    
    void resetSoloState();
    
  private:
    std::string m_pluginUri;
    std::string m_bundlePath;  
    bool m_bisLR2MS;
};

#endif