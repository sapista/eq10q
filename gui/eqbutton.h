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

#ifndef EQ_BUTTON_H
	#define EQ_BUTTON_H
	
#include <iostream>
#include <iomanip>
#include <cmath>

#include <gtkmm/box.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/alignment.h>

class EQButton : public Gtk::VBox{
  public:
    EQButton(int iType, float *fPtr, sigc::slot<void> slot, int *iSemafor);
    virtual ~EQButton();
    virtual void setValue(float fVal);
    virtual float getValue();
    virtual void hideSpin();
    virtual void setSpinNumber();
    
  protected:
    Gtk::Alignment m_ButtonAlign;
    Gtk::SpinButton m_TextEntry;
	
	///TODO: Pq un punter a un objecte de nivell superior? Aixo sembla que no te sentit
    CtlButton *m_ptr_CtlButton;

	//GTK signal handlers
    virtual bool onButtonDoubleClicked(GdkEventButton* event);
    virtual void onEnterPressed();
    virtual void onSpinChange();
    
  private:
    int m_iFilterType;
    float m_fValue, *m_ptr_f; //punter a on guardem el valor ///TODO: Que fa aquest punter
    int *m_iStop; ///TODO: Que es quest STOP???
};
#endif