#include "ActiveHorizontalArrowTask.h"

#include "TimingDoc.h"
#include "TimingView.h"
#include "cmd.h"

//#include "HoverHatch.h"
//#include "HoverCombo.h"
//#include "HoverLine.h"
//#include "OnceDrawlet.h"
//#include "HoverGraphCaret.h"
#include "DiagramWavesWindow.h"
//#include "DiagramLabelsWindow.h"

ActiveHorizontalArrowTask::ActiveHorizontalArrowTask(const Task *task, int horizontalArrowIdx):
HorizontalArrowTask(task),
m_horizontalArrowIdx(horizontalArrowIdx),
state(activeArrow)
{
    SetDrawlets();
    m_doc = (TimingDocument *)m_view->GetDocument();
}

ActiveHorizontalArrowTask::~ActiveHorizontalArrowTask(){}

void ActiveHorizontalArrowTask::SetDrawlets()
{
    if (state == activeArrow)
    {
        GraphHorizontalArrow gharrow = m_view->GetGraphHorizontalArrows()[m_horizontalArrowIdx];
        HoverDrawlet *drawlet =
        gharrow.GetActiveDrawlet( m_view->GetActiveGraphCaretColour() );
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
    if ( state == movingArrow)
    {
        return;
    }
    if ( state == movingText )
    {
        return;
    }
}

void ActiveHorizontalArrowTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if ( event.ButtonDown(wxMOUSE_BTN_RIGHT) )
    {
        EndTask();
        return;
    }
    if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        ///
        int k = IsOverHorizontalArrow(pos);
        if ( k == -1 )
        {
            EndTask();
            return;
        }
        if ( m_horizontalArrowIdx == k )
        {
            const int snapTolerance = GetHorizontalArrowSnapTolerance();
            GraphHorizontalArrow gharrow = m_view->GetGraphHorizontalArrows()[m_horizontalArrowIdx];
            // check if position of text is clicked
            if( gharrow.IsTextPoint(pos, snapTolerance))
            {
                state = movingText;

                SetDrawlets();
                return;
            }
            // check if start point is clicked
            if( gharrow.IsStartPoint(pos, snapTolerance) )
            {
                state = movingStartPos;


                SetDrawlets();
                return;
            }
            // check if stop point is clicked
            if ( gharrow.IsStopPoint(pos, snapTolerance) )
            {
                state = movingEndPos;

                SetDrawlets();
                return;
            }
            state = movingArrow;
            SetDrawlets();
            return;
        }
    }

    if (event.Dragging() && event.ButtonIsDown(wxMOUSE_BTN_LEFT))
    {
        ///
    }

    if (event.ButtonUp(wxMOUSE_BTN_LEFT) )
    {
        ///
    }

    if (event.Leaving() && (state == movingArrow || state == movingStartPos || state == movingEndPos || state == movingText) )
    {
        state = activeArrow;
        SetDrawlets();
    }

}
void ActiveHorizontalArrowTask::Delete()
{
    if (CanDelete())
    {
        state = deleting;
        wxCommandProcessor *cmdproc = m_view->GetDocument()->GetCommandProcessor();
        if (cmdproc)
            cmdproc->Submit(new DeleteHorizontalArrowCommand(m_doc, m_horizontalArrowIdx));
    }
}
bool ActiveHorizontalArrowTask::CanDelete()
{
    return state == activeArrow;
}
void ActiveHorizontalArrowTask::Update()
{
    if ( state == deleting )
    {
        EndTask();
        return;
    }
    if (state == movingArrow || state == movingEndPos || state == movingStartPos || state == movingText )
    {
        state = activeArrow;
        SetDrawlets();
    }
}

void ActiveHorizontalArrowTask::OnMouse(const wxMouseEvent &event)
{
    //::wxLogMessage(_T("ActiveSignalTask::OnMouse"));
    if (event.ButtonDown(wxMOUSE_BTN_ANY ))
        EndTask();
}
void ActiveHorizontalArrowTask::OnKey(const wxKeyEvent &event, bool down)
{
    if ( down && event.GetKeyCode() == WXK_DELETE)
    {
        if ( CanDelete())
            Delete();
        return;
    }
    if ( down && event.GetKeyCode() == WXK_ESCAPE)
        EndTask();
}
void ActiveHorizontalArrowTask::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}
void ActiveHorizontalArrowTask::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}
void ActiveHorizontalArrowTask::LabelsKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
void ActiveHorizontalArrowTask::WavesKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
void ActiveHorizontalArrowTask::AxisKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}




