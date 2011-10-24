#include "ChangeSignalSpacerTask.h"

#include "TimingDoc.h"
#include "TimingView.h"
#include "cmd.h"

#include "DiagramLabelsWindow.h"
#include "DiagramWavesWindow.h"
#include "HoverLine.h"

ChangeSignalSpacerTask::ChangeSignalSpacerTask(const Task *task, int posToChange):
Task(task),
m_newSpace(-1)
{
    m_waveWin->ActivateText(false);
    //ctor
    if ( posToChange < 0 )//|| (unsigned int) m_sig >= m_view->heightOffsets.size()-1)
    {
        EndTask();
        return;
    }


    m_sig = posToChange / 2;
    m_above = posToChange % 2 == 0;

    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    if (doc->signals.size() <= (unsigned)m_sig )
    {
        EndTask();
        return;
    }

    m_startOfSignalPos = m_view->heightOffsets[m_sig];
    m_endOfSignalPos = m_view->heightOffsets[m_sig+1];

    if ( m_above )
    {
        m_space = doc->signals[m_sig].prespace;
        SetDrawlets(m_startOfSignalPos);
    }
    else
    {
        m_space = doc->signals[m_sig].space;
        SetDrawlets(m_endOfSignalPos);
    }

}

void ChangeSignalSpacerTask::SetDrawlets(int targetpos)
{
    if (targetpos == -1)
    {
        m_labelsWin->RemoveDrawlet();
        return;
    }
    else
    {
        m_labelsWin->SetDrawlet( new HoverLine(-1, targetpos, wxColour(32, 32, 0), 3) );
    }
}

ChangeSignalSpacerTask::~ChangeSignalSpacerTask()
{
    //dtor
}

void ChangeSignalSpacerTask::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if ( event.ButtonDown(wxMOUSE_BTN_ANY) )
    {
        EndTask();
        return;
    }
    if (event.Leaving())
    {
        m_newSpace = -1;
        SetDrawlets();
        return;
    }

    if (event.Dragging() && event.ButtonIsDown(wxMOUSE_BTN_LEFT))
    {
        m_newSpace = -1;
        if ( m_above )
        {
            if ( pos.y < m_startOfSignalPos )
            {
                m_newSpace = m_space + m_startOfSignalPos - pos.y;
                SetDrawlets(pos.y);
            }
            else if ( pos.y < m_startOfSignalPos + m_space)
            {
                m_newSpace = m_startOfSignalPos + m_space - pos.y;
                SetDrawlets(pos.y);
            }
            else
            {
                m_newSpace = 0;
                SetDrawlets(m_startOfSignalPos + m_space);
            }
        }
        else
        {
            if ( pos.y > m_endOfSignalPos)
            {
                m_newSpace = m_space + pos.y - m_endOfSignalPos;
                SetDrawlets(pos.y);
            }
            else if ( pos.y > m_endOfSignalPos - m_space)
            {
                m_newSpace = pos.y - (m_endOfSignalPos - m_space);
                SetDrawlets(pos.y);
            }
            else
            {
                m_newSpace = 0;
                SetDrawlets(m_endOfSignalPos - m_space);
            }
        }
        return;
    }
    if ( event.ButtonUp(wxMOUSE_BTN_LEFT) && m_newSpace != -1)
    {
        if (m_newSpace == -1)
        {
            EndTask();
            return;
        }

        TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
        wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
        if (cmdproc)
            cmdproc->Submit(new ChangeSpaceCommand(doc, m_sig, m_newSpace, m_above));
        // wait on Update() to end this task
    }
}
void ChangeSignalSpacerTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}
void ChangeSignalSpacerTask::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}

void ChangeSignalSpacerTask::OnMouse(const wxMouseEvent &event)
{
    if (event.ButtonDown(wxMOUSE_BTN_ANY ))
        EndTask();
}

void ChangeSignalSpacerTask::LabelsKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
void ChangeSignalSpacerTask::WavesKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
void ChangeSignalSpacerTask::AxisKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}

void ChangeSignalSpacerTask::OnKey(const wxKeyEvent &event, bool down)
{
    if ( down && event.GetKeyCode() == WXK_ESCAPE)
        EndTask();
}


void ChangeSignalSpacerTask::Update()
{
    //command finished so return to main task
    EndTask();
}

