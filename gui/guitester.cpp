#include "guitester.h"
#include "guiconstants.h"
#include <gtkmm/main.h>
#include <iostream>


//Build
//g++ -g guitester.cpp eqbutton.cpp ctlbutton.cpp pixmapcombo.cpp bandctl.cpp gainctl.cpp eqparams.cpp ../eqwindow.cpp ../dsp/filter.c -o test `pkg-config gtkmm-2.4 slv2 --libs --cflags`



HelloWorld::HelloWorld()
{
 
//   m_EqButton = Gtk::manage(new EQButton(0, &m_iMutex));
//   add(*m_EqButton);
//   m_EqButton->setValue(0.0);
//   m_EqButton->show();
  
   /*m_CtlButton = Gtk::manage(new CtlButton(1));
  add(*m_CtlButton);
  m_CtlButton->setButtonNumber(2);
  m_CtlButton->show();*/
   
   //add(m_combo);
   //m_combo.show();
  
//    m_BandCtl = Gtk::manage(new BandCtl(0, &m_bMutex));
//    add(*m_BandCtl);
//    m_BandCtl->show();
//    
//    m_BandCtl->setEnabled(true);
//    m_BandCtl->setGain(5.0);
//    m_BandCtl->setFreq(500);
//    m_BandCtl->setQ(2.5);
//    m_BandCtl->setFilterType(PEAK);
//    
//    m_BandCtl->signal_changed().connect( sigc::mem_fun(*this, &HelloWorld::onBandChange));

//   m_GainCtl = Gtk::manage(new GainCtl("In gain test", 1, true));
//   add(*m_GainCtl);
//   m_GainCtl->show();
//   
//   m_GainCtl->signal_changed().connect( sigc::mem_fun(*this, &HelloWorld::onGainChange));

  m_EqWin = Gtk::manage(new EqMainWindow(1, 10,"http://eq10q.sourceforge.net/eq/eq10qm"));
  add(*m_EqWin);
  m_EqWin->show();
}

HelloWorld::~HelloWorld()
{
  //delete m_EqButton;
  //delete m_CtlButton;
  //delete m_BandCtl;
  //delete m_GainCtl;
  delete m_EqWin;
}

// void HelloWorld::onBandChange(int iBand, int iField, float fValue)
// {
//   std::cout<<"Band number: "<<iBand<<" Changed: Field = "<<iField<<"\tValue = "<<fValue<<std::endl;
// }

// void HelloWorld::onGainChange(bool bIn, float fGain)
// {
//   std::cout<<"Gain bIN ="<<bIn<<"\tChanged Value = "<<fGain<<std::endl;
// }


int main (int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);

  HelloWorld helloworld;
  //Shows the window and returns when it is closed.
  Gtk::Main::run(helloworld);

  return 0;
}


