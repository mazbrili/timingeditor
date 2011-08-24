#include "AddTimeTask.h"

AddRemoveTimeTask::AddRemoveTimeTask(const Task *task, int tick, bool add):
Task(task),
m_tick(tick),
m_add(add)
{
    //ctor
}

AddRemoveTimeTask::~AddRemoveTimeTask()
{
    //dtor
}

void AddRemoveTimeTask::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if ( event.ButtonDown(wxMOUSE_BTN_RIGHT) )
    {
        EndTask();
        return;
    }

    if (event.Dragging() && event.ButtonIsDown(wxMOUSE_BTN_LEFT))
    {

    }
    if (event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        EndTask();
    }
    if (event.Leaving())
    {
        return;
    }
    if (event.Entering())
    {
        return;
    }
}
void AddRemoveTimeTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}
void AddRemoveTimeTask::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}

void AddRemoveTimeTask::OnMouse(const wxMouseEvent &event)
{
    if (event.ButtonDown(wxMOUSE_BTN_ANY ))
        EndTask();
}

void AddRemoveTimeTask::LabelsKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
void AddRemoveTimeTask::WavesKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
void AddRemoveTimeTask::AxisKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}

void AddRemoveTimeTask::OnKey(const wxKeyEvent &event, bool down)
{
    ::wxLogMessage(_T("AddRemoveTimeTask::OnKey"));
    if ( down && event.GetKeyCode() == WXK_ESCAPE)
    {
        ::wxLogMessage(_T("AddRemoveTimeTask::OnKey ESCAPE"));
        EndTask();
        return;
    }
}

