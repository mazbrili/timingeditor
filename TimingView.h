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

#include <wx/docmdi.h>

#include <vector>

#include "ClockSettingsPanel.h"
#include "TransitionSettingsPanel.h"
#include "AxisSettingsPanel.h"
#include "TimeCompressorSettingsPanel.h"

class DiagramSplitterWindow;
class ClockSettingsPanel;
class TransitionSettingsPanel;
class Signal;

class TimingView: public wxView
{
public:
    wxDocMDIChildFrame *frame;
    //TimingWindow *window;
    DiagramSplitterWindow *window;

    TimingView();
    ~TimingView() {}

    bool OnCreate(wxDocument *doc, long WXUNUSED(flags));
    void OnDraw(wxDC *dc);
    void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    bool OnClose(bool deleteWindow = true);

    std::vector<wxInt32> VisibleTicks;
    wxInt32 GridStepWidth;
    std::vector<wxCoord> heightOffsets;

    wxString GetFloatFormatStr() const;
    wxFont GetFont() const;
private:
    void UpdateVisibelTicksContainer();
    void UpdateHeightsContainer();

    ClockSettingsPanel *ClkSetPanel;
    TransitionSettingsPanel *TranSetPanel;
    AxisSettingsPanel *AxisSetPanel;
    TimeCompressorSettingsPanel *TmeCmprssrPanel;

    void AttachPanels();
    void DetachPanels();
    void UpdateTimeCompressorPanel(void);
    void UpdateClockPanel();
    void UpdateTransitionPanel();
    void UpdateAxisPanel();
public:
    void SetPanels(ClockSettingsPanel *csp, TransitionSettingsPanel *tsp, AxisSettingsPanel *asp, TimeCompressorSettingsPanel *tcsp);
public:
    void SetAxis(wxInt8 unit, wxInt32 ticklength, wxInt32 markerlength, wxInt32 offset, wxInt32 totallength);
    void SetClock(wxInt32 delay, wxInt32 ticks, bool shadow, bool DrawPeriod);
    void SetTransition(wxInt8 width, bool en50, bool en90);
    void SetTimeCompressor(wxInt32 time);
public:
    bool CanZoomIn(void);
    bool CanZoomOut(void);
    bool CanPaste(void);
    bool IsSomethingSelected(void);
    bool IsTextSelected(void);
    bool IsSignalSelected(void);
    wxInt32 GetSelectedSignalNumber();
    wxInt32 GetSelectedCompressorsIndex();
    bool DiscontSelected(void);
    bool IsSelectedSignalClock(void);
    bool CanDelete(void);
    void AddSignal(Signal sig);

private:
    virtual void OnActivateView(bool activate, wxView *activeView, wxView *deactiveView);
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

