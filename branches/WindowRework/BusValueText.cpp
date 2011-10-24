#include "BusValueText.h"
#include "TimingView.h"

#include "cmd.h"

bool BusValueText::m_active = false;

IMPLEMENT_DYNAMIC_CLASS(BusValueText,TimingTextCtrl)

BEGIN_EVENT_TABLE(BusValueText, TimingTextCtrl)
  //EVT_TEXT_ENTER(wxID_ANY, BusValueText::OnEnterCommand)
  EVT_SET_FOCUS(           BusValueText::OnSetFocus)
  //EVT_KILL_FOCUS(          BusValueText::OnKillFocus)
END_EVENT_TABLE()

BusValueText::BusValueText(wxPanel *parent, TimingView *view, const wxString &value, const wxPoint& pos, const wxPoint& unscrolledPos, unsigned int signalNumber, unsigned int tick):
TimingTextCtrl(parent, view, value, pos, wxDefaultSize, wxTE_PROCESS_ENTER | wxBORDER_NONE ), //wxBORDER_NONE| wxTE_DONTWRAP | wxBORDER_SIMPLE
m_signalNumber(signalNumber),
m_tick(tick),
m_unscrolledPos(unscrolledPos)
{
    if (m_active)
        Show();
    else
        Hide();
}

BusValueText::~BusValueText()
{}

//void BusValueText::OnEnterCommand(wxCommandEvent &event)
//{
//    m_view->SetTask(NULL);
//}
void BusValueText::OnSetFocus(wxFocusEvent &event)
{
    m_view->TextHasFocus(this);
}
void BusValueText::OnKillFocus(wxFocusEvent &event)
{}
wxCommand *BusValueText::GetEnterCommand()
{
    return new ChangeBusTextCommand((TimingDocument *)m_view->GetDocument(), m_signalNumber, m_tick, GetValue());
}
wxCommand *BusValueText::GetChangedCommand()
{
    return new ChangeBusTextCommand((TimingDocument *)m_view->GetDocument(), m_signalNumber, m_tick, GetValue());
}
void BusValueText::OnDrop(const wxString& data)
{}
void BusValueText::SetActive(const bool act)
{
    m_active = act;
    if ( act )
        Show();
    else
        Hide();
}
void BusValueText::Draw(wxDC &dc)const
{
    if ( !m_active )
    {
        dc.DrawText(GetValue(), m_unscrolledPos);
    }
}
