#include "Task.h"

#include "HoverCross.h"
#include "HoverText.h"
#include "HoverLine.h"

Task::Task(TimingView *view, DiagramLabelsWindow *labelsWin, DiagramAxisWindow *axisWin, DiagramWavesWindow *waveWin):
m_view(view),
m_labelsWin(labelsWin),
m_axisWin(axisWin),
m_waveWin(waveWin)
{
    //ctor
}

Task::~Task()
{
    //dtor
}
void Task::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if ( ! event.LeftDown() )
    {
        for ( unsigned int k = 0 ; k < m_view->heightOffsets.size()-1 ; ++k )
        {
            if(pos.y > m_view->heightOffsets[k+1] - 6 && pos.y < m_view->heightOffsets[k+1]-1 )
            {
                m_labelsWin->SetCursor(wxCursor(wxCURSOR_SIZENS));
                return;
            }
            else if ( pos.y > m_view->heightOffsets[k]+1 && pos.y < m_view->heightOffsets[k] + 6)
            {
                m_labelsWin->SetCursor(wxCursor(wxCURSOR_SIZENS));
                return;
            }
        }
        m_labelsWin->SetCursor(wxNullCursor);
        m_axisWin->RemoveDrawlet();
        m_waveWin->RemoveDrawlet();
        return;
    }
}
void Task::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    m_waveWin->SetDrawlet( new HoverCross(wxPoint(pos.x,pos.y)) );
    //m_axisWin->SetDrawlet( new HoverText(_T("gaga"), wxPoint(10,10) ) );
    m_axisWin->SetDrawlet( new HoverLine(wxPoint(pos.x, 0), wxPoint(pos.x, m_view->GetHeightOfAxisWindow())));
}
void Task::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{

}

wxInt32 Task::GetSelectedSignalNumber(){return -1;}
wxInt32 Task::GetSelectedDiscontinuity(){return -1;}
void Task::Copy()
{
    if (wxTheClipboard->Open())
    {
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
//                wxTheClipboard->SetData( new TimingEditorSignaDataObject(sig) );
//            }
//        }
        wxTheClipboard->Close();
    }
}
void Task::Cut()
{
    if (wxTheClipboard->Open())
    {
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
//                wxTheClipboard->SetData( new TimingEditorSignaDataObject(sig) );
//            }
//        }
        wxTheClipboard->Close();
////        window->DeleteSelection();
    }
}
void Task::Paste()
{
    if (wxTheClipboard->Open())
    {
////        if (wxTheClipboard->IsSupported( wxDF_TEXT ))
//        {
//            wxTextDataObject data;
//            wxTheClipboard->GetData( data );
//            window->InsertText( data.GetText() );
//        }
//        else
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
    //window->DeleteSelection();
    return;
}
void Task::SelectAll()
{
     //if( window )
        ////window->SelectAll();
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

////    if ( window && window->CanPaste() )
//        return true;

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

