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
//
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "ClockEditDlg.h"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(ClockEditDlg, wxDialog)
    EVT_UPDATE_UI(wxID_OK, ClockEditDlg::OnUpdateOk)
END_EVENT_TABLE()

ClockEditDlg::ClockEditDlg( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style )
: wxDialog( parent, id, title, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 2, 2, 0, 0 );

	m_staticText1 = new wxStaticText( this, ID_DEFAULT, wxT("Legth of half period [Ticks]"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_staticText1, 0, wxALL, 5 );

	m_textPeriod = new wxTextCtrl( this, ID_DEFAULT, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator( wxFILTER_NUMERIC ) );
	gSizer1->Add( m_textPeriod, 0, wxALL|wxEXPAND, 5 );

	m_staticText2 = new wxStaticText( this, ID_DEFAULT, wxT("Delay [Ticks]"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_staticText2, 0, wxALL, 5 );

	m_textDelay = new wxTextCtrl( this, ID_DEFAULT, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator( wxFILTER_NUMERIC ) );
	gSizer1->Add( m_textDelay, 0, wxALL|wxEXPAND, 5 );

	bSizer1->Add( gSizer1, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	m_buttonOK = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonOK, 0, wxALL, 5 );

	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonCancel, 0, wxALL, 5 );

	bSizer1->Add( bSizer2, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
}

void ClockEditDlg::OnUpdateOk(wxUpdateUIEvent& event)
{
    long val;
    m_textPeriod->GetValue().ToLong(&val);
    if ( val <= 0 )
    {
        event.Enable(false);
        return;
    }
    m_textDelay->GetValue().ToLong(&val);
    if ( val < 0 )
    {
        event.Enable(false);
        return;
    }
    event.Enable(true);
}
