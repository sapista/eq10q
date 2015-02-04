/***************************************************************************
 *   Copyright (C) 2015 by Pere Ràfols Soler                               *
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
 
#include "toggle_button.h"

#define OUTER_BORDER 2 
 
ToggleButton::ToggleButton ( const Glib::ustring& label )
:Button ( label ),
m_bActive(false)
{

}

ToggleButton::~ToggleButton()
{

}

bool ToggleButton::on_button_release_event ( GdkEventButton* event )
{
  if( event->x > OUTER_BORDER &&
    event->x < (width - OUTER_BORDER) &&
    event->y > OUTER_BORDER &&
    event->y < (height - OUTER_BORDER))
  {
    m_bActive = !m_bActive;
    m_sigClick.emit();  
  }
  
  if(m_bActive) return true;
  
  //Coords captured after the event cause mouse can change position during the event
  int x, y;
  get_pointer(x,y);
  if(x > OUTER_BORDER &&
     x < (width - OUTER_BORDER) &&
     y > OUTER_BORDER &&
     y < (height - OUTER_BORDER))
  {
    m_state = Button::FOCUS;
  }
  else
  {
    m_state = Button::RELEASE;
  }

  redraw();
  return true;
}

bool ToggleButton::on_mouse_leave_widget ( GdkEventCrossing* event )
{
  if(m_state == Button::FOCUS || m_state == Button::FOCUS_PRESS)
  {
    if(m_bActive)
    {
      m_state = Button::PRESS;
    }
    else
    {
      m_state = Button::RELEASE;
    }
    redraw();
  }
  return true;
}

bool ToggleButton::get_active()
{
  return m_bActive;
}

void ToggleButton::set_active ( bool active )
{
  m_bActive = active;
  if(m_bActive)
  {
    m_state = Button::PRESS;
  }
  else
  {
    m_state = Button::RELEASE;
  }
  redraw();
}

