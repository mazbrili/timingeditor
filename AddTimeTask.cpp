#include "AddTimeTask.h"

#include "TimingView.h"
#include "cmd.h"
#include "DiagramWavesWindow.h"
#include "DiagramAxisWindow.h"

#include "HoverHatch.h"

#include <wx/numdlg.h>

AddRemoveTimeTask::AddRemoveTimeTask(const Task *task, int tick, bool add):
Task(task),
m_startTick(tick),
m_endTick(tick),
m_add(add),
m_validMove(false)
{
    //ctor
    m_doc = (TimingDocument *)m_view->GetDocument();

    m_waitDoubleClick = m_add;
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
        if (m_add)
        {
            wxInt32 tick = (pos.x - m_view->GetWavesLeftSpace())/(m_view->GridStepWidth);
            if ( tick <= m_startTick)
            {   if (m_validMove == false)//do not update view since it is already OK
                    return;
                m_validMove = false;
            }
            else
            {
                if ( tick == m_endTick && m_validMove == true)
                    return;
                m_endTick = tick;
                m_validMove = true;
            }
        }
        else
        {

            if ( pos.x < m_view->GetWavesLeftSpace() ||
                 pos.x > m_view->GetWavesLeftSpace() + m_view->GridStepWidth*((int)m_view->VisibleTicks.size() - 1) )
            {
                if (m_validMove == false) //do not update view
                    return;
                m_validMove = false;
            }
            else
            {
                wxInt32 tick = GetTickFromPosition(pos);
                if (tick == m_endTick && m_validMove == true)//do not update view since it is already OK
                    return;
                m_endTick = tick;
                m_validMove = true;
            }
        }
        SetDrawlets();
        return;
    }
    if(event.ButtonDClick(wxMOUSE_BTN_LEFT))
    {
        if (m_add)
        {
            wxInt32 len = ::wxGetNumberFromUser( _T("Ticks to add"), _T("the number of ticks to add in the diagram"), _T("Ticks to add"), 10, 1, 100);
            if (len != -1)
                m_doc->GetCommandProcessor()->Submit(
                    new AddTimeCommand((TimingDocument *)m_view->GetDocument(),
                                        m_view->VisibleTicks[m_startTick],
                                        len)
                );
        }
        EndTask();
        return;
    }
    if (event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        if ( m_validMove )
        {
            if (m_add)
                m_doc->GetCommandProcessor()->Submit(
                    new AddTimeCommand((TimingDocument *)m_view->GetDocument(),
                                       m_view->VisibleTicks[m_startTick],
                                       m_endTick - m_startTick)
                );
            else
                m_doc->GetCommandProcessor()->Submit(
                    new RemoveTimeCommand((TimingDocument *)m_view->GetDocument(),
                                          m_view->VisibleTicks[m_startTick],
                                          m_view->VisibleTicks[m_endTick])
                );
            m_waitDoubleClick = false;
        }
        if (!m_waitDoubleClick)
            EndTask();
        m_waitDoubleClick = false;
        return;
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
void AddRemoveTimeTask::SetDrawlets()
{
    if ( ! m_validMove )
    {
        m_waveWin->RemoveDrawlet();
        m_axisWin->RemoveDrawlet();
        return;
    }
    if (m_add)
    {
        wxInt32 s = m_startTick, e = m_endTick;
        wxCoord x = m_view->GetWavesLeftSpace() + m_view->GridStepWidth*s;
        wxCoord w = m_view->GridStepWidth*(e-s);
        m_waveWin->SetDrawlet(new HoverHatch(wxPoint(x,0), wxSize(w,0), wxColour(32, 32, 0), HoverHatch::expand_y));
    }
    else
    {
        wxInt32 s = m_startTick, e = m_endTick;
        if ( s > e )
        {
            s = m_endTick;
            e = m_startTick;
        }
        wxCoord x = m_view->GetWavesLeftSpace() + m_view->GridStepWidth*s;
        wxCoord w = m_view->GridStepWidth*(e-s);
        m_waveWin->SetDrawlet(new HoverHatch(wxPoint(x,0), wxSize(w,0), wxColour(32, 32, 0), HoverHatch::expand_y));
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
    if ( event.ButtonDown(wxMOUSE_BTN_ANY ) || event.ButtonUp(wxMOUSE_BTN_ANY) )
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
    if ( down && event.GetKeyCode() == WXK_ESCAPE)
    {
        EndTask();
        return;
    }
}

