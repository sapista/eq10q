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

#ifndef BAND_CTL_H
  #define BAND_CTL_H
  

#include "filter.h"
#include <gtkmm/drawingarea.h>
#include <gdkmm/pixbuf.h>
#include <glibmm/refptr.h>
#include <cairomm/surface.h>
#include <gtkmm/menu.h>
#include <gtkmm/image.h>

class BandCtl : public Gtk::DrawingArea
{
  public:
    BandCtl(const int iBandNum,bool *bSemafor, const char* bundlepath);
    
    virtual ~BandCtl();
    float getGain();
    float getFreq();
    float getQ();
    float getFilterType();
    bool getEnabled();
    
    void setGain(float fGain);
    void setFreq(float fFreq);
    void setQ(float fQ);
    void setFilterType(float fType);
    void setEnabled(bool bIsEnabled);
    void glowBand(bool glow);
    
    //signal accessor: 
	//Parameters:
	//int -> BandNumber
	//int -> field(gain, freq, Q, type, ON/OFF)
	//Float -> value
    typedef sigc::signal<void, int, int, float> signal_ctlBandChanged;
    signal_ctlBandChanged signal_changed();
    
    typedef sigc::signal<void, int> signal_BandSelected;
    signal_BandSelected signal_band_selected();
    
    typedef sigc::signal<void> signal_BandUnSelected;
    signal_BandUnSelected signal_band_unselected();
        
  protected:
    
    //Signal Handlers
    virtual void on_menu_lpf();
    virtual void on_menu_hpf();
    virtual void on_menu_loshelf();
    virtual void on_menu_hishelf();
    virtual void on_menu_peak();
    virtual void on_menu_notch();
    virtual void on_menu_hide();
    
    //Mouse grab signal handlers
    virtual bool on_button_press_event(GdkEventButton* event);
    virtual bool on_button_release_event(GdkEventButton* event);
    virtual bool on_scrollwheel_event(GdkEventScroll* event);
    virtual bool on_mouse_motion_event(GdkEventMotion* event);
    virtual bool on_mouse_leave_widget(GdkEventCrossing* event);
    virtual bool on_key_press_event(GdkEventKey* event);
    virtual bool on_focus_out_event(GdkEventFocus* event);
    virtual void redraw();

    //Override default signal handler:
    virtual bool on_expose_event(GdkEventExpose* event);
    
    //Keyboard signal handler
    sigc::connection  keyPressEvent;
        
  private:
    
    //Internal structur representing a button in Gtk::DrawingArea
    struct Button 
    {
      double x0, y0, x1, y1;
      bool focus;
      bool pressed;
      bool text; //Entring text  with keyboard to this widget
      std::stringstream ss;
      float value;
      std::string units;
      float max, min;
    };
    bool m_bBtnInitialized;
    
    Button m_EnableBtn, m_TypeBtn, m_GainBtn, m_FreqBtn, m_QBtn;   
    Gtk::Menu* m_TypePopUp;   
    Gtk::Image *icon_lpf, *icon_hpf, *icon_loShel, *icon_hiShel, *icon_peak, *icon_notch;
    Gtk::ImageMenuItem *itm_lpf, *itm_hpf, *itm_loShel, *itm_hiShel, *itm_peak, *itm_notch;
    FilterType m_FilterType;
    int m_iBandNum;
    bool m_bBandIsEnabled;
    std::string m_budlepath;
    Glib::ustring m_BandTitle;
    int width, height;
    int m_iAntValueX, m_iAntValueY;
    Gdk::Color m_Color;
    int m_HpfLpf_slope;
    bool m_bGlowBand;
    
    Glib::RefPtr<Gdk::Pixbuf> m_img_ptr_lpf, m_img_ptr_hpf, m_img_ptr_loshelf, m_img_ptr_hishelf, m_img_ptr_peak, m_img_ptr_notch;
    Cairo::RefPtr<Cairo::ImageSurface> m_image_surface_ptr;    
    Cairo::RefPtr< Cairo::Context> m_image_context_ptr;
    void loadTypeImg();
    bool parseBtnString(Button *btn);
    void drawBandButton(Button *btn, Cairo::RefPtr<Cairo::Context> cr);
    void setFilterTypeLPFHPFAcordSlope();
    
    //Band change signal
    signal_ctlBandChanged m_bandChangedSignal;
    signal_BandSelected m_bandSelectedSignal;
    signal_BandUnSelected m_bandUnSelectedSignal;
};
#endif

