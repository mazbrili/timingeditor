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

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "TimingApp.h"//GetApp
#include "ClockSettingsPanel.h"
#include "enumers.h"
#include "TimingView.h"
#include "TimingWindow.h"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(ClockSettingsPanel, wxPanel)
    EVT_UPDATE_UI(TIMING_ID_PANEL_CLK_APPLY,  ClockSettingsPanel::OnUpdatePanelClkApply)
    EVT_UPDATE_UI_RANGE(TIMING_ID_PANEL_CLK_TXTPERIOD, TIMING_ID_PANEL_CLK_TXTDELAY,
        ClockSettingsPanel::OnUpdateTextFields)
    EVT_BUTTON(TIMING_ID_PANEL_CLK_APPLY, ClockSettingsPanel::OnApply)
END_EVENT_TABLE()


void ClockSettingsPanel::SetUnmodified()
{
    m_textDelay->SetModified(false);
    m_textPeriod->SetModified(false);
}
void ClockSettingsPanel::OnUpdateTextFields(wxUpdateUIEvent& event)
{
    if ( wnd && wnd->IsSignalSelected() && wnd->IsSelectedSignalClock() )
    {
        event.Enable(true);
        return;
    }
    event.Enable(false);
}
void ClockSettingsPanel::OnApply(wxCommandEvent &event)
{
    SetUnmodified();

    long valp, vald;
    m_textPeriod->GetValue().ToLong(&valp);
    if ( valp <= 0 ) return;
    m_textDelay->GetValue().ToLong(&vald);
    if ( vald < 0 ) return;
    if ( wnd ) wnd->SetClock(vald, valp);
}
void ClockSettingsPanel::OnUpdatePanelClkApply(wxUpdateUIEvent& event)
{
    if ( wnd && wnd->IsSignalSelected() && wnd->IsSelectedSignalClock() )
    {
        long valp, vald;
        m_textPeriod->GetValue().ToLong(&valp);
        if ( valp <= 0 )
        {
            event.Enable(false);
            return;
        }
        m_textDelay->GetValue().ToLong(&vald);
        if ( vald < 0 )
        {
            event.Enable(false);
            return;
        }
        if ( ! ( m_textDelay->IsModified() || m_textPeriod->IsModified() ) )
        {
            event.Enable(false);
            return;
        }
        event.Enable(true);
        return;
    }
    event.Enable(false);
}
ClockSettingsPanel::ClockSettingsPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* gSizer1;
	gSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );

	m_staticText1 = new wxStaticText( this, ID_DEFAULT, wxT("Legth of half period [Ticks]"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_staticText1, 0, wxALL, 5 );

	m_textPeriod = new wxTextCtrl( this, TIMING_ID_PANEL_CLK_TXTPERIOD, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator( wxFILTER_NUMERIC ) );
	gSizer1->Add( m_textPeriod, 0, wxEXPAND|wxALL, 5 );

	m_staticText2 = new wxStaticText( this, ID_DEFAULT, wxT("Delay [Ticks]"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_staticText2, 0, wxALL, 5 );

	m_textDelay = new wxTextCtrl( this, TIMING_ID_PANEL_CLK_TXTDELAY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator( wxFILTER_NUMERIC ) );
	gSizer1->Add( m_textDelay, 0, wxEXPAND|wxALL, 5 );

	bSizer1->Add( gSizer1, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	m_buttonApply = new wxButton( this, TIMING_ID_PANEL_CLK_APPLY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonApply, 0, wxALL, 5 );

    m_buttonCancel = (wxButton*)NULL;
	//m_buttonCancel = new wxButton( this, TIMING_ID_PANEL_CLK_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	//bSizer2->Add( m_buttonCancel, 0, wxALL, 5 );

	bSizer1->Add( bSizer2, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
}

