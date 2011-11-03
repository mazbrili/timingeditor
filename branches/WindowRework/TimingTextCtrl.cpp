#include "TimingTextCtrl.h"
#include "TimingView.h"
#include "EditTextTask.h"

#include <wx/cmdproc.h>

BEGIN_EVENT_TABLE(TimingTextCtrl, wxTextCtrl)
  EVT_TEXT_ENTER(wxID_ANY, TimingTextCtrl::OnEnterCommand)
  EVT_TEXT(wxID_ANY,       TimingTextCtrl::OnText)
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
    //SetDropTarget(new TimingTextDropTarget(this));
}

TimingTextCtrl::~TimingTextCtrl()
{
    //dtor
    // delete the drop targets
    //SetDropTarget(NULL);
}
void TimingTextCtrl::Draw(wxDC &dc)
{
    dc.DrawText(GetValue(), GetPosition());
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
void TimingTextCtrl::SetFocusToParent()
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

//void TimingTextCtrl::OnDrop(const wxString& data)
//{
//
//    wxMessageBox(_T("dropped")+data);
//}

//bool TimingTextDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data)
//{
//    //wxTextDropTarget::OnDropText(x,y, data);
//    m_owner->OnDrop(data);
//}

void TimingTextCtrl::OnText(wxCommandEvent &event)
{
    //drop text is there a better solution?
    if(!edittask)
    {
        m_view->SetTask(NULL);

        wxCommandProcessor *cmdproc = m_view->GetDocument()->GetCommandProcessor();
        if (cmdproc)
            cmdproc->Submit(GetChangedCommand());

    }
}
