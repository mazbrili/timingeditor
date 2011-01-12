#include "Task.h"

#include "DiagramAxisWindow.h"
#include "DiagramLabelsWindow.h"
#include "DiagramWavesWindow.h"
#include "TimingView.h"
#include "DataObject.h"
#include "HoverCross.h"
#include "HoverText.h"
#include "HoverLine.h"
#include "HoverCombo.h"

#include "EditTextTask.h"
#include "cmd.h"


Task::Task(TimingView *view, DiagramLabelsWindow *labelsWin, DiagramAxisWindow *axisWin, DiagramWavesWindow *waveWin):
m_view(view),
m_labelsWin(labelsWin),
m_axisWin(axisWin),
m_waveWin(waveWin)
{
    InitTask();
}
void Task::InitTask()
{
    //ctor
    m_axisWin->RemoveDrawlet();
    m_waveWin->RemoveDrawlet();
    m_labelsWin->RemoveDrawlet();

    m_axisWin->SetCursor(*wxCROSS_CURSOR);
    m_waveWin->SetCursor(*wxCROSS_CURSOR);
    m_labelsWin->SetCursor(wxNullCursor);
}

Task::~Task()
{
    //dtor
}
void Task::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if ( event.Moving() )
    {
        bool found = false;
        for ( unsigned int k = 0 ; k < m_view->heightOffsets.size()-1 ; ++k )
        {
            if(pos.y > m_view->heightOffsets[k+1] - 3 && pos.y < m_view->heightOffsets[k+1]-1 )
            {
                m_labelsWin->SetCursor(wxCursor(wxCURSOR_SIZENS));
                found = true;
                break;
            }
            else if ( pos.y > m_view->heightOffsets[k]+1 && pos.y < m_view->heightOffsets[k] + 3)
            {
                m_labelsWin->SetCursor(wxCursor(wxCURSOR_SIZENS));
                found = true;
                break;
            }
        }
        if ( !found )
            m_labelsWin->SetCursor(wxNullCursor);
        m_axisWin->RemoveDrawlet();
        m_waveWin->RemoveDrawlet();
        return;
    }

}
void Task::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if ( event.Moving() )
    {
        if ( IsOverWaves(pos) )
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
    if(event.Leaving())
    {
        m_axisWin->RemoveDrawlet();
        m_waveWin->RemoveDrawlet();
        return;
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
    if( event.Leaving())
    {
        m_axisWin->RemoveDrawlet();
        m_waveWin->RemoveDrawlet();
        return;
    }
}

void Task::LabelsKey(const wxKeyEvent &event, bool down){}
void Task::WavesKey(const wxKeyEvent &event, bool down){}
void Task::AxisKey(const wxKeyEvent &event, bool down){}

wxInt32 Task::GetSelectedSignalNumber(){return -1;}
wxInt32 Task::GetSelectedDiscontinuity(){return -1;}
void Task::Copy()
{
//    if (wxTheClipboard->Open())
//    {
////        // This data objects are held by the clipboard,
//        // so do not delete them in the app.
//        if ( window->IsTextSelected() )
//        {
//            wxString str(window->GetText());
//            wxTheClipboard->SetData( new wxTextDataObject(str) );
//        }
//        else if ( window->IsSignalSelected() )
//        {
//            TimingDocument *doc = (TimingDocument*)m_viewDocument;
//            if ( doc )
//            {
//                Signal *sig = new Signal;
//                *sig = doc->signals[window->GetSelectedSignalNr()];
//                wxTheClipboard->SetData( new TimingEditorSignalDataObject(sig) );
//            }
//        }
//        wxTheClipboard->Close();
//    }
}
void Task::Cut()
{
//    if (wxTheClipboard->Open())
//    {
////        // This data objects are held by the clipboard,
//        // so do not delete them in the app.
//        if ( window->IsTextSelected() )
//        {
//            wxString str(window->GetText());
//            wxTheClipboard->SetData( new wxTextDataObject(str) );
//        }
//        else if ( window->IsSignalSelected() )
//        {
//            TimingDocument *doc = (TimingDocument*)m_viewDocument;
//            if ( doc )
//            {
//                Signal *sig = new Signal;
//                *sig = doc->signals[window->GetSelectedSignalNr()];
//                wxTheClipboard->SetData( new TimingEditorSignalDataObject(sig) );
//            }
//        }
//        wxTheClipboard->Close();
//            window->DeleteSelection();
//    }
}
void Task::Paste()
{
    if (wxTheClipboard->Open())
    {
        if (wxTheClipboard->IsSupported( wxDataFormat(TimingEditorSignalFormatId) ) )
        {
            Signal *sig = new Signal;
            TimingEditorSignalDataObject data(sig);

            wxTheClipboard->GetData( data );
            AddSignal( sig );
            delete sig;
        }
        wxTheClipboard->Close();
    }
}

void Task::Delete()
{
    return;
}
void Task::SelectAll()
{
    return;
}
bool Task::CanPaste()
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
bool Task::CanDelete()
{

////    if ( window && (
//        window->IsTextSelected() ||
//        window->IsSignalSelected() ||
//        window->VLineIsSelected() ||
//        window->HArrowIsSelected() ||
//        window->CanDeleteText() ||
//        window->DiscontSelected() )
//    )
//        return true;
    return false;
}
bool Task::HasActiveSelection()
{

////    if ( window && (
//        window->IsTextSelected() ||
//        window->IsSignalSelected() ||
//        window->VLineIsSelected() ||
//        window->HArrowIsSelected() ||
//        window->IsDiscontinuitySelected() )
//    )
//        return true;
    return false;
}

void Task::AddSignal(Signal *sig)
{
    TimingDocument *doc = (TimingDocument*)m_view->GetDocument();
    if ( !doc ) return;

    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
    cmdproc->Submit(new AddSignalCommand(doc, GetSelectedSignalNumber(), *sig) );
}
wxInt32 Task::GetTickFromPosition(const wxPoint &pos)
{
    return (pos.x - m_view->GetWavesLeftSpace())/(m_view->GridStepWidth);
}
bool Task::IsOverWaves(const wxPoint &pos)
{
    return //pos.x > m_view->GetWavesLeftSpace() &&
         pos.x < m_view->GetWavesLeftSpace() + m_view->GridStepWidth*((int)m_view->VisibleTicks.size() - 1) &&
         //pos.y > m_view->heightOffsets[0]-5 &&
         pos.y < m_view->heightOffsets[m_view->heightOffsets.size()-1]+5;
}
void Task::TextHasFocus(TimingTextCtrl *ctrl)
{
    m_view->SetTask(new EditTextTask(m_view, m_labelsWin, m_axisWin, m_waveWin, ctrl));
}
