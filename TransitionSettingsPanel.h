//    Copyright 2008 Daniel Anselmi
//
//    This file is part of TimingEditor.
//
//    TimingEditor is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    TimingEditor is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with TimingEditor.  If not, see <http://www.gnu.org/licenses/>.
//
//    Contact e-mail: daniel anselmi <danselmi@gmx.ch>
//    Program URL   : http://sourceforge.net/projects/timingeditor/
//
//
///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 14 2006)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __TransitionSettingsPanel__
#define __TransitionSettingsPanel__

// Define WX_GCH in order to support precompiled headers with GCC compiler.
// You have to create the header "wx_pch.h" and include all files needed
// for compile your gui inside it.
// Then, compile it and place the file "wx_pch.h.gch" into the same
// directory that "wx_pch.h".
#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#include <wx/button.h>
#include <wx/spinctrl.h>

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default

/**
 * Class MyPanel1
 */
class TimingWindow;
class TransitionSettingsPanel : public wxPanel
{
    public:
        void SetTransitionWidth(wxInt8 tw){m_spinCtrl1->SetValue(tw);}
        void Set5090(bool val){m_checkBox1->SetValue(val);}
        //wxInt8 GetTransitionWidth(){return m_spinCtrl1->GetValue();}
        //bool Get5090(){return m_checkBox1->GetValue();}

    public:
        TimingWindow *wnd;

	private:
        void OnUpdatePanelApply(wxUpdateUIEvent &event);
        void OnUpdateControls(wxUpdateUIEvent &event);
        void OnApply(wxCommandEvent &event);

	protected:
		wxStaticText* m_staticText1;
		wxSpinCtrl* m_spinCtrl1;
		wxCheckBox* m_checkBox1;
		wxButton* m_button4;

	public:
		TransitionSettingsPanel( wxWindow* parent, int id = -1, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 500,132 ), int style = wxTAB_TRAVERSAL );
    DECLARE_EVENT_TABLE()
};

#endif //__noname__

