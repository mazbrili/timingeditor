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
state(activeArrow),
m_isValidMove(false)
{
    m_doc = (TimingDocument *)m_view->GetDocument();
    Init();
}
void ActiveHorizontalArrowTask::Init()
{
    SetDrawlets();
    m_verticalLine = m_doc->horizontalArrows[m_horizontalArrowIdx].fromVerticalLine;
    m_secondVerticalLine = m_doc->horizontalArrows[m_horizontalArrowIdx].toVerticalLine;
}

ActiveHorizontalArrowTask::~ActiveHorizontalArrowTask(){}

void ActiveHorizontalArrowTask::SetDrawlets()
{
    if (state == activeArrow)
    {
        GraphHorizontalArrow gharrow = m_view->GetGraphHorizontalArrows()[m_horizontalArrowIdx];
        HoverDrawlet *drawlet = GetActiveDrawlet(gharrow.GetStartPoint(), gharrow.GetStopPoint(),
                                                 wxTRANSPARENT, m_view->GetActiveGraphCaretColour(), gharrow.GetTextPoint(), wxSOLID);
        m_waveWin->SetDrawlet(drawlet);
        return;
    }
    if ( state == movingStartPos )
    {
        GraphHorizontalArrow gharrow = m_view->GetGraphHorizontalArrows()[m_horizontalArrowIdx];
        HoverDrawlet *drawlet = GetActiveDrawlet(wxPoint(m_pos.x,gharrow.GetStopPoint().y), gharrow.GetStopPoint(), wxSOLID, m_view->GetActiveGraphCaretColour());//, gharrow.GetTextPoint(), wxSOLID);
        m_waveWin->SetDrawlet(drawlet);
        return;
    }
    if ( state == movingEndPos )
    {
        GraphHorizontalArrow gharrow = m_view->GetGraphHorizontalArrows()[m_horizontalArrowIdx];
        HoverDrawlet *drawlet = GetActiveDrawlet(gharrow.GetStartPoint(), wxPoint(m_pos.x, gharrow.GetStartPoint().y ), wxSOLID, m_view->GetActiveGraphCaretColour());//, gharrow.GetTextPoint(), wxSOLID);
        m_waveWin->SetDrawlet(drawlet);
        return;
    }
    if ( state == movingArrow)
    {
        GraphHorizontalArrow gharrow = m_view->GetGraphHorizontalArrows()[m_horizontalArrowIdx];
        HoverDrawlet *drawlet = GetActiveDrawlet(wxPoint(gharrow.GetStartPoint().x, m_pos.y), wxPoint(gharrow.GetStopPoint().x, m_pos.y), wxSHORT_DASH, m_view->GetActiveGraphCaretColour());//, gharrow.GetTextPoint(), wxSOLID);
        m_waveWin->SetDrawlet(drawlet);
        return;
    }
    if ( state == movingText )
    {
        GraphHorizontalArrow gharrow = m_view->GetGraphHorizontalArrows()[m_horizontalArrowIdx];
        HoverDrawlet *drawlet = GetActiveDrawlet(gharrow.GetStartPoint(), gharrow.GetStopPoint(),
                                                 wxTRANSPARENT, m_view->GetActiveGraphCaretColour(), m_pos, wxSHORT_DASH);
        m_waveWin->SetDrawlet(drawlet);
        return;
    }
}

void ActiveHorizontalArrowTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    GraphHorizontalArrow gharrow = m_view->GetGraphHorizontalArrows()[m_horizontalArrowIdx];

    if ( event.ButtonDown(wxMOUSE_BTN_RIGHT) )
    {
        EndTask();
        return;
    }
    if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        const int snapTolerance = GetHorizontalArrowSnapTolerance();

        // check if position of text is clicked
        if( gharrow.IsTextPoint(pos, snapTolerance))
        {
            ::wxLogMessage(_T("ActiveSignalTask::text"));
            state = movingText;
            m_pos = gharrow.GetTextPoint();
            CheckMovingText(pos);
            SetDrawlets();
            return;
        }

        // is a click on an arrow?
        int k = IsOverHorizontalArrow(pos);
        if ( k == -1 )
        {
            EndTask();
            return;
        }
        if ( m_horizontalArrowIdx == k )
        {
            // check if start point is clicked
            if( gharrow.IsStartPoint(pos, snapTolerance) )
            {
                ::wxLogMessage(_T("ActiveSignalTask::start"));
                state = movingStartPos;
                m_pos = gharrow.GetStartPoint();
                CheckMovingStart(pos);
                SetDrawlets();
                return;
            }
            // check if stop point is clicked
            if ( gharrow.IsStopPoint(pos, snapTolerance) )
            {
                ::wxLogMessage(_T("ActiveSignalTask::stop"));
                state = movingEndPos;
                m_pos = gharrow.GetStopPoint();
                CheckMovingStop(pos);
                SetDrawlets();
                return;
            }
            state = movingArrow;
            m_pos = wxPoint(pos.x, gharrow.GetStartPoint().y);
            CheckMovingArrow(pos);
            SetDrawlets();
            return;
        }
        else
        {
            /// ????
            m_horizontalArrowIdx = k;
            Init();
            SetDrawlets();
            return;
        }
    }

    if (event.Dragging() && event.ButtonIsDown(wxMOUSE_BTN_LEFT))
    {
        ///
        switch(state)
        {
            case movingText:
                CheckMovingText(pos);
                break;
            case movingArrow:
                CheckMovingArrow(pos);
                break;
            case movingStartPos:
                CheckMovingStart(pos);
                break;
            case movingEndPos:
                CheckMovingStop(pos);
                break;
            default:
                return;
        }

        SetDrawlets();
        return;
    }

    if (event.ButtonUp(wxMOUSE_BTN_LEFT) )
    {
        ///
    }
    if (event.Entering() && !event.ButtonIsDown(wxMOUSE_BTN_LEFT) )
    {
        state = activeArrow;
        Init();
        SetDrawlets();
    }
//    if (event.Leaving() && (state == movingArrow || state == movingStartPos || state == movingEndPos || state == movingText) )
//    {
//        state = activeArrow;
//        SetDrawlets();
//    }

}

void ActiveHorizontalArrowTask::CheckMovingArrow(const wxPoint &pos)
{
    //"top" has the smaller y position than "bottom"
    int top = m_doc->verticalLines[m_verticalLine].StartPos;
    int top2 = m_doc->verticalLines[m_secondVerticalLine].StartPos;
    if ( top2 > top )
        top = top2; // take the "lower" as the top line
    int bot = m_doc->verticalLines[m_verticalLine].EndPos+1;
    int bot2 = m_doc->verticalLines[m_secondVerticalLine].EndPos+1;
    if (bot2 < bot)
        bot = bot2; // take the smaller as the bottom line

    top = m_view->heightOffsets[top];
    bot = m_view->heightOffsets[bot];

    m_isValidMove = pos.y > top && pos.y < bot;

    if (m_isValidMove)
        m_pos = pos;
}
void ActiveHorizontalArrowTask::CheckMovingStart(const wxPoint &pos)
{

}
void ActiveHorizontalArrowTask::CheckMovingStop(const wxPoint &pos)
{

}
void ActiveHorizontalArrowTask::CheckMovingText(const wxPoint &pos)
{
    m_isValidMove = IsOverWaves(pos);
    if (m_isValidMove)
        m_pos = pos;
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




