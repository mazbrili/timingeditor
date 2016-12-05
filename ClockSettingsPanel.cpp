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
#include "ClockSettingsPanel.h"
#include "enumers.h"
#include "TimingView.h"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(ClockSettingsPanel, wxPanel)
    EVT_UPDATE_UI(TIMING_ID_PANEL_CLK_APPLY,  ClockSettingsPanel::OnUpdatePanelClkApply)
    EVT_UPDATE_UI(TIMING_ID_PANEL_CLK_PERIOD, ClockSettingsPanel::OnUpdateClockPeriod)
    EVT_UPDATE_UI_RANGE(TIMING_ID_PANEL_CLK_TXTPERIOD, TIMING_ID_PANEL_CLK_SHADOW,
        ClockSettingsPanel::OnUpdateTextFields)
    EVT_BUTTON(TIMING_ID_PANEL_CLK_APPLY, ClockSettingsPanel::OnApply)
END_EVENT_TABLE()



void ClockSettingsPanel::SetUnmodified()
{
    m_textDelay->SetModified(false);
    m_textPeriod->SetModified(false);
    Shadowed = m_checkShadow->GetValue();
    DrawPeriod = m_checkPeriodCount->GetValue();
}
void ClockSettingsPanel::OnUpdateTextFields(wxUpdateUIEvent& event)
{
    if ( view && view->IsSignalSelected() && view->IsClockSelected() )
    {
        event.Enable(true);
        return;
    }
    event.Enable(false);
}
void ClockSettingsPanel::OnUpdateClockPeriod(wxUpdateUIEvent& event)
{
    if ( view && view->IsSignalSelected() &&
        view->IsClockSelected() && m_checkShadow->IsChecked())
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
    if ( view )
        view->SetClock(vald, valp, Shadowed, DrawPeriod);
}
void ClockSettingsPanel::SetShadowed(bool sha)
{
    m_checkShadow->SetValue(sha);
    Shadowed = sha;

    m_checkPeriodCount->Enable(sha);
}
void ClockSettingsPanel::SetShowPeriod(bool en)
{
    m_checkPeriodCount->SetValue(en);
    DrawPeriod = en;
}


void ClockSettingsPanel::OnUpdatePanelClkApply(wxUpdateUIEvent& event)
{
    if ( view && view->IsSignalSelected() && view->IsClockSelected() )
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
        if ( ! ( m_textDelay->IsModified() ||
                 m_textPeriod->IsModified() ||
                 Shadowed != m_checkShadow->GetValue() ||
                 DrawPeriod != m_checkPeriodCount->GetValue() ) )
        {
            event.Enable(false);
            return;
        }
        event.Enable(true);
        return;
    }
    event.Enable(false);
}
ClockSettingsPanel::ClockSettingsPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) :
    wxPanel( parent, id, pos, size, style ),
    view(NULL)
{
    wxStaticText *staticText;
	wxBoxSizer* bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* gSizer1 = new wxFlexGridSizer( 2, 0, 0 );

	staticText = new wxStaticText( this, ID_DEFAULT, _("Legth of half period [Ticks]"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( staticText, 0, wxALL, 5 );
	m_textPeriod = new wxTextCtrl( this, TIMING_ID_PANEL_CLK_TXTPERIOD, "", wxDefaultPosition, wxDefaultSize, 0, wxTextValidator( wxFILTER_NUMERIC ) );
	gSizer1->Add( m_textPeriod, 0, wxEXPAND|wxALL, 5 );

	staticText = new wxStaticText( this, ID_DEFAULT, _("Delay [Ticks]"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( staticText, 0, wxALL, 5 );
	m_textDelay = new wxTextCtrl( this, TIMING_ID_PANEL_CLK_TXTDELAY, "", wxDefaultPosition, wxDefaultSize, 0, wxTextValidator( wxFILTER_NUMERIC ) );
	gSizer1->Add( m_textDelay, 0, wxEXPAND|wxALL, 5 );


	bSizer1->Add( gSizer1, 1, wxEXPAND, 5 );


    wxBoxSizer* bSizer5 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	m_checkShadow = new wxCheckBox(this, TIMING_ID_PANEL_CLK_SHADOW, _("generate vertical shadow"));
	m_checkPeriodCount = new wxCheckBox(this, TIMING_ID_PANEL_CLK_PERIOD, _("show numbering clock periods"));

    bSizer5->Add(m_checkShadow,  0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
    bSizer5->Add(m_checkPeriodCount,  0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


    bSizer2->Add(bSizer5, wxEXPAND|wxALL, 5);
	bSizer1->Add( bSizer2, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );


	wxBoxSizer* bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	m_buttonApply = new wxButton( this, TIMING_ID_PANEL_CLK_APPLY, _("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonApply, 0, wxALL, 5 );
	bSizer1->Add( bSizer3, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	instance = this;
}

ClockSettingsPanel* ClockSettingsPanel::instance = NULL;
ClockSettingsPanel *ClockSettingsPanel::GetInstance()
{
    return instance;
}

ClockSettingsPanel::~ClockSettingsPanel()
{
    if (instance == this)
        instance = NULL;
}
