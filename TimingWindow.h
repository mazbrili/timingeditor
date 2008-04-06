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

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <vector>

#include <wx/docview.h>
#include <wx/dnd.h>
#include <wx/timer.h>

#include "TimingDoc.h"

class ClockSettingsPanel;
class TransitionSettingsPanel;
class AxisSettingsPanel;
class TimeCompressorSettingsPanel;

class DropCaret
{
public:
    DropCaret(){}
    ~DropCaret(){}
    wxPoint GetPosition(){return m_pos;}
    void Move(wxPoint pt){m_pos = pt;}
    void Move(int x, int y){m_pos = wxPoint(x,y);}
    void SetSize(int width, int height){m_size = wxSize(width, height);}
    void SetSize(wxSize size){m_size = size;}
    void Show(bool show = true){ m_show = show;}
    bool IsVisible(){return m_show;}
    void Hide(){m_show = false;}
    void Draw(wxDC &dc)
    {
        if ( m_show )
            dc.Blit(m_pos, m_size, &dc, wxPoint(0, 0), wxINVERT);
    }
private:
    wxPoint m_pos;
    wxSize m_size;
    bool m_show;
};

//BEGIN_DECLARE_EVENT_TYPES()
//    DECLARE_EVENT_TYPE(TimingDiagramEvent, -1)
//END_DECLARE_EVENT_TYPES()



class TimingWindow: public wxScrolledWindow
{
public:
    //TimingWindow(wxView *v, wxMDIChildFrame *frame, const wxPoint& pos, const wxSize& size, long style);
    TimingWindow(wxView *v, wxWindow *parent,
        ClockSettingsPanel *clkpanel,
        TransitionSettingsPanel *trnpanel,
        AxisSettingsPanel *axspanel,
        TimeCompressorSettingsPanel * tcpanel);
    ~TimingWindow();

    bool IsTextSelected(void);
    bool IsSignalSelected(void);
    bool IsSelectedSignalClock(void);
    bool VLineIsSelected(void);
    bool HArrowIsSelected(void);
    bool DiscontSelected(void);
    bool CanPaste(void);
    bool CanDeleteText(void);
    void InsertText(wxString str);
    bool IsSomethingSelected(void);

    void DeleteText(void);
    wxString GetText(void);
    void SelectAll(void);

    void AttachPanels();
    void DetachPanels();



    void Draw(wxDC& dc, bool exporting = false);
    wxPoint GetBitmapSize();
    //void SendEvent(const wxString& str);
private:
    void UpdateClockPanel();
    void UpdateTransitionPanel();
    void UpdateAxisPanel();
private: /// event methods
    void OnPaint(wxPaintEvent &event);
    void OnEraseBackground(wxEraseEvent &event);
    void OnMouseLeftDown(wxMouseEvent &event);
    void OnMouseLeftUp(wxMouseEvent &event);
    void OnMouseRightDown(wxMouseEvent& event);
    void OnMouseRightUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent &event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent &event);
    void OnChar(wxKeyEvent &event);
    //void OnSetFocus(wxFocusEvent &event);
    //void OnKillFocus(wxFocusEvent &event);
    void OnLeaveWindow(wxMouseEvent &event);
    void OnEnterWindow(wxMouseEvent &event);
    void OnDraw(wxDC& dc);

    void OnTimer(wxTimerEvent& event);

    void PaintBackground(wxDC &dc);
    void InitTextDrawing();
    wxInt32 textNumber;
    wxPoint DrawEditableText(wxDC &dc, wxString str, wxPoint &offset);

private:
    wxCaret *caret;

    std::vector<wxPoint> textOffsets;
    std::vector<wxPoint> textSizes;
    std::vector<wxString> texts;
    bool mouseoverwindow;

    wxCoord signalNamesWidth;
    wxPoint cursorpos;



    wxInt32 fontsize;
    wxFont font;


    wxCoord axisStart, axisStop;
    bool scrollingleft;
    bool scrollingright;
    wxTimer scrolltimer;

    wxInt32 GridStepWidth;

    std::vector<wxCoord> heightOffsets;
    wxInt32 editingNumber;
    Signal  editingSignal;
    wxPoint editingPoint[2];
    wxInt32 editingValA;
    wxInt32 editingValB;
    wxInt32 editingValC;
    wxString editingText;
    /*wxInt32 selectedVLine;
    bool changingVLineLength;
    bool changingVLineUpperLength;
    wxInt32 changingVLineTargetPos;*/
    std::vector<wxPoint> VLineOffsets;
    std::vector<wxInt32> VLineToOffset;

    std::vector<wxPoint> HArrowOffsets;
    std::vector<wxInt32> HArrowToOffset;

     enum states
    {
        Neutral,

        /// selecting and changing the axis
        AxisIsMarked,
        EditAxisLeft,
        EditAxisRight,

        /// editing, selecting and moving signals (3), change space (2)
        EditSignal,
        SignalIsSelected,
        MovingSignal,
        ChangingUpperSpace,
        ChangingLowerSpace,

        /// one state to edit discontinuities
        EditingDiscontinuity,

        /// vertical lines: create new (2), selected (1), change length (2), moving around (1)
        InsertingVLineWaitingFirstPoint,
        InsertingVLineWaitingSecondPoint,
        VLineIsMarked,
        ChangingVLineLengthUpper,
        ChangingVLineLengthLower,
        MovingVLine,

        ///
        InsertingHArrowWaitingFirstPoint,
        InsertingHArrowWaitingSecondPoint,
        HArrowIsMarked,
        ChangingHArrowLengthLeft,
        ChangingHArrowLengthRight,
        MovingHArrow,
        HArrowMovingText,
        SelectingText,
        TextFieldSelected,
        //TextSelected,

        DisconSelected,
    };
    states WindowState;

    ClockSettingsPanel *ClkSetPanel;
    TransitionSettingsPanel *TranSetPanel;
    AxisSettingsPanel *AxisSetPanel;
    TimeCompressorSettingsPanel *TmeCmprssrPanel;
    ///void DrawState(wxDC& dc);
public:
    wxInt8 GetTransitionWidth();
    //bool GetEn5090();
    void SetClock(wxInt32 delay, wxInt32 ticks);
    void SetNeutralState(void);
    wxInt32 GetSelectedSignalNr();
    void SetTransition(wxInt8 width, bool en50, bool en90);
    void SetAxis(wxInt8 unit, wxInt32 ticklength, wxInt32 tacklength, wxInt32 offset);
    void SetTimeCompressor(wxInt32 time);
    void UpdateTimeCompressorPanel(void);

    void OnDragEnter(void);
    void OnDragLeave(void);
    wxDragResult OnDragOver(wxPoint pt, wxDragResult def);
    bool OnDrop(wxPoint pt, wxString str );

    bool CanZoomTicksOut(void);
    bool CanZoomTicksIn(void);
    void ZoomTicksOut(void);
    void ZoomTicksIn(void);
    void DeleteSignal(void);
    void DeleteVLine(void);
    void DeleteHArrow(void);
    void DeleteDiscont(void);
    void DeleteSelection(void);
    void OnSelectInsertDiscontTool(void);
    void OnSelectRulerTool(void);
    void OnSelectHArrowTool(void);
    void OnSelectTextTool(void);
    void OnSelectNeutralTool(void);

public:
    wxView *view;

private:
    DECLARE_EVENT_TABLE()
};

#endif
