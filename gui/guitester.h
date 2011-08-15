#ifndef GTKMM_EXAMPLE_HELLOWORLD_H
#define GTKMM_EXAMPLE_HELLOWORLD_H

#include <gtkmm/window.h>
//#include  "eqbutton.h"
//#include "pixmapcombo.h"
#include "bandctl.h"

class HelloWorld : public Gtk::Window
{

public:
  HelloWorld();
  virtual ~HelloWorld();

protected:

  //Member widgets:
  //EQButton *m_EqButton;
  //CtlButton *m_CtlButton;
  //PixMapCombo m_combo;
  BandCtl *m_BandCtl;
    
  bool m_bMutex;
};

#endif // GTKMM_EXAMPLE_HELLOWORLD_H