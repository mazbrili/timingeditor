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


#ifndef TIMINGMAINFRAME_H
#define TIMINGMAINFRAME_H

#include <wx/docmdi.h>
#include <wx/aui/aui.h>
#include "TimingApp.h"

class DiagramSplitterWindow;
class ClockSettingsPanel;
class TransitionSettingsPanel;
class AxisSettingsPanel;
class TimeCompressorSettingsPanel;
class TimingView;
class TimingMainFrame: public wxDocMDIParentFrame//wxFrame
{
    public:
        //TimingMainFrame( wxWindow* parent, wxString title = wxT("Timing"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 481,466 ), int style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
        TimingMainFrame(wxDocManager *manager , wxFrame *frame, int id = wxID_ANY, const wxString& title = _T("TimingEditor"), wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize, int style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
        ~TimingMainFrame();
    ///member vars
    private:
        wxMenu *editMenu;
        ClockSettingsPanel *clksetpanel;
        TransitionSettingsPanel *trnssetpanel;
        AxisSettingsPanel *axissetpanel;
        TimeCompressorSettingsPanel *tcsetpanel;
    public:
//        wxString GetClockSettingsPanelDelay() { return clksetpanel->GetDelayText(); }
//        wxString GetClockSettingsPanelTicks() { return clksetpanel->GetTicksText(); }
//        void     SetClockSettingsPanelUnmodified(){clksetpanel->SetUnmodified();}
//        wxInt8   GetTransitionWidth(){return trnssetpanel->GetTransitionWidth();}
//        bool     GetTransition5090(){return trnssetpanel->Get5090();}

    private:
        ///void OnTimingEvent(wxCommandEvent &event);
        /*void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);*/
        void OnAbout(wxCommandEvent& event);
        void OnTip(wxCommandEvent &event);
        void OnHelp(wxCommandEvent &event);

        void OnUpdateCopy(wxUpdateUIEvent& event);
        void OnUpdateCut(wxUpdateUIEvent& event);
        void OnUpdatePaste(wxUpdateUIEvent& event);
        void OnUpdateDelete(wxUpdateUIEvent& event);

        void OnUpdateGlassN(wxUpdateUIEvent &event);
        void OnUpdateGlassP(wxUpdateUIEvent &event);
        void OnUpdateDiscont(wxUpdateUIEvent& event);



        bool IsSomethingSelected();
        bool IsTextSelected(void);
        bool IsSignalSelected(void);
        bool CanDelete(void);

    public:

        //TimingWindow *CreateWindow(wxView *view, wxMDIChildFrame *parent);
        DiagramSplitterWindow *CreateWindow(TimingView *view, wxMDIChildFrame *parent);
        void InitToolBar();
        void InitStatusBar();
        void InitMenuBar();

        void ShowTip(bool force = false);
        void SaveFramePositions(wxConfig *config);
        void LoadFramePositions(wxConfig *config);
        void SaveAuiPerspective(wxConfig *config);
        void LoadAuiPerspective(wxConfig *config);

    private:
        wxAuiManager *m_manager;

    DECLARE_CLASS(TimingMainFrame)
    DECLARE_EVENT_TABLE()
};



#endif //TIMINGMAINFRAME_H

