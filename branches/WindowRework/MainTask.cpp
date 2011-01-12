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
#include "cmd.h"


MainTask::MainTask(TimingView *view, DiagramLabelsWindow *labelsWin, DiagramAxisWindow *axisWin, DiagramWavesWindow *waveWin):
Task(view, labelsWin, axisWin, waveWin)
{}
void MainTask::InitTask(){}
MainTask::~MainTask()
{}
void MainTask::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos)
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
void MainTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    Task::WavesMouse(event, pos);
}
void MainTask::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    Task::AxisMouse(event, pos);
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
    m_view->SetTask(new EditTextTask(m_view, m_labelsWin, m_axisWin, m_waveWin, ctrl));
}
