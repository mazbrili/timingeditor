#include "Task.h"

#include "HoverCross.h"
#include "HoverText.h"
#include "HoverLine.h"
#include "HoverCombo.h"

Task::Task(TimingView *view, DiagramLabelsWindow *labelsWin, DiagramAxisWindow *axisWin, DiagramWavesWindow *waveWin):
m_view(view),
m_labelsWin(labelsWin),
m_axisWin(axisWin),
m_waveWin(waveWin)
{
    //ctor
    m_axisWin->RemoveDrawlet();
    m_waveWin->RemoveDrawlet();
    m_labelsWin->RemoveDrawlet();
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
            if(pos.y > m_view->heightOffsets[k+1] - 6 && pos.y < m_view->heightOffsets[k+1]-1 )
            {
                m_labelsWin->SetCursor(wxCursor(wxCURSOR_SIZENS));
                found = true;
                break;
            }
            else if ( pos.y > m_view->heightOffsets[k]+1 && pos.y < m_view->heightOffsets[k] + 6)
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
        m_waveWin->SetDrawlet( new HoverCross(wxPoint(pos.x, pos.y), *wxLIGHT_GREY) );
        m_axisWin->SetDrawlet(
            new HoverCombo(
                new HoverLine(wxPoint(pos.x, 0),
                              wxPoint(pos.x, m_view->GetHeightOfAxisWindow()),
                              *wxLIGHT_GREY),
                new HoverText(_T("gaga"), wxPoint(pos.x,5), *wxLIGHT_GREY )
            )
        );
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
        m_waveWin->SetDrawlet(new HoverLine(wxPoint(pos.x, 0),
                              wxPoint(pos.x, m_waveWin->GetSize().GetY()),
                              *wxLIGHT_GREY));
        m_axisWin->SetDrawlet(
            new HoverCombo(
                new HoverLine(wxPoint(pos.x, 0),
                              wxPoint(pos.x, m_view->GetHeightOfAxisWindow()),
                              *wxLIGHT_GREY),
                new HoverText(_T("gaga"), wxPoint(pos.x,5), *wxLIGHT_GREY )
            )
        );
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

