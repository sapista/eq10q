#include "guitester.h"
#include "widgets/guiconstants.h"

#include <iostream>
#include <cmath>
#include "lv2_ui.h"

//Build
//g++ -g guitester.cpp eqbutton.cpp ctlbutton.cpp pixmapcombo.cpp bandctl.cpp gainctl.cpp eqparams.cpp faderwidget.cpp vuwidget.cpp ../eqwindow.cpp ../dsp/filter.c -o test `pkg-config gtkmm-2.4 slv2 --libs --cflags`

void writeTestFunction(LV2UI_Controller controller, uint32_t port_index, uint32_t buffer_size, uint32_t format, const void* buffer)
{  
  float data = *(float*)buffer;
  std::string sPort;
  const int m_iNumOfChannels = 2;
  const int m_iNumOfBands = 10;
  
  // Updating values in GUI ========================================================
	switch (port_index)
	{
	  case EQ_BYPASS:
	    sPort = "Bypass";
	  break;

	  case EQ_INGAIN:
	    sPort = "In Gain";
	  break;

	  case EQ_OUTGAIN:
	    sPort = "Out Gain";
	  break;

	  default:
	    //Connect BandGain ports
	    if((int)port_index >= (PORT_OFFSET + 2*m_iNumOfChannels) && (int)port_index < (PORT_OFFSET + 2*m_iNumOfChannels + m_iNumOfBands))
	    {
	      sPort = "Band Gain ";
	    }

	    //Connect BandFreq ports
	    else if((int)port_index >= (PORT_OFFSET + 2*m_iNumOfChannels + m_iNumOfBands) && (int)port_index < (PORT_OFFSET + 2*m_iNumOfChannels + 2*m_iNumOfBands))
	    {
	      sPort = "Band Freq ";
	    }

	    //Connect BandParam ports
	    else if((int)port_index >= (PORT_OFFSET + 2*m_iNumOfChannels + 2*m_iNumOfBands) && (int)port_index < (PORT_OFFSET + 2*m_iNumOfChannels + 3*m_iNumOfBands))
	    {
	      sPort = "Band Q ";
	    }

	    //Connect BandType ports
	    else if((int)port_index >= (PORT_OFFSET + 2*m_iNumOfChannels + 3*m_iNumOfBands) && (int)port_index < (PORT_OFFSET + 2*m_iNumOfChannels + 4*m_iNumOfBands))
	    {
	      sPort = "Band Type ";
	    }

	    //Connect BandEnabled ports
	    else if((int)port_index >= (PORT_OFFSET + 2*m_iNumOfChannels + 4*m_iNumOfBands) && (int)port_index < (PORT_OFFSET + 2*m_iNumOfChannels + 5*m_iNumOfBands))
	    {
 	      sPort = "Band Enabled ";
	    }

	    //Connect VuInput ports
	    else if((int)port_index >= (PORT_OFFSET + 2*m_iNumOfChannels + 5*m_iNumOfBands) && (int)port_index < (PORT_OFFSET + 2*m_iNumOfChannels + 5*m_iNumOfBands + m_iNumOfChannels))
	    {
 	      sPort = "Vu Input";
	    }

	    //Connect VuOutput ports
	    else if((int)port_index >= (PORT_OFFSET + 2*m_iNumOfChannels + 5*m_iNumOfBands + m_iNumOfChannels) && (int)port_index < (PORT_OFFSET + 2*m_iNumOfChannels + 5*m_iNumOfBands + 2*m_iNumOfChannels))
	    {
	      sPort = "Vu Output";
	    }
	    
	    //Unknown port
	    else
	    {
	      sPort = "UNKNOWN";
	    }
	  break;
	}       
  
  
  std::cout<<"LV2 Write function tester: Port = "<< sPort <<" Data = "<<data<<std::endl;
}

HelloWorld::HelloWorld()
{
  m_EqWin = Gtk::manage(new EqMainWindow(2, 10,"http://eq10q.sourceforge.net/eq/eq10qs",@Eq_Gui_Test_Path@));
  m_GateWin =  Gtk::manage(new GateMainWindow("http://eq10q.sourceforge.net/eq/eq10qs",@Eq_Gui_Test_Path@));
  //add(*m_EqWin);
  m_hbox.pack_start(*m_EqWin);
  m_hbox.pack_start(*m_GateWin);
  add(m_hbox);

  show_all_children();
  
  //Signals connections
  ///TODO REMOVE
  /********************************************************************************************************
  m_EqWin->signal_Bypass_Changed().connect(sigc::mem_fun(*this, &HelloWorld::on_BypassChanged));
  m_EqWin->signal_InputGain_Changed().connect(sigc::mem_fun(*this, &HelloWorld::on_InputGainChanged));
  m_EqWin->signal_OutputGain_Changed().connect(sigc::mem_fun(*this, &HelloWorld::on_OutputGainChanged));
  
  m_EqWin->signal_BandGain_Changed().connect(sigc::mem_fun(*this, &HelloWorld::on_BandGainChanged));
  m_EqWin->signal_BandFreq_Changed().connect(sigc::mem_fun(*this, &HelloWorld::on_BandFreqChanged));
  m_EqWin->signal_BandQ_Changed().connect(sigc::mem_fun(*this, &HelloWorld::on_BandQChanged));
  m_EqWin->signal_BandType_Changed().connect(sigc::mem_fun(*this, &HelloWorld::on_BandTypeChanged));
  m_EqWin->signal_BandEnabled_Changed().connect(sigc::mem_fun(*this, &HelloWorld::on_BandEnabledChanged));
  **********************************************************************************************************/
  
  //Prepare writefunction
  m_EqWin->write_function = writeTestFunction;
  
}

HelloWorld::~HelloWorld()
{
 delete m_EqWin;
}

void HelloWorld::on_realize()
{
  Gtk::Widget::on_realize();
    
  //Initialize all
  ///TODO REMOVE
  float data;
  for( int i = 0; i < 10; i++)
  {
    data = 0;
    m_EqWin->gui_port_event(NULL, PORT_OFFSET + 2*2 + 4*10 + i, 4, 0, &data);
  }
}


//Slots signal handlers
void HelloWorld::on_BypassChanged(bool bypass)
{
  //std::cout<<"Bypass = "<<bypass<<std::endl;
}

void HelloWorld::on_InputGainChanged(float gain)
{
  //std::cout<<"In Gain = "<<gain<<std::endl;
}

void HelloWorld::on_OutputGainChanged(float gain)
{
  //std::cout<<"Out Gain = "<<gain<<std::endl;
}

void HelloWorld::on_BandGainChanged(int band, float gain)
{
  //std::cout<<"Band "<<band<<" Gain = "<<gain<<std::endl;
}

void HelloWorld::on_BandFreqChanged(int band, float freq)
{
  //std::cout<<"Band "<<band<<" Freq = "<<freq<<std::endl;
}

void HelloWorld::on_BandQChanged(int band, float q)
{
  //std::cout<<"Band "<<band<<" Q = "<<q<<std::endl; 
}

void HelloWorld::on_BandTypeChanged(int band, int type)
{
  //std::cout<<"Band "<<band<<" Type = "<<type<<std::endl;  
}

void HelloWorld::on_BandEnabledChanged(int band, bool enabled)
{
  //std::cout<<"Band "<<band<<" Enabled = "<<enabled<<std::endl;  
}




