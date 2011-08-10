#include "BusValueText.h"
#include "TimingView.h"

#include "cmd.h"

IMPLEMENT_DYNAMIC_CLASS(BusValueText,TimingTextCtrl)

BEGIN_EVENT_TABLE(BusValueText, TimingTextCtrl)
  //EVT_TEXT_ENTER(wxID_ANY, BusValueText::OnEnterCommand)
  EVT_SET_FOCUS(           BusValueText::OnSetFocus)
  //EVT_KILL_FOCUS(          BusValueText::OnKillFocus)
END_EVENT_TABLE()

BusValueText::BusValueText(wxPanel *parent, TimingView *view, const wxString &value, const wxPoint& pos, unsigned int signalNumber, unsigned int tick):
TimingTextCtrl(parent, view, value, pos, wxDefaultSize, wxTE_PROCESS_ENTER | wxBORDER_NONE ), //wxBORDER_NONE| wxTE_DONTWRAP | wxBORDER_SIMPLE
m_signalNumber(signalNumber),
m_tick(tick)
{}

BusValueText::~BusValueText()
{}

//void BusValueText::OnEnterCommand(wxCommandEvent &event)
//{
//    m_view->SetTask(NULL);
//}
void BusValueText::OnSetFocus(wxFocusEvent &event)
{
    ::wxLogMessage(_T("BusValueText::OnSetFocus %d, tick: %d"), m_signalNumber, m_tick);
    m_view->TextHasFocus(this);
}
void BusValueText::OnKillFocus(wxFocusEvent &event)
{
    ::wxLogMessage(_T("BusValueText::OnKillFocus %d, tick: %d"), m_signalNumber, m_tick);
}
wxCommand *BusValueText::GetEnterCommand()
{
    return new ChangeBusTextCommand((TimingDocument *)m_view->GetDocument(), m_signalNumber, m_tick, GetValue());
}
wxCommand *BusValueText::GetChangedCommand()
{
    return new ChangeBusTextCommand((TimingDocument *)m_view->GetDocument(), m_signalNumber, m_tick, GetValue());
}

void BusValueText::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    TimingTextCtrl::SetSize(x, y, width, height, sizeFlags);
}
void BusValueText::OnDrop(const wxString& data)
{
    ::wxLogMessage(_T("BusValueText::OnDrop %d, tick: %d"), m_signalNumber, m_tick);
}
