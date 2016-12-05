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
#include <wx/aboutdlg.h>
#include "myTipProvider.h"

#include <wx/log.h>

#include "TimingMainFrame.h"
#include "TimingApp.h"//GetApp
#include "enumers.h"

#include "TimingView.h"
#include "dndfile.h"
#include "DiagramSplitterWindow.h"
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
#include "art/cross.xpm"
#include "art/risingedge.xpm"
#include "art/clockedge.xpm"
#include "art/busedge.xpm"
#include "art/textedit_cur.xpm"

///////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS(TimingMainFrame, wxDocMDIParentFrame)
BEGIN_EVENT_TABLE(TimingMainFrame, wxDocMDIParentFrame )
    EVT_MENU(wxID_ABOUT,                               TimingMainFrame::OnAbout)
    EVT_MENU(TIMING_ID_HELP,                           TimingMainFrame::OnHelp)
    EVT_MENU(TIMING_ID_TIP,                            TimingMainFrame::OnTip)
    EVT_MENU(TIMING_ID_MENUITEM_CLOCK_TYPE,            TimingMainFrame::HandlePanel)
    EVT_MENU(TIMING_ID_MENUITEM_TIME_COMPRESSOR,       TimingMainFrame::HandlePanel)
    EVT_MENU(TIMING_ID_MENUITEM_AXIS_TIME,             TimingMainFrame::HandlePanel)
    EVT_MENU(TIMING_ID_MENUITEM_TRANSITION,            TimingMainFrame::HandlePanel)
    EVT_MENU(TIMING_ID_MENUITEM_DEFAULT,               TimingMainFrame::LoadDefaultLayout)
    EVT_UPDATE_UI(wxID_COPY,                           TimingMainFrame::OnUpdateCopy)
    EVT_UPDATE_UI(wxID_CUT,                            TimingMainFrame::OnUpdateCut)
    EVT_UPDATE_UI(wxID_PASTE,                          TimingMainFrame::OnUpdatePaste)
    EVT_UPDATE_UI(TIMING_ID_DELETE,                    TimingMainFrame::OnUpdateDelete)
    EVT_UPDATE_UI(TIMING_ID_GLASS_N,                   TimingMainFrame::OnUpdateGlassN)
    EVT_UPDATE_UI(TIMING_ID_GLASS_P,                   TimingMainFrame::OnUpdateGlassP)
    EVT_UPDATE_UI(TIMING_ID_DISCONTINUITY,             TimingMainFrame::OnUpdateTools)
    EVT_UPDATE_UI(TIMING_ID_RULER,                     TimingMainFrame::OnUpdateTools)
    EVT_UPDATE_UI(TIMING_ID_HORIZONTALARROW,           TimingMainFrame::OnUpdateTools)
    EVT_UPDATE_UI(TIMING_ID_EDIT,                      TimingMainFrame::OnUpdateTools)
    EVT_UPDATE_UI( TIMING_ID_MENUITEM_CLOCK_TYPE,      TimingMainFrame::UpdateMenuItems)
    EVT_UPDATE_UI( TIMING_ID_MENUITEM_TIME_COMPRESSOR, TimingMainFrame::UpdateMenuItems)
    EVT_UPDATE_UI( TIMING_ID_MENUITEM_AXIS_TIME,       TimingMainFrame::UpdateMenuItems)
    EVT_UPDATE_UI( TIMING_ID_MENUITEM_TRANSITION,      TimingMainFrame::UpdateMenuItems)
    EVT_UPDATE_UI_RANGE(TIMING_ID_ADD_CLOCK,
                TIMING_ID_ADD_BUS,                     TimingMainFrame::OnUpdateTools)
END_EVENT_TABLE()


TimingMainFrame::TimingMainFrame(wxDocManager *manager, wxFrame *frame, int id, const wxString& title, wxPoint pos, wxSize size, int style )
  : wxDocMDIParentFrame(manager, frame, id, title, pos, size, style),
  editMenu(NULL)
{

    //// Give it an icon
#ifdef __WXMSW__
    SetIcon(wxICON(aaaa)); // To Set App Icon
#endif
#ifdef __X__
    SetIcon(wxIcon("nassi.xpm"));
#endif


    InitStatusBar();
    InitMenuBar();

	/// Accelerators
    wxAcceleratorEntry entries[1];
    entries[0].Set(wxACCEL_CTRL | wxACCEL_SHIFT , (int) 'Z', wxID_REDO);
    wxAcceleratorTable accel(1, entries);
    SetAcceleratorTable(accel);

    SetDropTarget(new DnDFile(manager));

    /// wxAui
    /// notify wxAUI which frame to use
    m_manager = new wxAuiManager(this);

    InitToolBar();

    clksetpanel = new ClockSettingsPanel(this);
    trnssetpanel = new TransitionSettingsPanel(this);
    axissetpanel = new AxisSettingsPanel(this);
    tcsetpanel = new TimeCompressorSettingsPanel(this);


    m_manager->AddPane(clksetpanel , wxAuiPaneInfo().MinSize(200,-1).Right().Name("ClockPanel").Caption(_("Clock-type signal Settings")));
    m_manager->AddPane(trnssetpanel, wxAuiPaneInfo().MinSize(200,-1).Right().Name("TransitionPanel").Caption(_("Transition Settings")));
    m_manager->AddPane(axissetpanel, wxAuiPaneInfo().MinSize(200,-1).Right().Name("AxisPanel").Caption(_("Axis/Time Settings")));
    m_manager->AddPane(tcsetpanel  , wxAuiPaneInfo().MinSize(200,-1).Right().Name("TimeCompressorPanel").Caption(_("Time compressor Settings")));

    wxTextCtrl *logctrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    m_manager->AddPane(logctrl, wxAuiPaneInfo().MinSize(200,-1).Right().Name("LogPanel").Caption(_("Log Panel")).CloseButton(false));
    wxLogTextCtrl *loggerctrl = new wxLogTextCtrl(logctrl);
    DefalutLayout = m_manager->SavePerspective();

    wxLog::SetActiveTarget(loggerctrl);

    //Centre(wxBOTH);
    wxConfig *cfg = wxGetApp().GetConfig();
    cfg->SetPath( "/FileHistory" );
    m_docManager->FileHistoryLoad(*cfg);
    cfg->SetPath("/");


    LoadFramePositions(cfg);
    LoadAuiPerspective(cfg);

    // tell the manager to "commit" all the changes just made
    m_manager->Update();
}

void TimingMainFrame::InitMenuBar()
{
    /// Make a menubar
    wxMenu *file_menu = new wxMenu;
    wxMenu *edit_menu = (wxMenu *) NULL;

    file_menu->Append(wxID_NEW, _("&New...\tCtrl-N"), _("Create a new file"));
    file_menu->Append(wxID_OPEN, _("&Open...\tCtrl-O"), _("Open a existing file"));
    //file_menu->Append(NASSI_ID_IMPORT_SOURCE, _T("&Import...\tCtrl-I"), _T("Import from C source file"));
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, _("&Quit\tAlt-F4"), _("Quit the application") );
    // A nice touch: a history of files visited. Use this menu.
    m_docManager->FileHistoryUseMenu(file_menu);
    //m_docManager->FileHistoryAddFilesToMenu();

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, _("&About\tF1"), _("Show info about this application"));
    help_menu->Append(TIMING_ID_TIP, _("Tip"), _("Tips on using TimingEditor") );
    help_menu->Append(TIMING_ID_HELP, _("Help"), _("Open help pages"));

    wxMenu *panel_menu = new wxMenu;
    panel_menu->Append(TIMING_ID_MENUITEM_CLOCK_TYPE, _("Clock-type"), wxEmptyString, wxITEM_CHECK);
    panel_menu->Append(TIMING_ID_MENUITEM_TIME_COMPRESSOR, _("Time compressor"), wxEmptyString, wxITEM_CHECK);
    panel_menu->Append(TIMING_ID_MENUITEM_AXIS_TIME, _("Axis/Time"), wxEmptyString, wxITEM_CHECK);
    panel_menu->Append(TIMING_ID_MENUITEM_TRANSITION, _("Transition"), wxEmptyString, wxITEM_CHECK);
    panel_menu->AppendSeparator();
    panel_menu->Append(TIMING_ID_MENUITEM_DEFAULT, _("Default"));

    wxMenu *view_menu = new wxMenu;
    //view_menu->AppendSeparator();
    view_menu->Append(wxID_ANY, _("Panel"),panel_menu, _("Show or Hide Panel"));


    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _("&File"));
    menu_bar->Append(view_menu, _("&View"));
    if (edit_menu)
        menu_bar->Append(edit_menu, _("&Edit"));
    menu_bar->Append(help_menu, _("&Help"));

#ifdef __WXMAC__

    wxMenuBar::MacSetCommonMenuBar(menu_bar);
#endif //def __WXMAC__
    /// Associate the menu bar with the frame
    SetMenuBar(menu_bar);

}

void TimingMainFrame::InitStatusBar()
{
    CreateStatusBar(2);
    wxInt32 widths[2] = {-1, 60 };
    SetStatusWidths(2, &widths[0]);
    SetStatusBarPane(0);
    SetStatusText(_("Welcome to TimingEditor!"), 0);
}

TimingMainFrame::~TimingMainFrame()
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

void TimingMainFrame::OnAbout(wxCommandEvent &event)
{

    wxAboutDialogInfo info;
    info.SetName("TimingEditor");
    info.SetVersion("0.2");
    info.SetDescription("Timing diagram editor.");
    info.SetCopyright("(C) 2008 2009 2010 2011 2012 2013 2014 2015 2016");
    info.AddDeveloper("Daniel Anselmi");
    info.AddDeveloper("Yves Studer");

    wxAboutBox(info);

}

void TimingMainFrame::OnHelp(wxCommandEvent &event)
{
    wxLaunchDefaultBrowser("http://timingeditor.wiki.sourceforge.net/");
}

void TimingMainFrame::OnUpdateCopy(wxUpdateUIEvent& event)
{
    event.Enable(CanCopy());
}
void TimingMainFrame::OnUpdateCut(wxUpdateUIEvent& event)
{
    event.Enable(CanCut());
}
void TimingMainFrame::OnUpdateDelete(wxUpdateUIEvent& event)
{
    event.Enable(CanDelete());
}

wxString TimingMainFrame::DetectSourceOfEvent(const long EventId)
{
    wxString ID = "";
    if(EventId == TIMING_ID_MENUITEM_CLOCK_TYPE)
        ID = "ClockPanel";
    else if(EventId == TIMING_ID_MENUITEM_TIME_COMPRESSOR)
        ID = "TimeCompressorPanel";
    else if(EventId == TIMING_ID_MENUITEM_AXIS_TIME)
        ID = "AxisPanel";
    else if(EventId == TIMING_ID_MENUITEM_TRANSITION)
        ID = "TransitionPanel";
    return ID;
}

void TimingMainFrame::HandlePanel(wxCommandEvent& event)
{
    wxString ID = DetectSourceOfEvent(event.GetId());
    if(m_manager->GetPane(ID).IsShown())
        m_manager->GetPane(ID).Hide().CaptionVisible();
    else
        m_manager->GetPane(ID).Show().CaptionVisible();
    m_manager->Update();
}

void TimingMainFrame::UpdateMenuItems(wxUpdateUIEvent &event)
{
    wxString ID = DetectSourceOfEvent(event.GetId());
    if(m_manager->GetPane(ID).IsShown())
        event.Check(true);
    else
        event.Check(false);
}

void TimingMainFrame::LoadDefaultLayout(wxCommandEvent& event)
{
    m_manager->LoadPerspective(DefalutLayout);
}


bool TimingMainFrame::CanCopy(void)const
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view && view->CanCopy() )
        return true;
    return false;
}
bool TimingMainFrame::CanCut(void)const
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view && view->CanCut() )
        return true;
    return false;
}
bool TimingMainFrame::CanPaste(void)const
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view && view->CanPaste() )
        return true;
    return false;
}

bool TimingMainFrame::CanDelete(void)const
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view && view->CanDelete() )
        return true;
    return false;
}
void TimingMainFrame::OnUpdatePaste(wxUpdateUIEvent& event)
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view  && view->CanPaste() )
    {
        event.Enable(true);
        return;
    }
    event.Enable(false);
}
void TimingMainFrame::OnUpdateTools(wxUpdateUIEvent& event)
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view )
    {
        event.Enable(true);
        return;
    }
    event.Enable(false);
}

DiagramSplitterWindow *TimingMainFrame::CreateWindow(TimingView *view, wxMDIChildFrame *parent)
{
    //view->SetPanels(clksetpanel, trnssetpanel, axissetpanel, tcsetpanel);
    return new DiagramSplitterWindow(view, parent, wxID_ANY);
}

void TimingMainFrame::InitToolBar()
{
    wxAuiToolBar *toolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW);
    toolBar->AddTool(wxID_NEW, _("New file"), wxBitmap( new_xpm ), _("Create a new file"), wxITEM_NORMAL);
    toolBar->AddTool(wxID_OPEN, _("Open file"), wxBitmap( open_xpm ));
    toolBar->AddTool(wxID_SAVE, _("Save file"), wxBitmap( save_xpm ));
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_CUT, _("Cut"), wxBitmap( cut_xpm ));
    toolBar->AddTool(wxID_COPY, _("Copy"), wxBitmap( copy_xpm ));
    toolBar->AddTool(wxID_PASTE, _("Paste"), wxBitmap( paste_xpm ));
    toolBar->AddSeparator();
    //toolBar->AddTool(wxID_PRINT, _T("Print"), wxBitmap( print_xpm ), wxNullBitmap, wxITEM_NORMAL, _T("Print"), _T("Print the active file"));
    //toolBar->AddSeparator();
    toolBar->AddTool(wxID_UNDO, _("Undo"), wxBitmap( undo_xpm ), _("Undo last operation"), wxITEM_NORMAL);
    toolBar->AddTool(wxID_REDO, _("Redo"), wxBitmap( redo_xpm ), _("Redo last operation"), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(TIMING_ID_GLASS_N, _("Zoom out"), wxBitmap(glassntool_xpm), _("Zoom out the Document"), wxITEM_NORMAL);
    toolBar->AddTool(TIMING_ID_GLASS_P, _("Zoom in"), wxBitmap(glassptool_xpm), _("Zoom in the Document"), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(TIMING_ID_ADD_CLOCK, _("Add Clock"), wxBitmap(clockedge_xpm), _("Add a clock to the Document"));
    toolBar->AddTool(TIMING_ID_ADD_SIGNAL, _("Add Signal"), wxBitmap(risingedge_xpm), _("Add a signal to the Document"));
    toolBar->AddTool(TIMING_ID_ADD_BUS, _("Add Bus"), wxBitmap(busedge_xpm), _("Add a bus to the Document"));
    toolBar->AddSeparator();
    toolBar->AddTool(TIMING_ID_EDIT, _("Select"), wxBitmap(cross_xpm), _(" Select something or change signal/bus"));
    toolBar->AddTool(TIMING_ID_DISCONTINUITY, _("Edit time compressors"), wxBitmap(tri_xpm), _("Add/Remove time compressors by clicking on the top axis"));
    toolBar->AddTool(TIMING_ID_RULER, _T("Draw vertical line"), wxBitmap(ruler_cur_xpm), _("Draw vertical line"));
    toolBar->AddTool(TIMING_ID_HORIZONTALARROW, _("Draw horizontal arrow"),  wxBitmap(harrow_cur_xpm), _("Draw a horizontal arrow") );
    toolBar->AddTool(TIMING_ID_EDITTEXT, _("Edit text label"), wxBitmap(textedit_cur_xpm), _("Edit text labels") );
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_ABOUT, _("Help"), wxBitmap( help_xpm ), _("Show info about this application"));
    toolBar->Realize();

    toolBar->Realize();
    //SetToolBar(toolBar);



    m_manager->AddPane(toolBar, wxAuiPaneInfo().
                  Name("tb1").Caption(_("Big Toolbar")).
                  ToolbarPane().Top().
                  LeftDockable(false).RightDockable(false));

}


void TimingMainFrame::OnTip(wxCommandEvent &event)
{
    ShowTip(true);
}
void TimingMainFrame::ShowTip(bool force)
{
    wxConfig *config = wxGetApp().GetConfig();

    bool ShowTipsAtStartup = true;
    wxInt32 TipNumber = 0;
    config->Read("/StartupTips/ShowTipsAtStarup", &ShowTipsAtStartup);
    config->Read("/StartupTips/TipNumber", &TipNumber);

    if ( ShowTipsAtStartup || force )
    {
        //wxTipProvider *tipProvider = wxCreateFileTipProvider("tips", TipNumber);
        wxTipProvider *tipProvider = new myTipProvider(TipNumber);;
        ShowTipsAtStartup = wxShowTip(this, tipProvider, ShowTipsAtStartup);
        TipNumber = tipProvider->GetCurrentTip();
        delete tipProvider;

        config->Write("/StartupTips/ShowTipsAtStarup", ShowTipsAtStartup);
        config->Write("/StartupTips/TipNumber", TipNumber);
    }
}

void TimingMainFrame::OnUpdateGlassN(wxUpdateUIEvent &event)
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view && view->CanZoomOut() )
    {
        event.Enable(true);
        return;
    }
    event.Enable(false);
}
void TimingMainFrame::OnUpdateGlassP(wxUpdateUIEvent &event)
{
    TimingView *view = (TimingView *)wxGetApp().GetDocManager()->GetCurrentView();
    if ( view && view->CanZoomIn() )
    {
        event.Enable(true);
        return;
    }
    event.Enable(false);
}
void TimingMainFrame::SaveFramePositions(wxConfig *config)
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
        config->Write("/MainFrame/x", (long) x);
        config->Write("/MainFrame/y", (long) y);
        config->Write("/MainFrame/w", (long) w);
        config->Write("/MainFrame/h", (long) h);
    }
    config->Write("/MainFrame/s", (long) s);
}
void TimingMainFrame::SaveAuiPerspective(wxConfig *config)
{
    wxString str = m_manager->SavePerspective();
    config->Write("/MainFrame/AuiPerspective", str );
    /// store version to let newer versions of gui a cahnce to build a new valid perspective
    config->Write("/MainFrame/AuiPerspectiveVersion", 3);
}
void TimingMainFrame::LoadAuiPerspective(wxConfig *config)
{
    wxString str;
    long v;

    /// if version stored is older than current do not load old perspective
    config->Read("/MainFrame/AuiPerspectiveVersion", &v, 0);
    if ( v == 3 )
    {
        config->Read("/MainFrame/AuiPerspective", &str);
        m_manager->LoadPerspective(str);
    }
}
void TimingMainFrame::LoadFramePositions(wxConfig *config)
{
    /// restore frame position and size
        wxInt32 x, y, w, h, s;
        x = config->Read("/MainFrame/x", 50);
        y = config->Read("/MainFrame/y", 50);
        w = config->Read("/MainFrame/w", 600);
        h = config->Read("/MainFrame/h", 500);
        s = config->Read("/MainFrame/s", (long)0);
        Move(x, y);
        SetClientSize(w, h);
        if ( s > 0 )
            Maximize(true);
        if ( s < 0)
            Iconize(true);
}
