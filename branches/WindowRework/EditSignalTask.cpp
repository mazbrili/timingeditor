#include "EditSignalTask.h"
#include "TimingView.h"
#include "TimingDoc.h"
#include "cmd.h"
#include "DiagramWavesWindow.h"
#include "EditSignalDrawlet.h"


namespace{
vals GetNextValue( vals val, bool isBus, bool rotateRight = true)
{
    vals nval;
    if ( !isBus )
    {
        if ( rotateRight )
        {
            switch ( val )
            {
                case zero: nval = one ; break;
                case one : nval = hz  ; break;
                case hz  : nval = u   ; break;
                default:
                case u   : nval = dc1 ; break;
                case dc1 : nval = dc2 ; break;
                case dc2 : nval = zero; break;
            }
        }
        else
        {
            switch(val)
            {
                case zero: nval = dc2 ; break;
                case one : nval = zero; break;
                case hz  : nval = one ; break;
                default:
                case u   : nval = hz  ; break;
                case dc1 : nval = u   ; break;
                case dc2 : nval = dc1 ; break;
            }
        }
    }
    else
    {
        if ( rotateRight )
        {
            switch ( val )
            {
                case zero: nval = one ; break;
                case one : nval = hz  ; break;
                case hz  : nval = u   ; break;
                default:
                case u   : nval = zero; break;
            }
        }
        else
        {
            switch ( val )
            {
                case zero: nval = u   ; break;
                case one : nval = zero; break;
                case hz  : nval = one ; break;
                default:
                case u   : nval = hz  ; break;
            }
        }
    }
    return nval;
}
}

EditSignalTask::EditSignalTask(const Task *task, unsigned int sig, wxInt32 tick, const wxPoint &pos, bool leftDown):
Task(task),
m_sig(sig),
m_leftDown(leftDown),
m_startTick(tick)
{
    //ctor
    m_doc = (TimingDocument*)m_view->GetDocument();
    if ( ! m_doc )
    {
        EndTask();
        return;
    }

    // check if click on an edge of asignal
    m_startedOnEdge = false; // not yet implemented

    m_validMove = true;
    m_endTick = m_startTick;

    SetDrawlets();
}

EditSignalTask::~EditSignalTask()
{
    //dtor
}

void EditSignalTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if ((event.ButtonDown(wxMOUSE_BTN_RIGHT) && m_leftDown) || (event.ButtonDown(wxMOUSE_BTN_LEFT) && !m_leftDown) )
    {
        EndTask();
        return;
    }


    if ( event.Leaving() )
    {
        if ( m_validMove )
        {
            m_validMove = false;
            SetDrawlets();
        }
        return;
    }

    if(event.Dragging() /*&& ((event.ButtonIsDown(wxMOUSE_BTN_LEFT) && m_leftDown)|| event.ButtonIsDown(wxMOUSE_BTN_RIGHT) && !m_leftDown )*/)
    {
        if ( !m_startedOnEdge )
            MouseDragState(pos);
        else
            MouseDragEdge(pos);
        return;
    }
    if ( (event.ButtonUp(wxMOUSE_BTN_LEFT) && m_leftDown) || (event.ButtonUp(wxMOUSE_BTN_RIGHT) && !m_leftDown) )
    {
        if ( !m_startedOnEdge )
            MouseUpState(pos);
        else
            MouseUpEdge(pos);
        return;
    }
}
void EditSignalTask::MouseDragState(const wxPoint &pos)
{
    int signalidx = IsOverWaves(pos);
    if ( signalidx == -1 || (unsigned int)signalidx != m_sig )
    {
        if (m_validMove)
        {
            m_validMove = false;
            SetDrawlets();
        }
        return;
    }

    wxInt32 endTick = GetTickFromPosition(pos);
    if ( m_endTick != endTick || !m_validMove)
    {
        m_validMove = true;
        m_endTick = endTick;
        SetDrawlets();
        return;
    }
}
void EditSignalTask::MouseUpState(const wxPoint &pos)
{
    if (m_validMove)
    {
        wxInt32 s = m_startTick, e = m_endTick;
        if (s > e)
        {
            s = e;
            e = m_startTick;
        }

        Signal sig = m_doc->signals[m_sig];

        vals val = sig.values[m_view->VisibleTicks[m_startTick]];
        vals newval = GetNextValue(val, sig.IsBus, m_leftDown);
        e = m_view->VisibleTicks[e];
        s = m_view->VisibleTicks[s];
        for ( wxInt32 k = s ; k <= e ; k++)
            sig.values[k]=newval;
        m_doc->GetCommandProcessor()->Submit(new ChangeSignal(m_doc, m_sig, sig));
        //EndTask will be called by Update
    }
    else
    {
        EndTask();
    }
}

void EditSignalTask::Update()
{
    EndTask();
}

void EditSignalTask::SetDrawlets()
{
    if (!m_validMove)
    {
        m_waveWin->RemoveDrawlet();
        return;
    }
    if ( !m_startedOnEdge )
    {
        wxInt32 s = m_startTick, e = m_endTick;
        if (s > e)
        {
            s = e;
            e = m_startTick;
        }

        Signal *sig = new Signal;
        *sig = m_doc->signals[m_sig];
        vals val = sig->values[m_view->VisibleTicks[m_startTick]];
        vals nval = GetNextValue(val, sig->IsBus, m_leftDown);
        e = m_view->VisibleTicks[e];
        s = m_view->VisibleTicks[s];
        for ( wxInt32 k = s ; k <=e ; k++)
            sig->values[k]=nval;
        m_waveWin->SetDrawlet(new EditSignalDrawlet(sig, m_view, m_view->heightOffsets[m_sig]));
    }
}

// needs an implementation:
void EditSignalTask::MouseDragEdge(const wxPoint &pos){}
void EditSignalTask::MouseUpEdge(const wxPoint &pos){}


// defaults:
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
