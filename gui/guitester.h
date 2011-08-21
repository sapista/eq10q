#ifndef GTKMM_EXAMPLE_HELLOWORLD_H
#define GTKMM_EXAMPLE_HELLOWORLD_H

#include <gtkmm/window.h>
//#include  "eqbutton.h"
//#include "pixmapcombo.h"
//#include "bandctl.h"
//#include "gainctl.h"
#include "../eqwindow.hh"

class HelloWorld : public Gtk::Window
{

public:
  HelloWorld();
  virtual ~HelloWorld();
  //void onBandChange(int iBand, int iField, float fValue);
  //void onGainChange(bool bIn, float fGain);

protected:

  //Member widgets:
  //EQButton *m_EqButton;
  //CtlButton *m_CtlButton;
  //PixMapCombo m_combo;
  //BandCtl *m_BandCtl;
  //GainCtl *m_GainCtl;
  EqMainWindow *m_EqWin;
  
  //bool m_bMutex;
};

#endif // GTKMM_EXAMPLE_HELLOWORLD_H