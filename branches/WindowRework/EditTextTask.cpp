#include "EditTextTask.h"

#include "TimingTextCtrl.h"

#include "DiagramAxisWindow.h"
#include "DiagramLabelsWindow.h"
#include "DiagramWavesWindow.h"
#include "TimingView.h"
#include "HoverCombo.h"
#include "HoverGraphCaret.h"
#include "HoverLine.h"

#include "cmd.h"

EditTextTask::EditTextTask(TimingView *view, DiagramLabelsWindow *labelsWin, DiagramAxisWindow *axisWin, DiagramWavesWindow *waveWin, TimingTextCtrl *txtctrl):
Task(view, labelsWin, axisWin, waveWin),
m_txtctrl(txtctrl)
{
    editdone = false;
    txtctrl->edittask = this;}

EditTextTask::~EditTextTask()
{
    if (!editdone)
        m_txtctrl->RestoreText();
}

void EditTextTask::LabelsKey(const wxKeyEvent &event, bool down)
{
    wxLogDebug(_T("EditTextTask::LabelsKey"));
}
void EditTextTask::WavesKey(const wxKeyEvent &event, bool down)
{

}
void EditTextTask::AxisKey(const wxKeyEvent &event, bool down)
{

}
void EditTextTask::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}
void EditTextTask::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}
void EditTextTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}
void EditTextTask::OnMouse(const wxMouseEvent &event)
{
    if (event.Button(wxMOUSE_BTN_LEFT))
        EndTask();
    if (event.Button(wxMOUSE_BTN_RIGHT))
        m_view->SetTask(NULL);
}
void EditTextTask::EndTask()
{
    // create command and send to commandProcessor
    SendCommandToProcessor();
    m_view->SetTask(NULL);
}
void EditTextTask::Paste()
{
    m_txtctrl->Paste();
}
bool EditTextTask::CanPaste()
{
    bool ret;
    if (wxTheClipboard->Open())
    {
        ret = wxTheClipboard->IsSupported( wxDF_TEXT );
        wxTheClipboard->Close();
    }
    return ret;
}
void EditTextTask::Copy()
{
    if (wxTheClipboard->Open())
    {
        // This data objects are held by the clipboard,
        // so do not delete them in the app.
        wxString str = m_txtctrl->GetStringSelection();
        if ( !str.IsEmpty() )
            wxTheClipboard->SetData( new wxTextDataObject(str) );
        wxTheClipboard->Close();
    }
// MSW and Motif only
//    m_txtctrl->Copy();
}
void EditTextTask::Cut()
{
    m_txtctrl->Cut();
}
void EditTextTask::Delete()
{
    long from, to;
    m_txtctrl->GetSelection(&from, &to);
    if (from != to)
        m_txtctrl->Remove(from, to);
}
void EditTextTask::SelectAll()
{
    m_txtctrl->SetSelection(-1, -1);
}
bool EditTextTask::CanDelete()
{
    return HasActiveSelection();
}
bool EditTextTask::HasActiveSelection()
{
    return !(m_txtctrl->GetStringSelection().IsEmpty());
}
void EditTextTask::TextHasFocus(TimingTextCtrl *ctrl)
{
    //break loop
    if (m_txtctrl == ctrl) return;
    SendCommandToProcessor();

    m_labelsWin->SetFocus();

    m_view->SetTask(NULL/*new EditTextTask(m_view, m_labelsWin, m_axisWin, m_waveWin, ctrl)*/);
}
void EditTextTask::SendCommandToProcessor()
{
    m_view->GetDocument()->GetCommandProcessor()->Submit(m_txtctrl->GetCommand());
    editdone = true;
}

