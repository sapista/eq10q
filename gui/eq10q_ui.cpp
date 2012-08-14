/***************************************************************************
 *   Copyright (C) 2011 by Pere Ràfols Soler                               *
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

/***************************************************************************
This file contains some definitions of the EQ plugin UI
This plugin is inside the Sapista Plugins Bundle
****************************************************************************/

//LV2 UI header
#include <slv2/lv2_ui.h>

#include <gtkmm/main.h>

#include "widgets/eqwindow.h"
#include "eqconfig.h"

static LV2UI_Descriptor *eq10q_guiDescriptor = NULL;

static LV2_Handle instantiateEq10q_gui(const _LV2UI_Descriptor *descriptor, const char *plugin_uri, const char *bundle_path, LV2UI_Write_Function write_function, LV2UI_Controller controller, LV2UI_Widget *widget, const LV2_Feature *const *features)
{
  Gtk::Main::init_gtkmm_internals();
  
  EqMainWindow* gui_data = Gtk::manage(new EqMainWindow(EQ_CHANNELS_COUNT, EQ_BANDS_COUNT, plugin_uri));
  gui_data->controller = controller;
  gui_data->write_function = write_function;

  *widget = gui_data->gobj();

  return (LV2_Handle)gui_data;
}


static void cleanupEq10q_gui(LV2UI_Handle instance)
{
  delete static_cast<EqMainWindow*>(instance);
}

static void portEventEq10q_gui(LV2UI_Handle ui, uint32_t port_index, uint32_t buffer_size, uint32_t format, const void *buffer)
{
  EqMainWindow *gui = (EqMainWindow *)ui;
  gui->gui_port_event(ui, port_index, buffer_size, format, buffer);
}


static void init_gui()
{
  eq10q_guiDescriptor = (LV2UI_Descriptor *)malloc(sizeof(LV2UI_Descriptor));
  eq10q_guiDescriptor->URI = EQ_URI;
  eq10q_guiDescriptor->instantiate = instantiateEq10q_gui;
  eq10q_guiDescriptor->cleanup = cleanupEq10q_gui;
  eq10q_guiDescriptor->port_event = portEventEq10q_gui;
  eq10q_guiDescriptor->extension_data = NULL;
}

//LV2_SYMBOL_EXPORT
const LV2UI_Descriptor *lv2ui_descriptor(uint32_t index)
{
    if (!eq10q_guiDescriptor) { init_gui(); }

    switch (index) {
	    case 0:
		    return eq10q_guiDescriptor;
	    default:
		    return NULL;
    }
}

 
