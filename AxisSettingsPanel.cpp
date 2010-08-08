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
////////////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "AxisSettingsPanel.h"

#include "TimingApp.h"//GetApp
#include "enumers.h"
#include "TimingWindow.h"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(AxisSettingsPanel, wxPanel)
    EVT_UPDATE_UI(TIMING_ID_PANEL_AXIS_APPLY,  AxisSettingsPanel::OnUpdatePanelApply)
    EVT_UPDATE_UI_RANGE(TIMING_ID_PANEL_AXIS_TICKLENGTH, TIMING_ID_PANEL_AXIS_OFFSET,
        AxisSettingsPanel::OnUpdateFields)
    EVT_BUTTON(TIMING_ID_PANEL_AXIS_APPLY, AxisSettingsPanel::OnApply)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////////////////////

void AxisSettingsPanel::OnApply(wxCommandEvent &event)
{
    SetUnmodified();

    long valticks;
    long valmarker;
    long valoffset;
    long valtotallength;

    wxInt8 unit;
    wxInt32 ticklength;
    wxInt32 markerlength;
    wxInt32 offset;
    wxInt32 totallength;

    m_textTickLenght->GetValue().ToLong(&valticks);
    m_textMarkerLength->GetValue().ToLong(&valmarker);
    m_textOffset->GetValue().ToLong(&valoffset);
    m_textTotalLength->GetValue().ToLong(&valtotallength);

    unit = m_choice1->GetSelection();
    ticklength = (wxInt32)valticks;
    markerlength = (wxInt32)valmarker;
    offset = (wxInt32)valoffset;
    totallength =(wxInt32)valtotallength;

    if ( wnd ) wnd->SetAxis(unit, ticklength, markerlength, offset, totallength);

}
void AxisSettingsPanel::SetUnmodified()
{
    m_textTickLenght->SetModified(false);
    m_textMarkerLength->SetModified(false);
    m_textOffset->SetModified(false);

    TickUnit = m_choice1->GetSelection();
}
void AxisSettingsPanel::OnUpdateFields(wxUpdateUIEvent& event)
{
    if ( wnd )
    {
        event.Enable(true);
        return;
    }
    event.Enable(false);
}
void AxisSettingsPanel::SetTickLength(wxInt32 ticklength)
{
    m_textTickLenght->SetValue(wxString::Format(_T("%d"), ticklength));
}
void AxisSettingsPanel::SetLengthUnit(wxInt8 unit)
{
    m_choice1->Select(unit);
    TickUnit = unit;
}
void AxisSettingsPanel::SetMarkerLength(wxInt32 markerlength)
{
    m_textMarkerLength->SetValue(wxString::Format(_T("%d"),markerlength));
}
void AxisSettingsPanel::SetOffset(wxInt32 offset)
{
    m_textOffset->SetValue(wxString::Format(_T("%d"),offset));
}
void AxisSettingsPanel::SetTotalLengt(wxInt32 totallength)
{
    m_textTotalLength->SetValue(wxString::Format(_T("%d"), totallength));
}
void AxisSettingsPanel::OnUpdatePanelApply(wxUpdateUIEvent& event)
{
    if ( wnd )
    {
        long valticks;
        long valmarker;
        long valtotallength;
//        long valoffset;

        m_textTickLenght->GetValue().ToLong(&valticks);
        if ( valticks <= 0 )
        {
            event.Enable(false);
            return;
        }
        m_textMarkerLength->GetValue().ToLong(&valmarker);
        if ( valmarker <= 0 )
        {
            event.Enable(false);
            return;
        }
        m_textTotalLength->GetValue().ToLong(&valtotallength);
        if ( valtotallength <= 0)
        {
            event.Enable(false);
            return;
        }

        if ( ! ( m_textTickLenght->IsModified() ||
                m_textOffset->IsModified() ||
                m_textMarkerLength->IsModified() ||
                m_textTotalLength->IsModified() ||
                TickUnit != m_choice1->GetSelection()   ) )
        {
            event.Enable(false);
            return;
        }
        event.Enable(true);
        return;
    }
    event.Enable(false);
}
AxisSettingsPanel::AxisSettingsPanel( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) :
    wxPanel( parent, id, pos, size, style ),
    wnd(NULL)
{
    wxStaticText* staticText;
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	wxGridSizer* gSizer2;
	gSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );

	staticText = new wxStaticText( this, ID_DEFAULT, wxT("Length of a Tick"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( staticText, 0, wxALL, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );

	m_textTickLenght = new wxTextCtrl( this, TIMING_ID_PANEL_AXIS_TICKLENGTH, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator( wxFILTER_NUMERIC ) );
	bSizer5->Add( m_textTickLenght, 0, wxALL, 5 );

	wxString m_choice1Choices[] = { wxT("fs"), wxT("ps"), wxT("ns"), wxT("us"), wxT("ms"), wxT("s"),  wxT("ks"), };
	int m_choice1NChoices = sizeof( m_choice1Choices ) / sizeof( wxString );
	m_choice1 = new wxChoice( this, TIMING_ID_PANEL_AXIS_UNITCHOICE, wxDefaultPosition, wxDefaultSize, m_choice1NChoices, m_choice1Choices, 0 );
	bSizer5->Add( m_choice1, 0, wxALL, 5 );
	m_choice1->Select(1);

	gSizer2->Add( bSizer5, 1, wxEXPAND, 5 );

	staticText = new wxStaticText( this, ID_DEFAULT, wxT("Ticks between markers"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( staticText, 0, wxALL, 5 );

	m_textMarkerLength = new wxTextCtrl( this, TIMING_ID_PANEL_AXIS_MARKERLENGTH, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator( wxFILTER_NUMERIC ) );
	gSizer2->Add( m_textMarkerLength, 0, wxALL, 5 );

	staticText = new wxStaticText( this, ID_DEFAULT, wxT("Total Ticks of diagram"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( staticText, 0, wxALL, 5 );

	m_textTotalLength = new wxTextCtrl( this, TIMING_ID_PANEL_AXIS_TOTALLENGTH, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator( wxFILTER_NUMERIC ) );
	gSizer2->Add( m_textTotalLength, 0, wxALL, 5 );

	staticText = new wxStaticText( this, ID_DEFAULT, wxT("Offset at start [Ticks]"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( staticText, 0, wxALL, 5 );

	m_textOffset = new wxTextCtrl( this, TIMING_ID_PANEL_AXIS_OFFSET, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator( wxFILTER_NUMERIC ));
	gSizer2->Add( m_textOffset, 0, wxALL, 5 );

	bSizer3->Add( gSizer2, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	m_applyButton = new wxButton( this, TIMING_ID_PANEL_AXIS_APPLY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_applyButton, 0, wxALL, 5 );

	bSizer3->Add( bSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );

	this->SetSizer( bSizer3 );
	this->Layout();

	bSizer3->Fit( this );
}

