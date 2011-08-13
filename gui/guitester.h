#ifndef GTKMM_EXAMPLE_HELLOWORLD_H
#define GTKMM_EXAMPLE_HELLOWORLD_H

#include <gtkmm/window.h>
#include  "eqbutton.h"
#include "pixmapcombo.h"

class HelloWorld : public Gtk::Window
{

public:
  HelloWorld();
  virtual ~HelloWorld();

protected:

  //Member widgets:
  //EQButton *m_EqButton;
  //CtlButton *m_CtlButton;
  PixMapCombo m_combo;
  
  int m_iMutex;
};

#endif // GTKMM_EXAMPLE_HELLOWORLD_H