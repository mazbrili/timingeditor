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
///////////////////////////////////////////////////////////////////////////

#ifndef __noname__
#define __noname__

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

/**
 * Class MyDialog1
 */
class ClockEditDlg : public wxDialog
{
	private:
        void OnUpdateOk(wxUpdateUIEvent& event);

	protected:
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textPeriod;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textDelay;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
    public:
    void SetTextPeriod(wxString str){m_textPeriod->SetValue(str);}
    void SetTextDelay(wxString str){m_textDelay->SetValue(str);}
    wxString GetTextPeriod(void){return m_textPeriod->GetValue();}
    wxString GetTextDelay(void){return m_textDelay->GetValue();}


	public:
		ClockEditDlg( wxWindow* parent, int id = -1, wxString title = wxT(""), wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize, int style = wxDEFAULT_DIALOG_STYLE );
private:
    DECLARE_EVENT_TABLE()
};

#endif //__noname__
