#include "TimingTextCtrl.h"

#include "DiagramLabelsWindow.h"

IMPLEMENT_DYNAMIC_CLASS(TimingTextCtrl, wxTextCtrl)

BEGIN_EVENT_TABLE(TimingTextCtrl, wxTextCtrl)
  //EVT_COMMAND       (wxID_UNDO, wxEVT_COMMAND_TEXT_UPDATED , TimingTextCtrl::OnUndoRedo)
  //EVT_COMMAND       (wxID_REDO, wxEVT_UN , TimingTextCtrl::OnUndoRedo)
  EVT_TEXT          (wxID_ANY,  TimingTextCtrl::OnText)
  EVT_TEXT_ENTER    (wxID_ANY,  TimingTextCtrl::OnTextEnter)
  EVT_KILL_FOCUS    (           TimingTextCtrl::OnKillFocus)
  EVT_SET_FOCUS     (           TimingTextCtrl::OnSetFocus)
END_EVENT_TABLE()


TimingTextCtrl::TimingTextCtrl(wxWindow *parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size)
:wxTextCtrl(parent, id, value, pos, size, wxTE_PROCESS_ENTER | wxBORDER_NONE),
labelparent(NULL)
{
    SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
    SetOptimalSize();
}

TimingTextCtrl::TimingTextCtrl(DiagramLabelsWindow *parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size)
:wxTextCtrl((wxWindow*)parent, id, value, pos, size, wxTE_PROCESS_ENTER | wxBORDER_NONE),
labelparent(parent)
{
    SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
    SetOptimalSize();
}

void TimingTextCtrl::OnUndoRedo(wxCommandEvent &event)
{
    // do not process here!
    event.Skip();
}
void TimingTextCtrl::SetOptimalSize()
{
    int x, y;
    GetTextExtent(GetValue(), &x, &y);
    SetClientSize(x+15, y);
}
void TimingTextCtrl::OnText(wxCommandEvent &event)
{
    SetOptimalSize();
}
void TimingTextCtrl::OnTextEnter(wxCommandEvent &event)
{
    wxMessageBox(_T("here"));
}

void TimingTextCtrl::OnSetFocus(wxFocusEvent &event)
{
    //ChangeValue(_T("$t$"));
}
void TimingTextCtrl::OnKillFocus(wxFocusEvent &event)
{
    //ChangeValue(_T("100ns"));
}

