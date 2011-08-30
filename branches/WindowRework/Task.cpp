#include "Task.h"

#include <wx/clipbrd.h>

#include "DiagramAxisWindow.h"
#include "DiagramLabelsWindow.h"
#include "DiagramWavesWindow.h"
#include "TimingView.h"
#include "TimingApp.h"
#include "TimingMainFrame.h"
#include "DataObject.h"
#include "HoverCross.h"
#include "HoverText.h"
#include "HoverLine.h"
#include "HoverCombo.h"
#include "GraphSignal.h"

#include "GraphVerticalLine.h"
#include "TimingTextCtrl.h"

#include "cmd.h"


Task::Task(TimingView *view, DiagramLabelsWindow *labelsWin, DiagramAxisWindow *axisWin, DiagramWavesWindow *waveWin):
m_view(view),
m_labelsWin(labelsWin),
m_axisWin(axisWin),
m_waveWin(waveWin)
{
    Init();
}

Task::Task(const Task *task):
m_view(task->m_view),
m_labelsWin(task->m_labelsWin),
m_axisWin(task->m_axisWin),
m_waveWin(task->m_waveWin)
{
    Init();
}

void Task::Init()
{
    //ctor
    m_axisWin->RemoveDrawlet();
    m_waveWin->RemoveDrawlet();
    m_labelsWin->RemoveDrawlet();

    m_axisWin->SetCursor(*wxCROSS_CURSOR);
    m_waveWin->SetCursor(*wxCROSS_CURSOR);
    m_labelsWin->SetCursor(wxNullCursor);

    SetStatusText(_(""));
}

Task::~Task()
{
    //dtor
}
void Task::EndTask()
{
    m_view->SetTask(NULL);
}
void Task::SetStatusText(const wxString& text)
{
    TimingApp &app = ::wxGetApp();
    app.GetMainFrame()->SetStatusText(text);
}

void Task::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos){}
void Task::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if ( event.Moving() || event.Dragging() )
    {
        if ( IsOverWavesRect(pos) )
        {
            wxInt32 tick = GetTickFromPosition(pos);

            wxString str = wxString::Format(_T("tick: %d, t: "), m_view->VisibleTicks[tick]);
            str += m_view->GetTimeString( m_view->VisibleTicks[tick] );

            m_axisWin->SetDrawlet(
                new HoverCombo(
                    new HoverLine(wxPoint(pos.x, 0),
                                  wxPoint(pos.x, m_view->GetHeightOfAxisWindow()),
                                  *wxLIGHT_GREY),
                    new HoverText(str, wxPoint(pos.x,5), *wxLIGHT_GREY )
                )
            );
        }
        else
            m_axisWin->SetDrawlet(new HoverLine(wxPoint(pos.x, 0),
                                  wxPoint(pos.x, m_view->GetHeightOfAxisWindow()),
                                  *wxLIGHT_GREY));
        m_waveWin->SetDrawlet( new HoverCross(wxPoint(pos.x, pos.y), *wxLIGHT_GREY) );
    }
    else if(event.Leaving())
    {
        m_axisWin->RemoveDrawlet();
        m_waveWin->RemoveDrawlet();
    }
}
void Task::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if ( event.Moving() )
    {
        wxInt32 tick = GetTickFromPosition(pos);
        if ( pos.x < m_view->GetWavesLeftSpace() + m_view->GridStepWidth*((int)m_view->VisibleTicks.size() - 1))
        {
            wxString str = wxString::Format(_T("tick: %d, t: "), m_view->VisibleTicks[tick]);
            str += m_view->GetTimeString( m_view->VisibleTicks[tick] );
            m_axisWin->SetDrawlet(
                new HoverCombo(
                    new HoverLine(wxPoint(pos.x, 0),
                                  wxPoint(pos.x, m_view->GetHeightOfAxisWindow()),
                                  *wxLIGHT_GREY),
                    new HoverText(str, wxPoint(pos.x,5), *wxLIGHT_GREY )
                )
            );
        }
        else
            m_axisWin->SetDrawlet(new HoverLine(wxPoint(pos.x, 0),
                                  wxPoint(pos.x, m_view->GetHeightOfAxisWindow()),
                                  *wxLIGHT_GREY));


        m_waveWin->SetDrawlet(new HoverLine(wxPoint(pos.x, 0),
                              wxPoint(pos.x, m_waveWin->GetSize().GetY()),
                              *wxLIGHT_GREY));
    }
    else if( event.Leaving())
    {
        m_axisWin->RemoveDrawlet();
        m_waveWin->RemoveDrawlet();
    }
}

void Task::LabelsKey(const wxKeyEvent &event, bool down){}
void Task::WavesKey(const wxKeyEvent &event, bool down){}
void Task::AxisKey(const wxKeyEvent &event, bool down){}

wxInt32 Task::GetSelectedSignalNumber()const{return -1;}
wxInt32 Task::GetSelectedDiscontinuity()const{return -1;}
void Task::Copy(){}
void Task::Cut(){}
void Task::Paste(){}
void Task::Delete(){}
void Task::SelectAll(){}
bool Task::CanPaste()const
{
    return false;
}
bool Task::CanDelete()const
{
    return false;
}
bool Task::CanCopy()const
{
    return false;
}
bool Task::CanCut()const
{
    return CanCopy() & !IsReadOnly();
}
bool Task::IsReadOnly()const
{
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    if (doc)
        return doc->IsReadOnly();
    return true;
}
wxInt32 Task::GetTickFromPosition(const wxPoint &pos)const
{
    return (pos.x - m_view->GetWavesLeftSpace())/(m_view->GridStepWidth);
}
wxInt32 Task::GetSignalFromPosition(const wxPoint &pos)const
{
    for ( unsigned int k = 0 ; k < m_view->heightOffsets.size()-1 ; ++k )
    {
        if(pos.y > m_view->heightOffsets[k] && pos.y < m_view->heightOffsets[k+1] )
        {
            return k;
        }
    }
    return -1;
}
bool Task::IsOverWavesRect(const wxPoint &pos)const
{
    return pos.x > m_view->GetWavesLeftSpace() &&
         pos.x < m_view->GetWavesLeftSpace() + m_view->GridStepWidth*((int)m_view->VisibleTicks.size() - 1) &&
         pos.y > m_view->heightOffsets[0]-5 &&
         pos.y < m_view->heightOffsets[m_view->heightOffsets.size()-1]+5;
}
int Task::IsOverWaves(const wxPoint &pos)const
{
    if ( pos.x > m_view->GetWavesLeftSpace() &&
         pos.x < m_view->GetWavesLeftSpace() + m_view->GridStepWidth*((int)m_view->VisibleTicks.size() - 1))
    {
        const GraphSignals gSignals = m_view->GetGraphSignals();
        for ( unsigned int i = 0 ; i < gSignals.size() ; i++ )
        {
            if ( gSignals[i]->HasYPos(pos.y))
                return i;
        }
    }
    return -1;
}
int Task::IsOverVerticalLine(const wxPoint &pos)const
{
    const GraphVerticalLines vlines = m_view->GetGraphVerticalLines();
    for (unsigned int i = 0 ; i < vlines.size() ; i++ )
    {
        for ( int snapTolerance = 0 ;  snapTolerance <= GetVerticalLineSnapTolerance() ; snapTolerance++ )
            if (vlines[i].HasPoint(pos, snapTolerance))
                return i;
    }

    return -1;
}
int Task::IsOverHorizontalArrow(const wxPoint &pos)const
{
    const GraphHorizontalArrows harrows = m_view->GetGraphHorizontalArrows();
    for ( unsigned int i = 0 ; i < harrows.size() ; i++ )
    {
        for(int snapTolerance = 0 ; snapTolerance <= GetHorizontalArrowSnapTolerance() ; snapTolerance++ )
            if ( harrows[i].HasPoint(pos, snapTolerance))
                return i;

    }
    return -1;
}


bool Task::IsOnAddTimeRange(const wxPoint &pos)const
{
    int axispos =  m_view->GetDistanceToAxis() + m_view->GetDistanceToTicksLine() + m_view->GetDistanceFromTicksLine();
    return  pos.y >= axispos - GetRangeAddRemoveTime() &&
        pos.y <= axispos &&
        pos.x > m_view->GetWavesLeftSpace() &&
        pos.x <  m_view->GetWavesLeftSpace() + (int)((m_view->GridStepWidth)*(m_view->VisibleTicks.size() - 1));
}
bool Task::IsOnRemoveTimeRange(const wxPoint &pos)const
{
    int axispos =  m_view->GetDistanceToAxis() + m_view->GetDistanceToTicksLine() + m_view->GetDistanceFromTicksLine();
    return pos.y > axispos &&
        pos.y <= axispos + GetRangeAddRemoveTime() &&
        pos.x > m_view->GetWavesLeftSpace() &&
        pos.x <  m_view->GetWavesLeftSpace() + (int)((m_view->GridStepWidth)*(m_view->VisibleTicks.size() - 1));
}

void Task::TextHasFocus(TimingTextCtrl *ctrl)
{
    // ???
    ctrl->SetFocusToParent();
}

void Task::UpdateTimeCompressorPanel(bool attach)const
{
    TimeCompressorSettingsPanel::GetInstance()->view = NULL;
}

void Task::UpdateClockSettingsPanel(bool attach)const
{
    ClockSettingsPanel::GetInstance()->view = NULL;
}
bool Task::IsSignalInClipboard()const
{
    if (wxTheClipboard->Open())
    {
        if ( wxTheClipboard->IsSupported( wxDataFormat(TimingEditorSignalFormatId) ) )
        {
            wxTheClipboard->Close();
            return true;
        }
        wxTheClipboard->Close();
    }
    return false;
}
void Task::AddSignal(Signal *sig)const
{
    TimingDocument *doc = (TimingDocument*)m_view->GetDocument();
    if ( !doc ) return;

    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
    if(cmdproc)
        cmdproc->Submit(new AddSignalCommand(doc, GetSelectedSignalNumber(), *sig) );
}
void Task::PasteSignalFromClipboard()const
{
    if (wxTheClipboard->Open())
    {
        if (wxTheClipboard->IsSupported( wxDataFormat(TimingEditorSignalFormatId) ) )
        {
            Signal *sig = new Signal;
            TimingEditorSignalDataObject data(sig);

            wxTheClipboard->GetData( data );
            AddSignal( sig );
            // do not delete sig since it is "owned" by the dataObject
            //delete sig;
        }
        wxTheClipboard->Close();
    }
}
void Task::Update(){}
