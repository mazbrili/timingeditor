#include "TimingTextCtrl.h"
#include "TimingView.h"
#include "EditTextTask.h"

BEGIN_EVENT_TABLE(TimingTextCtrl, wxTextCtrl)
  EVT_TEXT_ENTER(wxID_ANY, TimingTextCtrl::OnEnterCommand)
  //EVT_SET_FOCUS(           TimingTextCtrl::OnSetFocus)
  //EVT_KILL_FOCUS(          TimingTextCtrl::OnKillFocus)
END_EVENT_TABLE()

TimingTextCtrl::TimingTextCtrl(wxPanel *parent, TimingView *view, const wxString &value, const wxPoint& pos, const wxSize& size, long style):
wxTextCtrl(parent, wxID_ANY, value, pos, size, style ),
m_view(view),
unchanged(value),
edittask(NULL)
{
    //ctor
}

TimingTextCtrl::~TimingTextCtrl()
{
    //dtor
}
void TimingTextCtrl::RestoreText()
{
    ChangeValue(unchanged);

    SetFocusToParent();

    edittask = NULL;
}
void TimingTextCtrl::OnEnterCommand(wxCommandEvent &event)
{
    if ( edittask )
    {
        edittask->EndTask();
        edittask = NULL;
    }
}
void TimingTextCtrl:: SetFocusToParent()
{
    // should not fail or
    // can not fail
    // because labels, axis and waves are panels
    wxPanel *parentpanel = dynamic_cast<wxPanel *>( m_parent );
    if ( parentpanel )
        parentpanel->SetFocusIgnoringChildren();
//    else
//        m_parent->SetFocus();
}
