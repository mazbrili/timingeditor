#include "ActiveVerticalLineTask.h"

//#include <wx/clipbrd.h>
//#include <wx/dnd.h>

#include "TimingDoc.h"
#include "TimingView.h"
#include "cmd.h"

//#include "HoverHatch.h"
#include "HoverCombo.h"
#include "HoverLine.h"
//#include "OnceDrawlet.h"
//#include "HoverGraphCaret.h"
#include "DiagramWavesWindow.h"
//#include "DiagramLabelsWindow.h"


ActiveVerticalLineTask::ActiveVerticalLineTask(const Task *task, int verticalLineIdx):
VerticalLineTask(task),
m_verticalLineIdx(verticalLineIdx),
state(activeLine)
{
    SetDrawlets();
    m_doc = (TimingDocument *)m_view->GetDocument();
}

ActiveVerticalLineTask::~ActiveVerticalLineTask(){}

void ActiveVerticalLineTask::SetDrawlets()
{
    if ( state == activeLine )
    {
        GraphVerticalLine gvline = m_view->GetGraphVerticalLines()[m_verticalLineIdx];
        HoverDrawlet *drawlet =
        gvline.GetActiveDrawlet( m_view->GetActiveGraphCaretColour() );
        m_waveWin->SetDrawlet(drawlet);
        return;
    }
    if ( state == movingStartPos )
    {
        if (old_yStart != yStartPos )
        {
            old_yStart = yStartPos;

            wxPoint start, stop;
            start.x = m_view->GetWavesLeftSpace() + m_view->GridStepWidth*xpos;
            if (xposoffset == 1 )
                start.x += m_view->GridStepWidth*m_doc->TransitWidth/2/100;
            if (xposoffset == 2 )
                start.x += m_view->GridStepWidth*m_doc->TransitWidth/100;
            stop.x = start.x;

            start.y = m_view->heightOffsets[yStartPos];
            stop.y = m_view->heightOffsets[m_doc->verticalLines[m_verticalLineIdx].EndPos+1];

            m_waveWin->SetDrawlet(
                new HoverCombo(
                    GetDrawlet(start, stop),
                    m_view->GetGraphVerticalLines()[m_verticalLineIdx].GetLineDrawlet(*wxLIGHT_GREY, wxSHORT_DASH)
                )
            );
        }
        return;
    }
    if ( state == movingEndPos )
    {
        if (old_yStop != yEndPos )
        {
            old_yStop = yEndPos;

            wxPoint start, stop;
            start.x = m_view->GetWavesLeftSpace() + m_view->GridStepWidth*xpos;
            if (xposoffset == 1 )
                start.x += m_view->GridStepWidth*m_doc->TransitWidth/2/100;
            if (xposoffset == 2 )
                start.x += m_view->GridStepWidth*m_doc->TransitWidth/100;
            stop.x = start.x;

            start.y = m_view->heightOffsets[m_doc->verticalLines[m_verticalLineIdx].StartPos];
            stop.y = m_view->heightOffsets[yEndPos];

            m_waveWin->SetDrawlet(
                new HoverCombo(
                    GetDrawlet(start, stop),
                    m_view->GetGraphVerticalLines()[m_verticalLineIdx].GetLineDrawlet(*wxLIGHT_GREY, wxSHORT_DASH)
                )
            );
        }
        return;
    }
    if ( state == movingLine)
    {
        if ( old_xpos != xpos || old_xposoffset != xposoffset)
        {
            old_xpos = xpos;
            old_xposoffset = xposoffset;

            wxPoint start, stop;
            start.x = m_view->GetWavesLeftSpace() + m_view->GridStepWidth*xpos;
            if (xposoffset == 1 )
                start.x += m_view->GridStepWidth*m_doc->TransitWidth/2/100;
            if (xposoffset == 2 )
                start.x += m_view->GridStepWidth*m_doc->TransitWidth/100;
            stop.x = start.x;

            start.y = m_view->heightOffsets[m_doc->verticalLines[m_verticalLineIdx].StartPos];
            stop.y = m_view->heightOffsets[m_doc->verticalLines[m_verticalLineIdx].EndPos+1];

            m_waveWin->SetDrawlet(
                new HoverCombo(
                    GetDrawlet(start, stop),
                    m_view->GetGraphVerticalLines()[m_verticalLineIdx].GetLineDrawlet(*wxLIGHT_GREY, wxSHORT_DASH)
                )
            );
        }
        return;
    }

}
void ActiveVerticalLineTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if ( event.ButtonDown(wxMOUSE_BTN_RIGHT) )
    {
        EndTask();
        return;
    }
    if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        int k = IsOverVerticalLine(pos);
        if ( k == -1 )
        {
            EndTask();
            return;
        }
        if ( m_verticalLineIdx == k )
        {
            const int verticalLineSnapTolerance = GetVerticalLineSnapTolerance();
            GraphVerticalLine gvline = m_view->GetGraphVerticalLines()[m_verticalLineIdx];
            if ( gvline.IsStartPoint(pos, verticalLineSnapTolerance))
            {
                state = movingStartPos;
                yStartPos = GetYPos(pos);
                old_yStart = yStartPos;
                xpos = m_doc->verticalLines[m_verticalLineIdx].vpos;
                xposoffset = m_doc->verticalLines[m_verticalLineIdx].vposoffset;
                SetDrawlets();
                return;
            }
            if ( gvline.IsStopPoint(pos, verticalLineSnapTolerance))
            {
                state = movingEndPos;
                yEndPos = GetYPos(pos);
                old_yStop = yEndPos;
                xpos = m_doc->verticalLines[m_verticalLineIdx].vpos;
                xposoffset = m_doc->verticalLines[m_verticalLineIdx].vposoffset;
                SetDrawlets();
                return;
            }
            CalcXPos(pos);
            state = movingLine;
            old_xpos = xpos;
            old_xposoffset = xposoffset;
            SetDrawlets();
            return;
        }
        state = activeLine;
        m_verticalLineIdx = k;
        SetDrawlets();
        return;
    }

    if (event.Dragging() && event.ButtonIsDown(wxMOUSE_BTN_LEFT))
    {
        if ( state == movingLine )
        {
            CalcXPos(pos);
            SetDrawlets();
            return;
        }
        if ( state == movingStartPos )
        {
            yStartPos = GetYPos(pos);
            for ( unsigned int arrowidx = 0 ; arrowidx < m_doc->horizontalArrows.size() ; arrowidx++ )
            {
                //check if the indexed line is connected by an arrow
                if ( m_verticalLineIdx == m_doc->horizontalArrows[arrowidx].fromVerticalLine ||
                     m_verticalLineIdx == m_doc->horizontalArrows[arrowidx].toVerticalLine )
                {
                    if (m_doc->horizontalArrows[arrowidx].signalnmbr > yStartPos)
                        yStartPos = m_doc->horizontalArrows[arrowidx].signalnmbr;
                }
            }
            if ( yStartPos == m_doc->verticalLines[m_verticalLineIdx].EndPos+1)
                yStartPos = m_doc->verticalLines[m_verticalLineIdx].StartPos;
            SetDrawlets();
            return;
        }
        if ( state == movingEndPos)
        {
            yEndPos = GetYPos(pos);
            for ( unsigned int arrowidx = 0 ; arrowidx < m_doc->horizontalArrows.size() ; arrowidx++ )
            {
                //check if the indexed line is connected by an arrow
                if ( m_verticalLineIdx == m_doc->horizontalArrows[arrowidx].fromVerticalLine ||
                     m_verticalLineIdx == m_doc->horizontalArrows[arrowidx].toVerticalLine )
                {
                    if ( m_doc->horizontalArrows[arrowidx].signalnmbr > yEndPos)
                        yEndPos = m_doc->horizontalArrows[arrowidx].signalnmbr;
                }
            }
            if ( yEndPos ==  m_doc->verticalLines[m_verticalLineIdx].StartPos)
                yEndPos = m_doc->verticalLines[m_verticalLineIdx].EndPos;
            SetDrawlets();
            return;
        }
    }

    if (event.ButtonUp(wxMOUSE_BTN_LEFT) )
    {
        if ( state == movingLine )
        {
            CalcXPos(pos);
            if ( m_doc->verticalLines[m_verticalLineIdx].vpos != xpos || m_doc->verticalLines[m_verticalLineIdx].vposoffset != xposoffset)
            {
                m_doc->GetCommandProcessor()->Submit(
                    new ChangeVerticalLineCommand(m_doc, m_verticalLineIdx, xpos, m_doc->verticalLines[m_verticalLineIdx].StartPos,
                                           m_doc->verticalLines[m_verticalLineIdx].EndPos, xposoffset));
            }
            else
                state = activeLine;

            return;
        }
        if ( state == movingStartPos )
        {
            if (yStartPos != m_doc->verticalLines[m_verticalLineIdx].StartPos)
            {
                m_doc->GetCommandProcessor()->Submit(
                    new ChangeVerticalLineCommand(m_doc, m_verticalLineIdx, m_doc->verticalLines[m_verticalLineIdx].vpos,
                                           yStartPos,
                                           m_doc->verticalLines[m_verticalLineIdx].EndPos,
                                           m_doc->verticalLines[m_verticalLineIdx].vposoffset));
            }
            else
                state = activeLine;
            return;
        }
        if ( state == movingEndPos)
        {
            if ( yEndPos != m_doc->verticalLines[m_verticalLineIdx].EndPos)
            {
                m_doc->GetCommandProcessor()->Submit(
                    new ChangeVerticalLineCommand(m_doc, m_verticalLineIdx, m_doc->verticalLines[m_verticalLineIdx].vpos,
                                           m_doc->verticalLines[m_verticalLineIdx].StartPos,
                                           yEndPos-1,
                                           m_doc->verticalLines[m_verticalLineIdx].vposoffset));
            }
            else
                state = activeLine;
            return;
        }
    }

    if (event.Leaving() && (state == movingLine || state == movingStartPos || state == movingEndPos) )
    {
        state = activeLine;
        SetDrawlets();
    }

}
void ActiveVerticalLineTask::Delete()
{
    if (CanDelete())
    {
        state = deleting;
        wxCommandProcessor *cmdproc = m_view->GetDocument()->GetCommandProcessor();
        if (cmdproc)
            cmdproc->Submit(new DeleteVerticalLineCommand(m_doc, m_verticalLineIdx));
    }
}
bool ActiveVerticalLineTask::CanDelete()const
{
    return state == activeLine;
}
void ActiveVerticalLineTask::Update()
{
    if ( state == deleting )
    {
        EndTask();
        return;
    }
    if (state == movingLine || state == movingEndPos || state == movingStartPos )
    {
        state = activeLine;
        SetDrawlets();
    }
}

void ActiveVerticalLineTask::OnMouse(const wxMouseEvent &event)
{
    if (event.ButtonDown(wxMOUSE_BTN_ANY ))
        EndTask();
}
void ActiveVerticalLineTask::OnKey(const wxKeyEvent &event, bool down)
{
    if ( down && event.GetKeyCode() == WXK_DELETE)
    {
        if ( CanDelete())
        {
            Delete();
            return;
        }
    }
    if ( down && event.GetKeyCode() == WXK_ESCAPE)
        EndTask();
}
void ActiveVerticalLineTask::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}
void ActiveVerticalLineTask::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}
void ActiveVerticalLineTask::LabelsKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
void ActiveVerticalLineTask::WavesKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
void ActiveVerticalLineTask::AxisKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
