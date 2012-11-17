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

#include <gtkmm/box.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/alignment.h>

#include "ctlbutton.h"
#include "filter.h"

class EQButton : public Gtk::VBox{
  public:
    EQButton(int iType, bool *bSemafor);
    virtual ~EQButton();
    virtual void setValue(float fVal);
    virtual float getValue();
       
    //signal accessor: 
    typedef sigc::signal<void> signal_EqButtonChanged;
    signal_EqButtonChanged signal_changed();

    typedef sigc::signal<void, bool> signal_EqButtonSpinState;
    signal_EqButtonSpinState spinState_changed();
    
  protected:
    Gtk::Alignment m_ButtonAlign;
    Gtk::SpinButton m_TextEntry;
    CtlButton *m_ptr_CtlButton;

    //GTK signal handlers
    virtual void onEnterPressed();
    virtual void onButtonDoubleClicked();
    virtual void onSpinChange();
    virtual void onCtlButtonChanged();
    
  private:
    FilterType m_FilterType;
    float m_fValue;
    bool *m_bStop; //This is muttual exclusion pointer. This pointer have to be common to all syncronitzated buttons in order to avoid multiple textEntry at the same time.
    bool m_bTextEntryMode;
    
    //EQButton change signal
    signal_EqButtonChanged m_EqButtonChangedSignal;
    signal_EqButtonSpinState m_EqButtonSpinState;
};
#endif