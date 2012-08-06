//TEST EQ GUI alone:


#include <gtkmm/main.h>
#include "eqwindow.hh"



int main(int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);
  EqMainWindow eqwin;
Gtk::Main:run(eqwin);

return 0;
}

//g++ testGui.cpp eqwindow.cpp gui/pixmapcombo.cpp gui/ctlbutton.cpp  gui/band_ctl.cpp  `pkg-config --cflags --libs gtkmm-2.4`  -o test_gui
