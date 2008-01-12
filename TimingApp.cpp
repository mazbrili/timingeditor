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

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include <wx/cmdline.h>
#include <wx/docview.h>
#include <wx/docmdi.h>
#include <wx/config.h>
#include <wx/tipdlg.h>

#include "TimingApp.h"
#include "TimingFrame.h"
#include "TimingView.h"
#include "TimingDoc.h"
#include "enumers.h"



IMPLEMENT_APP(TimingApp);

bool TimingApp::OnInit()
{
    wxCmdLineParser parser(argc, argv);
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        /*{ wxCMD_LINE_SWITCH, _T("v"), _T("verbose"), _T("be verbose") },
        { wxCMD_LINE_SWITCH, _T("q"), _T("quiet"),   _T("be quiet") },
        { wxCMD_LINE_OPTION, _T("o"), _T("output"),  _T("output file") },
        { wxCMD_LINE_OPTION, _T("i"), _T("input"),   _T("input dir") },
        { wxCMD_LINE_OPTION, _T("s"), _T("size"),    _T("output block size"), wxCMD_LINE_VAL_NUMBER },
        { wxCMD_LINE_OPTION, _T("d"), _T("date"),    _T("output file date"), wxCMD_LINE_VAL_DATE },*/
        { wxCMD_LINE_PARAM,  NULL, NULL, _T("input file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE|wxCMD_LINE_PARAM_OPTIONAL },
        { wxCMD_LINE_NONE }
    };
    parser.SetDesc(cmdLineDesc);
    wxInt32 parsres = parser.Parse();
    if ( parsres != 0 )
        return false;

    /// Create a document manager
    m_docManager = new wxDocManager;

    /// Create a config
    m_config = new wxConfig(_T("TimingEditor"));

    /// Create a template relating documents to their views
    (void) new wxDocTemplate(m_docManager, _T("Timing Diagrams"), _T("*.tdg"), _T(""), _T("tdg"),
            _T("Timing Diagrams"), _T("Timing Diagram"),
            CLASSINFO(TimingDocument), CLASSINFO(TimingView));

    /// Create the main frame window
    mainframe = new TimingFrame(m_docManager,(wxFrame *) NULL);



    //// Give it an icon
#ifdef __WXMSW__
    //mainframe->SetIcon(wxIcon(_T("nassi")));
    mainframe->SetIcon(wxICON(aaaa)); // To Set App Icon
#endif
#ifdef __X__
    mainframe->SetIcon(wxIcon(_T("nassi.xpm")));
#endif

    /// Make a menubar
    wxMenu *file_menu = new wxMenu;
    wxMenu *edit_menu = (wxMenu *) NULL;

    file_menu->Append(wxID_NEW, _T("&New...\tCtrl-N"), _T("Create a new file"));
    file_menu->Append(wxID_OPEN, _T("&Open...\tCtrl-O"), _T("Open a existing file"));
    //file_menu->Append(NASSI_ID_IMPORT_SOURCE, _T("&Import...\tCtrl-I"), _T("Import from C source file"));
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, _T("&Quit\tAlt-F4"), _T("Quit the application") );
    // A nice touch: a history of files visited. Use this menu.
    m_docManager->FileHistoryUseMenu(file_menu);
    //m_docManager->FileHistoryAddFilesToMenu();

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, _T("&About\tF1"), _T("Show info about this application"));
    help_menu->Append(TIMING_ID_TIP, _T("Tip"), _T("Tips on using TimingEditor") );

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _T("&File"));
    if (edit_menu)
        menu_bar->Append(edit_menu, _T("&Edit"));
    menu_bar->Append(help_menu, _T("&Help"));

#ifdef __WXMAC__

    wxMenuBar::MacSetCommonMenuBar(menu_bar);
#endif //def __WXMAC__
    /// Associate the menu bar with the frame
    mainframe->SetMenuBar(menu_bar);
    mainframe->Centre(wxBOTH);

    //// statusbar
    mainframe->CreateStatusBar(3);
    wxInt32 widths[3] = { 200, -1, 40 };
    mainframe->SetStatusWidths(3, &widths[0]);
    mainframe->SetStatusBarPane(0);
    //mainframe->SetStatusText(_T("Welcome to NassiEdit!"), 0);

    m_config->SetPath(_T("/FileHistory"));
    m_docManager->FileHistoryLoad(*m_config);
    m_config->SetPath(_T("/"));

    {/// restore frame position and size
        wxInt32 x, y, w, h, s;
        x = m_config->Read(_T("/MainFrame/x"), 50);
        y = m_config->Read(_T("/MainFrame/y"), 50);
        w = m_config->Read(_T("/MainFrame/w"), 600);
        h = m_config->Read(_T("/MainFrame/h"), 500);
        s = m_config->Read(_T("/MainFrame/s"), (long)0);
        mainframe->Move(x, y);
        mainframe->SetClientSize(w, h);
        if ( s > 0 )
            mainframe->Maximize(true);
        if ( s < 0)
            mainframe->Iconize(true);
    }

#ifndef __WXMAC__
    mainframe->Show(true);
#endif //ndef __WXMAC__

    SetTopWindow(mainframe);



    //wxInitAllImageHandlers();
    wxImage::AddHandler( new wxPNGHandler );


    /// open documents specified as parameters from command line
    for ( size_t n = 0 ; n < parser.GetParamCount() ; n++ )
        m_docManager->CreateDocument(parser.GetParam(n), wxDOC_SILENT );


    {//// Tip of the day
        bool ShowTipsAtStartup = true;
        wxInt32 TipNumber = 0;
        m_config->Read(_T("/StartupTips/ShowTipsAtStarup"), &ShowTipsAtStartup);
        m_config->Read(_T("/StartupTips/TipNumber"), &TipNumber);

        if ( ShowTipsAtStartup )
        {
            wxTipProvider *tipProvider = wxCreateFileTipProvider(_T("tips"), TipNumber);
            ShowTipsAtStartup = wxShowTip(mainframe, tipProvider);
            TipNumber = tipProvider->GetCurrentTip();
            delete tipProvider;

            m_config->Write(_T("/StartupTips/ShowTipsAtStarup"), ShowTipsAtStartup);
            m_config->Write(_T("/StartupTips/TipNumber"), TipNumber);
        }
    }

    return true;
}

int TimingApp::OnExit(void)
{
    m_config->SetPath(_T("/FileHistory"));
    m_docManager->FileHistorySave(*m_config);
    m_config->SetPath(_T("/"));
    delete m_docManager;
    delete m_config;
    return 0;
}

TimingApp::TimingApp(void){}


wxMDIChildFrame *TimingApp::CreateChildFrame(wxDocument *doc, wxView *view)
{
    /// Make a child frame
    wxDocMDIChildFrame *subframe =
        new wxDocMDIChildFrame(doc, view, GetMainFrame(), wxID_ANY, _T("Child Frame"));

    #ifdef __WXMSW__
        subframe->SetIcon(wxIcon(_T("wxwinicon")));
    #else
        subframe->SetIcon(wxIcon(_T("wxwinicon.xpm")));
    #endif

    /// Make a menubar
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(wxID_NEW, _T("&New...\tCtrl-N"), _T("Create a new file"));
    file_menu->Append(wxID_OPEN, _T("&Open...\tCtrl-O"), _T("Open a existing file"));
    file_menu->Append(wxID_CLOSE, _T("&Close\tCtrl-W"), _T("Close the active file"));
    file_menu->Append(wxID_SAVE, _T("&Save\tCtrl-S"), _T("Save the active file"));
    file_menu->Append(wxID_SAVEAS, _T("Save &As..."), _T("Save the active file under a different name"));
    file_menu->AppendSeparator();
    //file_menu->Append(wxID_PRINT, _T("&Print...\tCtrl-P"), _T("Print the active file"));
    //file_menu->Append(wxID_PRINT_SETUP, _T("Print &Setup..."), _T("Prepare the printer"));
    //file_menu->Append(wxID_PREVIEW, _T("Print Pre&view"), _T("Preview the print of the active file"));
    //file_menu->AppendSeparator();
    file_menu->Append(TIMING_ID_EXPORT_BITMAP, _T("Export to PNG"), _T("export diagramm to a bitmap in PNG format"));
    file_menu->Append(TIMING_ID_EXPORT_SVG, _T("Export to SVG"), _T("export diagram to a scalable vector graphic"));
    file_menu->Append(TIMING_ID_EXPORT_PS, _T("Export to PS"), _T("export diagram to postscript format"));
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, _T("&Quit\tAlt-F4"), _T("Quit the application"));
    //file history
    m_docManager->FileHistoryUseMenu(file_menu);
    m_docManager->FileHistoryAddFilesToMenu(file_menu);
    ///  don't forget to remove the FileHistoryMeni from the file menu while closing the view: FileHistoryRemoveMenu(fileMenu);



    wxMenu *edit_menu = new wxMenu;
    edit_menu->Append(wxID_UNDO, _T("&Undo\tCtrl-Z"), _T("Undo the last operation"));
    edit_menu->Append(wxID_REDO, _T("&Redo\tCtrl-Shift-Z"), _T("Redo the last operation"));
    edit_menu->AppendSeparator();
    edit_menu->Append(wxID_CUT, _T("Cu&t\tCtrl-X"), _T("Cut"));
    edit_menu->Append(wxID_COPY, _T("&Copy\tCtrl-C"), _T("Copy"));
    edit_menu->Append(wxID_PASTE, _T("&Paste\tCtrl-V"), _T("Paste"));
    edit_menu->Append(wxID_DELETE, _T("&Delete\tDel"), _T("Delete"));
    edit_menu->AppendSeparator();
    edit_menu->Append(TIMING_ID_ADD_CLOCK, _T("&Add Clock"), _T("&Add Clock") );
    edit_menu->Append(TIMING_ID_ADD_SIGNAL, _T("Add &Signal"), _T("Add &Signal"));
    edit_menu->Append(TIMING_ID_ADD_BUS, _T("Add &Bus"), _T("Add &Bus"));
    //edit_menu->AppendSeparator();
    doc->GetCommandProcessor()->SetEditMenu(edit_menu);

    wxMenu *viewMenu = new wxMenu;
    viewMenu->Append(TIMING_ID_GLASS_P, _T("Zoom in\tF7"), _T("Zoom in the Document"));
    viewMenu->Append(TIMING_ID_GLASS_N, _T("Zoom out\tF6"), _T("Zoom out the Document"));

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, _T("&About\tF1"), _T("Show info about this application"));
    help_menu->Append(TIMING_ID_TIP, _T("Tip"), _T("Tips on using wxDocViewMDISample") );

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _T("&File"));
    menu_bar->Append(edit_menu, _T("&Edit"));
    menu_bar->Append(viewMenu, _T("&View"));
    menu_bar->Append(help_menu, _T("&Help"));

    /// Associate the menu bar with the frame
    subframe->SetMenuBar(menu_bar);

    return subframe;
}
