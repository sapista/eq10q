#ifndef GTKMM_EXAMPLE_HELLOWORLD_H
#define GTKMM_EXAMPLE_HELLOWORLD_H

//#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include  "eqbutton.h"

class HelloWorld : public Gtk::Window
{

public:
  HelloWorld();
  virtual ~HelloWorld();

protected:
  //Signal handlers:
  //void on_button_clicked();

  //Member widgets:
  //Gtk::Button m_button;
  EQButton *m_EqButton;
  int m_iMutex;
};

#endif // GTKMM_EXAMPLE_HELLOWORLD_H