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


#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "TimingApp.h"//GetApp
#include "TimeCompressorSettingsPanel.h"
#include "enumers.h"
#include "TimingView.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(TimeCompressorSettingsPanel, wxPanel)
    EVT_UPDATE_UI(TIMING_ID_PANEL_TC_APPLY,    TimeCompressorSettingsPanel::OnUpdatePanelApply)
    EVT_UPDATE_UI(TIMING_ID_PANEL_TC_TXTFIELD, TimeCompressorSettingsPanel::OnUpdateTextField)
    EVT_BUTTON(TIMING_ID_PANEL_TC_APPLY,       TimeCompressorSettingsPanel::OnApply)
END_EVENT_TABLE()

TimeCompressorSettingsPanel::TimeCompressorSettingsPanel
    ( wxWindow* parent )
    : wxPanel( parent, wxID_ANY, wxDefaultPosition, wxSize( 500,300 ), wxTAB_TRAVERSAL ),
    view(NULL)
{
    wxStaticText* staticText;

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );

	staticText = new wxStaticText( this, ID_DEFAULT, wxT("Length of gap [Ticks]"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( staticText, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textTime = new wxTextCtrl( this, TIMING_ID_PANEL_TC_TXTFIELD, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator( wxFILTER_NUMERIC ) );
	fgSizer1->Add( m_textTime, 0, wxALL, 5 );

	bSizer3->Add( fgSizer1, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	m_buttonApply = new wxButton( this, TIMING_ID_PANEL_TC_APPLY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_buttonApply, 0, wxALL/*|wxALIGN_CENTER_HORIZONTAL*/, 5 );

	bSizer3->Add( bSizer4, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );

	this->SetSizer( bSizer3 );
	this->Layout();
	bSizer3->Fit( this );
	instance = this;
}
void TimeCompressorSettingsPanel::OnUpdateTextField(wxUpdateUIEvent& event)
{
    if ( view && view->IsDiscontinuitySelected() )
    {
        event.Enable(true);
        return;
    }
    event.Enable(false);
}
void TimeCompressorSettingsPanel::OnApply(wxCommandEvent &event)
{
    SetUnmodified();

    long val;
    m_textTime->GetValue().ToLong(&val);

    if ( view ) view->SetTimeCompressor(val);
}
void TimeCompressorSettingsPanel::OnUpdatePanelApply(wxUpdateUIEvent& event)
{
    if ( view && view->IsDiscontinuitySelected() )
    {
        long val;
        m_textTime->GetValue().ToLong(&val);
        if ( val <= 1 )
        {
            event.Enable(false);
            return;
        }
        if ( ! ( m_textTime->IsModified() ) )
        {
            event.Enable(false);
            return;
        }
        event.Enable(true);
        return;
    }
    event.Enable(false);
}

TimeCompressorSettingsPanel* TimeCompressorSettingsPanel::instance = NULL;
TimeCompressorSettingsPanel *TimeCompressorSettingsPanel::GetInstance()
{
    return instance;
}

TimeCompressorSettingsPanel::~TimeCompressorSettingsPanel()
{
    if (instance == this)
        instance = NULL;
}
