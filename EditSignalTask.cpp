#include "EditSignalTask.h"

EditSignalTask::EditSignalTask(const Task *task, unsigned int sig, wxInt32 tick, bool leftDown):
Task(task),
m_sig(sig),
m_leftDown(leftDown),
m_startTick(tick)
{
    //ctor
    m_onEdge = false; // not implemented yet
}

EditSignalTask::~EditSignalTask()
{
    //dtor
}

void EditSignalTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{

}


void EditSignalTask::SetDrawlets()
{

}


void EditSignalTask::OnMouse(const wxMouseEvent &event)
{
    if (event.ButtonDown(wxMOUSE_BTN_ANY ))
        EndTask();
}
void EditSignalTask::OnKey(const wxKeyEvent &event, bool down)
{
    if ( down && event.GetKeyCode() == WXK_ESCAPE)
        EndTask();
}
void EditSignalTask::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}
void EditSignalTask::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}
void EditSignalTask::LabelsKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
void EditSignalTask::WavesKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
void EditSignalTask::AxisKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
