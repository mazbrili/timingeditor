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
//#include <wx/tipdlg.h>
#include "myTipProvider.h"

#include "TimingFrame.h"
#include "TimingApp.h"//GetApp
#include "enumers.h"

#include "TimingView.h"
#include "dndfile.h"
#include "TimingWindow.h"
#include "ClockSettingsPanel.h"
#include "TransitionSettingsPanel.h"
#include "AxisSettingsPanel.h"
#include "TimeCompressorSettingsPanel.h"

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
#include "art/cross.xpm"
#include "art/risingedge.xpm"
#include "art/clockedge.xpm"
#include "art/busedge.xpm"



///////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS(TimingFrame, wxDocMDIParentFrame)
BEGIN_EVENT_TABLE(TimingFrame, wxDocMDIParentFrame )
    EVT_MENU(wxID_ABOUT,                     TimingFrame::OnAbout)
    EVT_MENU(TIMING_ID_TIP,                  TimingFrame::OnTip)
    EVT_UPDATE_UI(wxID_COPY,                 TimingFrame::OnUpdateCopy)
    EVT_UPDATE_UI(wxID_CUT,                  TimingFrame::OnUpdateCut)
    EVT_UPDATE_UI(wxID_PASTE,                TimingFrame::OnUpdatePaste)
    EVT_UPDATE_UI(TIMING_ID_DELETE,          TimingFrame::OnUpdateDelete)
    EVT_UPDATE_UI(TIMING_ID_GLASS_N,         TimingFrame::OnUpdateGlassN)
    EVT_UPDATE_UI(TIMING_ID_GLASS_P,         TimingFrame::OnUpdateGlassP)
    EVT_UPDATE_UI(TIMING_ID_DISCONTINUATION, TimingFrame::OnUpdateDiscont)
    EVT_UPDATE_UI(TIMING_ID_RULER,           TimingFrame::OnUpdateDiscont)
    EVT_UPDATE_UI(TIMING_ID_HARROW,          TimingFrame::OnUpdateDiscont)
    EVT_UPDATE_UI(TIMING_ID_EDITTEXT,        TimingFrame::OnUpdateDiscont)
    EVT_UPDATE_UI(TIMING_ID_NEUTRAL,         TimingFrame::OnUpdateDiscont)
    EVT_UPDATE_UI_RANGE(TIMING_ID_ADD_CLOCK,
                TIMING_ID_ADD_BUS,           TimingFrame::OnUpdateDiscont)
END_EVENT_TABLE()


TimingFrame::TimingFrame(wxDocManager *manager, wxFrame *frame, int id, const wxString& title, wxPoint pos, wxSize size, int style )
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

    /// wxAui
    /// notify wxAUI which frame to use
    m_manager = new wxAuiManager(this);

    clksetpanel = new ClockSettingsPanel(this);
    trnssetpanel = new TransitionSettingsPanel(this);
    axissetpanel = new AxisSettingsPanel(this);
    tcsetpanel = new TimeCompressorSettingsPanel(this);


    m_manager->AddPane(clksetpanel , wxAuiPaneInfo().MinSize(200,-1).Right().Name(_("ClockPanel")).Caption(wxT("Clock-type signal Settings")).CloseButton(false));
    m_manager->AddPane(trnssetpanel, wxAuiPaneInfo().MinSize(200,-1).Right().Name(_("TransitionPanel")).Caption(wxT("Transition Settings")).CloseButton(false));
    m_manager->AddPane(axissetpanel, wxAuiPaneInfo().MinSize(200,-1).Right().Name(_("AxisPanel")).Caption(wxT("Axis/Time Settings")).CloseButton(false));
    m_manager->AddPane(tcsetpanel  , wxAuiPaneInfo().MinSize(200,-1).Right().Name(_("TimeCompressorPanel")).Caption(wxT("Time compressor Settings")).CloseButton(false));

    // tell the manager to "commit" all the changes just made
    m_manager->Update();
}

TimingFrame::~TimingFrame()
{
    wxConfig *cfg = wxGetApp().GetConfig();
    SaveFramePositions(cfg);
    SaveAuiPerspective(cfg);

    m_manager->UnInit();
    delete m_manager;
    delete clksetpanel;
    delete trnssetpanel;
    delete axissetpanel;
    delete tcsetpanel;
}

void TimingFrame::OnAbout(wxCommandEvent &event)
{
    wxMessageBox(_T("TimingEditor..."), _T("About TimingEditor"));
}

void TimingFrame::OnUpdateCopy(wxUpdateUIEvent& event)
{
    event.Enable(IsTextSelected() || IsSignalSelected());
}
void TimingFrame::OnUpdateCut(wxUpdateUIEvent& event)
{
    event.Enable(IsTextSelected() || IsSignalSelected());
}
void TimingFrame::OnUpdateDelete(wxUpdateUIEvent& event)
{
    event.Enable(CanDelete());
}
bool TimingFrame::CanDelete(void)
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view && view->CanDelete() )
        return true;
    return false;
}
bool TimingFrame::IsSomethingSelected(void)
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view && view->IsSomethingSelected() )
        return true;
    return false;
}
bool TimingFrame::IsTextSelected(void)
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view && view->IsTextSelected() )
        return true;
    return false;
}
bool TimingFrame::IsSignalSelected(void)
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view && view->IsSignalSelected() )
        return true;
    return false;
}
void TimingFrame::OnUpdatePaste(wxUpdateUIEvent& event)
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view  && view->CanPaste() )
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
    return new TimingWindow(view, parent, clksetpanel, trnssetpanel, axissetpanel, tcsetpanel);
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
    toolBar->AddTool(TIMING_ID_ADD_CLOCK, _T("Add Clock"), wxBitmap(clockedge_xpm), wxNullBitmap, wxITEM_NORMAL, _T("Add Clock"), _T("Add a clock to the Document"));
    toolBar->AddTool(TIMING_ID_ADD_SIGNAL, _T("Add Signal"), wxBitmap(risingedge_xpm), wxNullBitmap, wxITEM_NORMAL, _T("Add Signal"), _T("Add a signal to the Document"));
    toolBar->AddTool(TIMING_ID_ADD_BUS, _T("Add Bus"), wxBitmap(busedge_xpm), wxNullBitmap, wxITEM_NORMAL, _T("Add Bus"), _T("Add a bus to the Document"));
    toolBar->AddSeparator();
    toolBar->AddTool(TIMING_ID_NEUTRAL, _T("Select"), wxBitmap(cross_xpm), wxNullBitmap, wxITEM_NORMAL, _T("Select"), _T(" Select something or change signal/bus"));
    toolBar->AddTool(TIMING_ID_DISCONTINUATION, _T("Edit time compressors"), wxBitmap(tri_xpm), wxNullBitmap, wxITEM_NORMAL, _T("Edit time compressors"), _T("Edit time compressors by clicking on the bottom axis"));
    toolBar->AddTool(TIMING_ID_RULER, _T("Draw vertical line"), wxBitmap(ruler_cur_xpm), wxNullBitmap, wxITEM_NORMAL, _T("Draw vertical line"), _T("Draw vertical line"));
    toolBar->AddTool(TIMING_ID_HARROW, _T("Draw hotizontal arrow"),  wxBitmap(harrow_cur_xpm), wxNullBitmap, wxITEM_NORMAL, _T("Draw a hotizontal arrow"),_T("Draw a hotizontal arrow") );
    toolBar->AddTool(TIMING_ID_EDITTEXT, _T("Edit text label"), wxBitmap(textedit_cur_xpm),  wxNullBitmap, wxITEM_NORMAL, _T("Edit text labels"),_T("Edit text labels") );
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_ABOUT, _T("Help"), wxBitmap( help_xpm ), wxNullBitmap, wxITEM_NORMAL, _T("Help"), _T("Show info about this application"));
    toolBar->Realize();
    SetToolBar(toolBar);
}


void TimingFrame::OnTip(wxCommandEvent &event)
{
    ShowTip(true);
}
void TimingFrame::ShowTip(bool force)
{
    wxConfig *config = wxGetApp().GetConfig();

    bool ShowTipsAtStartup = true;
    wxInt32 TipNumber = 0;
    config->Read(_T("/StartupTips/ShowTipsAtStarup"), &ShowTipsAtStartup);
    config->Read(_T("/StartupTips/TipNumber"), &TipNumber);

    if ( ShowTipsAtStartup || force )
    {
        //wxTipProvider *tipProvider = wxCreateFileTipProvider(_T("tips"), TipNumber);
        wxTipProvider *tipProvider = new myTipProvider(TipNumber);;
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
    if ( view && view->CanZoomOut() )
    {
        event.Enable(true);
        return;
    }
    event.Enable(false);
}
void TimingFrame::OnUpdateGlassP(wxUpdateUIEvent &event)
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view && view->CanZoomIn() )
    {
        event.Enable(true);
        return;
    }
    event.Enable(false);
}
void TimingFrame::SaveFramePositions(wxConfig *config)
{
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
        config->Write(_T("/MainFrame/x"), (long) x);
        config->Write(_T("/MainFrame/y"), (long) y);
        config->Write(_T("/MainFrame/w"), (long) w);
        config->Write(_T("/MainFrame/h"), (long) h);
    }
    config->Write(_T("/MainFrame/s"), (long) s);
}
void TimingFrame::SaveAuiPerspective(wxConfig *config)
{
    wxString str = m_manager->SavePerspective();
    config->Write( _T( "/MainFrame/AuiPerspective" ), str );
    /// store version to let newer versions of gui a cahnce to build a new valid perspective
    config->Write( _T( "/MainFrame/AuiPerspectiveVersion" ), 2);
}
void TimingFrame::LoadAuiPerspective(wxConfig *config)
{
    wxString str;
    long v;

    /// if version stored is older than current do not load old perspective
    config->Read(_T( "/MainFrame/AuiPerspectiveVersion" ), &v, 0);
    if ( v == 2 )
    {
        config->Read(_T( "/MainFrame/AuiPerspective" ), &str);
        m_manager->LoadPerspective(str);
    }
}
void TimingFrame::LoadFramePositions(wxConfig *config)
{
    /// restore frame position and size
        wxInt32 x, y, w, h, s;
        x = config->Read(_T("/MainFrame/x"), 50);
        y = config->Read(_T("/MainFrame/y"), 50);
        w = config->Read(_T("/MainFrame/w"), 600);
        h = config->Read(_T("/MainFrame/h"), 500);
        s = config->Read(_T("/MainFrame/s"), (long)0);
        Move(x, y);
        SetClientSize(w, h);
        if ( s > 0 )
            Maximize(true);
        if ( s < 0)
            Iconize(true);
}
