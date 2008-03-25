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

#ifndef __VIEW_H__
#define __VIEW_H__

#include <wx/docview.h>

class TimingWindow;
class ClockSettingsPanel;
class TransitionSettingsPanel;

class TimingView: public wxView
{
    public:
        wxMDIChildFrame *frame;
        TimingWindow *window;
        ClockSettingsPanel *ClkSetPanel;
        TransitionSettingsPanel *TranSetPanel;

        TimingView();
        ~TimingView() {}

        bool OnCreate(wxDocument *doc, long WXUNUSED(flags));
        void OnDraw(wxDC *dc);
        void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
        bool OnClose(bool deleteWindow = true);

    public:
        bool CanZoomIn(void);
        bool CanZoomOut(void);
        bool CanPaste(void);
        bool IsSomethingSelected(void);
        bool IsTextSelected(void);
        bool CanDelete(void);

    private:
        void OnActivate(wxActivateEvent &event);
        void OnDelete(wxCommandEvent& event);
        //void OnDeleteSignal(wxCommandEvent& event);
        void OnSelectAll(wxCommandEvent& event);
        void OnCopy(wxCommandEvent& event);
        void OnCut(wxCommandEvent& event);
        void OnPaste(wxCommandEvent& event);

        void OnZoomTicksOut(wxCommandEvent& WXUNUSED(event) );
        void OnZoomTicksIn(wxCommandEvent& WXUNUSED(event) );
        //void OnEditClock(wxCommandEvent& event);
        void OnInsertDiscontTool(wxCommandEvent& event);
        void OnSelectRuler(wxCommandEvent& event);
        void OnSelectHArrow(wxCommandEvent& event);
        void OnSelectTextTool(wxCommandEvent& event);
        void OnSelectNeutral(wxCommandEvent& event);

        void OnAddClock(wxCommandEvent& event);
        void OnAddSignal(wxCommandEvent& event);
        void OnAddBus(wxCommandEvent& event);

        void OnExportBitmap(wxCommandEvent& event);
        void OnExportSVG(wxCommandEvent& event);
        void OnExportPS(wxCommandEvent& event);

        void OnClockApply(wxCommandEvent& event);
        void OnTrnansitionPanelApply(wxCommandEvent& event);
    private:
        DECLARE_DYNAMIC_CLASS(TimingView)
        DECLARE_EVENT_TABLE()
};


#endif

