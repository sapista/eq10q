#include "guitester.h"
#include "guiconstants.h"
#include <gtkmm/main.h>
#include <iostream>
#include <cmath>

//Build
//g++ -g guitester.cpp eqbutton.cpp ctlbutton.cpp pixmapcombo.cpp bandctl.cpp gainctl.cpp eqparams.cpp vuwidget.cpp ../eqwindow.cpp ../dsp/filter.c -o test `pkg-config gtkmm-2.4 slv2 --libs --cflags`



HelloWorld::HelloWorld()
{
    //Test Band CtlWidget
    m_BandCtl = Gtk::manage(new BandCtl(0, &m_bMutex));
    m_BandCtl->setEnabled(true);
    m_BandCtl->setGain(5.0);
    m_BandCtl->setFreq(500);
    m_BandCtl->setQ(2.5);
    m_BandCtl->setFilterType(PEAK);
    m_BandCtl->signal_changed().connect( sigc::mem_fun(*this, &HelloWorld::onBandChange));

    m_GainCtl = Gtk::manage(new GainCtl("In gain test", 1, true));
    m_GainCtl->signal_changed().connect( sigc::mem_fun(*this, &HelloWorld::onGainChange));

//   m_EqWin = Gtk::manage(new EqMainWindow(1, 10,"http://eq10q.sourceforge.net/eq/eq10qm"));
//   add(*m_EqWin);
//   m_EqWin->show();

    m_ScaleL.set_digits(1);
    m_ScaleL.set_draw_value(true);
    m_ScaleL.set_value_pos(Gtk::POS_TOP);
    m_ScaleL.set_inverted(true);
    m_ScaleL.set_range(-50.0, 10.0);
    m_ScaleL.set_value(0.0);
    m_ScaleL.signal_value_changed().connect(sigc::mem_fun(*this, &HelloWorld::onGainLChanged));
    
    m_ScaleR.set_digits(1);
    m_ScaleR.set_draw_value(true);
    m_ScaleR.set_value_pos(Gtk::POS_TOP);
    m_ScaleR.set_inverted(true);
    m_ScaleR.set_range(-50.0, 10.0);
    m_ScaleR.set_value(0.0);
    m_ScaleR.signal_value_changed().connect(sigc::mem_fun(*this, &HelloWorld::onGainRChanged));
    
    m_Vu = Gtk::manage(new VUWidget(2));
    
    m_FaderTest = Gtk::manage(new FaderWidget(15,-15));
    
    m_Box.pack_start(*m_FaderTest);
    m_Box.pack_start(*m_GainCtl);
    m_Box.pack_start(*m_BandCtl);
    m_Box.pack_start(m_ScaleL);
    m_Box.pack_start(m_ScaleR);
    m_Box.pack_start(*m_Vu);
    add(m_Box);
    m_ScaleL.show();
    m_ScaleR.show();
    m_Vu->show();
    m_GainCtl->show();
    m_Box.show();
    m_FaderTest->show();
    //show_all_children();
}

void HelloWorld::onGainLChanged()
{
  m_Vu->setValue(0, pow(10.0,(float)m_ScaleL.get_value()/20.0));
}

void HelloWorld::onGainRChanged()
{
  m_Vu->setValue(1, pow(10.0,(float)m_ScaleR.get_value()/20.0));
}

HelloWorld::~HelloWorld()
{
  delete m_BandCtl;
  delete m_GainCtl;
  delete m_Vu;
}

void HelloWorld::onBandChange(int iBand, int iField, float fValue)
{
  std::cout<<"Band number: "<<iBand<<" Changed: Field = "<<iField<<"\tValue = "<<fValue<<std::endl;
}

void HelloWorld::onGainChange(bool bIn, float fGain)
{
   std::cout<<"Gain bIN ="<<bIn<<"\tChanged Value = "<<fGain<<std::endl;
   
   //TESTING  THE FADER WIDGET
   m_FaderTest->set_value(fGain);
}


int main (int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);

  HelloWorld helloworld;
  //Shows the window and returns when it is closed.
  Gtk::Main::run(helloworld);

  return 0;
}


