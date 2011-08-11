/***************************************************************************
 *   Copyright (C) 2009 by Pere Ràfols Soler                               *
 *   sapista2@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef CTL_BUTTON_H
	#define CTL_BUTTON_H
	
#include <iostream>
#include <iomanip>
#include <cmath>

#include <gtkmm/button.h>

#include "eqbutton.h"

#define ACCELERATION 15
#define GAIN_TYPE 0 ///TODO: Aquest define replica codi, i veig venir que ho fara a totes les classes!!!
#define FREQ_TYPE 1
#define Q_TYPE    2

class CtlButton : public Gtk::Button{
  public:
    CtlButton(int iType); ///TODO: PQ tenim un punter MouseValue des de constructor?
    virtual ~CtlButton();
	
	///TODO: verificar que les funcions setPress()  setDepress() no fan falta
    //void setPress();
    //void setDepress();
	
    void setButtonNumber(float num);
	float getButtonNumber(); ///TODO: Crec que falta un funcio per configurar el valor des d'un objecte exterior
    
  protected:
    virtual void onButtonPressed();
    virtual void onButtonDepressed();
    virtual bool onMouseMove(GdkEventMotion* event);
    virtual float computeValue(int x, int y);
  
  private:  
    bool /*m_bPress,*/ m_bIsXDirection, m_bFirstTime; ///TODO: si realment es pot, eliminar m_bPress
    int  m_iActValue, m_iAntValue, m_iFilterType;
    float  m_fValue;
    //EQButton *eqbutton_ref_m_iPtr; ///TODO: Punter a EQButton, no ho veig clar que aixo vingui per constructor, investigar si te sentit -> Aquest punter no s'usa enlloc!!!
};

#endif