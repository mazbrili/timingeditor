#include "BusWidthText.h"
#include "TimingView.h"

#include "cmd.h"

IMPLEMENT_DYNAMIC_CLASS(BusWidthText,TimingTextCtrl)

BEGIN_EVENT_TABLE(BusWidthText, TimingTextCtrl)
  //EVT_TEXT_ENTER(wxID_ANY, BusWidthText::OnEnterCommand)
  EVT_SET_FOCUS(           BusWidthText::OnSetFocus)
  //EVT_KILL_FOCUS(          BusWidthText::OnKillFocus)
END_EVENT_TABLE()

BusWidthText::BusWidthText(wxWindow *parent, TimingView *view, const wxString &value, const wxPoint& pos, const wxSize& size, unsigned int signalNumber):
TimingTextCtrl(parent, view, value, pos, size, wxTE_PROCESS_ENTER | wxBORDER_NONE ), //wxBORDER_NONE| wxTE_DONTWRAP | wxBORDER_SIMPLE
m_signalNumber(signalNumber)
{
    //ctor
    ChangeValue(_("[") + value + _("]"));
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
    wxLogDebug(wxString::Format(_T("BusWidthText::OnSetFocus %d, unchanged = ")+unchanged,m_signalNumber  ));
    ChangeValue(unchanged);
    m_view->TextHasFocus(this);
}
void BusWidthText::OnKillFocus(wxFocusEvent &event)
{
    wxLogDebug(wxString::Format(_T("BusWidthText::OnKillFocus %d"),m_signalNumber ));
}
wxCommand *BusWidthText::GetCommand()
{
    return new ChangeSignalBuswidth((TimingDocument *)m_view->GetDocument(), GetValue(), m_signalNumber);
}
void BusWidthText::RestoreText()
{
    ChangeValue(_("[") + unchanged + _("]"));

    //should not fail, because labels, axis and waves are panels
    wxPanel *parentpanel = dynamic_cast<wxPanel *>( m_parent );
    if ( parentpanel )
        parentpanel->SetFocusIgnoringChildren();
    else
        m_parent->SetFocus();

    edittask = NULL;
}
