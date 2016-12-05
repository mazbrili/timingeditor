#include "EditTextTask.h"


#include <wx/clipbrd.h>

#include "TimingTextCtrl.h"

#include "DiagramAxisWindow.h"
#include "DiagramLabelsWindow.h"
#include "DiagramWavesWindow.h"
#include "TimingView.h"
#include "cmd.h"

EditTextTask::EditTextTask(const Task *task, TimingTextCtrl *txtctrl):
Task(task),
m_txtctrl(txtctrl)
{
    Init();
    m_waveWin->ActivateText(true);
}

void EditTextTask::Init()
{
    editdone = false;
    m_txtctrl->edittask = this;
}

EditTextTask::~EditTextTask()
{
    if (!editdone)
        m_txtctrl->RestoreText();
    m_txtctrl->edittask = NULL;
}

//void EditTextTask::LabelsKey(const wxKeyEvent &event, bool down)
//{
//    //::wxLogMessage("EditTextTask::LabelsKey");
//}
//void EditTextTask::WavesKey(const wxKeyEvent &event, bool down)
//{
//
//}
//void EditTextTask::AxisKey(const wxKeyEvent &event, bool down)
//{
//
//}
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
    SendCommandToProcessor();
    Task::EndTask();
}
void EditTextTask::Paste()
{
    m_txtctrl->Paste();
}
bool EditTextTask::CanPaste()const
{
    bool ret = false;
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
bool EditTextTask::IsTextSelected()const
{
    return !(m_txtctrl->GetStringSelection().IsEmpty());
}
bool EditTextTask::CanDelete()const
{
    return IsTextSelected() && !IsReadOnly();
}
bool EditTextTask::CanCopy()const
{
    return IsTextSelected();
}
bool EditTextTask::CanCut()const
{
    return IsTextSelected() && !IsReadOnly();
}
void EditTextTask::TextHasFocus(TimingTextCtrl *ctrl)
{
    if (m_txtctrl == ctrl) return;
    SendCommandToProcessor();

    //m_labelsWin->SetFocus();

    //another control gets the focus so end this task
    m_view->SetTask(NULL);
}
void EditTextTask::SendCommandToProcessor()
{
    // create command and send to commandProcessor
    wxCommandProcessor *cmdprc = m_view->GetDocument()->GetCommandProcessor();
    if  (cmdprc)
        cmdprc->Submit(m_txtctrl->GetEnterCommand());
    editdone = true;
}

