//    Copyright 2008 Daniel Anselmi
//
//    This file is part of TimingEditor.
//
//    TimingEditor is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    TimingEditor is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with TimingEditor.  If not, see <http://www.gnu.org/licenses/>.
//
//    Contact e-mail: daniel anselmi <danselmi@gmx.ch>
//    Program URL   : http://sourceforge.net/projects/timingeditor/
//
//

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/clipbrd.h>
#include <wx/svg/dcsvg.h>
#include <wx/dcps.h>
#include <wx/filename.h>

#include "TimingView.h"
#include "TimingWindow.h"
#include "TimingMainFrame.h"
#include "TimingApp.h"
#include "cmd.h"
#include "enumers.h"
#include "DataObject.h"

#include "DiagramSplitterWindow.h"


IMPLEMENT_DYNAMIC_CLASS(TimingView, wxView)
BEGIN_EVENT_TABLE(TimingView, wxView)
    EVT_MENU(TIMING_ID_DELETE,          TimingView::OnDelete)
    EVT_MENU(wxID_SELECTALL,            TimingView::OnSelectAll)
    EVT_MENU(wxID_COPY,                 TimingView::OnCopy)
    EVT_MENU(wxID_CUT,                  TimingView::OnCut)
    EVT_MENU(wxID_PASTE,                TimingView::OnPaste)
    EVT_MENU(TIMING_ID_GLASS_N,         TimingView::OnZoomTicksOut)
    EVT_MENU(TIMING_ID_GLASS_P,         TimingView::OnZoomTicksIn)
    //EVT_MENU(TIMING_ID_CHANGECLOCK,     TimingView::OnEditClock)
    EVT_MENU(TIMING_ID_ADD_CLOCK,       TimingView::OnAddClock)
    EVT_MENU(TIMING_ID_ADD_SIGNAL,      TimingView::OnAddSignal)
    EVT_MENU(TIMING_ID_ADD_BUS,         TimingView::OnAddBus)
    EVT_MENU(TIMING_ID_DISCONTINUATION, TimingView::OnInsertDiscontTool)
    EVT_MENU(TIMING_ID_RULER,           TimingView::OnSelectRuler)
    EVT_MENU(TIMING_ID_HARROW,          TimingView::OnSelectHArrow)
    EVT_MENU(TIMING_ID_NEUTRAL,         TimingView::OnSelectNeutral)
    EVT_MENU(TIMING_ID_EDITTEXT,        TimingView::OnSelectTextTool)
    EVT_MENU(TIMING_ID_EXPORT_BITMAP,   TimingView::OnExportBitmap)
    EVT_MENU(TIMING_ID_EXPORT_SVG,      TimingView::OnExportSVG)
    EVT_MENU(TIMING_ID_EXPORT_PS,       TimingView::OnExportPS)
END_EVENT_TABLE()

void TimingView::OnActivateView(bool activate, wxView *activeView, wxView *deactiveView)
{
    if ( activate && this == activeView )
        AttachPanels();
}

TimingView::TimingView()
{
    window = (DiagramSplitterWindow *)NULL;
    frame = (wxDocMDIChildFrame *)NULL;
    GridStepWidth = 15;
    ClkSetPanel = (ClockSettingsPanel *)NULL;
    TranSetPanel = (TransitionSettingsPanel *)NULL;
    AxisSetPanel = (AxisSettingsPanel *)NULL;
    TmeCmprssrPanel = (TimeCompressorSettingsPanel *)NULL;
}
bool TimingView::OnCreate(wxDocument *doc, long WXUNUSED(flags))
{
    frame = wxGetApp().CreateChildFrame(doc, this);
    window = wxGetApp().GetMainFrame()->CreateWindow(this, frame);

#ifdef __X__
    int x, y;
    frame->GetSize(&x, &y);
    frame->SetSize(wxDefaultCoord, wxDefaultCoord, x, y);
#endif

    frame->Show(true);
    Activate(true);
    UpdateVisibelTicksContainer();
    if ( ClkSetPanel == NULL ) wxMessageBox(_T("HALT!!!"));
    else AttachPanels();

    return true;
}
void TimingView::SetPanels(ClockSettingsPanel *csp, TransitionSettingsPanel *tsp, AxisSettingsPanel *asp, TimeCompressorSettingsPanel *tcsp)
{
    ClkSetPanel = csp;
    TranSetPanel = tsp;
    AxisSetPanel = asp;
    TmeCmprssrPanel = tcsp;
}

void TimingView::AttachPanels()
{
    TimingDocument *doc = (TimingDocument *)GetDocument();

    if ( !doc )
    {
        DetachPanels();
        return;
    }

    ClkSetPanel->view = this;
    if ( IsSelectedSignalClock() )
        UpdateClockPanel();

    TranSetPanel->view = this;
    UpdateTransitionPanel();

    AxisSetPanel->view = this;
    UpdateAxisPanel();

    TmeCmprssrPanel->view = this;
    UpdateTimeCompressorPanel();
}
void TimingView::DetachPanels()
{
    ClkSetPanel->view = (TimingView *)NULL;
    TranSetPanel->view = (TimingView *)NULL;
    AxisSetPanel->view = (TimingView *)NULL;
    TmeCmprssrPanel->view = (TimingView *)NULL;
}

void TimingView::UpdateClockPanel()
{
    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( !doc ) return;

    if ( IsSelectedSignalClock() )
    {
//        wxInt32 n;
//        wxString str;

////        n = doc->signals[editingNumber].delay;
//        str = wxString::Format ( _( "%d" ) , n);
//        ClkSetPanel->SetDelayText(str);
//
//        n = doc->signals[editingNumber].ticks;
//        str = wxString::Format ( _( "%d" ) , n);
//        ClkSetPanel->SetTicksText(str);
//
//        ClkSetPanel->SetShadowed(doc->signals[editingNumber].GenerateBackground);
//        ClkSetPanel->SetShowPeriod(doc->signals[editingNumber].ShowPeriodCount);
    }
}
void TimingView::UpdateTransitionPanel()
{
    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( !doc ) return;

    TranSetPanel->SetTransitionWidth(doc->TransitWidth);
    TranSetPanel->Set50(doc->en50);
    TranSetPanel->Set90(doc->en90);

}

void TimingView::UpdateAxisPanel()
{
    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( !doc ) return;

    AxisSetPanel->SetTickLength(doc->TickLength);
    AxisSetPanel->SetLengthUnit(doc->TickLengthUnit + 5);
    AxisSetPanel->SetMarkerLength(doc->MarkerLength);
    AxisSetPanel->SetOffset(doc->timeOffset);
    AxisSetPanel->SetTotalLengt(doc->length);
}

void TimingView::UpdateTimeCompressorPanel(void)
{
    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( !doc ) return;

    if ( !DiscontSelected() ) return;

    for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx )
    ////if ( doc->compressors[indx].pos == editingValA )
    //{
        TmeCmprssrPanel->SetTimeText(
            wxString::Format( _("%d"), doc->compressors[indx].length )
        );
    //}
}
void TimingView::SetAxis(wxInt8 unit, wxInt32 ticklength, wxInt32 markerlength, wxInt32 offset, wxInt32 totallength)
{
    ///SetFocus();
    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( !doc ) return;
    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
    cmdproc->Submit(
        new ChangeAxisSettings(doc, unit-5, ticklength, markerlength, offset, totallength )
    );
    UpdateAxisPanel();
}
void TimingView::SetClock(wxInt32 delay, wxInt32 ticks, bool shadow, bool DrawPeriod)
{
    ///SetFocus();
    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( !doc ) return;
    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();

    if ( IsSelectedSignalClock() )
    {
        cmdproc->Submit(
            new ChangeClockParamCommand(doc, GetSelectedSignalNumber() , ticks, delay, shadow, DrawPeriod)
        );
    }
}
void TimingView::SetTransition(wxInt8 width, bool en50, bool en90)
{
    ///SetFocus();
    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( !doc ) return;
    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
    cmdproc->Submit(
        new ChangeTransitionWidth(doc, width, en50, en90)
    );
    UpdateTransitionPanel();
}
void TimingView::SetTimeCompressor(wxInt32 time)
{
    ///SetFocus();
    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( !doc ) return;
    if ( !DiscontSelected() ) return;

    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
    for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
     if ( doc->compressors[indx].pos == GetSelectedCompressorsIndex() )
        cmdproc->Submit(
            new ChangeTimeCompressor(doc, GetSelectedCompressorsIndex(), time, doc->compressors[indx].enabled)
        );

    UpdateTimeCompressorPanel();
}

void TimingView::OnDraw(wxDC *dc){} // is virtual and has nothing to do (flicker free machanism is walking on another way)

void TimingView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint))
{
    TimingDocument *doc = (TimingDocument *)m_viewDocument;
    wxFileName fn( doc->GetFilename() );
    wxString fname = fn.GetName();

    if ( fname.IsEmpty() )
        fname = doc->GetTitle();

    if (frame)
    {
        if ( doc->IsModified() )
            frame->SetTitle( _("*") + fname );
        else
            frame->SetTitle( fname );
    }


    UpdateVisibelTicksContainer();
    UpdateHeightsContainer();

    AttachPanels();
    if (window)
        window->Update();
}
void TimingView::UpdateHeightsContainer()
{
    TimingDocument *doc = (TimingDocument *)m_viewDocument;

    heightOffsets.clear();
    wxCoord yoffset = 20;
    for ( unsigned int k = 0 ; k < doc->signals.size() ; ++k )
    {
        /// remember position
        heightOffsets.push_back(yoffset);
        /// update position for next signal
        yoffset += doc->SignalHeight;
        yoffset += doc->MinimumSignalDistance;
        yoffset += doc->signals[k].space;
        yoffset += doc->signals[k].prespace;
    }
    heightOffsets.push_back(yoffset);// remember last offset
}
void TimingView::UpdateVisibelTicksContainer()
{
    TimingDocument *doc = (TimingDocument *)m_viewDocument;

    VisibleTicks.clear();
    wxInt32 n = 0;
    while ( n <= doc->length )
    {
        VisibleTicks.push_back(n);

        bool hascompressor = false;
        for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
        if ( doc->compressors[indx].pos == n && doc->compressors[indx].enabled )
        {
            n += doc->compressors[indx].length;
            if ( n > doc->length )
                VisibleTicks.push_back(n);
            hascompressor = true;
        }
        if ( !hascompressor )
            ++n;

    }
}

wxString TimingView::GetFloatFormatStr() const
{
    unsigned char digitsAfterDecimalpoint = 2;
    return wxString::Format(_T("\%.%df "), digitsAfterDecimalpoint);
}
wxFont TimingView::GetFont() const
{
    return wxFont(10, wxFONTFAMILY_SWISS,  wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
}

void TimingView::OnDelete(wxCommandEvent& WXUNUSED(event) )
{
    //window->DeleteSelection();
    return;
}

// Clean up windows used for displaying the view.
bool TimingView::OnClose(bool deleteWindow)
{
    if (!GetDocument()->Close()) return false;

////    window->DetachPanels();
//
//    // Clear the canvas in  case we're in single-window mode,
//    // and the canvas stays.
//    window->ClearBackground();
    window->view = (TimingView *)NULL;
    window = (DiagramSplitterWindow *)NULL;

    SetFrame((wxFrame*)NULL);
    Activate(false);

    // Remove file menu from those managed by the command history
    wxMenu* fileMenu = frame->GetMenuBar()->GetMenu(0);
    wxGetApp().GetDocManager()->FileHistoryRemoveMenu(fileMenu);

    if (deleteWindow)
        delete frame;
    return true;
}

void TimingView::OnCopy(wxCommandEvent& WXUNUSED(event) )
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

void TimingView::OnCut(wxCommandEvent& WXUNUSED(event) )
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

void TimingView::OnPaste(wxCommandEvent& WXUNUSED(event) )
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
            TimingEditorSignaDataObject data(sig);

            wxTheClipboard->GetData( data );
            AddSignal( *sig );
        }
        wxTheClipboard->Close();
    }
}
void TimingView::AddSignal(Signal sig)
{
    TimingDocument *doc = (TimingDocument*)m_viewDocument;
    if ( !doc ) return;

////    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
//    cmdproc->Submit(new AddSignalCommand(doc, window->GetSelectedSignalNr(), sig) );
}

void TimingView::OnSelectAll(wxCommandEvent& WXUNUSED(event) )
{
    //if( window )
        ////window->SelectAll();
}
void TimingView::OnZoomTicksOut(wxCommandEvent& WXUNUSED(event) )
{
    if ( GridStepWidth > 6 )
        GridStepWidth  *= 0.7;
    if ( GridStepWidth < 6 ) GridStepWidth = 6;
    if ( window )
        window->Update();
}
void TimingView::OnZoomTicksIn(wxCommandEvent& WXUNUSED(event) )
{
    if ( GridStepWidth < 150 )
        GridStepWidth  *= 1.4;
    if ( GridStepWidth > 150 ) GridStepWidth = 150;
    if ( window )
        window->Update();
}
void TimingView::OnAddClock(wxCommandEvent& event)
{
    TimingDocument *doc = (TimingDocument*)m_viewDocument;
    wxInt32 n = 0;////window->GetSelectedSignalNr();

    Signal sig;
    sig.delay=1;
    sig.ticks=2;
    sig.IsClock = true;
    sig.IsBus = false;
    sig.name = _("newClock");
    sig.space = 0;
    sig.prespace = 0;

    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();

////    window->SetNeutralState();
//    window->SetCursor(*wxCROSS_CURSOR);

    cmdproc->Submit( new AddSignalCommand(doc, n, sig) );

}
void TimingView::OnAddSignal(wxCommandEvent& event)
{
    TimingDocument *doc = (TimingDocument*)m_viewDocument;
    wxInt32 n = 0;///window->GetSelectedSignalNr();

    Signal sig;
    sig.delay = 1;
    sig.ticks = 2;
    sig.IsClock = false;
    sig.IsBus = false;
    sig.name = _("newSignal");
    sig.space = 0;
    sig.prespace = 0;
    for (wxInt32 n = 0 ; n < doc->length; ++n)
        sig.values.push_back(zero);

////    window->SetNeutralState();
//    window->SetCursor(*wxCROSS_CURSOR);

    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();

    cmdproc->Submit( new AddSignalCommand(doc, n, sig) );

}
void TimingView::OnAddBus(wxCommandEvent& event)
{
    TimingDocument *doc = (TimingDocument*)m_viewDocument;
    wxInt32 n = 0;////window->GetSelectedSignalNr();

    Signal sig;
    sig.delay = 1;
    sig.ticks = 2;
    sig.IsClock = false;
    sig.IsBus = true;
    sig.name = _("newBus");
    sig.space = 0;
    sig.prespace = 0;
    for (wxInt32 n = 0 ; n < doc->length; ++n)
        sig.values.push_back(zero);
    sig.TextValues[0] = _("Data");

////    window->SetNeutralState();
//    window->SetCursor(*wxCROSS_CURSOR);

    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();

    cmdproc->Submit( new AddSignalCommand(doc, n, sig) );
}
void TimingView::OnInsertDiscontTool(wxCommandEvent& event)
{
    //if ( window )
        ////window->OnSelectInsertDiscontTool();
}
void TimingView::OnSelectRuler(wxCommandEvent& event)
{
    //if ( window )
        ////window->OnSelectRulerTool();
}
void TimingView::OnSelectHArrow(wxCommandEvent& event)
{
    //if ( window )
        ////window->OnSelectHArrowTool();
}
void TimingView::OnSelectTextTool(wxCommandEvent& event)
{
    //if ( window )
        ////window->OnSelectTextTool();
}
void TimingView::OnSelectNeutral(wxCommandEvent& event)
{
    //if ( window )
        ////window->OnSelectNeutralTool();
}
bool TimingView::CanZoomIn(void)
{
    return GridStepWidth < 150;
}
bool TimingView::CanZoomOut(void)
{
    return GridStepWidth > 6;
}
bool TimingView::CanPaste(void)
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
bool TimingView::IsSomethingSelected(void)
{
////    if ( window && (
//        window->IsTextSelected() ||
//        window->IsSignalSelected() ||
//        window->VLineIsSelected() ||
//        window->HArrowIsSelected() ||
//        window->DiscontSelected() )
//    )
//        return true;
    return false;
}
bool TimingView::IsTextSelected(void)
{
////    if ( window && window->IsTextSelected() )
//        return true;
    return false;
}
wxInt32 TimingView::GetSelectedSignalNumber()
{
    return -1;
}
wxInt32 TimingView::GetSelectedCompressorsIndex()
{
    ////
    return -1;
}
bool TimingView::DiscontSelected()
{
    ////
    return false;
}
bool TimingView::IsSelectedSignalClock()
{
    wxInt32 selsig = GetSelectedSignalNumber();

    if ( selsig < 0 )
        return false;

    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( doc->signals.size() < (unsigned int)selsig)
        return false;

    return doc->signals[selsig].IsClock;
}

bool TimingView::IsSignalSelected(void)
{
    return GetSelectedSignalNumber() != -1;
}
bool TimingView::CanDelete(void)
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
void TimingView::OnExportBitmap(wxCommandEvent& event)
{
//    wxFileDialog dlg( wxGetApp().GetMainFrame(), _T("Choose a file for exporting into it"),_T(""),_T(""),_T("PNG files (*.png)|*.png"),wxSAVE | wxOVERWRITE_PROMPT );
//    if ( dlg.ShowModal() != wxID_OK )
//        return;
//
//    wxString filename = dlg.GetPath();
//    if ( filename.empty() )
//        return;
//
//    /// calc size
//    //wxBitmap bm(10,10);
//    wxMemoryDC *memdc = new wxMemoryDC();
//    //memdc->SelectObject(bm);
//
//    wxPoint s = window->GetBitmapSize();
//
//
//    wxBitmap bitmap(s.x, s.y);
//    memdc->SelectObject(bitmap);
//
//    /// draw using the memdc
//    //memdc->SetBackground(*wxWHITE_BRUSH);
//    //window->PaintBackground(*memdc);
//    memdc->SetBrush(*wxWHITE_BRUSH);
//    memdc->SetPen(*wxWHITE_PEN);
//    memdc->DrawRectangle(0, 0, s.x, s.y);
//    memdc->SetPen(*wxBLACK_PEN);
//    window->Draw(*memdc, true);
//
//
//    memdc->SelectObject(wxNullBitmap);
//    bitmap.SaveFile(filename, wxBITMAP_TYPE_PNG); //wxBITMAP_TYPE_JPEG);

}

void TimingView::OnExportSVG(wxCommandEvent& event)
{
//    wxFileDialog dlg( wxGetApp().GetMainFrame(), _T("Choose a file for exporting into it"), _T(""), _T(""), _T("SVG files (*.svg)|*.svg"), wxSAVE | wxOVERWRITE_PROMPT);
//    if ( dlg.ShowModal() != wxID_OK )
//        return;
//    wxString filename = dlg.GetPath();
//    if ( filename.empty() )
//        return;
//
//    wxPoint s = window->GetBitmapSize();
//    wxSVGFileDC *svgdc = new wxSVGFileDC(filename, s.x, s.y);
//    svgdc->SetBrush(*wxWHITE_BRUSH);
//    svgdc->SetPen(*wxBLACK_PEN);
//    //svgdc->DrawRectangle(0, 0, s.x, s.y);
//    window->Draw(*svgdc, true);
//
//    delete svgdc;
}

void TimingView::OnExportPS(wxCommandEvent& event)
{
//    wxFileDialog dlg( wxGetApp().GetMainFrame(), _T("Choose a file for exporting into it"), _T(""), _T(""), _T("PostScrip files (*.ps)|*.ps"), wxSAVE | wxOVERWRITE_PROMPT);
//    if ( dlg.ShowModal() != wxID_OK )
//        return;
//    wxString filename = dlg.GetPath();
//    if ( filename.empty() )
//        return;
//
//    wxPoint s = window->GetBitmapSize();
//    //wxPostScriptDC *psdc = new wxPostScriptDC(filename);
//    wxPrintData g_printData;
//    g_printData.SetFilename(filename);
//    wxPostScriptDC *psdc = new wxPostScriptDC(g_printData);
//
//    psdc->StartDoc(_T("Printing PS"));
//    //psdc->SetUserScale(1.0,1.0);
//    //psdc->SetBackground(*wxTRANSPARENT_BRUSH);
//    psdc->Clear();
//    psdc->SetBackgroundMode(wxTRANSPARENT);
//    psdc->SetBrush(*wxWHITE_BRUSH);
//    psdc->SetPen(*wxBLACK_PEN);
//    //svgdc->DrawRectangle(0, 0, s.x, s.y);
//    window->Draw(*psdc, true);
//
//    delete psdc;
}


