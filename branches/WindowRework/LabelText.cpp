#include "LabelText.h"
#include "TimingView.h"

#include "cmd.h"

IMPLEMENT_DYNAMIC_CLASS(LabelText,TimingTextCtrl)

BEGIN_EVENT_TABLE(LabelText, TimingTextCtrl)
  //EVT_TEXT_ENTER(wxID_ANY, LabelText::OnEnterCommand)
  EVT_SET_FOCUS(           LabelText::OnSetFocus)
  //EVT_KILL_FOCUS(          LabelText::OnKillFocus)
END_EVENT_TABLE()

LabelText::LabelText(wxPanel *parent, TimingView *view, const wxString &value, const wxPoint& pos, const wxSize& size, unsigned int signalNumber):
TimingTextCtrl(parent, view, value, pos, size, wxTE_PROCESS_ENTER | wxBORDER_NONE ), //wxBORDER_NONE| wxTE_DONTWRAP | wxBORDER_SIMPLE
m_signalNumber(signalNumber),
buswidth(NULL)
{}

LabelText::~LabelText()
{}

void LabelText::SetBuswidthField(TimingTextCtrl * widthctrl)
{
    buswidth = widthctrl;
}

//void LabelText::OnEnterCommand(wxCommandEvent &event)
//{
//    m_view->SetTask(NULL);
//}
void LabelText::OnSetFocus(wxFocusEvent &event)
{
    wxLogDebug(_T("LabelText::OnSetFocus %d"), m_signalNumber);
    m_view->TextHasFocus(this);
}
void LabelText::OnKillFocus(wxFocusEvent &event)
{}
wxCommand *LabelText::GetCommand()
{
    return new ChangeSignalName((TimingDocument *)m_view->GetDocument(), GetValue(), m_signalNumber);
}

void LabelText::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    TimingTextCtrl::SetSize(x, y, width, height, sizeFlags);

    if (buswidth)
    {
        int width, height;
        GetSize(&width, &height);
        buswidth->SetSize(x+width+10, y, wxDefaultCoord, wxDefaultCoord );
    }
}

