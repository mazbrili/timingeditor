#include "AddVerticalLineTask.h"

#include "DiagramAxisWindow.h"
#include "DiagramLabelsWindow.h"
#include "DiagramWavesWindow.h"
#include "TimingView.h"
#include "HoverCombo.h"
#include "HoverGraphCaret.h"
#include "HoverLine.h"

#include "cmd.h"

#include "art/ruler_cur.xpm" // defines ruler_cur_xpm

AddVerticalLineTask::AddVerticalLineTask(TimingView *view, DiagramLabelsWindow *labelsWin, DiagramAxisWindow *axisWin, DiagramWavesWindow *waveWin):
Task(view, labelsWin, axisWin, waveWin)
{
    InitTask();
}
void AddVerticalLineTask::InitTask()
{
    m_axisWin->RemoveDrawlet();
    m_waveWin->RemoveDrawlet();
    m_labelsWin->RemoveDrawlet();
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
}
void AddVerticalLineTask::DoCheckWhenMouseDown(const wxPoint &pos)
{
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();

    if ( !doc || !doc->signals.size() )return;

    if (!IsOverWaves(pos) ) return;

    xpos = GetTickFromPosition(pos);

    //xpos = m_view->VisibleTicks[tick];

    wxCoord p = pos.x - m_view->GetWavesLeftSpace() - xpos*m_view->GridStepWidth;
    if ( doc->en50 && doc->en90 )
    {
        if      ( p < m_view->GridStepWidth*(doc->TransitWidth/4.0)/100.0 )
            xposoffset = 0;
        else if ( p < m_view->GridStepWidth*((3.0*doc->TransitWidth/4.0)/100.0) )
            xposoffset = 1;
        else if ( p < m_view->GridStepWidth*(50.0+doc->TransitWidth/2.0)/100.0 )
            xposoffset = 2;
        else
        {
            xpos++;
            xposoffset = 0;
        }
    }
    else if ( doc->en50 )
    {
        if ( p < m_view->GridStepWidth*(doc->TransitWidth/4.0)/100.0 )
            xposoffset = 0;
        else if ( p < m_view->GridStepWidth*((50.0+doc->TransitWidth/4.0)/100.0) )
            xposoffset = 1;
        else
        {
            xpos++;
            xposoffset = 0;
        }
    }
    else if ( doc->en90 )
    {
        if ( p < m_view->GridStepWidth*(doc->TransitWidth/2.0)/100.0 )
            xposoffset = 0;
        else if ( p < m_view->GridStepWidth * (50.0+doc->TransitWidth/2.0) / 100.0 )
            xposoffset = 2;
        else
        {
            xpos++;
            xposoffset = 0;
        }
    }
    else
    {
        if ( p > m_view->GridStepWidth/2 )
            xpos++;
        xposoffset = 0;
    }

    unsigned int k;
    for ( k = 0 ; k < m_view->heightOffsets.size()-1 ; ++k )
        if ( pos.y < (m_view->heightOffsets[k] + m_view->heightOffsets[k+1])/2 )
            break;

    if ( pos.y > m_view->heightOffsets[k] )
    {
        // clicked top of signal
        yStartPos = k;
        yEndPos = k+1;
    }
    else
    {
        // clicked bottom of previous signal
        yStartPos = k;
        yEndPos = k - 1;
    }
    //wxLogDebug(_("ybeg: %d, yend: %d, xpos: %d"), yStartPos, yEndPos, xpos);
    m_axisWin->RemoveDrawlet();
    state = waitingSecondPoint;
    SetDrawlet();
}

void AddVerticalLineTask::DoCheckMoving(const wxPoint &pos)
{
    wxInt32 endpos;
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();

    if ( !doc || !doc->signals.size() )return;

    if (!IsOverWaves(pos) ) return;

    unsigned int k;
    for ( k = 0 ; k < m_view->heightOffsets.size()-1 ; ++k )
        if ( pos.y < (m_view->heightOffsets[k] + m_view->heightOffsets[k+1])/2 )
            break;

    endpos = k;

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


    unsigned int k;
    for ( k = 0 ; k < m_view->heightOffsets.size()-1 ; ++k )
        if ( pos.y < (m_view->heightOffsets[k] + m_view->heightOffsets[k+1])/2 )
            break;

    yEndPos = k;

    if ( yEndPos != yStartPos )
    {
        VLine newline;
        wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
        if ( yEndPos < yStartPos )
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

    start.y = m_view->heightOffsets[yStartPos];
    stop.y = m_view->heightOffsets[yEndPos];

    m_waveWin->SetDrawlet(
        new HoverCombo(
            new HoverCombo(
                new HoverGraphCaret(start - wxPoint(3,3),wxSize(7,7), m_view->GetGraphCaretColour()),
                new HoverGraphCaret(stop - wxPoint(3,3),wxSize(7,7), m_view->GetGraphCaretColour())
            ),
            new HoverLine(start, stop, *wxLIGHT_GREY)
        )
    );
}
