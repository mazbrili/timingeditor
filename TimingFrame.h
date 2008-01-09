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


#ifndef TIMINGFRAME_H
#define TIMINGFRAME_H

#include <wx/docmdi.h>
#include "TimingApp.h"

class TimingWindow;
class TimingFrame: public wxDocMDIParentFrame//wxFrame
{

    public:
    public:
        //TimingFrame( wxWindow* parent, wxString title = wxT("Timing"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 481,466 ), int style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
        TimingFrame(wxDocManager *manager , wxFrame *frame, int id = wxID_ANY, const wxString& title = _T("TimingEditor"), wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize, int style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
        ~TimingFrame();
    ///member vars
    private:
        wxMenu *editMenu;


    private:
        /*void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);*/
        void OnAbout(wxCommandEvent& event);
        void OnTip(wxCommandEvent &event);

        void OnUpdateCopy(wxUpdateUIEvent& event);
        void OnUpdateCut(wxUpdateUIEvent& event);
        void OnUpdatePaste(wxUpdateUIEvent& event);
        void OnUpdateDelete(wxUpdateUIEvent& event);

        void OnUpdateGlassN(wxUpdateUIEvent &event);
        void OnUpdateGlassP(wxUpdateUIEvent &event);
        void OnUpdateDiscont(wxUpdateUIEvent& event);

    /*protected:
        wxMenuBar* mbar;
        wxStatusBar* statusBar;*/

        bool IsSomethingSelected();
        bool IsTextSelected(void);
        bool CanDelete(void);

    ///public methods
    public:
        //TimingWindow *CreateCanvas(wxView *view, wxMDIChildFrame *parent);
        TimingWindow *CreateWindow(wxView *view, wxMDIChildFrame *parent);
        void InitToolBar(wxToolBar* toolBar);

    DECLARE_CLASS(TimingFrame)
    DECLARE_EVENT_TABLE()
};



#endif //__GUIFrame__

