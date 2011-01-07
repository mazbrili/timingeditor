#include "AddVerticalLineTask.h"

AddVerticalLineTask::AddVerticalLineTask(TimingView *view, DiagramLabelsWindow *labelsWin, DiagramAxisWindow *axisWin, DiagramWavesWindow *waveWin):
Task(view, labelsWin, axisWin, waveWin)
{
    //ctor
    m_axisWin->RemoveDrawlet();
    m_waveWin->RemoveDrawlet();
    m_labelsWin->RemoveDrawlet();
    state = waitingFirstPoint;
}

AddVerticalLineTask::~AddVerticalLineTask()
{
    //dtor
}


void AddVerticalLineTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if (event.Moving())
    {
        if (state == waitingFirstPoint )
        {
            Task::WavesMouse(event, pos);
        }
        else // waitingSecondPoint
        {

        }
    }
    else if (event.ButtonDown(wxMOUSE_BTN_LEFT) )
    {
        if (state == waitingFirstPoint )
        {
            //check position, if valid then remember
            //position and change to second state
        }
    }
    else if (event.ButtonDown(wxMOUSE_BTN_RIGHT) )
    {
        EndTask();
    }
    else if ( event.Entering() )
    {
        // button not pressed -> must be in state waitingFirstPoint
        if ( !event.ButtonIsDown(wxMOUSE_BTN_LEFT) )
            state = waitingFirstPoint;
    }
}
void AddVerticalLineTask::LabelsKey(const wxKeyEvent &event, bool down)
{
    if ( down && event.GetKeyCode() == WXK_ESCAPE)
        EndTask();
}
void AddVerticalLineTask::WavesKey(const wxKeyEvent &event, bool down)
{
    if ( down && event.GetKeyCode() == WXK_ESCAPE)
        EndTask();
}
void AddVerticalLineTask::AxisKey(const wxKeyEvent &event, bool down)
{
    if ( down && event.GetKeyCode() == WXK_ESCAPE)
        EndTask();
}
void AddVerticalLineTask::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    NonInvolvedWindowMouse(event);
}
void AddVerticalLineTask::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{

    NonInvolvedWindowMouse(event);
}
void AddVerticalLineTask::NonInvolvedWindowMouse(const wxMouseEvent &event)
{
    if (event.Entering())
        m_axisWin->RemoveDrawlet();
    else if ( event.ButtonUp(wxMOUSE_BTN_ANY ) )
        state = waitingFirstPoint;
    else if( event.Moving() )
        m_waveWin->RemoveDrawlet();
    else if ( event.RightDown() )
        EndTask();
}
void AddVerticalLineTask::Paste(){}
bool AddVerticalLineTask::CanPaste(){return false;}

void AddVerticalLineTask::EndTask()
{
    m_view->SetTask(NULL);
    delete this;
}
