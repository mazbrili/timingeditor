#include "MainTask.h"

#include <wx/clipbrd.h>

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
#include "EditTimeCompressorTask.h"
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
    if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        //if ( != -1 )
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

wxInt32 MainTask::GetSelectedSignalNumber(){return -1;}
wxInt32 MainTask::GetSelectedDiscontinuity(){return -1;}
void MainTask::Copy()
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
void MainTask::Cut()
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
void MainTask::Paste()
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

void MainTask::Delete(){}
void MainTask::SelectAll(){}
bool MainTask::CanPaste()
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
bool MainTask::CanDelete()
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
bool MainTask::HasActiveSelection()
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

void MainTask::AddSignal(Signal *sig)
{
    TimingDocument *doc = (TimingDocument*)m_view->GetDocument();
    if ( !doc ) return;

    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
    cmdproc->Submit(new AddSignalCommand(doc, GetSelectedSignalNumber(), *sig) );
}
void MainTask::TextHasFocus(TimingTextCtrl *ctrl)
{
// TODO (danselmi#1#): Check for a real default implementation
    m_view->SetTask(new EditTextTask(this, ctrl));
}
