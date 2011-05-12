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
#include "enumers.h"
#include "TimingView.h"

#include "TransitionSettingsPanel.h"
#include "enumers.h"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(TransitionSettingsPanel, wxPanel)
    EVT_UPDATE_UI(TIMING_ID_PANEL_TRANS_APPLY,  TransitionSettingsPanel::OnUpdatePanelApply)
   EVT_UPDATE_UI_RANGE(TIMING_ID_PANEL_TRANS_CHECK50, TIMING_ID_PANEL_TRANS_WIDTH,
        TransitionSettingsPanel::OnUpdateControls)
    EVT_BUTTON(TIMING_ID_PANEL_TRANS_APPLY , TransitionSettingsPanel::OnApply )
END_EVENT_TABLE()

void TransitionSettingsPanel::OnApply(wxCommandEvent &event)
{
    SetUnmodified();

    if ( view ) view->SetTransition(
        (wxInt8)m_spinCtrl1->GetValue(),
        m_check50->GetValue(),
        m_check90->GetValue()
    );
}

void TransitionSettingsPanel::Set50(bool val)
{
    m_check50->SetValue(val);
    en50 = val;
}
void TransitionSettingsPanel::Set90(bool val)
{
    m_check90->SetValue(val);
    en90 = val;
}
void TransitionSettingsPanel::SetTransitionWidth(wxInt8 tw)
{
    m_spinCtrl1->SetValue(tw);
    transwidth = tw;
}

void TransitionSettingsPanel::OnUpdatePanelApply(wxUpdateUIEvent& event)
{
    if ( view )
    {
        if ( transwidth != m_spinCtrl1->GetValue() )
        {
            event.Enable(true);
            return;
        }
        if ( en50 != m_check50->GetValue() )
        {
            event.Enable(true);
            return;
        }
        if ( en90 != m_check90->GetValue() )
        {
            event.Enable(true);
            return;
        }
    }
    event.Enable(false);
}
void TransitionSettingsPanel::SetUnmodified()
{
    transwidth = m_spinCtrl1->GetValue();
    en50 = m_check50->GetValue();
    en90 = m_check90->GetValue();
}
void TransitionSettingsPanel::OnUpdateControls(wxUpdateUIEvent& event)
{
    if ( view )
    {
        event.Enable(true);
        return;
    }
    event.Enable(false);
}
TransitionSettingsPanel::TransitionSettingsPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) :
    wxPanel( parent, id, pos, size, style ),
    view(NULL)
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( this, ID_DEFAULT, wxT("Transition width (% of a Tick)"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_staticText1, 0, wxALL, 5 );

	m_spinCtrl1 = new wxSpinCtrl( this, TIMING_ID_PANEL_TRANS_WIDTH, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0);
	bSizer3->Add( m_spinCtrl1, 1, wxALL, 5 );

	bSizer1->Add( bSizer3, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 2, 0, 0 );

	bSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	//m_checkBox1 = new wxCheckBox( this, TIMING_ID_PANEL_TRANS_CHECK, wxT("Allow vertical lines after 50% and 90% of a transition"), wxDefaultPosition, wxDefaultSize, 0 );
	m_check10 = new wxCheckBox( this, TIMING_ID_PANEL_TRANS_CHECK10, wxT("Snap vertical lines at 10% of transition"), wxDefaultPosition, wxDefaultSize, 0 );
	m_check10->SetValue(true);
	m_check10->Enable(false);
	m_check50 = new wxCheckBox( this, TIMING_ID_PANEL_TRANS_CHECK50, wxT("Snap vertical lines at 50% of transition"), wxDefaultPosition, wxDefaultSize, 0 );
	m_check90 = new wxCheckBox( this, TIMING_ID_PANEL_TRANS_CHECK90, wxT("Snap vertical lines at 90% of transition"), wxDefaultPosition, wxDefaultSize, 0 );

	//bSizer2->Add( m_checkBox1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	bSizer2->Add(m_check10,  0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	bSizer2->Add(m_check50,  0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	bSizer2->Add(m_check90,  0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	bSizer1->Add( bSizer2, 0, wxEXPAND, 5 );

	m_button4 = new wxButton( this, TIMING_ID_PANEL_TRANS_APPLY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_button4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	instance = this;
}

TransitionSettingsPanel* TransitionSettingsPanel::instance = NULL;
TransitionSettingsPanel *TransitionSettingsPanel::GetInstance()
{
    return instance;
}

TransitionSettingsPanel::~TransitionSettingsPanel()
{
    if (instance == this)
        instance = NULL;
}


