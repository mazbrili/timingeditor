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

#include "TimingView.h"
#include "TimingWindow.h"
#include "TimingFrame.h"
#include "TimingApp.h"
//#include "doc.h"
#include "ClockEditDlg.h"
#include "cmd.h"
#include "enumers.h"


IMPLEMENT_DYNAMIC_CLASS(TimingView, wxView)
BEGIN_EVENT_TABLE(TimingView, wxView)
    EVT_MENU(wxID_DELETE,               TimingView::OnDelete)
    EVT_MENU(wxID_SELECTALL,            TimingView::OnSelectAll)
    EVT_MENU(wxID_COPY,                 TimingView::OnCopy)
    EVT_MENU(wxID_CUT,                  TimingView::OnCut)
    EVT_MENU(wxID_PASTE,                TimingView::OnPaste)
    EVT_MENU(TIMING_ID_GLASS_N,         TimingView::OnZoomTicksOut)
    EVT_MENU(TIMING_ID_GLASS_P,         TimingView::OnZoomTicksIn)
    //EVT_MENU(TIMING_ID_DELETE_SIGNAL,   TimingView::OnDeleteSignal)
    EVT_MENU(TIMING_ID_CHANGECLOCK,     TimingView::OnEditClock)
    EVT_MENU(TIMING_ID_ADD_CLOCK,       TimingView::OnAddClock)
    EVT_MENU(TIMING_ID_ADD_SIGNAL,      TimingView::OnAddSignal)
    EVT_MENU(TIMING_ID_ADD_BUS,         TimingView::OnAddBus)
    EVT_MENU(TIMING_ID_DISCONTINUATION, TimingView::OnInsertDiscontTool)
    EVT_MENU(TIMING_ID_RULER,           TimingView::OnSelectRuler)
    EVT_MENU(TIMING_ID_HARROW,          TimingView::OnSelectHArrow)
    EVT_MENU(TIMING_ID_EDITTEXT,        TimingView::OnSelectTextTool)
    EVT_MENU(TIMING_ID_EXPORT_BITMAP,   TimingView::OnExportBitmap)
    EVT_MENU(TIMING_ID_EXPORT_SVG,      TimingView::OnExportSVG)
    EVT_MENU(TIMING_ID_EXPORT_PS,       TimingView::OnExportPS)
END_EVENT_TABLE()

void TimingView::OnEditClock(wxCommandEvent& event)
{
    ClockEditDlg dlg(window, wxID_ANY, _T("Change clock parameters") );
    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( !doc ) return;
    wxInt32 signr = window->GetSelectedSignalNr();
    if ( signr == -1) return;
    wxInt32 period = doc->signals[signr].ticks;
    wxInt32 delay = doc->signals[signr].delay;

    dlg.SetTextPeriod(wxString::Format(_T("%d"), period));
    dlg.SetTextDelay(wxString::Format(_T("%d"), delay));

    if ( dlg.ShowModal() == wxID_OK )
    {
        long val;
        dlg.GetTextPeriod().ToLong(&val);
        period = val;
        dlg.GetTextDelay().ToLong(&val);
        delay = val;
        wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
        cmdproc->Submit(
            new ChangeClockParamCommand(doc, signr, period, delay)
        );
    }
}
bool TimingView::OnCreate(wxDocument *doc, long WXUNUSED(flags) )
{
    frame = wxGetApp().CreateChildFrame(doc, this);
    frame->SetTitle(_T("MyView"));

     window = wxGetApp().GetMainFrame()->CreateWindow(this, frame);
#ifdef __X__
    // X seems to require a forced resize
    int x, y;
    frame->GetSize(&x, &y);
    frame->SetSize(wxDefaultCoord, wxDefaultCoord, x, y);
#endif
    frame->Show(true);
    Activate(true);

    return true;
}
void TimingView::OnDraw(wxDC *dc){} // is virtual, has nothing to do (flicker free machanism is walking on another way)

void TimingView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint))
{
  if (window)
    window->Refresh();
}

//void TimingView::OnDeleteSignal(wxCommandEvent& event)
//{
//    window->DeleteSignal();
//}


void TimingView::OnDelete(wxCommandEvent& WXUNUSED(event) )
{
    window->DeleteSelection();
    return;
}

// Clean up windows used for displaying the view.
bool TimingView::OnClose(bool deleteWindow)
{
    if (!GetDocument()->Close()) return false;

    // Clear the canvas in  case we're in single-window mode,
    // and the canvas stays.
    window->ClearBackground();
    window->view = (wxView *) NULL;
    window = (TimingWindow *) NULL;

    wxString s(wxTheApp->GetAppName());
    if (frame) frame->SetTitle(s);

    SetFrame((wxFrame*)NULL);
    Activate(false);

    // Remove file menu from those managed by the command history
    wxMenu* fileMenu = frame->GetMenuBar()->GetMenu(0);
    wxGetApp().GetDocManager()->FileHistoryRemoveMenu(fileMenu);

    if (deleteWindow)
    {
        delete frame;
        return true;
    }
    return true;
}

void TimingView::OnCopy(wxCommandEvent& WXUNUSED(event) )
{
    if (wxTheClipboard->Open())
    {
        // This data objects are held by the clipboard,
        // so do not delete them in the app.
        wxString str(window->GetText());
        wxTheClipboard->SetData( new wxTextDataObject(str) );
        wxTheClipboard->Close();
        //window->DeleteText();
    }
}

void TimingView::OnCut(wxCommandEvent& WXUNUSED(event) )
{
    if (wxTheClipboard->Open())
    {
        // This data objects are held by the clipboard,
        // so do not delete them in the app.
        wxString str(window->GetText());
        wxTheClipboard->SetData( new wxTextDataObject(str) );
        wxTheClipboard->Close();
        window->DeleteText();
    }
}

void TimingView::OnPaste(wxCommandEvent& WXUNUSED(event) )
{
    if (wxTheClipboard->Open())
    {
        if (wxTheClipboard->IsSupported( wxDF_TEXT ))
        {
            wxTextDataObject data;
            wxTheClipboard->GetData( data );
            //wxMessageBox( data.GetText() );
            window->InsertText( data.GetText() );
        }
        wxTheClipboard->Close();
    }
}

void TimingView::OnSelectAll(wxCommandEvent& WXUNUSED(event) )
{
    window->SelectAll();
}
void TimingView::OnZoomTicksOut(wxCommandEvent& WXUNUSED(event) )
{
    if( !window ) return;
    window->ZoomTicksOut();
}
void TimingView::OnZoomTicksIn(wxCommandEvent& WXUNUSED(event) )
{
    if( ! window ) return;
    window->ZoomTicksIn();
}
void TimingView::OnAddClock(wxCommandEvent& event)
{
    TimingDocument *doc = (TimingDocument*)m_viewDocument;
    wxInt32 n = window->GetSelectedSignalNr();

    Signal sig;
    sig.delay=1;
    sig.ticks=2;
    sig.IsClock = true;
    sig.IsBus = false;
    sig.name = _("newClock");
    sig.space = 0;
    sig.prespace = 0;

    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();

    cmdproc->Submit( new AddSignalCommand(doc, n, sig) );

}
void TimingView::OnAddSignal(wxCommandEvent& event)
{
    TimingDocument *doc = (TimingDocument*)m_viewDocument;
    wxInt32 n = window->GetSelectedSignalNr();

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

    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();

    cmdproc->Submit( new AddSignalCommand(doc, n, sig) );

}
void TimingView::OnAddBus(wxCommandEvent& event)
{
    TimingDocument *doc = (TimingDocument*)m_viewDocument;
    wxInt32 n = window->GetSelectedSignalNr();

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
    //sig.TextValues[0] = _("Data");

    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();

    cmdproc->Submit( new AddSignalCommand(doc, n, sig) );
}

void TimingView::OnInsertDiscontTool(wxCommandEvent& event)
{
    window->OnSelectInsertDiscontTool();
}

void TimingView::OnSelectRuler(wxCommandEvent& event)
{
    window->OnSelectRulerTool();
}
void TimingView::OnSelectHArrow(wxCommandEvent& event)
{
    window->OnSelectHArrowTool();
}

void TimingView::OnSelectTextTool(wxCommandEvent& event)
{
    window->OnSelectTextTool();
}

void TimingView::OnExportBitmap(wxCommandEvent& event)
{
    wxFileDialog dlg( wxGetApp().GetMainFrame(), _T("Choose a file for exporting into it"),_T(""),_T(""),_T("PNG files (*.png)|*.png"),wxSAVE | wxOVERWRITE_PROMPT );
    if ( dlg.ShowModal() != wxID_OK )
        return;

    wxString filename = dlg.GetPath();
    if ( filename.empty() )
        return;

    /// calc size
//    wxBitmap bm(10,10);
    wxMemoryDC *memdc = new wxMemoryDC();
//    memdc->SelectObject(bm);

    wxPoint s = window->GetBitmapSize();


    wxBitmap bitmap(s.x, s.y);
    memdc->SelectObject(bitmap);

    /// draw using the memdc
    //memdc->SetBackground(*wxWHITE_BRUSH);
    //window->PaintBackground(*memdc);
    memdc->SetBrush(*wxWHITE_BRUSH);
    memdc->SetPen(*wxWHITE_PEN);
    memdc->DrawRectangle(0, 0, s.x, s.y);
    memdc->SetPen(*wxBLACK_PEN);
    window->Draw(*memdc, true);


    memdc->SelectObject(wxNullBitmap);
    bitmap.SaveFile(filename, wxBITMAP_TYPE_PNG); //wxBITMAP_TYPE_JPEG);

}

void TimingView::OnExportSVG(wxCommandEvent& event)
{
    wxFileDialog dlg( wxGetApp().GetMainFrame(), _T("Choose a file for exporting into it"), _T(""), _T(""), _T("SVG files (*.svg)|*.svg"), wxSAVE | wxOVERWRITE_PROMPT);
    if ( dlg.ShowModal() != wxID_OK )
        return;
    wxString filename = dlg.GetPath();
    if ( filename.empty() )
        return;

    wxPoint s = window->GetBitmapSize();
    wxSVGFileDC *svgdc = new wxSVGFileDC(filename, s.x, s.y);
    svgdc->SetBrush(*wxWHITE_BRUSH);
    svgdc->SetPen(*wxBLACK_PEN);
    //svgdc->DrawRectangle(0, 0, s.x, s.y);
    window->Draw(*svgdc, true);

    delete svgdc;
}

void TimingView::OnExportPS(wxCommandEvent& event)
{
    wxFileDialog dlg( wxGetApp().GetMainFrame(), _T("Choose a file for exporting into it"), _T(""), _T(""), _T("SVG files (*.ps)|*.ps"), wxSAVE | wxOVERWRITE_PROMPT);
    if ( dlg.ShowModal() != wxID_OK )
        return;
    wxString filename = dlg.GetPath();
    if ( filename.empty() )
        return;

    wxPoint s = window->GetBitmapSize();
    wxPostScriptDC *psdc = new wxPostScriptDC(filename);

    psdc->SetBrush(*wxWHITE_BRUSH);
    psdc->SetPen(*wxBLACK_PEN);
    //svgdc->DrawRectangle(0, 0, s.x, s.y);
    window->Draw(*psdc, true);

    delete psdc;
}


