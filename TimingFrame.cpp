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

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include <wx/config.h>
#include <wx/tipdlg.h>

#include "TimingFrame.h"
#include "TimingApp.h"//GetApp
#include "enumers.h"

#include "TimingWindow.h"
#include "TimingView.h"
#include "dndfile.h"


#include "art/new.xpm"
#include "art/open.xpm"
#include "art/save.xpm"
#include "art/cut.xpm"
#include "art/copy.xpm"
#include "art/paste.xpm"
#include "art/undo.xpm"
#include "art/redo.xpm"
#include "art/help.xpm"
#include "art/glassntool.xpm"
#include "art/glassptool.xpm"
#include "art/tri.xpm"
#include "art/ruler_cur.xpm"
#include "art/harrow_cur.xpm"
#include "art/textedit_cur.xpm"




///////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS(TimingFrame, wxDocMDIParentFrame)
BEGIN_EVENT_TABLE(TimingFrame, wxDocMDIParentFrame )
    EVT_MENU(wxID_ABOUT,                     TimingFrame::OnAbout)
    EVT_MENU(TIMING_ID_TIP,                  TimingFrame::OnTip)
    EVT_UPDATE_UI(wxID_COPY,                 TimingFrame::OnUpdateCopy)
    EVT_UPDATE_UI(wxID_CUT,                  TimingFrame::OnUpdateCut)
    EVT_UPDATE_UI(wxID_PASTE,                TimingFrame::OnUpdatePaste)
    EVT_UPDATE_UI(wxID_DELETE,               TimingFrame::OnUpdateDelete)
    EVT_UPDATE_UI(TIMING_ID_GLASS_N,         TimingFrame::OnUpdateGlassN)
    EVT_UPDATE_UI(TIMING_ID_GLASS_P,         TimingFrame::OnUpdateGlassP)
    EVT_UPDATE_UI(TIMING_ID_DISCONTINUATION, TimingFrame::OnUpdateDiscont)
    EVT_UPDATE_UI(TIMING_ID_RULER,           TimingFrame::OnUpdateDiscont)
    EVT_UPDATE_UI(TIMING_ID_HARROW,          TimingFrame::OnUpdateDiscont)
    EVT_UPDATE_UI(TIMING_ID_EDITTEXT,        TimingFrame::OnUpdateDiscont)
END_EVENT_TABLE()


TimingFrame::TimingFrame(wxDocManager *manager, wxFrame *frame, int id, const wxString& title, wxPoint pos, wxSize size, int style )
  //: wxFrame( parent, id, title, pos, size, style )
  : wxDocMDIParentFrame(manager, frame, id, title, pos, size, style)


{
    editMenu = (wxMenu *) NULL;

    CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL);
    InitToolBar(GetToolBar());

	/// Accelerators
    wxAcceleratorEntry entries[1];
    entries[0].Set(wxACCEL_CTRL | wxACCEL_SHIFT , (int) 'Z', wxID_REDO);
    wxAcceleratorTable accel(1, entries);
    SetAcceleratorTable(accel);

    SetDropTarget(new DnDFile(manager));
}

TimingFrame::~TimingFrame()
{
    wxConfig *pConfig = wxGetApp().GetConfig();
    if ( pConfig == NULL )
        return;

    // save the frame position
    wxInt32 x, y, w, h, s;
    GetClientSize(&w, &h);
    GetPosition(&x, &y);
    s = 0;
    if ( IsMaximized() )
        s = 1;
    if ( IsIconized() )
        s= -1;
    if ( s == 0 )
    {
        pConfig->Write(_T("/MainFrame/x"), (long) x);
        pConfig->Write(_T("/MainFrame/y"), (long) y);
        pConfig->Write(_T("/MainFrame/w"), (long) w);
        pConfig->Write(_T("/MainFrame/h"), (long) h);
    }
    pConfig->Write(_T("/MainFrame/s"), (long) s);
}

void TimingFrame::OnAbout(wxCommandEvent &event)
{
    /*wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));*/
    wxMessageBox(_T("TimingEditor..."), _T("About TimingEditor"));
}

void TimingFrame::OnUpdateCopy(wxUpdateUIEvent& event)
{
    event.Enable(IsTextSelected());
}
void TimingFrame::OnUpdateCut(wxUpdateUIEvent& event)
{
    event.Enable(IsTextSelected());
}
void TimingFrame::OnUpdateDelete(wxUpdateUIEvent& event)
{
    event.Enable(CanDelete());
}
bool TimingFrame::CanDelete(void)
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view )
        if ( view->window )
            if
            ( /*view->window->selpos[0] != -1 &&
                 view->window->selpos[1] != -1 &&
                 view->window->selpos[0] != view->window->selpos[1] */
                view->window->IsTextSelected() ||
                view->window->IsSignalSelected() ||
                view->window->VLineIsSelected() ||
                view->window->HArrowIsSelected() ||
                view->window->CanDeleteText()
            )
                return true;
    return false;
}
bool TimingFrame::IsSomethingSelected(void)
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view )
        if ( view->window )
            if
            ( /*view->window->selpos[0] != -1 &&
                 view->window->selpos[1] != -1 &&
                 view->window->selpos[0] != view->window->selpos[1] */
                view->window->IsTextSelected() ||
                view->window->IsSignalSelected() ||
                view->window->VLineIsSelected() ||
                view->window->HArrowIsSelected()
            )
                return true;
    return false;
}
bool TimingFrame::IsTextSelected(void)
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view )
        if ( view->window )
            if ( view->window->IsTextSelected() )
                return true;
    return false;
}
void TimingFrame::OnUpdatePaste(wxUpdateUIEvent& event)
{
    //event.Enable(true);
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view )
        if ( view->window )
            if ( view->window->CanPaste() )
            {
                event.Enable(true);
                return;
            }
    event.Enable(false);
}
void TimingFrame::OnUpdateDiscont(wxUpdateUIEvent& event)
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view )
    {
        event.Enable(true);
        return;
    }
    event.Enable(false);
}
TimingWindow *TimingFrame::CreateWindow(wxView *view, wxMDIChildFrame *parent)
{
    //TimingWindow *canvas = new TimingWindow(view, parent);
    //canvas->SetCursor(wxCursor(wxCURSOR_ARROW));
    //canvas->SetScrollRate(5, 5);
    //return canvas;

    return new TimingWindow(view, parent);
}

void TimingFrame::InitToolBar(wxToolBar* toolBar)
{
    toolBar->AddTool(wxID_NEW, _T("New file"), wxBitmap( new_xpm ), wxNullBitmap, wxITEM_NORMAL, _T("New file"), _T("Create a new file"));
    toolBar->AddTool(wxID_OPEN, _T("Open file"), wxBitmap( open_xpm ), wxNullBitmap, wxITEM_NORMAL, _T("Open file"), _T(""));
    toolBar->AddTool(wxID_SAVE, _T("Save file"), wxBitmap( save_xpm ), wxNullBitmap, wxITEM_NORMAL, _T("Save file"), _T(""));
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_CUT, _T("Cut"), wxBitmap( cut_xpm ), wxNullBitmap, wxITEM_NORMAL, _T("Cut"), _T("Cut"));
    toolBar->AddTool(wxID_COPY, _T("Copy"), wxBitmap( copy_xpm ), wxNullBitmap, wxITEM_NORMAL, _T("Copy"), _T("Copy"));
    toolBar->AddTool(wxID_PASTE, _T("Paste"), wxBitmap( paste_xpm ), wxNullBitmap, wxITEM_NORMAL, _T("Paste"), _T("Paste"));
    toolBar->AddSeparator();
    //toolBar->AddTool(wxID_PRINT, _T("Print"), wxBitmap( print_xpm ), wxNullBitmap, wxITEM_NORMAL, _T("Print"), _T("Print the active file"));
    //toolBar->AddSeparator();
    toolBar->AddTool(wxID_UNDO, _T("Undo"), wxBitmap( undo_xpm ), wxNullBitmap, wxITEM_NORMAL, _T("Undo"), _T("Undo last operation"));
    toolBar->AddTool(wxID_REDO, _T("Redo"), wxBitmap( redo_xpm ), wxNullBitmap, wxITEM_NORMAL, _T("Redo"), _T("Redo last operation"));
    toolBar->AddSeparator();
    toolBar->AddTool(TIMING_ID_GLASS_N, _T("Zoom out"), wxBitmap(glassntool_xpm) , wxNullBitmap, wxITEM_NORMAL, _T("Zoom out"), _T("Zoom out the Document"));
    toolBar->AddTool(TIMING_ID_GLASS_P, _T("Zoom in"), wxBitmap(glassptool_xpm) , wxNullBitmap, wxITEM_NORMAL, _T("Zoom in"), _T("Zoom in the Document"));
    toolBar->AddSeparator();
    toolBar->AddTool(TIMING_ID_DISCONTINUATION, _T("Edit time compressors"), wxBitmap(tri_xpm), wxNullBitmap, wxITEM_NORMAL, _T("Edit time compressors"), _T("Edit time compressors by clicking on the bottom axis"));
    toolBar->AddTool(TIMING_ID_RULER, _T("Draw vertical line"), wxBitmap(ruler_cur_xpm), wxNullBitmap, wxITEM_NORMAL, _T("Draw vertical line"), _T("Draw vertical line"));
    toolBar->AddTool(TIMING_ID_HARROW, _T("Draw a hotizontal arrow"),  wxBitmap(harrow_cur_xpm), wxNullBitmap, wxITEM_NORMAL, _T("Draw a hotizontal arrow"),_T("Draw a hotizontal arrow") );
    toolBar->AddTool(TIMING_ID_EDITTEXT, _T("Edit text labels"), wxBitmap(textedit_cur_xpm),  wxNullBitmap, wxITEM_NORMAL, _T("Edit text labels"),_T("Edit text labels") );
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_ABOUT, _T("Help"), wxBitmap( help_xpm ), wxNullBitmap, wxITEM_NORMAL, _T("Help"), _T("Show info about this application"));
    toolBar->Realize();
    SetToolBar(toolBar);
}


void TimingFrame::OnTip(wxCommandEvent &event)
{
    wxConfig *config = wxGetApp().GetConfig();

    bool ShowTipsAtStartup = true;
    wxInt32 TipNumber = 0;
    config->Read(_T("/StartupTips/ShowTipsAtStarup"), &ShowTipsAtStartup);
    config->Read(_T("/StartupTips/TipNumber"), &TipNumber);

    //if ( ShowTipsAtStartup )
    {
        wxTipProvider *tipProvider = wxCreateFileTipProvider(_T("tips"), TipNumber);
        ShowTipsAtStartup = wxShowTip(this, tipProvider, ShowTipsAtStartup);
        TipNumber = tipProvider->GetCurrentTip();
        delete tipProvider;

        config->Write(_T("/StartupTips/ShowTipsAtStarup"), ShowTipsAtStartup);
        config->Write(_T("/StartupTips/TipNumber"), TipNumber);
    }

}

void TimingFrame::OnUpdateGlassN(wxUpdateUIEvent &event)
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view )
        if ( view->window )
            if
            ( view->window->CanZoomTicksOut() )
            {
                event.Enable(true);
                return;
            }
    event.Enable(false);
}
void TimingFrame::OnUpdateGlassP(wxUpdateUIEvent &event)
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view )
        if ( view->window )
            if ( view->window->CanZoomTicksIn() )
            {
                event.Enable(true);
                return;
            }
    event.Enable(false);
}
