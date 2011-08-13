#include "guitester.h"
#include <gtkmm/main.h>
#include <iostream>


//Build
//g++ -g guitester.cpp eqbutton.cpp ctlbutton.cpp -o test `pkg-config gtkmm-2.4 --libs --cflags`



HelloWorld::HelloWorld()
{
 
//   m_iMutex = 0;
//   m_EqButton = Gtk::manage(new EQButton(0, &m_iMutex));
//   add(*m_EqButton);
//   m_EqButton->setValue(0.0);
//   m_EqButton->show();
  
   /*m_CtlButton = Gtk::manage(new CtlButton(1));
  add(*m_CtlButton);
  m_CtlButton->setButtonNumber(2);
  m_CtlButton->show();*/
   
   add(m_combo);
   m_combo.show();
  

}

HelloWorld::~HelloWorld()
{
  //delete m_EqButton;
  //delete m_CtlButton;
}



int main (int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);

  HelloWorld helloworld;
  //Shows the window and returns when it is closed.
  Gtk::Main::run(helloworld);

  return 0;
}