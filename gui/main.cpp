#include <iostream>
#include "guitester.h"
#include <gtkmm/main.h>
#include <gtkmm/rc.h>


#define GTKRC_THEME "/usr/share/themes/Industrial/gtk-2.0/gtkrc"


int main (int argc, char *argv[])
{ 
  Gtk::Main kit(argc, argv);

  HelloWorld helloworld;
  
  //Override the gtk theme provided by host to eq10q-theme
  /*
  Gtk::RC::parse_string(GTKRC_THEME);
  Gtk::RC::add_default_file(GTKRC_THEME);
  bool bResult = Gtk::RC::reparse_all();
  */
  
  //Shows the window and returns when it is closed.
  Gtk::Main::run(helloworld);

  return 0;
}