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

#ifndef GATE_MAIN_WIN_H
  #define GATE_MAIN_WIN_H

#include <iostream>
#include <string>

#include <gtkmm/eventbox.h>
#include <gtkmm/alignment.h>
#include <gtkmm/box.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/button.h>
#include <gtkmm/frame.h>
#include <gtkmm/image.h>

#include <cmath>

//LV2 UI header
#include "../lv2_ui.h"

#include "vuwidget.h"
#include "faderwidget.h"

//Test print information, comment out for the final release
//#define PRINT_DEBUG_INFO

using namespace sigc;

class GateMainWindow : public Gtk::EventBox
{
  public:
    GateMainWindow(const char *uri, const char *bundlePath);
    virtual ~GateMainWindow();   
    
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
	  case 0:
	    //TODO A completar amb tots els ports del GATE
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
    FaderWidget *m_ThresholdFader;
    Gtk::HBox m_VuBox;
    
    //Signal Handlers
    void onThresholdChange();
    void onRealize();
    
  private:
    std::string m_pluginUri;
    std::string m_bundlePath;    
    
    ///TODO: I'm here!
    ///Definicio preliminar de gatewindow.h practicament completada
    ///Pendent de probar que la implementacio nova de Vu funciona OK amb Gain reduction
    ///Pendent de implementar inversió de numeros a Vu
    ///Millorant flexibilitat de VU
    ///Calibracio dels VU es incorrecte! 0 dB no quadren amb 0 dB!!!
};

#endif
