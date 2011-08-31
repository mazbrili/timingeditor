#include "MainTask.h"


#include "DiagramAxisWindow.h"
#include "DiagramLabelsWindow.h"
#include "DiagramWavesWindow.h"
#include "TimingView.h"
#include "HoverCross.h"
#include "HoverText.h"
#include "HoverLine.h"
#include "HoverCombo.h"
#include "HoverCheckbox.h"

#include "EditTextTask.h"
#include "EditTimeCompressorTask.h"
#include "ActiveSignalTask.h"
#include "ActiveVerticalLineTask.h"
#include "ActiveHorizontalArrowTask.h"
#include "ChangeSignalSpacerTask.h"
#include "AddTimeTask.h"
#include "EditSignalTask.h"
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

    if ( event.Moving() )
    {
        if ( IsOnResizeHeightPos(pos) != -1)
        {
            m_labelsWin->SetCursor(wxCursor(wxCURSOR_SIZENS));
            SetStatusText(_("change space around signal"));
        }
        else
        {
            m_labelsWin->SetCursor(wxNullCursor);
            SetStatusText(_(""));
        }

        m_axisWin->RemoveDrawlet();
        m_waveWin->RemoveDrawlet();
        return;
    }

    int k = GetSignalFromPosition(pos);
    if ( k != -1 && event.ButtonDown(wxMOUSE_BTN_LEFT) )
    {
        int resize = IsOnResizeHeightPos(pos);
        if (resize == -1)
            m_view->SetTask(new ActiveSignalTask(this, k));
        else
            m_view->SetTask(new ChangeSignalSpacerTask(this, resize));
        return;
    }

}
int MainTask::IsOnResizeHeightPos(const wxPoint &pos)
{
    const int CatchWidth = 5;
    int k = GetSignalFromPosition(pos);
    if ( k == -1)
        return -1;
    if ( pos.y > m_view->heightOffsets[k]+1 && pos.y < m_view->heightOffsets[k] + CatchWidth+1)
        return 2*k;
    if (pos.y > m_view->heightOffsets[k+1] - CatchWidth-1 && pos.y < m_view->heightOffsets[k+1]-1)
        return 2*k+1;
    return -1;
}
void MainTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if ( event.ButtonDown(wxMOUSE_BTN_LEFT) )
    {
        int arrowidx = IsOverHorizontalArrow(pos);
        if (arrowidx != -1)
        {
            m_view->SetTask(new ActiveHorizontalArrowTask(this, arrowidx));
            return;
        }

        int lineidx = IsOverVerticalLine(pos);
        if ( lineidx != -1 )
        {
            //::wxLogMessage(wxString::Format(_T("MainTask::WavesMouse IsOverVerticalLine %d"), lineidx ));
            m_view->SetTask(new ActiveVerticalLineTask(this, lineidx));
            return;
        }

        int signalidx = IsOverWaves(pos);
        if ( signalidx != -1)
        {
            if( !(((TimingDocument*)m_view->GetDocument())->signals[signalidx].IsBus) )
            {
                wxInt32 tick = m_view->VisibleTicks[GetTickFromPosition(pos)];
                m_view->SetTask(new EditSignalTask(this, signalidx, tick));
            }
            return;
        }
    }

    if (event.ButtonDown(wxMOUSE_BTN_RIGHT))
    {
        int signalidx = IsOverWaves(pos);
        if ( signalidx != -1)
        {
            wxInt32 tick = m_view->VisibleTicks[GetTickFromPosition(pos)];

            m_view->SetTask(new EditSignalTask(this, signalidx, tick, false));
            return;
        }
    }

    Task::WavesMouse(event, pos);
    if ( event.Moving() )
    {
        int arrowidx = IsOverHorizontalArrow(pos);
        if (arrowidx != -1)
        {
            SetStatusText(_("Select Arrow (click)"));
            return;
        }

        int lineidx = IsOverVerticalLine(pos);
        if ( lineidx != -1 )
        {
            SetStatusText(_("Select Line (click)"));
            return;
        }

        int signalidx = IsOverWaves(pos);
        if ( signalidx != -1 )
        {
            if( !(((TimingDocument*)m_view->GetDocument())->signals[signalidx].IsClock) )
                SetStatusText(_("change signal (rclick / lclick)"));

            return;
        }
        SetStatusText(_(""));
    }
}
void MainTask::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    //call default
    Task::AxisMouse(event, pos);
    if (event.Moving())
    {
        if (IsOnAddTimeRange(pos))
        {
            m_axisWin->SetDrawlet(new HoverCheckbox(pos+wxPoint(10,-10)));
            SetStatusText( _("'Add' time (drag or double click)") );
            return;
        }
        if (IsOnRemoveTimeRange(pos))
        {
            m_axisWin->SetDrawlet(new HoverCheckbox(pos+wxPoint(10,-10), false));
            SetStatusText(_("'Remove' time by dragging"));
            return;
        }

        wxInt32 tick = GetTickFromPosition(pos);
        tick = m_view->VisibleTicks[tick];
        TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
        if(doc->compressors.find(tick) != doc->compressors.end())
        {
            SetStatusText(_("Edit time compressor"));
            return;
        }
        else
            SetStatusText(_(""));
    }

    if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        wxInt32 tick = GetTickFromPosition(pos);
        if (IsOnAddTimeRange(pos))
        {
            m_view->SetTask(new AddRemoveTimeTask(this, tick));
            return;
        }
        if (IsOnRemoveTimeRange(pos))
        {
            m_view->SetTask(new AddRemoveTimeTask(this, tick, false));
            return;
        }

        tick = m_view->VisibleTicks[tick];
        TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
        if(doc->compressors.find(tick) != doc->compressors.end())
        {
            m_view->SetTask(new EditTimeCompressorTask(this, tick));
            return;
        }
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
    SetStatusText(_("change Text"));
}