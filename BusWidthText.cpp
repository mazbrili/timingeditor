#include "BusWidthText.h"
#include "TimingView.h"

#include "cmd.h"

IMPLEMENT_DYNAMIC_CLASS(BusWidthText,TimingTextCtrl)

BEGIN_EVENT_TABLE(BusWidthText, TimingTextCtrl)
  //EVT_TEXT_ENTER(wxID_ANY, BusWidthText::OnEnterCommand)
  EVT_SET_FOCUS(           BusWidthText::OnSetFocus)
  //EVT_KILL_FOCUS(          BusWidthText::OnKillFocus)
END_EVENT_TABLE()

BusWidthText::BusWidthText(wxPanel *parent, TimingView *view, const wxString &value, const wxPoint& pos, const wxSize& size, unsigned int signalNumber):
TimingTextCtrl(parent, view, value, pos, size, wxTE_PROCESS_ENTER | wxBORDER_NONE ), //wxBORDER_NONE| wxTE_DONTWRAP | wxBORDER_SIMPLE
m_signalNumber(signalNumber)
{
    //ctor
    //ChangeValue("[" + value + "]");
}

BusWidthText::~BusWidthText()
{
    //dtor
}
//void BusWidthText::OnEnterCommand(wxCommandEvent &event)
//{
//    m_view->SetTask(NULL);
//}
void BusWidthText::OnSetFocus(wxFocusEvent &event)
{
    ChangeValue(unchanged);
    m_view->TextHasFocus(this);
}
void BusWidthText::OnKillFocus(wxFocusEvent &event)
{}
wxCommand *BusWidthText::GetEnterCommand()
{
    return new ChangeSignalBuswidth((TimingDocument *)m_view->GetDocument(), GetValue(), m_signalNumber);
}
wxCommand *BusWidthText::GetChangedCommand()
{
    return new ChangeSignalBuswidth((TimingDocument *)m_view->GetDocument(), GetValue(), m_signalNumber);
}
//void BusWidthText::RestoreText()
//{
//    ChangeValue("[" + unchanged + "]");
//    ChangeValue( unchanged );
//
//    should not fail, because labels, axis and waves are panels
//    wxPanel *parentpanel = dynamic_cast<wxPanel *>( m_parent );
//    if ( parentpanel )
//        parentpanel->SetFocusIgnoringChildren();
//    else
//        m_parent->SetFocus();
//
//    edittask = NULL;
//}
