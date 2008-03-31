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

#ifndef __TimeCompressorPanel__
#define __TimeCompressorPanel__

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

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default
class TimingWindow;
class TimeCompressorSettingsPanel : public wxPanel
{
	private:
        void OnUpdatePanelApply(wxUpdateUIEvent& event);
        void OnUpdateTextField(wxUpdateUIEvent& event);
        void OnApply(wxCommandEvent &event);
        //void OnDelete(wxCommandEvent &event);

    public:
        TimingWindow *wnd;


	protected:
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textTime;
		wxButton* m_buttonApply;
    public:
        void SetTimeText(const wxString &str){m_textTime->SetValue(str);}
        void SetUnmodified(){m_textTime->SetModified(false);}

	public:
		TimeCompressorSettingsPanel( wxWindow* parent, int id = -1, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 500,300 ), int style = wxTAB_TRAVERSAL );
	DECLARE_EVENT_TABLE()
};

#endif //__TimeCompressorPanel__

