#include "EditTimeCompressorTask.h"

#include "TimingView.h"
#include "DiagramAxisWindow.h"

#include "TimingDoc.h"
#include "cmd.h"

EditTimeCompressorTask::EditTimeCompressorTask(const Task *task, wxInt32 tick):
Task(task),
m_tick(tick)
{
    //ctor
    m_axisWin->Refresh();
    UpdateTimeCompressorPanel();
}

EditTimeCompressorTask::~EditTimeCompressorTask()
{
    m_tick=-1;
    m_axisWin->Refresh();
}

bool EditTimeCompressorTask::CanDelete()
{
    return m_tick != -1;
}
void EditTimeCompressorTask::Delete()
{
    wxCommand *cmd = new RemoveDiscontCommand((TimingDocument *) m_view->GetDocument(), m_tick);
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    if ( doc )
        doc->GetCommandProcessor()->Submit( cmd );
    else
        delete cmd;

    EndTask();
}

wxInt32 EditTimeCompressorTask::GetSelectedDiscontinuity()
{
    return m_tick;
}


void EditTimeCompressorTask::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}
void EditTimeCompressorTask::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    Task::AxisMouse(event, pos);
    if(event.ButtonDClick(wxMOUSE_BTN_LEFT))
    {
        TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
        wxInt32 tick = m_view->VisibleTicks[GetTickFromPosition(pos)];
        if(doc->compressors.find(tick) != doc->compressors.end())
        {
            doc->GetCommandProcessor()->Submit(
                new ChangeTimeCompressor(doc, tick, !doc->compressors[tick].enabled)
            );
            return;
        }
    }
    if(event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
        wxInt32 tick = m_view->VisibleTicks[GetTickFromPosition(pos)];
        if(doc->compressors.find(tick) != doc->compressors.end())
        {
            m_tick = tick;
            UpdateTimeCompressorPanel();
            m_axisWin->Refresh();
        }
        else
            EndTask();
    }

    if(event.ButtonDown(wxMOUSE_BTN_RIGHT))
    {
        EndTask();
    }
}
void EditTimeCompressorTask::EndTask()
{
    m_tick = -1;
    UpdateTimeCompressorPanel();
    m_axisWin->Refresh();
    m_view->SetTask(NULL);
}
void EditTimeCompressorTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}
void EditTimeCompressorTask::OnMouse(const wxMouseEvent &event)
{
    if (event.Button(wxMOUSE_BTN_ANY ))
    {
        EndTask();
    }
}

void EditTimeCompressorTask::LabelsKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
void EditTimeCompressorTask::WavesKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
void EditTimeCompressorTask::AxisKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}

void EditTimeCompressorTask::OnKey(const wxKeyEvent &event, bool down)
{
    if ( down && event.GetKeyCode() == WXK_DELETE)
    {
        Delete();
        return;
    }
    if ( down && event.GetKeyCode() == WXK_ESCAPE)
        EndTask();
}
void EditTimeCompressorTask::UpdateTimeCompressorPanel(bool attach)
{
    if (!attach)
    {
        TimeCompressorSettingsPanel::GetInstance()->view = NULL;
        return;
    }
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    if ( !doc ) return;

    if ( m_tick == -1 ) return;
    TimeCompressorSettingsPanel::GetInstance()->view = m_view;
    if(doc->compressors.find(m_tick) != doc->compressors.end())
        TimeCompressorSettingsPanel::GetInstance()->SetTimeText(
            wxString::Format( _("%d"), doc->compressors[m_tick].length )
        );
}
