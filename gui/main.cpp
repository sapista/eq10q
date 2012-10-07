#include <iostream>
#include "guitester.h"
#include <gtkmm/main.h>

#define GTKRC_THEME "/usr/share/themes/Industrial/gtk-2.0/gtkrc"


int main (int argc, char *argv[])
{ 
  Gtk::Main kit(argc, argv);

  HelloWorld helloworld;
    
  //Shows the window and returns when it is closed.
  Gtk::Main::run(helloworld);

  return 0;
}