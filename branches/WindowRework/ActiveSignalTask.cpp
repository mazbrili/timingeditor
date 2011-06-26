#include "ActiveSignalTask.h"

#include <wx/clipbrd.h>
//#include <wx/dnd.h>

#include "ClockSettingsPanel.h"
#include "TimingDoc.h"
#include "TimingView.h"
#include "DataObject.h"
#include "cmd.h"

#include "HoverHatch.h"
#include "HoverCombo.h"
#include "HoverLine.h"
#include "DiagramWavesWindow.h"
#include "DiagramLabelsWindow.h"

#include "art/MoveSig.xpm"
#include "art/CopySig.xpm"

ActiveSignalTask::ActiveSignalTask(const Task *task, int sig):
Task(task),
m_sig(sig),
state(idleState)
{
    //ctor
    //::wxLogMessage(_T("ActiveSignalTask::ActiveSignalTask"));
    if ( m_sig < 0 || (unsigned int) m_sig >= m_view->heightOffsets.size()-1)
    {
        EndTask();
        return;
    }

    UpdateClockSettingsPanel();

    SetDrawlets();
}

void ActiveSignalTask::SetDrawlets(int targetpos)
{
    wxCoord y = m_view->heightOffsets[m_sig];
    wxCoord h = -y + m_view->heightOffsets[m_sig + 1];
    m_waveWin->SetDrawlet(new HoverHatch(wxPoint(0,y), wxSize(0,h), wxColour(32, 32, 0), HoverHatch::expand_x));

    HoverDrawlet *drawlet = new HoverHatch(wxPoint(0,y), wxSize(0,h), wxColour(32, 32, 0),HoverHatch::expand_x);
    if (targetpos >= 0)
        drawlet = new HoverCombo(drawlet, new HoverLine(-1, m_view->heightOffsets[targetpos], wxColour(32, 32, 0), 3));

    m_labelsWin->SetDrawlet(drawlet);
}

ActiveSignalTask::~ActiveSignalTask()
{
    //dtor
}

void ActiveSignalTask::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if ( event.ButtonDown(wxMOUSE_BTN_RIGHT) )
    {
        EndTask();
        return;
    }
    if ( event.ButtonDown(wxMOUSE_BTN_LEFT) )
    {
        state = idleState;
        int k = GetSignalFromPosition(pos);
        if ( k == -1 )
        {
            EndTask();
            return;
        }
        m_sig = k;
        SetDrawlets();
        UpdateClockSettingsPanel();
        return;
    }

    if (event.Dragging() && event.ButtonIsDown(wxMOUSE_BTN_LEFT))
    {
        if (state == idleState )
        {
        //wxLogMessage(_T("DoDragDrop"));
            state = movingSignal;
        }
        else // state == movingSignal
        {
            int k = GetSignalFromPosition(pos);
            if ( k != -1 )
            {
                if ( 2*pos.y > (m_view->heightOffsets[k] + m_view->heightOffsets[k+1]))
                    k++;
                if ( k == m_sig || k == m_sig+1)
                    k = -1;
            }
            SetDrawlets(k);
            if (! event.ControlDown())
            {
                wxBitmap cursBitmap(MoveSig_xpm);
                wxImage cursImage = cursBitmap.ConvertToImage();
                cursImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 5);
                cursImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
                m_labelsWin->SetCursor(wxCursor(cursImage));
            }
            else
            {
                wxBitmap cursBitmap(CopySig_xpm);
                wxImage cursImage = cursBitmap.ConvertToImage();
                cursImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 5);
                cursImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
                m_labelsWin->SetCursor(wxCursor(cursImage));
            }
        }
        return;
    }
    if (state == movingSignal && event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        int k = GetSignalFromPosition(pos);
        if ( k != -1 )
        {
            if ( 2*pos.y > (m_view->heightOffsets[k] + m_view->heightOffsets[k+1]))
                k++;
            if ( k == m_sig || k == m_sig+1)
                k = -1;
        }
        if ( k == -1 )
        {
            state = idleState;
            SetDrawlets();
        }
        else
        {
            wxCommand *cmd;
            TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
            if (! event.ControlDown())
            {
                cmd = new MoveSignalPosCommand(doc, m_sig, k);
            }
            else
            {
                //copy
                Signal sig = doc->signals[m_sig];
                cmd = new AddSignalCommand(doc, k, sig);
            }
            wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
            if (cmdproc)
                cmdproc->Submit(cmd);
        }
        m_labelsWin->SetCursor(wxNullCursor);
    }
    if (state == movingSignal && event.Leaving())
    {
        state = idleState;
        SetDrawlets();
    }
}
void ActiveSignalTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}
void ActiveSignalTask::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}

void ActiveSignalTask::OnMouse(const wxMouseEvent &event)
{
    //::wxLogMessage(_T("ActiveSignalTask::OnMouse"));
    if (event.ButtonDown(wxMOUSE_BTN_ANY ))
        EndTask();
}

void ActiveSignalTask::LabelsKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
void ActiveSignalTask::WavesKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
void ActiveSignalTask::AxisKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}

void ActiveSignalTask::OnKey(const wxKeyEvent &event, bool down)
{
    if ( down && event.GetKeyCode() == WXK_DELETE)
    {
        Delete();
        return;
    }
    if ( down && event.GetKeyCode() == WXK_ESCAPE)
        EndTask();
}
void ActiveSignalTask::EndTask()
{
    m_sig = -1;
    UpdateClockSettingsPanel(false);
    m_view->SetTask(NULL);
}
void ActiveSignalTask::UpdateClockSettingsPanel(bool attach)
{
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    if (!attach || !doc || m_sig == -1)
    {
        ClockSettingsPanel::GetInstance()->view = NULL;
        return;
    }

    ClockSettingsPanel::GetInstance()->view = m_view;

    if ( doc->signals.size() < (unsigned int)m_sig)
        return;

    if ( doc->signals[m_sig].IsClock )
    {
        wxInt32 n;
        wxString str;

        n = doc->signals[m_sig].delay;
        str = wxString::Format ( _( "%d" ) , n);
        ClockSettingsPanel::GetInstance()->SetDelayText(str);

        n = doc->signals[m_sig].ticks;
        str = wxString::Format ( _( "%d" ) , n);
        ClockSettingsPanel::GetInstance()->SetTicksText(str);

        ClockSettingsPanel::GetInstance()->SetShadowed(doc->signals[m_sig].GenerateBackground);
        ClockSettingsPanel::GetInstance()->SetShowPeriod(doc->signals[m_sig].ShowPeriodCount);
    }
}

bool ActiveSignalTask::CanDelete()
{
    return m_sig != -1;
}
int ActiveSignalTask::GetSelectedSignalNumber()
{
    return m_sig;
}

void ActiveSignalTask::Delete()
{
    state = idleState;
    wxCommandProcessor *cmdproc = m_view->GetDocument()->GetCommandProcessor();
    if (cmdproc)
        cmdproc->Submit(new DeleteSignalCommand((TimingDocument *)m_view->GetDocument(), m_sig));
    //EndTask();
}
bool ActiveSignalTask::CanCopy()
{
    return m_sig != -1;
}
void ActiveSignalTask::Copy()
{
    if (wxTheClipboard->Open())
    {
        if ( m_sig != -1 )
        {
            TimingDocument *doc = (TimingDocument*)m_view->GetDocument();
            if ( doc )
            {
                Signal *sig = new Signal;
                *sig = doc->signals[m_sig];
                // This data objects are held by the clipboard,
                // so do not delete them here in.
                wxTheClipboard->SetData( new TimingEditorSignalDataObject(sig) );
            }
        }
        wxTheClipboard->Close();
    }
}
void ActiveSignalTask::Cut()
{
    Copy();
    Delete();
}

void ActiveSignalTask::Paste()
{
    PasteSignalFromClipboard();
}
bool ActiveSignalTask::CanPaste()
{
    return IsSignalInClipboard();
}

void ActiveSignalTask::Update()
{
    TimingDocument *doc = (TimingDocument*)m_view->GetDocument();
    if ( doc )
    {
        unsigned int k = doc->signals.size();
        if ( k == 0 )
        {
            EndTask();
            return;
        }
        if ( (unsigned int)m_sig >= k )
            m_sig = k-1;
        SetDrawlets();
        UpdateClockSettingsPanel();

    }
}
