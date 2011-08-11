#include "helloworld.h"
#include <gtkmm/main.h>
#include <iostream>

HelloWorld::HelloWorld()
: m_button("Hello World")   // creates a new button with label "Hello World".
{
  // Sets the border width of the window.
  set_border_width(10);

  /*
  // When the button receives the "clicked" signal, it will call the
  // on_button_clicked() method defined below.
  m_button.signal_clicked().connect(sigc::mem_fun(*this,
              &HelloWorld::on_button_clicked));

  // This packs the button into the Window (a container).
  add(m_button);
  */
  
  m_iMutex = 0;
  m_EqButton = Gtk::manage(new EQButton(0, &m_iMutex));
  add(*m_EqButton);

  // The final step is to display this newly created widget...
  //m_button.show();
}

HelloWorld::~HelloWorld()
{
	delete m_EqButton;
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