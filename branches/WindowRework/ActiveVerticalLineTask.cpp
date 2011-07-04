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


ActiveVerticalLineTask::ActiveVerticalLineTask(const Task *task, int vlineidx):
VerticalLineTask(task),
m_vlineidx(vlineidx),
state(activeLine)
{
    SetDrawlets();
}

ActiveVerticalLineTask::~ActiveVerticalLineTask(){}

void ActiveVerticalLineTask::SetDrawlets()
{
    if ( state == activeLine )
    {
        GraphVerticalLine gvline = m_view->GetVerticalLines()[m_vlineidx];
        HoverDrawlet *drawlet =
        gvline.GetActiveDrawlet( m_view->GetActiveGraphCaretColour() );
        m_waveWin->SetDrawlet(drawlet);
        return;
    }
    if ( state == movingStartPos )
    {
        return;
    }
    if ( state == movingEndPos )
    {
        return;
    }
    if ( state == movingLine)
    {
        //::wxLogMessage(_T("ActiveVerticalLineTask::SetDrawlets movingLine "));

        if ( old_xpos != xpos || old_xposoffset != xposoffset)
        {
            old_xpos = xpos;
            old_xposoffset = xposoffset;
            TimingDocument *doc = (TimingDocument *)m_view->GetDocument();

            wxPoint start, stop;
            start.x = m_view->GetWavesLeftSpace() + m_view->GridStepWidth*xpos;
            if (xposoffset == 1 )
                start.x += m_view->GridStepWidth*doc->TransitWidth/2/100;
            if (xposoffset == 2 )
                start.x += m_view->GridStepWidth*doc->TransitWidth/100;
            stop.x = start.x;

            start.y = m_view->heightOffsets[doc->vertlines[m_vlineidx].StartPos];
            stop.y = m_view->heightOffsets[doc->vertlines[m_vlineidx].EndPos+1];

            m_waveWin->SetDrawlet(
                new HoverCombo(
                    GetDrawlet(start, stop),
                    m_view->GetVerticalLines()[m_vlineidx].GetLineDrawlet(*wxLIGHT_GREY, wxSHORT_DASH)
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
        if ( m_vlineidx == k )
        {
            GraphVerticalLine gvline = m_view->GetVerticalLines()[m_vlineidx];
            if ( gvline.IsStartPoint(pos, GetVerticalLineSnapTolerance()))
            {
                state = movingStartPos;
                SetDrawlets();
                return;
            }
            if ( gvline.IsStopPoint(pos, GetVerticalLineSnapTolerance()))
            {
                state = movingEndPos;
                SetDrawlets();
                return;
            }
            state = movingLine;
            CalcXPos(pos);
            old_xpos = xpos;
            old_xposoffset = xposoffset;
            SetDrawlets();
            return;
        }
        state = activeLine;
        m_vlineidx = k;
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
            return;
        }
        if ( state == movingEndPos)
        {
            yEndPos = GetYPos(pos);
            return;
        }
    }

    if (event.ButtonUp(wxMOUSE_BTN_LEFT) )
    {
        if ( state == movingLine )
        {
            CalcXPos(pos);
            TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
            if ( doc->vertlines[m_vlineidx].vpos != xpos || doc->vertlines[m_vlineidx].vposoffset != xposoffset)
            {
                doc->GetCommandProcessor()->Submit(
                    new ChangeVLineCommand(doc, m_vlineidx, xpos, doc->vertlines[m_vlineidx].StartPos,
                                           doc->vertlines[m_vlineidx].EndPos, xposoffset));
            }
            else
                state = activeLine;

            return;
        }
        if ( state == movingStartPos )
        {
            return;
        }
        if ( state == movingEndPos)
        {
            return;
        }
    }

    if (event.Leaving() && (state == movingLine || state == movingStartPos || state == movingEndPos) )
    {
        state = activeLine;
        SetDrawlets();
    }

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

bool ActiveVerticalLineTask::CanDelete()
{
    return state == activeLine;
}
void ActiveVerticalLineTask::Delete()
{
    if (CanDelete())
    {
        state = deleting;
        wxCommandProcessor *cmdproc = m_view->GetDocument()->GetCommandProcessor();
        if (cmdproc)
            cmdproc->Submit(new DeleteVLineCommand((TimingDocument *)m_view->GetDocument(), m_vlineidx));
    }
}

void ActiveVerticalLineTask::Update()
{
    if ( state == deleting )
    {
        EndTask();
        return;
    }
    if (state == movingLine )
    {
        state = activeLine;
        SetDrawlets();
    }
}


void ActiveVerticalLineTask::EndTask()
{
    m_view->SetTask(NULL);
}
void ActiveVerticalLineTask::OnMouse(const wxMouseEvent &event)
{
    //::wxLogMessage(_T("ActiveSignalTask::OnMouse"));
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