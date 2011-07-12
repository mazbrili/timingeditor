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
#include "GraphVerticalLine.h"
#include "GraphHorizontalArrow.h"


class DiagramSplitterWindow;
class ClockSettingsPanel;
class TransitionSettingsPanel;
class Signal;
class GraphSignal;
class Task;
class MainTask;

class TimingTextCtrl;

typedef std::vector<GraphVerticalLine> GraphVerticalLines;
typedef std::vector<GraphHorizontalArrow> GraphHorizontalArrows;
typedef std::vector<GraphSignal*> GraphSignals;

class TimingView: public wxView
{
public:
    wxDocMDIChildFrame *frame;
    DiagramSplitterWindow *splitterwindow;

    TimingView();
    ~TimingView() {}

    bool OnCreate(wxDocument *doc, long WXUNUSED(flags));
    void OnDraw(wxDC *dc);
    void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    bool OnClose(bool deleteWindow = true);

    std::vector<wxInt32> VisibleTicks;
    wxInt32 GridStepWidth;
    //double GridStepWidth;
    std::vector<wxCoord> heightOffsets;

    wxString GetFloatFormatStr() const;
    wxString GetTimeString(wxInt32 ticks);
    wxFont GetFont() const;
    int GetWavesLeftSpace()const;

    const GraphVerticalLines &GetGraphVerticalLines()const;
    const GraphHorizontalArrows &GetHorizontalArrows()const;
    const GraphSignals &GetGraphSignals()const;

    wxColour GetShadowColour()const;
    wxColour GetWaveSeparatorColour()const;
    wxColour GetBackgroundColour()const;
    wxColour GetLineColour()const;
    wxColour GetTextColour()const;
    wxColour GetUndefinedSignalColour()const;
    wxColour GetAxisBackgroundColour()const;
    wxColour GetAxisLineColour()const;
    wxColour GetLabelsBackgroundColour()const;
    wxColour GetLabelsTextColour()const;
    wxColour GetLabelsLineColour()const;
    wxColour GetCompressorColour()const;
    wxColour GetActiveGraphCaretColour()const;
    wxCoord GetHeightOfAxisWindow()const;

    const int GetScrollPixelsPerUnit()const{return 10;}

    const int GetDistanceToTicksLine()const{return 25;}
    const int GetDistanceFromTicksLine()const{return 5;}
    const int GetDistanceToAxis()const{return 25;}

private:
    void UpdateVisibelTicksContainer();
    void UpdateHeightsContainer();
    void UpdateVerticalLines();
    void UpdateHorizontalArrows();

//    ClockSettingsPanel *ClkSetPanel;
//    TransitionSettingsPanel *TranSetPanel;
//    AxisSettingsPanel *AxisSetPanel;
//    TimeCompressorSettingsPanel *TmeCmprssrPanel;

    void AttachPanels();
    void DetachPanels();
    //void UpdateTimeCompressorPanel(void);
    //void UpdateClockPanel();
    void UpdateTransitionPanel();
    void UpdateAxisPanel();
    void UpdateSignals();

    GraphVerticalLines m_vertlines;
    GraphHorizontalArrows m_graphHorizontalArrows;
    GraphSignals m_graphsignals;


public:
    //void UpdateTimeCompressorPanel(void);
    //void SetPanels(ClockSettingsPanel *csp, TransitionSettingsPanel *tsp, AxisSettingsPanel *asp, TimeCompressorSettingsPanel *tcsp);

public:
    void SetAxis(wxInt8 unit, wxInt32 ticklength, wxInt32 markerlength, wxInt32 offset, wxInt32 totallength);
    void SetClock(wxInt32 delay, wxInt32 ticks, bool shadow, bool DrawPeriod);
    void SetTransition(wxInt8 width, bool en50, bool en90);
    void SetTimeCompressor(wxInt32 time);



/// ///////////////////////////////////////////////////////////////////////////
public:
    bool CanZoomIn(void);
    bool CanZoomOut(void);
    bool CanPAste(void);
    bool CanDelete(void);
    bool CanCopy(void);
    bool CanPaste(void);
    bool CanCut(void);
    //bool IsTextSelected(void);
    bool IsSignalSelected(void);
    bool IsClockSelected(void);
    bool IsDiscontinuitySelected(void);
    //bool CanEditText();

    void LabelsMouse(const wxMouseEvent &event, const wxPoint &pos);
    void WavesMouse(const wxMouseEvent &event, const wxPoint &pos);
    void AxisMouse(const wxMouseEvent &event, const wxPoint &pos);
    void LabelsKey(const wxKeyEvent &event, bool down);
    void WavesKey(const wxKeyEvent &event, bool down);
    void AxisKey(const wxKeyEvent &event, bool down);

    void SetTask(Task *task = NULL);
    void TextHasFocus(TimingTextCtrl *ctrl);
private:
    Task *task;
    MainTask *defaultTask;
public:
    wxInt32 GetSelectedSignalNumber();
    wxInt32 GetSelectedDiscontinuity();

private:
    virtual void OnActivateView(bool activate, wxView *activeView, wxView *deactiveView);
    void OnDelete(wxCommandEvent& event);
    //void OnDeleteSignal(wxCommandEvent& event);
    void OnSelectAll(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);

    void OnZoomOut(wxCommandEvent& WXUNUSED(event) );
    void OnZoomIn(wxCommandEvent& WXUNUSED(event) );
    //void OnEditClock(wxCommandEvent& event);
    void OnDiscontinuityTool(wxCommandEvent& event);
    void OnRulerTool(wxCommandEvent& event);
    void OnHorizontalArrowTool(wxCommandEvent& event);
    //void OnTextTool(wxCommandEvent& event);
    void OnEditTool(wxCommandEvent& event);

    void OnAddClock(wxCommandEvent& event);
    void OnAddSignal(wxCommandEvent& event);
    void OnAddBus(wxCommandEvent& event);

    void OnExportBitmap(wxCommandEvent& event);
    void OnExportSVG(wxCommandEvent& event);
    void OnExportPS(wxCommandEvent& event);

    void OnClockApply(wxCommandEvent& event);
    void OnTransitionPanelApply(wxCommandEvent& event);
private:
    DECLARE_DYNAMIC_CLASS(TimingView)
    DECLARE_EVENT_TABLE()
};


#endif

