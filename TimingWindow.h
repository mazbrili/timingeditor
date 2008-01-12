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
class TimingWindow: public wxScrolledWindow
{
public:
    //TimingWindow(wxView *v, wxMDIChildFrame *frame, const wxPoint& pos, const wxSize& size, long style);
    TimingWindow(wxView *v, wxWindow *parent);
    ~TimingWindow();
    virtual void OnDraw(wxDC& dc);


    bool IsTextSelected(void);
    bool IsSignalSelected(void);
    bool VLineIsSelected(void);
    bool HArrowIsSelected(void);
    bool CanPaste(void);
    bool CanDeleteText(void);
    void InsertText(wxString str);

    void DeleteText(void);
    wxString GetText(void);
    void SelectAll(void);


    void Draw(wxDC& dc, bool exporting = false);
    wxPoint GetBitmapSize();
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

    void OnTimer(wxTimerEvent& event);

    void PaintBackground(wxDC &dc);

private:
    wxCaret *caret;
    // remember text areas
    std::vector<wxPoint> textOffsets;
    std::vector<wxPoint> textSizes;
    std::vector<wxString> texts;
    //std::vector<wxString *> textStringPtrs;
    //wxString *currentlyEditedTextPtr;
    //wxString *droppedStringPtr;
    bool mouseoverwindow;
    /*bool overText;
    wxInt32 selpos[2];
    wxInt32 strpos;
    wxInt32 droppos;
    wxPoint dndpt;
    bool dndsource;
    bool drop;
    bool dropcopy;
    wxString DroppedText;
    DropCaret *dropcaret;
    void InsertDroppedText(wxString str);*/

    //bool EditMode(void) const;

    wxCoord signalNamesWidth;
    wxPoint cursorpos;

    bool IsSomethingSelected(void);

    wxInt32 fontsize;
    wxFont font;


    wxCoord axisStart, axisStop;
    bool scrollingleft;
    bool scrollingright;
    wxTimer scrolltimer;

    wxInt32 transitionWidth;
    wxInt32 transitionsPerGridStep;


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
    };
    states WindowState;
    void SetNeutralState(void);




public:
    wxInt32 GetSelectedSignalNr(){ return editingNumber; }
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
