#include "AddVerticalLineTask.h"

#include "DiagramAxisWindow.h"
#include "DiagramLabelsWindow.h"
#include "DiagramWavesWindow.h"
#include "TimingView.h"
#include "TimingDoc.h"

#include "cmd.h"

#include "art/ruler_cur.xpm" // defines ruler_cur_xpm

AddVerticalLineTask::AddVerticalLineTask(const Task *task):
VerticalLineTask(task)
{
    //done by base
//    m_axisWin->RemoveDrawlet();
//    m_waveWin->RemoveDrawlet();
//    m_labelsWin->RemoveDrawlet();
    Init();
}
void AddVerticalLineTask::Init()
{
    state = waitingFirstPoint;

    wxBitmap cursBitmap(ruler_cur_xpm);
    wxImage cursImage = cursBitmap.ConvertToImage();
    cursImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 8);
    cursImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 2);
    wxCursor newcursor(cursImage);

    m_waveWin->SetCursor(newcursor, true);
    m_axisWin->SetCursor(*wxCROSS_CURSOR);
    m_labelsWin->SetCursor(wxNullCursor);
}

AddVerticalLineTask::~AddVerticalLineTask()
{
    //dtor
}


void AddVerticalLineTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if (event.Moving() && state == waitingFirstPoint )
    {
        Task::WavesMouse(event, pos);
    }
    else if ( event.Dragging() && state == waitingSecondPoint )
    {
        DoCheckMoving(pos);
    }
    else if (event.ButtonDown(wxMOUSE_BTN_LEFT) && state == waitingFirstPoint )
    {
        DoCheckWhenMouseDown(pos);
    }
    else if ( event.ButtonUp(wxMOUSE_BTN_LEFT) && state == waitingSecondPoint )
    {
        DoCheckWhenMouseUp(pos);
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
    HandleKey(event, down);
}
void AddVerticalLineTask::WavesKey(const wxKeyEvent &event, bool down)
{
    HandleKey(event, down);
}
void AddVerticalLineTask::AxisKey(const wxKeyEvent &event, bool down)
{
    HandleKey(event, down);
}
void AddVerticalLineTask::HandleKey(const wxKeyEvent &event, bool down)
{
    if ( /*down &&*/ event.GetKeyCode() == WXK_ESCAPE)
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
void AddVerticalLineTask::DoCheckWhenMouseDown(const wxPoint &pos)
{
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();

    if ( !doc || !doc->signals.size() )return;

    if (!IsOverWaves(pos) ) return;

    CalcXPos(pos);

    yStartPos = GetYPos(pos);
    yEndPos = yStartPos;

    m_axisWin->RemoveDrawlet();
    state = waitingSecondPoint;
    SetDrawlet();
}

void AddVerticalLineTask::DoCheckMoving(const wxPoint &pos)
{
//    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
//    if ( !doc || !doc->signals.size() )return;

    if (!IsOverWaves(pos) ) return;

    int endpos = GetYPos(pos);

    if ( yEndPos != endpos )
    {
        yEndPos = endpos;
        SetDrawlet();
    }
    return;
}

void AddVerticalLineTask::DoCheckWhenMouseUp(const wxPoint &pos)
{
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();

    if ( !doc || !doc->signals.size() )return;

    if (!IsOverWaves(pos) ) return;

    yEndPos = GetYPos(pos);

    if ( yEndPos != yStartPos )
    {
        VLine newline;
        wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
        if ( yStartPos < yEndPos )
        {
            newline.StartPos = yStartPos;
            newline.EndPos = yEndPos-1;
        }
        else
        {
            newline.StartPos = yEndPos;
            newline.EndPos = yStartPos-1;
        }
        newline.vpos = m_view->VisibleTicks[xpos];
        newline.vposoffset = xposoffset;

        cmdproc->Submit(new AddVLineCommand(
            doc, newline)
        );
    }
    state = waitingFirstPoint;
}
void AddVerticalLineTask::SetDrawlet()
{
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    wxPoint start, stop;
    start.x = m_view->GetWavesLeftSpace() + m_view->GridStepWidth*xpos;
    if (xposoffset == 1 )
        start.x += m_view->GridStepWidth*doc->TransitWidth/2/100;
    if (xposoffset == 2 )
        start.x += m_view->GridStepWidth*doc->TransitWidth/100;
    stop.x = start.x;

    wxInt32 ystart = yStartPos;
    wxInt32 yend = yEndPos;
    if( yStartPos > yEndPos)
    {
        yend = yStartPos;
        ystart = yEndPos;
    }
    start.y = m_view->heightOffsets[ystart];
    stop.y = m_view->heightOffsets[yend];

    m_waveWin->SetDrawlet( GetDrawlet(start, stop) );
}
void AddVerticalLineTask::Update()
{
    m_waveWin->RemoveDrawlet();
    Init();
}
