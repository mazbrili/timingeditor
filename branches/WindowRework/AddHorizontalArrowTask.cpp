#include "AddHorizontalArrowTask.h"


#include "DiagramAxisWindow.h"
#include "DiagramLabelsWindow.h"
#include "DiagramWavesWindow.h"
#include "TimingView.h"
#include "TimingDoc.h"

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

void AddHorizontalArrowTask::DoCheckWhenMouseDown(const wxPoint &pos)
{
}

void AddHorizontalArrowTask::DoCheckMoving(const wxPoint &pos)
{
}

void AddHorizontalArrowTask::DoCheckWhenMouseUp(const wxPoint &pos)
{
}
void AddHorizontalArrowTask::SetDrawlet()
{

}
void AddHorizontalArrowTask::Update()
{
    m_waveWin->RemoveDrawlet();
    Init();
}

