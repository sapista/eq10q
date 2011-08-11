#include "guitester.h"
#include <gtkmm/main.h>
#include <iostream>

HelloWorld::HelloWorld()
{


  
  /*
  m_iMutex = 0;
  m_EqButton = Gtk::manage(new EQButton(0, &m_iMutex));
  add(*m_EqButton);
  m_EqButton->setValue(0.0);
  m_EqButton->show();*/
  
   m_CtlButton = Gtk::manage(new CtlButton(1));
  add(*m_CtlButton);
  m_CtlButton->setButtonNumber(2);
  m_CtlButton->show();
  

}

HelloWorld::~HelloWorld()
{
  //delete m_EqButton;
  delete m_CtlButton;
}

/*
void HelloWorld::on_button_clicked()
{
  std::cout << "Hello World" << std::endl;
}
*/


int main (int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);

  HelloWorld helloworld;
  //Shows the window and returns when it is closed.
  Gtk::Main::run(helloworld);

  return 0;
}