#include "ActiveSignalTask.h"

#include <wx/clipbrd.h>

#include "ClockSettingsPanel.h"
#include "TimingDoc.h"
#include "TimingView.h"
#include "DataObject.h"
#include "cmd.h"

#include "HoverHatch.h"
#include "DiagramWavesWindow.h"
#include "DiagramLabelsWindow.h"

ActiveSignalTask::ActiveSignalTask(const Task *task, int sig):
Task(task),
m_sig(sig)
{
    //ctor
    UpdateClockSettingsPanel();
    //::wxLogMessage(_T("ActiveSignalTask::ActiveSignalTask"));
    if ( m_sig < 0 || (unsigned int) m_sig >= m_view->heightOffsets.size()-1)
    {
        EndTask();
        return;
    }

    SetDrawlets();
}

void ActiveSignalTask::SetDrawlets()
{
    wxCoord y = m_view->heightOffsets[m_sig];
    wxCoord h = -y + m_view->heightOffsets[m_sig + 1];
    m_waveWin->SetDrawlet(new HoverHatch(wxPoint(0,y), wxSize(0,h), wxColour(32, 32, 0), HoverHatch::expand_x));
    m_labelsWin->SetDrawlet(new HoverHatch(wxPoint(0,y), wxSize(0,h), wxColour(32, 32, 0),HoverHatch::expand_x));
}

ActiveSignalTask::~ActiveSignalTask()
{
    //dtor
}

void ActiveSignalTask::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if ( event.ButtonDown(wxMOUSE_BTN_RIGHT) )
    {
        EndTask();
        return;
    }
    if ( event.ButtonDown(wxMOUSE_BTN_LEFT) )
    {
        int k = GetSignalFromPosition(pos);
        if ( k == -1 )
        {
            EndTask();
            return;
        }
        m_sig = k;
        SetDrawlets();
    }
}
void ActiveSignalTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}
void ActiveSignalTask::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}
void ActiveSignalTask::LabelsKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
void ActiveSignalTask::WavesKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
void ActiveSignalTask::AxisKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}

void ActiveSignalTask::OnKey(const wxKeyEvent &event, bool down)
{
    if ( down && event.GetKeyCode() == WXK_DELETE)
        Delete();
    if ( down && event.GetKeyCode() == WXK_ESCAPE)
        EndTask();
}
void ActiveSignalTask::EndTask()
{
    m_sig = -1;
    UpdateClockSettingsPanel(false);
    //m_axisWin->Refresh();
    m_view->SetTask(NULL);
}
void ActiveSignalTask::UpdateClockSettingsPanel(bool attach)
{
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    if (!attach || !doc || m_sig == -1)
    {
        ClockSettingsPanel::GetInstance()->view = NULL;
        return;
    }

    ClockSettingsPanel::GetInstance()->view = m_view;

    if ( doc->signals.size() < (unsigned int)m_sig)
        return;

    if ( doc->signals[m_sig].IsClock )
    {
        wxInt32 n;
        wxString str;

        n = doc->signals[m_sig].delay;
        str = wxString::Format ( _( "%d" ) , n);
        ClockSettingsPanel::GetInstance()->SetDelayText(str);

        n = doc->signals[m_sig].ticks;
        str = wxString::Format ( _( "%d" ) , n);
        ClockSettingsPanel::GetInstance()->SetTicksText(str);

        ClockSettingsPanel::GetInstance()->SetShadowed(doc->signals[m_sig].GenerateBackground);
        ClockSettingsPanel::GetInstance()->SetShowPeriod(doc->signals[m_sig].ShowPeriodCount);
    }
}

bool ActiveSignalTask::CanDelete()
{
    return m_sig != -1;
}
int ActiveSignalTask::GetSelectedSignalNumber()
{
    return m_sig;
}

void ActiveSignalTask::OnMouse(const wxMouseEvent &event)
{
    //::wxLogMessage(_T("ActiveSignalTask::OnMouse"));
    if (event.ButtonDown(wxMOUSE_BTN_ANY ))
    {
        //::wxLogMessage(_T("ActiveSignalTask::OnMouse endtask"));
        EndTask();
    }
}

void ActiveSignalTask::Delete()
{
    wxCommandProcessor *cmdproc = m_view->GetDocument()->GetCommandProcessor();
    if (cmdproc)
        cmdproc->Submit(new DeleteSignalCommand((TimingDocument *)m_view->GetDocument(), m_sig));
    EndTask();
}
bool ActiveSignalTask::CanCopy()
{
    return m_sig != -1;
}
void ActiveSignalTask::Copy()
{
    if (wxTheClipboard->Open())
    {
        if ( m_sig != -1 )
        {
            TimingDocument *doc = (TimingDocument*)m_view->GetDocument();
            if ( doc )
            {
                Signal *sig = new Signal;
                *sig = doc->signals[m_sig];
                // This data objects are held by the clipboard,
                // so do not delete them here the app.
                wxTheClipboard->SetData( new TimingEditorSignalDataObject(sig) );
            }
        }
        wxTheClipboard->Close();
    }
}
void ActiveSignalTask::Cut()
{
    Copy();
    Delete();
}

void ActiveSignalTask::Paste()
{
    PasteSignalFromClipboard();
}
bool ActiveSignalTask::CanPaste()
{
    return IsSignalInClipboard();
}


