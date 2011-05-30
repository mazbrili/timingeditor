#include "MainTask.h"


#include "DiagramAxisWindow.h"
#include "DiagramLabelsWindow.h"
#include "DiagramWavesWindow.h"
#include "TimingView.h"
#include "HoverCross.h"
#include "HoverText.h"
#include "HoverLine.h"
#include "HoverCombo.h"

#include "EditTextTask.h"
#include "EditTimeCompressorTask.h"
#include "ActiveSignalTask.h"
#include "cmd.h"


MainTask::MainTask(TimingView *view, DiagramLabelsWindow *labelsWin, DiagramAxisWindow *axisWin, DiagramWavesWindow *waveWin)
:Task(view, labelsWin, axisWin, waveWin)
{
//    m_view = view;
//    m_labelsWin = labelsWin;
//    m_axisWin = axisWin;
//    m_waveWin = waveWin;
}

void MainTask::InitTask()
{
    Task::Init();
}
MainTask::~MainTask()
{}
void MainTask::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    const int CatchWidth = 5;
    int k = GetSignalFromPosition(pos);

    if ( event.Moving() )
    {
        if ( k != -1 && ((pos.y > m_view->heightOffsets[k+1] - CatchWidth-1 && pos.y < m_view->heightOffsets[k+1]-1) ||
            ( pos.y > m_view->heightOffsets[k]+1 && pos.y < m_view->heightOffsets[k] + CatchWidth+1)))
            m_labelsWin->SetCursor(wxCursor(wxCURSOR_SIZENS));
        else
            m_labelsWin->SetCursor(wxNullCursor);

        m_axisWin->RemoveDrawlet();
        m_waveWin->RemoveDrawlet();
        return;
    }
    if ( k != -1 && event.ButtonDown(wxMOUSE_BTN_LEFT) )
    {
        m_view->SetTask(new ActiveSignalTask(this, k));
    }

}
void MainTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    Task::WavesMouse(event, pos);
}
void MainTask::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    //call default
    Task::AxisMouse(event, pos);

    //::wxLogMessage(_T("MainTask::AxisMouse"));

    if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
        wxInt32 tick = m_view->VisibleTicks[GetTickFromPosition(pos)];
        if(doc->compressors.find(tick) != doc->compressors.end())
            m_view->SetTask(new EditTimeCompressorTask(this, tick));
    }
}

void MainTask::LabelsKey(const wxKeyEvent &event, bool down){}
void MainTask::WavesKey(const wxKeyEvent &event, bool down){}
void MainTask::AxisKey(const wxKeyEvent &event, bool down){}

void MainTask::Paste()
{
    PasteSignalFromClipboard();
}

bool MainTask::CanPaste()
{
    return IsSignalInClipboard();
}

void MainTask::TextHasFocus(TimingTextCtrl *ctrl)
{
// TODO (danselmi#1#): Check for a real default implementation
    m_view->SetTask(new EditTextTask(this, ctrl));
}
