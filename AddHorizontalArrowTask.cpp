#include "AddHorizontalArrowTask.h"


#include "DiagramAxisWindow.h"
#include "DiagramLabelsWindow.h"
#include "DiagramWavesWindow.h"
#include "TimingView.h"
#include "TimingDoc.h"
#include "cmd.h"

#include "art/harrow_cur.xpm"

AddHorizontalArrowTask::AddHorizontalArrowTask(const Task *task):
HorizontalArrowTask(task)
{
    //done by base
//    m_axisWin->RemoveDrawlet();
//    m_waveWin->RemoveDrawlet();
//    m_labelsWin->RemoveDrawlet();
    Init();
}

AddHorizontalArrowTask::~AddHorizontalArrowTask()
{
    //dtor
}

void AddHorizontalArrowTask::Init()
{
    state = waitingFirstPoint;

    wxBitmap cursBitmap(harrow_cur_xpm);
    wxImage cursImage = cursBitmap.ConvertToImage();
    cursImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 7);
    cursImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 6);
    wxCursor newcursor(cursImage);

    m_waveWin->SetCursor(newcursor, true);
    m_axisWin->SetCursor(*wxCROSS_CURSOR);
    m_labelsWin->SetCursor(wxNullCursor);
}

void AddHorizontalArrowTask::LabelsKey(const wxKeyEvent &event, bool down)
{
    HandleKey(event, down);
}
void AddHorizontalArrowTask::WavesKey(const wxKeyEvent &event, bool down)
{
    HandleKey(event, down);
}
void AddHorizontalArrowTask::AxisKey(const wxKeyEvent &event, bool down)
{
    HandleKey(event, down);
}
void AddHorizontalArrowTask::HandleKey(const wxKeyEvent &event, bool down)
{
    if ( down && event.GetKeyCode() == WXK_ESCAPE)
        EndTask();
}
void AddHorizontalArrowTask::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    NonInvolvedWindowMouse(event);
}
void AddHorizontalArrowTask::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    NonInvolvedWindowMouse(event);
}
void AddHorizontalArrowTask::NonInvolvedWindowMouse(const wxMouseEvent &event)
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

void AddHorizontalArrowTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if (( event.Moving() || event.Dragging() )&& state == waitingFirstPoint )
    {
        ::wxLogMessage(_T("AddHorizontalArrowTask::WavesMouse Task::WavesMouse"));
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

void AddHorizontalArrowTask::DoCheckWhenMouseDown(const wxPoint &pos)
{
    if (!IsOverWaves(pos) ) return;

    int verticalLine = IsOverVerticalLine(pos);
    if (verticalLine != -1)
    {
        CalcYPos(pos);
        m_verticalLine = verticalLine;
        m_axisWin->RemoveDrawlet();
        state = waitingSecondPoint;
        m_secondVerticalLine = -1;
        SetInsertingDrawlet(pos);
    }
}

void AddHorizontalArrowTask::DoCheckMoving(const wxPoint &pos)
{
    //if (!IsOverWaves(pos) ) return;

    int old_secondVerticalLine = m_secondVerticalLine;
    m_secondVerticalLine = IsOverVerticalLine(wxPoint(pos.x, m_view->heightOffsets[m_ypos] + m_yposoffset ));

    if (m_secondVerticalLine == m_verticalLine)
    {
        m_waveWin->RemoveDrawlet();
        return;
    }

    if (m_secondVerticalLine == -1 || old_secondVerticalLine != m_secondVerticalLine)
        SetInsertingDrawlet(pos);
    return;
}

void AddHorizontalArrowTask::DoCheckWhenMouseUp(const wxPoint &pos)
{
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();

    if ( !doc )return;

    if (state != waitingSecondPoint ) return;

    if (m_secondVerticalLine != m_verticalLine && m_secondVerticalLine != -1)
    {
        HorizontalArrow newha;
        wxCommandProcessor *cmdproc = doc->GetCommandProcessor();

        newha.fromVerticalLine = m_verticalLine;
        newha.toVerticalLine = m_secondVerticalLine;
        newha.text = _("$t$");
        newha.textoffset.x = 0;
        newha.textoffset.y = -10;
        newha.textgridoffset = 0;
        newha.pos = m_yposoffset;
        newha.signalnmbr = m_ypos;
        cmdproc->Submit(new AddHorizonalArrowCommand(doc, newha));
    }

    state = waitingFirstPoint;
}
void AddHorizontalArrowTask::SetInsertingDrawlet(const wxPoint &pos)
{
    //::wxLogMessage(_T("AddHorizontalArrowTask::SetInsertingDrawlet"));
    wxPoint start, stop;
    start.y = m_view->heightOffsets[m_ypos] + m_yposoffset;
    start.x = m_view->GetGraphVerticalLines()[m_verticalLine].GetStartPoint().x;
    stop.y = start.y;
    if ( m_secondVerticalLine == -1)
        stop.x = pos.x;
    else
        stop.x = m_view->GetGraphVerticalLines()[m_secondVerticalLine].GetStartPoint().x;

    m_waveWin->SetDrawlet(GetDrawlet(start, stop, m_secondVerticalLine != -1 ? wxSOLID : wxSHORT_DASH ));
}

void AddHorizontalArrowTask::Update()
{
    m_waveWin->RemoveDrawlet();
    Init();
}

