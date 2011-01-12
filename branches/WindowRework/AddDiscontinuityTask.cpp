#include "AddDiscontinuityTask.h"

#include "DiagramAxisWindow.h"
#include "DiagramLabelsWindow.h"
#include "DiagramWavesWindow.h"
#include "TimingView.h"

#include "cmd.h"

#include "art/tri_cur.xpm" // defines ruler_cur_xpm

AddDiscontinuityTask::AddDiscontinuityTask(TimingView *view, DiagramLabelsWindow *labelsWin, DiagramAxisWindow *axisWin, DiagramWavesWindow *waveWin):
Task(view, labelsWin, axisWin, waveWin)
{
    InitTask();
}
void AddDiscontinuityTask::InitTask()
{
        //done by base:
//    m_axisWin->RemoveDrawlet();
//    m_waveWin->RemoveDrawlet();
//    m_labelsWin->RemoveDrawlet();
//    m_labelsWin->SetCursor(wxNullCursor);

    wxBitmap cursBitmap(tri_cur_xpm);
    wxImage cursImage = cursBitmap.ConvertToImage();
    cursImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 5);
    cursImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
    wxCursor newcursor(cursImage);

    m_waveWin->SetCursor(newcursor, true);
    m_axisWin->SetCursor(newcursor);
}

AddDiscontinuityTask::~AddDiscontinuityTask()
{}

void AddDiscontinuityTask::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if (event.Moving() || event.Leaving())
    {
        Task::AxisMouse(event, pos);
    }
    else if ( event.ButtonDown(wxMOUSE_BTN_RIGHT))
    {
        EndTask();
    }
    else if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        DoCheckWhenMouseDown(pos);
    }
}
void AddDiscontinuityTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if (event.Moving() || event.Leaving())
    {
        Task::WavesMouse(event, pos);
    }
    else if ( event.ButtonDown(wxMOUSE_BTN_RIGHT))
    {
        EndTask();
    }
    else if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        DoCheckWhenMouseDown(pos);
    }
}
void AddDiscontinuityTask::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if ( event.ButtonDown(wxMOUSE_BTN_RIGHT))
        EndTask();
}

void AddDiscontinuityTask::DoCheckWhenMouseDown(const wxPoint &pos)
{
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    if ( !doc || !doc->signals.size() )return;
    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
    if (!cmdproc) return;

    if ( pos.x > m_view->GetWavesLeftSpace() + m_view->GridStepWidth*((int)m_view->VisibleTicks.size() - 1))
        return;

    unsigned int discontpos = m_view->VisibleTicks[GetTickFromPosition(pos)];

    if ( doc->compressors.find(discontpos) != doc->compressors.end() )
         cmdproc->Submit(new RemoveDiscontCommand(doc, discontpos));
    else
        cmdproc->Submit(new AddDiscontCommand(doc, discontpos) );
}
void AddDiscontinuityTask::LabelsKey(const wxKeyEvent &event, bool down)
{
    HandleKey(event, down);
}
void AddDiscontinuityTask::WavesKey(const wxKeyEvent &event, bool down)
{
    HandleKey(event, down);
}
void AddDiscontinuityTask::AxisKey(const wxKeyEvent &event, bool down)
{
    HandleKey(event, down);
}
void AddDiscontinuityTask::HandleKey(const wxKeyEvent &event, bool down)
{
    if ( down && event.GetKeyCode() == WXK_ESCAPE)
        EndTask();
}
void AddDiscontinuityTask::Paste(){}
bool AddDiscontinuityTask::CanPaste(){return false;}

void AddDiscontinuityTask::EndTask()
{
    m_view->SetTask(NULL);
}

