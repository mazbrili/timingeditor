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
#include "TimingMainFrame.h"
#include "TimingSubFrame.h"
#include "TimingView.h"
#include "TimingDoc.h"

#include <wx/image.h>

IMPLEMENT_APP(TimingApp)

bool TimingApp::OnInit()
{

	bool wxsOK = true;
	wxInitAllImageHandlers();

    wxCmdLineParser parser(argc, argv);
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
      /*{ wxCMD_LINE_SWITCH, "v", "verbose", "be verbose" },
        { wxCMD_LINE_SWITCH, "q", "quiet",   "be quiet" },
        { wxCMD_LINE_OPTION, "o", "output",  "output file" },
        { wxCMD_LINE_OPTION, "i", "input",   "input dir" },
        { wxCMD_LINE_OPTION, "s", "size",    "output block size", wxCMD_LINE_VAL_NUMBER },
        { wxCMD_LINE_OPTION, "d", "date",    "output file date", wxCMD_LINE_VAL_DATE },*/
        { wxCMD_LINE_PARAM,  NULL, NULL, "input file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE|wxCMD_LINE_PARAM_OPTIONAL },
        { wxCMD_LINE_NONE }
    };
    parser.SetDesc(cmdLineDesc);
    wxInt32 parsres = parser.Parse();
    if ( parsres != 0 )
        return false;

    /// Create a document manager
    m_docManager = new wxDocManager;

    /// Create a config
    m_config = new wxConfig("TimingEditor");

    /// Create a template relating documents to their views
    (void) new wxDocTemplate(m_docManager, _("Timing Diagrams"), "*.tdg", "", "tdg",
            "Timing Diagrams", "Timing Diagram",
            CLASSINFO(TimingDocument), CLASSINFO(TimingView));

    /// Create the main frame window
    mainframe = new TimingMainFrame(m_docManager,(wxFrame *) NULL);

    SetTopWindow(mainframe);

#ifndef __WXMAC__
    mainframe->Show(true);
#endif

    /// open documents specified as parameters from command line
    for ( size_t n = 0 ; n < parser.GetParamCount() ; n++ )
        m_docManager->CreateDocument(parser.GetParam(n), wxDOC_SILENT );

    mainframe->ShowTip();

	return wxsOK;
}

int TimingApp::OnExit(void)
{
    m_config->SetPath("/FileHistory");
    m_docManager->FileHistorySave(*m_config);
    m_config->SetPath("/");
    delete m_docManager;
    delete m_config;
    return 0;
}

TimingApp::TimingApp(void){}


wxDocMDIChildFrame *TimingApp::CreateChildFrame(wxDocument *doc, wxView *view)
{
    /// Make a child frame
    wxDocMDIChildFrame *subframe =
        new TimingSubFrame(doc, view, GetMainFrame());

    return subframe;
}
