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

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/dcbuffer.h>
#include <wx/caret.h>
#include <wx/textdlg.h>
#include <wx/clipbrd.h>
#include <wx/dnd.h>


#include "art/tri_cur.xpm"
#include "art/ruler_cur.xpm"
#include "art/textedit_cur.xpm"
#include "art/harrow_cur.xpm"
#include "TimingApp.h"
#include "TimingWindow.h"
#include "TimingDoc.h"
#include "cmd.h"
#include "enumers.h"

#include "dndtarget.h"

//#include "view.h"

BEGIN_EVENT_TABLE(TimingWindow, wxScrolledWindow)
    EVT_PAINT           (TimingWindow::OnPaint)
    EVT_ERASE_BACKGROUND(TimingWindow::OnEraseBackground)
    EVT_LEFT_DOWN       (TimingWindow::OnMouseLeftDown)
    EVT_LEFT_DCLICK     (TimingWindow::OnMouseLeftDown)
    EVT_LEFT_UP         (TimingWindow::OnMouseLeftUp)
    EVT_RIGHT_DOWN      (TimingWindow::OnMouseRightDown)
    EVT_RIGHT_UP        (TimingWindow::OnMouseRightUp)
    EVT_MOTION          (TimingWindow::OnMouseMove)
    EVT_MOUSEWHEEL      (TimingWindow::OnMouseWheel)
    EVT_CHAR            (TimingWindow::OnChar)
    EVT_KEY_DOWN        (TimingWindow::OnKeyDown)
    EVT_LEAVE_WINDOW    (TimingWindow::OnLeaveWindow)
    EVT_ENTER_WINDOW    (TimingWindow::OnEnterWindow)
    //EVT_SET_FOCUS       (TimingWindow::OnSetFocus)
    //EVT_KILL_FOCUS      (TimingWindow::OnKillFocus)
    EVT_TIMER           (-1, TimingWindow::OnTimer)
END_EVENT_TABLE()




// Define a constructor for my canvas
TimingWindow::TimingWindow(wxView *v, wxWindow *parent)://wxMDIChildFrame *frame):
    wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxWANTS_CHARS),
    //currentlyEditedTextPtr(NULL),
    mouseoverwindow(true),
    //overText(false),
    //dndpt(-1,-1),
    fontsize(10),
    font(fontsize, wxFONTFAMILY_SWISS,  wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL),
    axisStart(0),
    axisStop(0),
    scrollingleft(false),
    scrollingright(false),
    scrolltimer(this),
    transitionWidth(3),
    transitionsPerGridStep(5),
    editingNumber(-1),


    WindowState(Neutral),

    view(v)
{
    SetDropTarget(new DragnDropTextTarget(this) );
    caret = new wxCaret(this, 4, 4);
    SetCaret(caret);
    //dropcaret = new DropCaret();

    //selpos[0] = -1;
    //selpos[1] = -1;
    SetScrollRate(5, 5);
    SetCursor(*wxCROSS_CURSOR);
    SetNeutralState();
    Refresh();
}
TimingWindow::~TimingWindow()
{
    //delete dropcaret;
}
bool TimingWindow::IsTextSelected(void)
{
    if (
        WindowState == TextFieldSelected &&
        editingValB != -1 &&
        editingValC != -1 &&
        editingValB != editingValC
    )
        return true;
    return false;
}
bool TimingWindow::CanPaste(void)
{
    if ( WindowState == TextFieldSelected && editingValA != -1  )
        if (wxTheClipboard->Open())
        {
            if (wxTheClipboard->IsSupported( wxDF_TEXT ))
            {
                wxTheClipboard->Close();
                return true;
            }
            wxTheClipboard->Close();
        }
    return false;
}

const int DistanceToTimeline = 25;
const int DistanceFromTimeline = 20;
const int SignalHeight       = 20;
const int MinimumSignalDistance = 10;
const int manipXoffset = 20;
const int manipRadius = 4;

wxPoint TimingWindow::GetBitmapSize()
{
    return wxPoint(
        axisStop-40,
        heightOffsets[heightOffsets.size()-1] - DistanceToTimeline - DistanceFromTimeline
    );
}

void TimingWindow::Draw(wxDC& dc, bool exporting)
{
    if (!view)
        return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc )
        return;

    dc.SetFont(font);

    wxSize virtsize(0,0);
    textOffsets.clear();
    textSizes.clear();
    texts.clear();
    //textStringPtrs.clear();
    heightOffsets.clear();

    wxInt32 textNumber = 0;

    wxColour bgcol = dc.GetTextBackground();
    wxColour col( 0xe0, 0xe0, 0xe0 );
    if ( !exporting && ( WindowState == SelectingText ||
                  WindowState == TextFieldSelected) )
    {
        dc.SetBackgroundMode(wxSOLID);
        dc.SetTextBackground(col);
    }


    wxCoord width = 0;
    wxPoint offset;
    if ( !exporting )
    {
        offset = wxPoint(
            40,
            DistanceToTimeline + DistanceFromTimeline
        );
    }
    else
    {
        offset = wxPoint(
            0,//40
            0//DistanceToTimeline + DistanceFromTimeline
        );
    }

    /// drawing signal names:
    for ( unsigned int k = 0 ; k < doc->signals.size() ; ++k )
    {
        wxString str = doc->signals[k].name;

        wxCoord nx = offset.x;

        texts.push_back(str);
        if ( WindowState == TextFieldSelected && editingNumber == textNumber )
            str = editingText;
        else if ( str.IsEmpty() /*&& WindowState != TextFieldSelected*/  )
        {
            str = _T(" ");
            wxCoord w, h;
            dc.GetTextExtent(str, &w, &h);
            nx -= w/2;
        }
        wxCoord w, h;//, desc;

        h = dc.GetCharHeight();

        wxCoord ny =  offset.y + SignalHeight/2 - h/2+MinimumSignalDistance/2;
        heightOffsets.push_back(offset.y);
        ny += doc->signals[k].prespace;
        wxPoint off(nx, ny );
        dc.DrawText(str, off.x, off.y);
        textOffsets.push_back(off);
        dc.GetTextExtent(str, &w, &h);
        wxPoint siz(w,h);
        textSizes.push_back( siz );

        if ( doc->signals[k].IsBus )
        {
            dc.SetTextBackground(bgcol);
            dc.SetBackgroundMode(wxTRANSPARENT);
            dc.DrawText(_T(" [ ]"), off.x+w, off.y);
            if ( !exporting &&
                ( WindowState == SelectingText ||
                  WindowState == TextFieldSelected) )
            {
                dc.SetBackgroundMode(wxSOLID);
                dc.SetTextBackground(col);
            }
        }

        if ( width < w )
            width = w;

        /// chang position of caret
        if ( WindowState == TextFieldSelected && editingNumber == textNumber )
        {
            if ( editingValA > str.Length() )
                 editingValA = str.Length();
            dc.GetTextExtent(str.Mid(0, editingValA), &w, &h);
            wxCoord xx, yy;
            CalcScrolledPosition( off.x+w, off.y, &xx, &yy);
            caret->Move(xx, yy);
            //caret->Move(off.x+w, off.y);

            /// highlight selected text
            if ( editingValB != -1 && editingValC != -1 )
            {
                wxCoord dx, dy, lx;
                if ( editingValB < editingValC )
                {
                    dc.GetTextExtent(str.Mid(0, editingValB), &dx, &dy);
                    dc.GetTextExtent(str.Mid(editingValB, editingValC-editingValB), &lx, &dy);
                }
                else
                {
                    dc.GetTextExtent(str.Mid(0, editingValC), &dx, &dy);
                    dc.GetTextExtent(str.Mid(editingValC, editingValB-editingValC), &lx, &dy);
                }
                dx += off.x;
                dy = dc.GetCharHeight();
                dc.Blit(dx, off.y, lx, dy, &dc, 0, 0, wxINVERT);
            }
        }
        ++textNumber;

        offset.y += SignalHeight;
        offset.y += MinimumSignalDistance;
        offset.y += doc->signals[k].space;
        offset.y += doc->signals[k].prespace;

    }
    heightOffsets.push_back(offset.y);
    dc.SetTextBackground(bgcol);
    dc.SetBackgroundMode(wxTRANSPARENT);

    signalNamesWidth = offset.x + width + 20;
    if ( virtsize.y < offset.y ) virtsize.y = offset.y;
    /// end of drawing signal names



    /// drawing the timeline
    if (  !exporting )
    {
        wxInt32 gridWidth = transitionsPerGridStep * transitionWidth;
        wxInt32 gridSteps = doc->length;
        axisStart = signalNamesWidth;
        //if ( !changingLength )
        if ( WindowState != EditAxisLeft && WindowState != EditAxisRight )
        {

                axisStop  = signalNamesWidth + gridWidth*gridSteps + 3;
                dc.DrawLine(axisStart -3 , DistanceToTimeline,
                        axisStop , DistanceToTimeline);
                for (wxInt32 n = 0; n <= gridSteps; ++n )
                {
                    dc.DrawLine(signalNamesWidth + n*gridWidth, DistanceToTimeline-2,
                        signalNamesWidth + n*gridWidth, DistanceToTimeline+3);
                }
                //if ( axismarked )
                if ( WindowState == AxisIsMarked )
                {
                    dc.SetBrush(*wxBLUE_BRUSH);
                    dc.DrawRectangle(axisStop-3, DistanceToTimeline-3, 7, 7);
                    dc.DrawRectangle(axisStart-3, DistanceToTimeline-3, 7, 7);
                }
        }
        else
        {
            wxInt32 newLength = editingValB;
            if ( WindowState == EditAxisRight )
            {
                axisStop  = signalNamesWidth + gridWidth*newLength + 3;
                dc.DrawLine(axisStart - 3, DistanceToTimeline,
                        axisStop, DistanceToTimeline);
                for (wxInt32 n = 0; n <= newLength; ++n )
                    dc.DrawLine(signalNamesWidth + n*gridWidth, DistanceToTimeline-2,
                        signalNamesWidth + n*gridWidth, DistanceToTimeline+3);
                dc.SetBrush(*wxBLUE_BRUSH);
                dc.DrawRectangle(axisStop-3, DistanceToTimeline-3, 7, 7);
                dc.DrawRectangle(axisStart-3, DistanceToTimeline-3, 7, 7);
            }
            else
            {
                axisStop  = signalNamesWidth + gridWidth*gridSteps + 3;
                dc.DrawLine(signalNamesWidth + gridWidth*gridSteps - gridWidth*newLength -3 ,
                    DistanceToTimeline,
                    axisStop , DistanceToTimeline
                );
                for (wxInt32 n = 0; n <= newLength; ++n )
                    dc.DrawLine(signalNamesWidth + gridWidth*gridSteps - gridWidth*newLength + n*gridWidth,
                        DistanceToTimeline-2,
                        signalNamesWidth + gridWidth*gridSteps - gridWidth*newLength + n*gridWidth,
                        DistanceToTimeline+3
                    );
                dc.SetBrush(*wxBLUE_BRUSH);
                dc.DrawRectangle(axisStop-3, DistanceToTimeline-3, 7, 7);
                dc.DrawRectangle(signalNamesWidth + gridWidth*gridSteps - gridWidth*newLength -3, DistanceToTimeline-3, 7, 7);
            }
        }
        width += 20 + gridWidth * gridSteps;// + 3;
    }

    /// points to manipulate the signal (through right click)
    if (  !exporting )
    {
        dc.SetBrush(*wxBLACK_BRUSH);
        for ( wxInt32 n = 0 ; n < doc->signals.size() ; ++n )
        {
            if ( WindowState == SignalIsSelected && editingNumber == n )
            {
                dc.SetBrush(*wxBLUE_BRUSH);
                dc.SetPen(*wxLIGHT_GREY_PEN);
            }
            dc.DrawCircle(manipXoffset,
                heightOffsets[n] + SignalHeight/2+MinimumSignalDistance/2+ doc->signals[n].prespace,
                manipRadius
            );
            if ( WindowState == SignalIsSelected && editingNumber == n )
            {
                dc.SetBrush(*wxBLACK_BRUSH);
                //dc.SetPen(wxNullPen);
                dc.SetPen(*wxBLACK_PEN);
            }
        }
        dc.SetBrush(wxNullBrush);
    }


    /// drawing around the selected signal
    //if ( selectedSignal != -1 )
    if ( !exporting && (WindowState == SignalIsSelected ||
        WindowState == ChangingLowerSpace ||
        WindowState == ChangingUpperSpace ||
        WindowState == MovingSignal))
    {
        dc.SetPen(*wxBLACK_PEN);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        wxInt32 newSpace = editingValA;
        if ( WindowState != SignalIsSelected && WindowState != MovingSignal )
        {
            if ( WindowState == ChangingLowerSpace )
            {
                dc.DrawRoundedRectangle(manipXoffset - 2*manipRadius, heightOffsets[editingNumber],
                axisStop-(manipXoffset - 2*manipRadius)+10,
                SignalHeight + MinimumSignalDistance + newSpace + doc->signals[editingNumber].prespace,
                10.0);
                /// the end
                editingPoint[0] = wxPoint(
                    (manipXoffset - 2*manipRadius)+(axisStop-(manipXoffset - 2*manipRadius)+10)/2,
                    heightOffsets[editingNumber] + SignalHeight + MinimumSignalDistance + newSpace + doc->signals[editingNumber].prespace
                );
                /// the start
                editingPoint[1] = wxPoint(
                    editingPoint[0].x,
                    heightOffsets[editingNumber]
                );
            }
            else
            {
                dc.DrawRoundedRectangle(manipXoffset - 2*manipRadius, heightOffsets[editingNumber] +
                    doc->signals[editingNumber].prespace - newSpace,
                axisStop-(manipXoffset - 2*manipRadius)+10,
                    SignalHeight + MinimumSignalDistance + newSpace + doc->signals[editingNumber].space,
                10.0);
                /// the end
                editingPoint[0] = wxPoint(
                    (manipXoffset - 2*manipRadius)+(axisStop-(manipXoffset - 2*manipRadius)+10)/2,
                    heightOffsets[editingNumber] + SignalHeight + MinimumSignalDistance + doc->signals[editingNumber].prespace + doc->signals[editingNumber].space
                );
                /// the start
                editingPoint[1] = wxPoint(
                    editingPoint[0].x,
                    heightOffsets[editingNumber] + doc->signals[editingNumber].prespace - newSpace
                );
            }
        }
        else
        {
            dc.DrawRoundedRectangle(manipXoffset - 2*manipRadius, heightOffsets[editingNumber],
                axisStop-(manipXoffset - 2*manipRadius)+10,
                SignalHeight + MinimumSignalDistance + doc->signals[editingNumber].space + doc->signals[editingNumber].prespace,
                10.0);
            /// the end
            editingPoint[0] = wxPoint(
                (manipXoffset - 2*manipRadius)+(axisStop-(manipXoffset - 2*manipRadius)+10)/2,
                    heightOffsets[editingNumber] + SignalHeight + MinimumSignalDistance +
                    doc->signals[editingNumber].space+
                    doc->signals[editingNumber].prespace
            );
            /// the start
            editingPoint[1] = wxPoint(
                editingPoint[0].x,
                heightOffsets[editingNumber]
            );
        }
        //dc.SetPen(wxNullPen);
        dc.SetPen(*wxBLACK_PEN);
        dc.SetBrush(wxNullBrush);

        dc.SetBrush(*wxBLUE_BRUSH);
        dc.DrawRectangle(
            editingPoint[0].x - 3,
            editingPoint[0].y -3,
            7, 7
        );
        dc.DrawRectangle(
            editingPoint[1].x - 3,
            editingPoint[1].y -3,
            7, 7
        );
    }

    /// indicate target position of moving signals
    if ( WindowState == MovingSignal )
    {
        wxPen pen(*wxBLACK, 2 );
        dc.SetPen(pen);
        dc.DrawLine(
            manipXoffset, heightOffsets[editingValA],
            axisStop,  heightOffsets[editingValA]
        );
        //dc.SetPen(wxNullPen);
        dc.SetPen(*wxBLACK_PEN);
    }

    /// drawing the signals
    wxPen defPen = dc.GetPen();
    wxPen boldPen(*wxBLACK,2);
    dc.SetPen(boldPen);
    if ( !exporting && ( WindowState == SelectingText ||
                  WindowState == TextFieldSelected) )
    {
        dc.SetBackgroundMode(wxSOLID);
        dc.SetTextBackground(col);
    }
    for ( wxInt32 n = 0 ; n < doc->signals.size() ; ++n )
    {
        offset.x = signalNamesWidth;
        offset.y = heightOffsets[n]+MinimumSignalDistance/2+ doc->signals[n].prespace;
        Signal sig;
        if ( WindowState == EditSignal && editingNumber == n && editingValB != -1 ) sig = editingSignal;
        else sig = doc->signals[n];
        if ( sig.IsClock )
        {
            bool val = false;
            wxInt32 n = 0;
            wxInt32 k = -((2*sig.ticks)- (sig.delay % (2*sig.ticks)));
            for (; k < doc->length ; ++k)
            {
                if ( k >= 0 )
                {
                    if ( k == 0 && n == 0)
                    {
                        dc.DrawLine(
                            offset.x, offset.y + SignalHeight,
                            offset.x, offset.y
                        );
                    }
                    if ( val ) // one
                    {
                        dc.DrawLine(
                            offset.x, offset.y + SignalHeight,
                            offset.x + transitionWidth*transitionsPerGridStep, offset.y + SignalHeight
                        );
                        if ( ++n == sig.ticks )
                        {
                            dc.DrawLine(
                                offset.x + transitionWidth*transitionsPerGridStep, offset.y + SignalHeight,
                                offset.x+transitionWidth*transitionsPerGridStep, offset.y
                            );
                            val = !val;
                            n = 0;
                        }
                    }
                    else
                    {
                        dc.DrawLine(
                            offset.x, offset.y,
                            offset.x+transitionWidth*transitionsPerGridStep, offset.y
                        );
                        if ( ++n == sig.ticks )
                        {
                            dc.DrawLine(
                                offset.x+transitionWidth*transitionsPerGridStep, offset.y,
                                offset.x + transitionWidth*transitionsPerGridStep, offset.y + SignalHeight
                            );
                            val = !val;
                            n = 0;
                        }
                    }
                    offset.x+= transitionWidth * transitionsPerGridStep;
                }
                else
                {
                    if ( ++n == sig.ticks )
                    {
                        val = !val;
                        n = 0;
                    }
                }
            }
        }
        else if ( !sig.IsBus )
        {
            offset.x = signalNamesWidth + 1*transitionWidth;
            for ( wxInt32 k = 0 ; k < doc->length ; ++k )
            {
                switch ( sig.values[k])
                {
                    case zero:
                        dc.DrawLine(
                            offset.x, offset.y+SignalHeight,
                            offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight
                        );

                        if ( k != doc->length -1 )
                        {
                            switch ( sig.values[k+1])
                            {
                                case zero:
                                    dc.DrawLine(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight,
                                        offset.x+transitionWidth*transitionsPerGridStep, offset.y+SignalHeight);
                                    break;
                                case one:
                                    dc.DrawLine(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight,
                                        offset.x+transitionWidth*transitionsPerGridStep, offset.y);
                                    break;
                                case hz :
                                    dc.DrawLine(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight,
                                        offset.x+transitionWidth*transitionsPerGridStep, offset.y+SignalHeight/2);
                                    break;
                                case u:
                                default:
                                    dc.DrawLine(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight,
                                        offset.x+transitionWidth*transitionsPerGridStep, offset.y+SignalHeight);
                                    dc.DrawLine(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight,
                                        offset.x+transitionWidth*transitionsPerGridStep, offset.y);
                            }
                        }
                        break;
                    case one:
                        dc.DrawLine(
                            offset.x, offset.y,
                            offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y
                        );
                        if ( k != doc->length -1 )
                        {
                            switch ( sig.values[k+1])
                            {
                                case zero:
                                    dc.DrawLine(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y,
                                        offset.x+transitionWidth*transitionsPerGridStep, offset.y+SignalHeight);
                                    break;
                                case one:
                                    dc.DrawLine(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y,
                                        offset.x+transitionWidth*transitionsPerGridStep, offset.y);
                                    break;
                                case hz :
                                    dc.DrawLine(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y,
                                        offset.x+transitionWidth*transitionsPerGridStep, offset.y+SignalHeight/2);
                                    break;
                                case u:
                                default:
                                    dc.DrawLine(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y,
                                        offset.x+transitionWidth*transitionsPerGridStep, offset.y);
                                    dc.DrawLine(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y,
                                        offset.x+transitionWidth*transitionsPerGridStep, offset.y+SignalHeight);
                            }
                        }
                        break;
                    case hz :
                        dc.DrawLine(
                            offset.x, offset.y+SignalHeight/2,
                            offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight/2
                        );
                        if ( k != doc->length -1 )
                        {
                            switch ( sig.values[k+1])
                            {
                                case zero:
                                    dc.DrawLine(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight/2,
                                        offset.x+transitionWidth*transitionsPerGridStep, offset.y+SignalHeight);
                                    break;
                                case one:
                                    dc.DrawLine(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight/2,
                                        offset.x+transitionWidth*transitionsPerGridStep, offset.y);
                                    break;
                                case hz :
                                    dc.DrawLine(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight/2,
                                        offset.x+transitionWidth*transitionsPerGridStep, offset.y+SignalHeight/2);
                                    break;
                                case u:
                                default:
                                    dc.DrawLine(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight/2,
                                        offset.x+transitionWidth*transitionsPerGridStep, offset.y+SignalHeight);
                                    dc.DrawLine(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight/2,
                                        offset.x+transitionWidth*transitionsPerGridStep, offset.y);
                            }
                        }
                        break;
                    case u:
                    default:
                        dc.DrawLine(
                            offset.x, offset.y+SignalHeight,
                            offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight
                        );
                        dc.DrawLine(
                            offset.x, offset.y,
                            offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y
                        );
                        if ( k != doc->length -1 )
                        {
                            switch ( sig.values[k+1])
                            {
                                case zero:
                                    dc.DrawLine(
                                        offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight,
                                        offset.x+transitionWidth*(transitionsPerGridStep), offset.y+SignalHeight
                                    );
                                    dc.DrawLine(
                                        offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y,
                                        offset.x+transitionWidth*(transitionsPerGridStep), offset.y+SignalHeight
                                    );
                                    break;
                                case one:
                                    dc.DrawLine(
                                        offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight,
                                        offset.x+transitionWidth*(transitionsPerGridStep), offset.y
                                    );
                                    dc.DrawLine(
                                        offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y,
                                        offset.x+transitionWidth*(transitionsPerGridStep), offset.y
                                    );
                                    break;
                                case hz:
                                    dc.DrawLine(
                                        offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight,
                                        offset.x+transitionWidth*(transitionsPerGridStep), offset.y+SignalHeight/2
                                    );
                                    dc.DrawLine(
                                        offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y,
                                        offset.x+transitionWidth*(transitionsPerGridStep), offset.y+SignalHeight/2
                                    );
                                    break;
                                case u:
                                default:
                                    dc.DrawLine(
                                        offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight,
                                        offset.x+transitionWidth*(transitionsPerGridStep  ), offset.y+SignalHeight
                                    );
                                    dc.DrawLine(
                                        offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y,
                                        offset.x+transitionWidth*(transitionsPerGridStep  ), offset.y
                                    );
                                    break;
                            }
                        }
                        break;
                }
                offset.x+= transitionWidth * transitionsPerGridStep;
            }
        }
        else // is bus
        {
            offset.x = signalNamesWidth + 1*transitionWidth;
            //wxInt32 i = 0;
            for ( wxInt32 k = 0 ; k < doc->length ; ++k )
            {
                switch ( sig.values[k] )
                {
                    case zero:
                    case one:
                        dc.DrawLine(
                            offset.x, offset.y + SignalHeight,
                            offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y + SignalHeight
                        );
                        dc.DrawLine(
                            offset.x, offset.y,
                            offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y
                        );
                        if ( k != doc->length -1 )
                        {
                            if ( sig.values[k+1] == sig.values[k])
                            {
                                dc.DrawLine(
                                    offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y + SignalHeight,
                                    offset.x+transitionWidth*(transitionsPerGridStep  ), offset.y + SignalHeight
                                );
                                dc.DrawLine(
                                    offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y,
                                    offset.x+transitionWidth*(transitionsPerGridStep  ), offset.y
                                );
                            }
                            else if ( sig.values[k+1] == hz )
                            {
                                dc.DrawLine(
                                    offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y + SignalHeight,
                                    offset.x+transitionWidth*(transitionsPerGridStep  ), offset.y + SignalHeight/2
                                );
                                dc.DrawLine(
                                    offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y,
                                    offset.x+transitionWidth*(transitionsPerGridStep  ), offset.y+ SignalHeight/2
                                );
                            }
                            else if ( sig.values[k+1] == u )
                            {
                                dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                dc.SetPen(*wxLIGHT_GREY_PEN);
                                wxPoint rec[3];
                                rec[0] = wxPoint(offset.x+transitionWidth*(transitionsPerGridStep), offset.y + SignalHeight);
                                rec[1] = wxPoint(offset.x+transitionWidth*(-0.5+transitionsPerGridStep), offset.y + SignalHeight/2);
                                rec[2] = wxPoint(offset.x+transitionWidth*(transitionsPerGridStep), offset.y);
                                dc.DrawPolygon(3, rec);
                                //dc.SetPen(wxNullPen);
                                //dc.SetPen(*wxBLACK_PEN);
                                dc.SetPen(boldPen);
                                dc.DrawLine(
                                    offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y + SignalHeight,
                                    offset.x+transitionWidth*(transitionsPerGridStep  ), offset.y
                                );
                                dc.DrawLine(
                                    offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y,
                                    offset.x+transitionWidth*(transitionsPerGridStep  ), offset.y + SignalHeight
                                );
                            }
                            else
                            {
                                dc.DrawLine(
                                    offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y + SignalHeight,
                                    offset.x+transitionWidth*(transitionsPerGridStep  ), offset.y
                                );
                                dc.DrawLine(
                                    offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y,
                                    offset.x+transitionWidth*(transitionsPerGridStep  ), offset.y + SignalHeight
                                );
                            }

                        }
                        if ( k == 0 || sig.values[k] != sig.values[k-1] )
                        {
                            /// the text:
                            wxPoint textoff(2,0);
                            wxString text = sig.TextValues[k];//[i]; //

                            texts.push_back(text);
                            if (WindowState == TextFieldSelected &&
                                editingNumber == textNumber )
                                text = editingText;
                            else if ( text.IsEmpty() )
                            {
                                text = _T(" ");
                                wxCoord w, h;
                                dc.GetTextExtent(text, &w, &h);
                                textoff.x -= w/2;
                            }
                            wxCoord h = dc.GetCharHeight();

                            textoff.x  += offset.x;
                            textoff.y  += offset.y + SignalHeight/2 - h/2;

                            dc.DrawText(text, textoff.x, textoff.y );
                            textOffsets.push_back(textoff);
                            wxCoord wid, hei;
                            dc.GetTextExtent(text, &wid, &hei);
                            textSizes.push_back( wxPoint(wid,hei) );
                            //textStringPtrs.push_back( &(doc->harrows[n].text) );

                            /// chang position of caret
                            if ( WindowState == TextFieldSelected && editingNumber == textNumber )
                            {
                                wxCoord w, h;
                                if ( editingValA > text.Length() )
                                     editingValA = text.Length();
                                dc.GetTextExtent(text.Mid(0, editingValA), &w, &h);
                                //caret->Move(textoff.x+w, textoff.y);
                                wxCoord xx, yy;
                                CalcScrolledPosition( textoff.x+w, textoff.y, &xx, &yy);
                                caret->Move(xx, yy);
                                //caret->Move(off.x+w, off.y);
                                /// highlight selected text
                                if ( editingValB != -1 && editingValC != -1 )
                                {
                                    wxCoord dx, dy, lx;
                                    if ( editingValB < editingValC )
                                    {
                                        dc.GetTextExtent(text.Mid(0, editingValB), &dx, &dy);
                                        dc.GetTextExtent(text.Mid(editingValB, editingValC-editingValB), &lx, &dy);
                                    }
                                    else
                                    {
                                        dc.GetTextExtent(text.Mid(0, editingValC), &dx, &dy);
                                        dc.GetTextExtent(text.Mid(editingValC, editingValB-editingValC), &lx, &dy);
                                    }
                                    dx += textoff.x;
                                    dy = dc.GetCharHeight();
                                    dc.Blit(dx, textoff.y, lx, dy, &dc, 0, 0, wxINVERT);
                                }
                            }
                            ++textNumber;

                            //i++;
                        }
                        break;
                    case hz :
                        dc.DrawLine(
                            offset.x, offset.y+SignalHeight/2,
                            offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight/2
                        );
                        if ( k != doc->length -1 )
                        {
                            switch ( sig.values[k+1])
                            {
                                case u:
                                default:
                                {
                                    dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                    dc.SetPen(*wxLIGHT_GREY_PEN);
                                    wxPoint rec[3];
                                    rec[0] = wxPoint(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight/2);
                                    rec[1] = wxPoint(offset.x+transitionWidth*transitionsPerGridStep,     offset.y+SignalHeight);
                                    rec[2] = wxPoint(offset.x+transitionWidth*transitionsPerGridStep,     offset.y);
                                    dc.DrawPolygon(3, rec);

                                    //dc.SetPen(wxNullPen);
                                    //dc.SetPen(*wxBLACK_PEN);
                                    dc.SetPen(boldPen);
                                }
                                case one:
                                case zero:
                                    dc.DrawLine(
                                        offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight/2,
                                        offset.x+transitionWidth* transitionsPerGridStep,    offset.y+SignalHeight
                                    );
                                    dc.DrawLine(
                                        offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight/2,
                                        offset.x+transitionWidth* transitionsPerGridStep,    offset.y
                                    );
                                    break;
                                case hz :
                                    dc.DrawLine(
                                        offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight/2,
                                        offset.x+transitionWidth*transitionsPerGridStep,     offset.y+SignalHeight/2
                                    );
                                    break;
                            }
                        }
                        break;
                    case u:
                    default:
                        {
                            dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                            dc.SetPen(*wxLIGHT_GREY_PEN);
                            wxPoint rec[4];
                            rec[0] = wxPoint(offset.x+1, offset.y+SignalHeight);
                            rec[1] = wxPoint(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight);
                            rec[2] = wxPoint(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y);
                            rec[3] = wxPoint(offset.x+1, offset.y);
                            dc.DrawPolygon(4, rec);
                            //dc.SetPen(wxNullPen);
                            //dc.SetPen(*wxBLACK_PEN);
                            dc.SetPen(boldPen);
                        }
                        dc.DrawLine(
                            offset.x, offset.y+SignalHeight,
                            offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight
                        );
                        dc.DrawLine(
                            offset.x, offset.y,
                            offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y
                        );
                        if ( k != doc->length -1 )
                        {
                            switch ( sig.values[k+1])
                            {
                                case zero:
                                case one:
                                    {
                                        dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                        dc.SetPen(*wxLIGHT_GREY_PEN);
                                        wxPoint rec[3];
                                        rec[0] = wxPoint(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight);
                                        rec[1] = wxPoint(offset.x+transitionWidth*(-0.5+transitionsPerGridStep), offset.y+SignalHeight/2);
                                        rec[2] = wxPoint(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y);
                                        dc.DrawPolygon(3, rec);
                                        //dc.SetPen(wxNullPen);
                                        //dc.SetPen(*wxBLACK_PEN);
                                        dc.SetPen(boldPen);
                                    }
                                    dc.DrawLine(
                                        offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight,
                                        offset.x+transitionWidth*(transitionsPerGridStep),   offset.y
                                    );
                                    dc.DrawLine(
                                        offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y,
                                        offset.x+transitionWidth*(transitionsPerGridStep),   offset.y+SignalHeight
                                    );
                                    break;
                                case hz:
                                    {
                                        dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                        dc.SetPen(*wxLIGHT_GREY_PEN);
                                        wxPoint rec[3];
                                        rec[0] = wxPoint(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight);
                                        rec[1] = wxPoint(offset.x+transitionWidth*(transitionsPerGridStep), offset.y+SignalHeight/2);
                                        rec[2] = wxPoint(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y);
                                        dc.DrawPolygon(3, rec);
                                        //dc.SetPen(wxNullPen);
                                        //dc.SetPen(*wxBLACK_PEN);
                                        dc.SetPen(boldPen);
                                    }
                                    dc.DrawLine(
                                        offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight,
                                        offset.x+transitionWidth*(transitionsPerGridStep), offset.y+SignalHeight/2
                                    );
                                    dc.DrawLine(
                                        offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y,
                                        offset.x+transitionWidth*(transitionsPerGridStep), offset.y+SignalHeight/2
                                    );
                                    break;
                                case u:
                                default:
                                    {
                                        dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                        dc.SetPen(*wxLIGHT_GREY_PEN);
                                        wxPoint rec[4];
                                        rec[0] = wxPoint(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight);
                                        rec[1] = wxPoint(offset.x+transitionWidth*(transitionsPerGridStep), offset.y+SignalHeight);
                                        rec[2] = wxPoint(offset.x+transitionWidth*(transitionsPerGridStep), offset.y);
                                        rec[3] = wxPoint(offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y);
                                        dc.DrawPolygon(4, rec);
                                        //dc.SetPen(wxNullPen);
                                        //dc.SetPen(*wxBLACK_PEN);
                                        dc.SetPen(boldPen);
                                    }
                                    dc.DrawLine(
                                        offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y+SignalHeight,
                                        offset.x+transitionWidth*(transitionsPerGridStep),   offset.y+SignalHeight
                                    );
                                    dc.DrawLine(
                                        offset.x+transitionWidth*(transitionsPerGridStep-1), offset.y,
                                        offset.x+transitionWidth*(transitionsPerGridStep),   offset.y
                                    );
                                    break;
                            }
                        }
                        break;
                }
                offset.x+= transitionWidth * transitionsPerGridStep;
            }
        }
        if ( !exporting && n < doc->signals.size() )
        {
            wxPen pen(*wxLIGHT_GREY, 1, wxDOT);
            dc.SetPen(pen);
            dc.DrawLine(signalNamesWidth, heightOffsets[n],
                        axisStop, heightOffsets[n]);
            if ( n + 1 == doc->signals.size() )
                dc.DrawLine(signalNamesWidth, heightOffsets[n+1],
                            axisStop, heightOffsets[n+1]);
            dc.SetPen(boldPen);
        }
    }
    dc.SetTextBackground(bgcol);
    dc.SetBackgroundMode(wxTRANSPARENT);


    /// drawing discontinuities
    std::set<wxInt32>::iterator it;
    for ( it = doc->discontinuities.begin() ; it != doc->discontinuities.end() ; it++)
    {
        /// triangle
        offset.x = signalNamesWidth  +
                    (0.5 + *it) * transitionWidth * transitionsPerGridStep
                    - 3
                    ;
        offset.y = DistanceToTimeline-3;
        wxPoint points[] =
        {
            offset + wxPoint(0,0),
            offset + wxPoint(8,0),
            offset + wxPoint(4, 6)
        };
        dc.SetBrush(*wxBLACK_BRUSH);
        if (  !exporting )
            dc.DrawPolygon(3, points);

        /// splines over signal
        for ( wxInt32 n = 0 ; n < doc->signals.size() ; ++n)
        {
            //wxPoint sploff(15, 10);
            offset.x = signalNamesWidth  +
                    (0.5 + *it) * transitionWidth * transitionsPerGridStep
                    - 2
                    ;
            offset.y = heightOffsets[n]+MinimumSignalDistance/2 + doc->signals[n].prespace;
            for ( wxInt32 n = 0 ; n < 4 ; ++n )
            {
                wxPoint ind(n,0);
                wxPoint points[] =
                {
                    offset + ind + wxPoint(+3, -0.2*SignalHeight),
                    offset + ind + wxPoint(+0, +0.2*SignalHeight),
                    offset + ind + wxPoint(+7, +0.8*SignalHeight),
                    offset + ind + wxPoint(+0, +1.2*SignalHeight)
                };
                if ( n == 0 || n == 3) //dc.SetPen(wxNullPen);
                    dc.SetPen(*wxBLACK_PEN);
                else
                    dc.SetPen( *wxWHITE_PEN );

                //if ( !exporting )
                dc.DrawSpline(4, points);
                //else
                //    dc.DrawSpline(4, points);
            }
        }
    }


    /// change the size of the scrollable window
    if ( virtsize.x < (width + 500) ) virtsize.x = (width + 500);
    SetVirtualSize(virtsize);


    /// drawing the vertical lines
    VLineOffsets.clear();
    VLineToOffset.clear();
    //wxPen vlinepen(*wxBLACK, 1, wxDOT);
    /*wxSOLID wxTRANSPARENT wxDOT wxLONG_DASH wxSHORT_DASH
    wxDOT_DASH*/
    //dc.SetPen(vlinepen);
    for ( wxInt32 k = 0 ; k < doc->vertlines.size() ; ++k)
    {
        wxPoint offset(signalNamesWidth + doc->vertlines[k].vpos * transitionWidth * transitionsPerGridStep,
        /*offset.y =*/ heightOffsets[doc->vertlines[k].StartPos]);
        wxInt32 tolen = heightOffsets[doc->vertlines[k].EndPos + 1];
        if ( editingNumber == k )
        {
            if ( WindowState == VLineIsMarked )
            {
                dc.DrawLine(
                    offset.x, offset.y,
                    offset.x, heightOffsets[doc->vertlines[k].EndPos + 1]
                );
                dc.SetBrush(*wxBLUE_BRUSH);
                dc.DrawRectangle(offset.x-3, offset.y -                                    3, 7, 7);
                dc.DrawRectangle(offset.x-3, heightOffsets[doc->vertlines[k].EndPos + 1] - 3, 7, 7);
                dc.SetBrush(wxNullBrush);
            }
            else if ( WindowState == ChangingVLineLengthUpper )
            {
                dc.DrawLine(
                    offset.x, heightOffsets[editingValA],
                    offset.x, heightOffsets[doc->vertlines[k].EndPos + 1]
                );
                dc.SetBrush(*wxBLUE_BRUSH);
                dc.DrawRectangle(offset.x-3, heightOffsets[editingValA] -                  3, 7, 7);
                dc.DrawRectangle(offset.x-3, heightOffsets[doc->vertlines[k].EndPos + 1] - 3, 7, 7);
                dc.SetBrush(wxNullBrush);
            }
            else if ( WindowState == ChangingVLineLengthLower )
            {
                dc.DrawLine(
                    offset.x, offset.y,
                    offset.x, heightOffsets[editingValA]
                );
                dc.SetBrush(*wxBLUE_BRUSH);
                dc.DrawRectangle(offset.x-3, offset.y -                   3, 7, 7);
                dc.DrawRectangle(offset.x-3, heightOffsets[editingValA] - 3, 7, 7);
                dc.SetBrush(wxNullBrush);
            }
            else if ( WindowState == MovingVLine )
            {
                offset.x = signalNamesWidth + editingValA * transitionWidth * transitionsPerGridStep;
                dc.DrawLine(
                    offset.x, offset.y,
                    offset.x, heightOffsets[doc->vertlines[k].EndPos + 1]
                );
                dc.SetBrush(*wxBLUE_BRUSH);
                dc.DrawRectangle(offset.x-3, offset.y -                                    3, 7, 7);
                dc.DrawRectangle(offset.x-3, heightOffsets[doc->vertlines[k].EndPos + 1] - 3, 7, 7);
                dc.SetBrush(wxNullBrush);
            }
            else
            {
                dc.DrawLine(
                    offset.x, offset.y,
                    offset.x, heightOffsets[doc->vertlines[k].EndPos + 1]
                );
            }
        }
        else
        {
            dc.DrawLine(
                offset.x, offset.y,
                offset.x, heightOffsets[doc->vertlines[k].EndPos + 1]
            );
        }
        VLineOffsets.push_back(offset);
        VLineToOffset.push_back(tolen);
    }
    if ( WindowState == InsertingVLineWaitingSecondPoint )
    {
        wxPoint offset(signalNamesWidth + editingPoint[0].x * transitionWidth * transitionsPerGridStep,
        offset.y = heightOffsets[editingPoint[0].y]);
        dc.DrawLine(
            offset.x, offset.y,
            offset.x, heightOffsets[editingPoint[1].y]
        );
        dc.SetBrush(*wxBLUE_BRUSH);
        dc.DrawRectangle(offset.x-3, offset.y -                         3, 7, 7);
        dc.DrawRectangle(offset.x-3, heightOffsets[editingPoint[1].y] - 3, 7, 7);
        dc.SetBrush(wxNullBrush);
    }
    //dc.SetPen(*wxBLACK_PEN);


    /// drawing the horizontal arrows
    if ( !exporting && (WindowState == SelectingText ||
                        WindowState == TextFieldSelected ))
    {
        dc.SetBackgroundMode(wxSOLID);
        dc.SetTextBackground(col);
    }
    HArrowOffsets.clear();
    HArrowToOffset.clear();
    for ( wxInt32 n = 0 ; n < doc->harrows.size() ; ++n )
    {
        wxInt32 distancfortimeline = 0;
        if ( !exporting )
            distancfortimeline = DistanceToTimeline + DistanceFromTimeline;
        HArrow &ha = doc->harrows[n];
        wxPoint offset(signalNamesWidth + doc->vertlines[ha.fromVLine].vpos * transitionWidth * transitionsPerGridStep,
        //        heightOffsets[doc->vertlines[ha.fromVLine].StartPos] + ha.pos);
                distancfortimeline + ha.pos);
        if ( editingNumber == n && WindowState == MovingHArrow )
            offset.y = editingValA + DistanceToTimeline + DistanceFromTimeline;
        wxPoint tooffset(signalNamesWidth + doc->vertlines[ha.toVLine].vpos * transitionWidth * transitionsPerGridStep,
            offset.y);

        if ( editingNumber == n && WindowState == ChangingHArrowLengthLeft )
        {
            if (editingValA == -1 )
                offset.x = cursorpos.x;
            else
                offset.x = signalNamesWidth + doc->vertlines[editingValA].vpos * transitionWidth * transitionsPerGridStep;
        }
        if ( editingNumber == n && WindowState == ChangingHArrowLengthRight )
        {
            if (editingValB == -1 )
                tooffset.x = cursorpos.x;
            else
                tooffset.x = signalNamesWidth + doc->vertlines[editingValB].vpos * transitionWidth * transitionsPerGridStep;
        }

        if ( offset.x > tooffset.x ) // swap
        {
            wxCoord t = offset.x;
            offset.x = tooffset.x;
            tooffset.x = t;
        }
        if ( editingNumber == n &&
            (WindowState == ChangingHArrowLengthRight ||
             WindowState == ChangingHArrowLengthLeft    ) &&
            (editingValA == -1 || editingValB == -1))
        {
            wxPen pen(*wxBLACK, 1, wxDOT);//wxLONG_DASH );
            dc.SetPen(pen);
            dc.DrawLine(  offset.x,   offset.y, tooffset.x,   tooffset.y);
            //dc.SetPen(wxNullPen);
            dc.SetPen(*wxBLACK_PEN);
        }
        else
            dc.DrawLine(  offset.x,   offset.y, tooffset.x,   tooffset.y);
        dc.DrawLine(  offset.x,   offset.y,   offset.x+5,   offset.y-3);
        dc.DrawLine(  offset.x,   offset.y,   offset.x+5,   offset.y+3);
        dc.DrawLine(tooffset.x, tooffset.y, tooffset.x-5, tooffset.y-3);
        dc.DrawLine(tooffset.x, tooffset.y, tooffset.x-5, tooffset.y+3);
        HArrowOffsets.push_back(offset);
        HArrowToOffset.push_back(tooffset.x);

        /// the text:
        wxPoint textoff(0,0);
        wxString text = ha.text;

        texts.push_back(text);
        if (WindowState == TextFieldSelected &&
            editingNumber == textNumber )
            text = editingText;
        else if ( text.IsEmpty() )
        {
            text = _T(" ");
            wxCoord w, h;
            dc.GetTextExtent(text, &w, &h);
            textoff.x -= w/2;
        }
        wxCoord h = dc.GetCharHeight();
        if ( editingNumber == n && WindowState == HArrowMovingText )
        {
            textoff.x  += (offset.x + tooffset.x)/2 + editingValA;
            textoff.y  += offset.y + editingValB - h;
        }
        else
        {
            textoff.x  += (offset.x + tooffset.x)/2 + ha.textoffset.x;
            textoff.y  += offset.y + ha.textoffset.y - h;
        }

        dc.DrawText(text, textoff.x, textoff.y );
        textOffsets.push_back(textoff);
        wxCoord wid, hei;
        dc.GetTextExtent(text, &wid, &hei);
        textSizes.push_back( wxPoint(wid,hei) );

        /// chang position of caret
//        if ( WindowState == TextFieldSelected && editingNumber == textNumber )
//        {
//            wxCoord w, h;
//            if ( editingValA > text.Length() )
//                 editingValA = text.Length();
//            dc.GetTextExtent(text.Mid(0, editingValA), &w, &h);
//            caret->Move(textoff.x+w, textoff.y);
        /// chang position of caret
        if ( WindowState == TextFieldSelected && editingNumber == textNumber )
        {
            wxCoord w, h;
            if ( editingValA > text.Length() )
                 editingValA = text.Length();
            dc.GetTextExtent(text.Mid(0, editingValA), &w, &h);
            //caret->Move(textoff.x+w, textoff.y);
            wxCoord xx, yy;
            CalcScrolledPosition( textoff.x+w, textoff.y, &xx, &yy);
            caret->Move(xx, yy);
            /// highlight selected text
            if ( editingValB != -1 && editingValC != -1 )
            {
                wxCoord dx, dy, lx;
                if ( editingValB < editingValC )
                {
                    dc.GetTextExtent(text.Mid(0, editingValB), &dx, &dy);
                    dc.GetTextExtent(text.Mid(editingValB, editingValC-editingValB), &lx, &dy);
                }
                else
                {
                    dc.GetTextExtent(text.Mid(0, editingValC), &dx, &dy);
                    dc.GetTextExtent(text.Mid(editingValC, editingValB-editingValC), &lx, &dy);
                }
                dx += textoff.x;
                dy = dc.GetCharHeight();
                dc.Blit(dx, textoff.y, lx, dy, &dc, 0, 0, wxINVERT);
            }
        }
        //
        ++textNumber;
        //textStringPtrs.push_back( &(doc->harrows[n].text) );

        if ( editingNumber == n && ( WindowState == HArrowIsMarked ||
               WindowState == ChangingHArrowLengthLeft ||
               WindowState == ChangingHArrowLengthRight ))
        {
            dc.SetBrush(*wxBLUE_BRUSH);
            dc.DrawRectangle(  offset.x-3, offset.y - 3, 7, 7);
            dc.DrawRectangle(tooffset.x-3, offset.y - 3, 7, 7);
            dc.SetBrush(wxNullBrush);
        }
        if ( editingNumber == n &&
            ( WindowState == HArrowIsMarked ||
              WindowState == HArrowMovingText ))
        {
            dc.SetBrush(*wxBLUE_BRUSH);
            dc.DrawRectangle(
                textoff.x - 3,
                textoff.y + h - 3,
                7, 7);
            dc.SetBrush(wxNullBrush);
        }
        if ( editingNumber == n &&
            ( WindowState == HArrowIsMarked ||
              WindowState == HArrowMovingText ||
              WindowState == ChangingHArrowLengthLeft ||
              WindowState == ChangingHArrowLengthRight ||
              WindowState == SelectingText))
        {
            wxPen pen(*wxBLACK, 1, wxSHORT_DASH );
            dc.SetPen(pen);
            dc.DrawLine(  (offset.x + tooffset.x)/2,   offset.y, textoff.x,   textoff.y+h);
            //dc.SetPen(wxNullPen);
            dc.SetPen(*wxBLACK_PEN);
        }

        //wxString text;
        //wxPoint textoffset;
    }
    if ( WindowState == InsertingHArrowWaitingSecondPoint )
    {
        wxPoint offset(signalNamesWidth + doc->vertlines[editingNumber].vpos * transitionWidth * transitionsPerGridStep,
        //        heightOffsets[doc->vertlines[ha.fromVLine].StartPos] + ha.pos);
                DistanceToTimeline + DistanceFromTimeline + editingValA);
        wxPoint tooffset;
        tooffset.y = offset.y;
        if ( editingValB == -1)
            tooffset.x = cursorpos.x;
        else
            tooffset.x = signalNamesWidth + doc->vertlines[editingValB].vpos * transitionWidth * transitionsPerGridStep;
        if ( offset.x > tooffset.x ) // swap
        {
            wxCoord t = offset.x;
            offset.x = tooffset.x;
            tooffset.x = t;
        }
        if ( editingValB == -1)
        {
            wxPen pen(*wxBLACK, 1, wxLONG_DASH );
            dc.SetPen(pen);
            dc.DrawLine(  offset.x,   offset.y, tooffset.x,   tooffset.y);
            //dc.SetPen(wxNullPen);
            dc.SetPen(*wxBLACK_PEN);
        }
        else
            dc.DrawLine(  offset.x,   offset.y, tooffset.x,   tooffset.y);
        dc.DrawLine(  offset.x,   offset.y,   offset.x+5,   offset.y-3);
        dc.DrawLine(  offset.x,   offset.y,   offset.x+5,   offset.y+3);
        dc.DrawLine(tooffset.x, tooffset.y, tooffset.x-5, tooffset.y-3);
        dc.DrawLine(tooffset.x, tooffset.y, tooffset.x-5, tooffset.y+3);
    }
    dc.SetTextBackground(bgcol);
    dc.SetBackgroundMode(wxTRANSPARENT);

    /// cursor cross over the whole canvas:
    if ( mouseoverwindow &&
        WindowState != SelectingText &&
        WindowState != TextFieldSelected
        )
    {
        dc.SetPen(*wxGREY_PEN);
        dc.CrossHair(cursorpos.x, cursorpos.y);
    }

    //dropcaret->Draw(dc);
}

/// //////////////don't use this event methdos///////////////////////////////////
void TimingWindow::OnEraseBackground(wxEraseEvent &event){/* must be empty */}
void TimingWindow::OnDraw(wxDC& dc){/* must be empty */}
/// ////////////////////////////////////////////////////////////////////////////
void TimingWindow::OnPaint(wxPaintEvent &event)
{
    wxBufferedPaintDC dc(this);
    PrepareDC(dc);
    PaintBackground(dc);
    Draw(dc);
}
void TimingWindow::PaintBackground(wxDC &dc)
{
    wxColour backgroundColour = *wxWHITE;
    /*wxColour backgroundColour = GetBackgroundColour();
    if ( !backgroundColour.Ok())
        backgroundColour = wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE);*/


    dc.SetBrush(wxBrush(backgroundColour));
    dc.SetPen(wxPen(backgroundColour, 1));
    wxRect windowRect(wxPoint(0,0), GetClientSize());
    CalcUnscrolledPosition(windowRect.x, windowRect.y, &windowRect.x, &windowRect.y);
    dc.DrawRectangle(windowRect);
    dc.SetBrush(wxNullBrush);
    dc.SetPen(wxNullPen);
}

void TimingWindow::OnLeaveWindow(wxMouseEvent &event)
{
    mouseoverwindow = false;
    this->Refresh();
}
void TimingWindow::OnEnterWindow(wxMouseEvent &event)
{
    mouseoverwindow = true;

    switch ( WindowState )
    {
        default:
        case Neutral:
        case AxisIsMarked:
        case SignalIsSelected:
        case EditingDiscontinuity:
            break;
        case EditAxisLeft:
        case EditAxisRight:
            if ( !event.LeftIsDown() )
                WindowState = AxisIsMarked;
            break;
        case EditSignal:
        case MovingSignal:
            if ( !event.LeftIsDown() )
                SetNeutralState();
            break;
        case ChangingUpperSpace:
        case ChangingLowerSpace:
            if ( !event.LeftIsDown() )
                WindowState = SignalIsSelected;
            break;
        case InsertingVLineWaitingFirstPoint:
            break;
        case InsertingVLineWaitingSecondPoint:
            if ( ! event.LeftIsDown() )
                WindowState = InsertingVLineWaitingFirstPoint;
            break;
        case VLineIsMarked:
            break;
        case ChangingVLineLengthUpper:
        case ChangingVLineLengthLower:
        case MovingVLine:
            if ( ! event.LeftIsDown() )
                WindowState = VLineIsMarked;
            break;
        case InsertingHArrowWaitingFirstPoint:
            break;
        case InsertingHArrowWaitingSecondPoint:
            if ( ! event.LeftIsDown() )
                WindowState = InsertingHArrowWaitingFirstPoint;
            break;
        case HArrowMovingText:
            if ( ! event.LeftIsDown() )
                WindowState = HArrowIsMarked;
            break;
    }

    Refresh();
}
void TimingWindow::OnMouseLeftDown(wxMouseEvent &event)
{
    if (!view) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;


    wxClientDC dc(this);
    PrepareDC(dc);
    dc.SetFont(font);
    wxPoint pt(event.GetLogicalPosition(dc));

    bool dorefresh = false;


    states newstate = WindowState;
    switch ( WindowState )
    {
        case Neutral:
            /// check click on axis
            if (pt.x > axisStart &&
                pt.x < axisStop &&
                pt.y > DistanceToTimeline - 5 &&
                pt.y < DistanceToTimeline + 5)
            {
                newstate = AxisIsMarked;
                dorefresh = true;
                break;
            }
            /// check click on left points
            editingNumber = -1;
            for ( wxInt32 n = 0 ; n < doc->signals.size() ; ++n )
            {
                if ( pt.x >= manipXoffset - 2*manipRadius && pt.y >= heightOffsets[n]+MinimumSignalDistance/2 + SignalHeight/2 + doc->signals[n].prespace - 2*manipRadius &&
                    pt.x <=  manipXoffset + 2*manipRadius && pt.y <= heightOffsets[n]+MinimumSignalDistance/2 + SignalHeight/2 + doc->signals[n].prespace + 2*manipRadius)
                {
                    newstate = SignalIsSelected;
                    editingNumber = n;
                    dorefresh = true;
                    break;
                }
            }
            /// check if user clicked into a waveform
            for ( wxInt32 n = 0 ; n < doc->signals.size() ; ++n )
            {
                if ( ! doc->signals[n].IsClock )
                    if ( pt.y >= heightOffsets[n]+MinimumSignalDistance/2+ doc->signals[n].prespace && pt.x >= axisStart &&
                        pt.y <= heightOffsets[n]+MinimumSignalDistance/2 + doc->signals[n].prespace + SignalHeight && pt.x <= axisStop )
                    {
                        newstate = EditSignal;
                        //editingSignalNr
                        //editingSignalPosStart
                        //editingSignalPosStop
                        editingNumber = n;
                        wxCoord p = pt.x - axisStart;
                        editingValA = p / (transitionWidth * transitionsPerGridStep);
                        editingSignal = doc->signals[n];
                        editingValB = editingValA;

                        vals val = editingSignal.values[editingValA];
                        vals nval;
                        if (  !editingSignal.IsBus )
                        {
                            switch ( val )
                            {
                                case zero: nval = one; break;
                                case one: nval = hz; break;
                                case hz: nval = u; break;
                                default:
                                case u: nval = zero; break;
                            }
                            editingSignal.values[editingValA] = nval;
                        }
                        else // IsBus
                        {
                            vals valr = u, vall = u;
                            if ( editingValA > 0 ) vall = editingSignal.values[editingValA - 1];
                            if ( editingValA <= doc->length ) valr = editingSignal.values[editingValA + 1];

                            if (( vall == one || vall == zero ) || ( valr == one || valr == zero ))
                                switch ( val )
                                {
                                    case zero: nval = one; break;
                                    case one: nval = hz; break;
                                    case hz: nval = u; break;
                                    default:
                                    case u: nval = zero; break;
                                }
                            else
                                switch ( val )
                                {
                                    case zero:
                                    case one: nval = hz; break;
                                    case hz: nval = u; break;
                                    default:
                                    case u: nval = zero; break;
                                }
                            editingSignal.values[editingValA] = nval;

                            /*std::vector<wxString> strs;
                            wxInt32 nnn = 0;
                            for ( wxInt32 ind = 0 ; ind < editingSignal.values.size() ; ++ind )
                            {
                                ind == editingValA)
                                editingSignal.values[ind] != zero
                                editingSignal.values[ind] != one
                                doc->signals[n].values[ind] == zero
                                doc->signals[n].values[ind] == one


                                            strs.push_back(editingSignal.TextValues[nnn]);
                                            nnn++;

                            }
                            editingSignal.TextValues = strs;*/
                        }
                        dorefresh = true;
                        break;
                    }
            }
            /// check if user clicked onto a vertical line
            for ( wxInt32 k = 0 ; k < VLineOffsets.size() ; ++k )
            {
                if (pt.x >= VLineOffsets[k].x - 3 &&
                    pt.x <= VLineOffsets[k].x + 3 &&
                    pt.y >= VLineOffsets[k].y &&
                    pt.y <= VLineToOffset[k] )
                {
                    newstate = VLineIsMarked;
                    editingNumber = k;
                    editingValA = -1;
                    editingValB = -1;
                    dorefresh = true;
                    break;
                }
            }
            /// check if user clicked onto horizontal arrow
            for ( wxInt32 k = 0 ; k < HArrowOffsets.size() ; ++k)
            {
                if ( pt.x >= HArrowOffsets[k].x &&
                     pt.x <= HArrowToOffset[k] &&
                     pt.y >= HArrowOffsets[k].y - 3 &&
                     pt.y <= HArrowOffsets[k].y + 3 )
                {
                    newstate = HArrowIsMarked;
                    editingNumber = k;
                    editingValA = -1;
                    editingValB = -1;
                    dorefresh = true;
                    break;
                }
            }
            break;
        case AxisIsMarked:
            /// check if user clicked the end of marked axis
            if (pt.x >= axisStop-5 && pt.y >= DistanceToTimeline - 5 &&
                pt.x <= axisStop+5 && pt.y <= DistanceToTimeline + 5 )
            {
                newstate = EditAxisRight;
                /*changingLength = true;
                changingLeftLength = false;*/
                //oldlength = doc->length;
                editingValA = doc->length;
                editingValB = editingValA;
                //newLength = oldlength;
                dorefresh = true;
                break;
            }
            /// check if user clicked the start of marked axis
            if (pt.x >= axisStart-5 && pt.y >= DistanceToTimeline - 5 &&
                pt.x <= axisStart+5 && pt.y <= DistanceToTimeline + 5)
            {
                newstate = EditAxisLeft;
                /*changingLength = true;
                changingLeftLength = true;*/
                //oldlength = doc->length;
                editingValA = doc->length;
                editingValB = editingValA;
                //newLength = oldlength;
                dorefresh = true;
                break;
            }
            /// check if not another click on the axis
            SetNeutralState();
            OnMouseLeftDown(event);
            return;
        case VLineIsMarked:
            /// check click on begin of vline (top)
            if (pt.x >= VLineOffsets[editingNumber].x - 3 &&
                pt.x <= VLineOffsets[editingNumber].x + 3 &&
                pt.y >= VLineOffsets[editingNumber].y - 3 &&
                pt.y <= VLineOffsets[editingNumber].y + 3 )
            {
                newstate = ChangingVLineLengthUpper;
                editingValA = doc->vertlines[editingNumber].StartPos;
                dorefresh = true;
                break;
            }
            /// check click on end of vline (bottom)
            if (pt.x >= VLineOffsets[editingNumber].x - 3 &&
                pt.x <= VLineOffsets[editingNumber].x + 3 &&
                pt.y >= VLineToOffset[editingNumber] - 3 &&
                pt.y <= VLineToOffset[editingNumber] + 3 )
            {
                newstate = ChangingVLineLengthLower;
                editingValA = doc->vertlines[editingNumber].EndPos+1;
                dorefresh = true;
                break;
            }
            /// check moving of vline
            if (pt.x >= VLineOffsets[editingNumber].x - 3 &&
                pt.x <= VLineOffsets[editingNumber].x + 3 &&
                pt.y >= VLineOffsets[editingNumber].y + 3 &&
                pt.y <= VLineToOffset[editingNumber] - 3 )
            {
                newstate = MovingVLine;
                editingValA = doc->vertlines[editingNumber].vpos;
                dorefresh = true;
                break;
            }
            /// check another click on vline
            SetNeutralState();
            OnMouseLeftDown(event);
            return;
        case SignalIsSelected:
            /// check change of space around signal
            if (pt.x >= editingPoint[0].x - 4 && pt.y >= editingPoint[0].y - 4 &&
                pt.x <= editingPoint[0].x + 4 && pt.y <= editingPoint[0].y + 4)
            {
                newstate = ChangingLowerSpace;
                editingValA = doc->signals[editingNumber].space;
                break;
            }
            if (pt.x >= editingPoint[1].x - 4 && pt.y >= editingPoint[1].y - 4 &&
                pt.x <= editingPoint[1].x + 4 && pt.y <= editingPoint[1].y + 4)
            {
                newstate = ChangingUpperSpace;
                editingValA = doc->signals[editingNumber].prespace;
                break;
            }

            SetNeutralState();
            OnMouseLeftDown(event);
            return;
        case EditingDiscontinuity:
            /// check insertion/remove of discont
            if (pt.x > axisStart &&
                pt.x < axisStop &&
                pt.y > DistanceToTimeline - 5 &&
                pt.y < DistanceToTimeline + 5 )
            {
                wxCoord p = pt.x-axisStart;
                wxInt32 discontpos = p / (transitionWidth * transitionsPerGridStep);
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                if ( doc->discontinuities.find(discontpos) == doc->discontinuities.end() )
                    cmdproc->Submit(new AddDiscontCommand(doc, discontpos) );
                else
                    cmdproc->Submit(new RemoveDiscontCommand(doc, discontpos) );
                dorefresh = true;
            }
            break;
        case InsertingVLineWaitingFirstPoint:
            {
                wxInt32 n = doc->signals.size();
                if ( n && // some signals in the document
                    pt.x > axisStart &&
                    pt.x < axisStop /*&&
                    pt.y > heightOffsets[0] &&
                    pt.y < heightOffsets[n]*/ )
                {
                    wxCoord p = pt.x-axisStart + (transitionWidth * transitionsPerGridStep)/2;
                    editingPoint[0].x = p / (transitionWidth * transitionsPerGridStep);

                    wxInt32 k;
                    for ( k = 0 ; k < n ; ++k )
                        if ( pt.y < (heightOffsets[k] + heightOffsets[k+1])/2 )
                            break;
                    if ( editingPoint[0].y != k )
                        dorefresh = true;
                    editingPoint[0].y = k;
                    editingPoint[1].y = k;
                    newstate = InsertingVLineWaitingSecondPoint;
                }
            }
            break;
        case InsertingHArrowWaitingFirstPoint:
            /// check if user clicked onto a vertical line
            for ( wxInt32 k = 0 ; k < VLineOffsets.size() ; ++k )
            {
                if (pt.x >= VLineOffsets[k].x - 3 &&
                    pt.x <= VLineOffsets[k].x + 3 &&
                    pt.y >= VLineOffsets[k].y &&
                    pt.y <= VLineToOffset[k] )
                {
                    newstate = InsertingHArrowWaitingSecondPoint;
                    editingNumber = k;
                    editingValA = pt.y - DistanceToTimeline - DistanceFromTimeline;
                    editingValB = - 1;
                    dorefresh = true;
                    break;
                }
            }
            break;
        case HArrowIsMarked:
            /// check click on start of harrow (left)
            if ( pt.x >= HArrowOffsets[editingNumber].x - 3 &&
                 pt.x <= HArrowOffsets[editingNumber].x + 3 &&
                 pt.y >= HArrowOffsets[editingNumber].y - 3 &&
                 pt.y <= HArrowOffsets[editingNumber].y + 3)
            {
                if ( HArrowOffsets[editingNumber].x ==
                    signalNamesWidth + doc->vertlines[doc->harrows[editingNumber].fromVLine].vpos *
                        transitionWidth * transitionsPerGridStep )
                    newstate = ChangingHArrowLengthLeft;
                else
                    newstate = ChangingHArrowLengthRight;
                editingValA = doc->harrows[editingNumber].fromVLine;
                editingValB = doc->harrows[editingNumber].toVLine;



                dorefresh = true;
                break;
            }
            /// check click on end of harrow (right)
            if ( pt.x >= HArrowToOffset[editingNumber] - 3 &&
                 pt.x <= HArrowToOffset[editingNumber] + 3 &&
                 pt.y >= HArrowOffsets[editingNumber].y - 3 &&
                 pt.y <= HArrowOffsets[editingNumber].y + 3)
            {
                if ( HArrowToOffset[editingNumber] ==
                    signalNamesWidth + doc->vertlines[doc->harrows[editingNumber].toVLine].vpos *
                        transitionWidth * transitionsPerGridStep )
                    newstate = ChangingHArrowLengthRight;
                else
                    newstate = ChangingHArrowLengthLeft;
                editingValA = doc->harrows[editingNumber].fromVLine;
                editingValB = doc->harrows[editingNumber].toVLine;
                dorefresh = true;
                break;
            }
            /// check moving of the text
            {
                wxCoord
                x = (HArrowOffsets[editingNumber].x + HArrowToOffset[editingNumber])/2 +
                    doc->harrows[editingNumber].textoffset.x,
                y = HArrowOffsets[editingNumber].y + doc->harrows[editingNumber].textoffset.y;
                if ( pt.x >=  x  - 3 &&
                     pt.x <=  x  + 3 &&
                     pt.y >=  y  - 3 &&
                     pt.y <=  y  + 3 )
                {
                    newstate = HArrowMovingText;
                    editingValA = pt.x - (HArrowOffsets[editingNumber].x + HArrowToOffset[editingNumber])/2;
                    editingValB = pt.y - HArrowOffsets[editingNumber].y;
                    dorefresh = true;
                    break;
                }
            }
            /// check moving of harrow
            if ( pt.x >= HArrowOffsets[editingNumber].x - 3 &&
                 pt.x <= HArrowToOffset[editingNumber] + 3 &&
                 pt.y >= HArrowOffsets[editingNumber].y - 3 &&
                 pt.y <= HArrowOffsets[editingNumber].y + 3)
            {
                newstate = MovingHArrow;
                editingValA = doc->harrows[editingNumber].pos;
                dorefresh = true;
                break;
            }
            /// check click on another harrow
            SetNeutralState();
            OnMouseLeftDown(event);
            return;
        case TextFieldSelected:
            if (pt.x > textOffsets[editingNumber].x &&
                pt.x < textOffsets[editingNumber].x + textSizes[editingNumber].x &&
                pt.y > textOffsets[editingNumber].y &&
                pt.y < textOffsets[editingNumber].y + textSizes[editingNumber].y )
            {
                /// calc with of the texts
                wxArrayInt widths;
                dc.GetPartialTextExtents(editingText, widths);
                widths.Insert(0, 0);

                /// serch position of click in text
                wxInt32 k;
                for ( k = 0 ; k < widths.GetCount()-1; ++k )
                    if ( pt.x <= textOffsets[editingNumber].x + (widths[k] + widths[k+1] )/2 )
                        break;

                if ( event.ShiftDown() )
                {
                    if ( editingValB == -1 )
                        editingValB = editingValA;
                    editingValC = k;
                    editingValA = k;
                    if ( editingValB == editingValC )
                        editingValB = editingValC = -1;
                    //dndpt = wxPoint( -1, -1);
                    //selectpossible = true;
                }
                else
                {
                    editingValA = k;
                    editingValB = -1;
                    editingValC = -1;
                }
                break;
            }
            else
            {
                /// generate the command to change the text
                // TODO (daniel#1#): command to change text
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(
                    new ChangeTextCommand(doc, editingNumber, editingText)
                );
                editingNumber = -1;
                editingValA = -1;
                editingValB = -1;
                editingValC = -1;
                WindowState = SelectingText;
                OnMouseLeftDown(event);
                return;
            }
            break;
        case SelectingText:
            {
                bool clickedText = false;
                for ( wxInt32 n = textOffsets.size() - 1 ; n >= 0 ; n-- )
                {
                    if ( pt.x > textOffsets[n].x &&
                        pt.x < textOffsets[n].x + textSizes[n].x &&
                        pt.y > textOffsets[n].y &&
                        pt.y < textOffsets[n].y + textSizes[n].y )
                    {
                        editingNumber = n;
                        newstate = TextFieldSelected;
                        editingText = texts[n];
                        dorefresh = true;
                        clickedText = true;

                        /// show the caret
                        if ( !caret->IsVisible() ) caret->Show();
                        caret->SetSize(1, dc.GetCharHeight());

                        /// calc with of the texts
                        wxArrayInt widths;
                        dc.GetPartialTextExtents(editingText, widths);
                        widths.Insert(0, 0);

                        /// serch position of click in text
                        wxInt32 k;
                        for ( k = 0 ; k < widths.GetCount()-1; ++k )
                            if ( pt.x <= textOffsets[n].x + (widths[k] + widths[k+1] )/2 )
                                break;
                        editingValA = k;
                        editingValB = -1;
                        editingValC = -1;
                        break;
                    }
                }
                if ( !clickedText )
                {
                    if ( caret->IsVisible() ) caret->Show(false);
                    editingValA = -1;
                    editingValB = -1;
                    editingValC = -1;
                }
            }
            break;
        default:
        case ChangingVLineLengthUpper:
        case ChangingVLineLengthLower:
        case EditAxisLeft:
        case EditAxisRight:
        case EditSignal:
        case MovingSignal:
        case ChangingUpperSpace:
        case ChangingLowerSpace:
        case InsertingVLineWaitingSecondPoint:
        case InsertingHArrowWaitingSecondPoint:
        case ChangingHArrowLengthLeft:
        case ChangingHArrowLengthRight:
        case MovingVLine: // correct?
        case MovingHArrow:

            /// should not happen in this states: MouseLeftDown
            SetNeutralState();
            break;
    }
    WindowState = newstate;

    if ( dorefresh ) this->Refresh();
    return;
}
void TimingWindow::SetNeutralState(void)
{
    WindowState = Neutral;
    scrollingleft = false;
    scrollingright = false;
    scrolltimer.Stop();
    editingNumber = -1;
    editingValA = -1;
    editingValB = -1;
    editingValC = -1;
}


void TimingWindow::OnMouseLeftUp(wxMouseEvent &event)
{
    if (!view) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;


    states newstate = WindowState;
    switch ( WindowState )
    {
        default:
        case Neutral:
        case VLineIsMarked:
        case AxisIsMarked:
        case EditingDiscontinuity:
        case InsertingVLineWaitingFirstPoint:
        case InsertingHArrowWaitingFirstPoint:
        case HArrowIsMarked:
        case SignalIsSelected:
        case SelectingText:
            break;
        case EditAxisLeft:
            if ( editingValB > 0 && editingValB != editingValA )
            {
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new ChangeLengthLeft(doc, editingValB) );
            }
            newstate = AxisIsMarked;
            break;
        case EditAxisRight:
            if ( editingValB > 0 && editingValB != editingValA )
            {
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new ChangeLength(doc, editingValB) );
            }
            scrollingleft = false;
            scrollingright = false;
            scrolltimer.Stop();
            newstate = AxisIsMarked;
            break;
        case EditSignal:
            /// Editing the signals values
            if ( editingValB != -1 )
            {
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new ChangeSignal(doc, editingNumber, editingSignal) );

            }
            newstate = Neutral;
            break;
        case MovingSignal:
            /// moving signals horizontally
            if (  editingValA != -1 )
            {
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new MoveSignalPosCommand(
                    doc, editingNumber, editingValA)
                );
            }
            newstate = SignalIsSelected;
            if (editingValA <=  editingNumber )
                editingNumber = editingValA;
            else if ( editingValA > editingNumber + 1)
                editingNumber = editingValA - 1;
            break;
        case ChangingUpperSpace:
            if ( doc->signals[editingNumber].prespace != editingValA )
            {
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new ChangeSpaceCommand(
                    doc, editingNumber, editingValA, true)
                );
            }
            newstate = SignalIsSelected;
            break;
        case ChangingLowerSpace:
            if ( doc->signals[editingNumber].space != editingValA )
            {
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new ChangeSpaceCommand(
                    doc, editingNumber, editingValA, false)
                );
            }
            newstate = SignalIsSelected;
            break;
        case InsertingVLineWaitingSecondPoint:
            if ( editingPoint[0].y != editingPoint[1].y )
            {
                VLine newline;
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                newline.vpos = editingPoint[0].x;
                if ( editingPoint[0].y > editingPoint[1].y )
                {
                    newline.StartPos = editingPoint[1].y;
                    newline.EndPos = editingPoint[0].y-1;
                }
                else
                {
                    newline.StartPos = editingPoint[0].y;
                    newline.EndPos = editingPoint[1].y-1;
                }
                cmdproc->Submit(new AddVLineCommand(
                    doc, newline)
                );
            }
            newstate = InsertingVLineWaitingFirstPoint;
            break;
        case ChangingVLineLengthUpper:
            if ( editingValA != (doc->vertlines[editingNumber].EndPos + 1) ) // must have a minimum distance
            {
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new ChangeVLineCommand(doc, editingNumber,
                    doc->vertlines[editingNumber].vpos,
                    editingValA,
                    doc->vertlines[editingNumber].EndPos)
                );
            }
            newstate = VLineIsMarked;
            break;
        case ChangingVLineLengthLower:
            if ( editingValA != doc->vertlines[editingNumber].StartPos )
            {
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new ChangeVLineCommand(doc, editingNumber,
                    doc->vertlines[editingNumber].vpos,
                    doc->vertlines[editingNumber].StartPos,
                    editingValA-1)
                );
            }
            newstate = VLineIsMarked;
            break;
        case MovingVLine:
            if ( editingValA != doc->vertlines[editingNumber].vpos )
            {
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new ChangeVLineCommand(doc, editingNumber,
                    editingValA,
                    doc->vertlines[editingNumber].StartPos,
                    doc->vertlines[editingNumber].EndPos)
                );
            }
            newstate = VLineIsMarked;
            break;
        case InsertingHArrowWaitingSecondPoint:
            if ( editingValB != -1 )
            {
                HArrow newha;
                newha.fromVLine = editingNumber;
                newha.toVLine = editingValB;
                newha.text = _("t");
                newha.textoffset.x = 5;
                newha.textoffset.y = -10;
                newha.pos = editingValA;

                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new AddHArrowCommand(
                    doc, newha)
                );
            }
            newstate = InsertingHArrowWaitingFirstPoint;
            break;
        case MovingHArrow:
            if ( editingValA != doc->harrows[editingNumber].pos )
            {
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new ChangeHArrowCommand(doc, editingNumber,
                    editingValA , doc->harrows[editingNumber].fromVLine, doc->harrows[editingNumber].toVLine)
                );
            }
            newstate = HArrowIsMarked;
            break;
        case ChangingHArrowLengthLeft:
        case ChangingHArrowLengthRight:
            if ( editingValA != -1 && editingValB != -1 )
            {
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new ChangeHArrowCommand(doc, editingNumber,
                    doc->harrows[editingNumber].pos,
                    editingValA, editingValB)
                );
            }
            newstate = HArrowIsMarked;
            break;
        case HArrowMovingText:
            //if ( editingValA != -1 && editingValB != -1 )
            {
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new ChangeHArrowTextPosCommand(doc, editingNumber,
                    editingValA, // the new x offset
                    editingValB // the new y offset
                ));
            }
            newstate = HArrowIsMarked;
            break;
    }
    WindowState = newstate;
    if ( newstate == Neutral )
        SetNeutralState();
    Refresh();
}

void TimingWindow::OnMouseRightDown(wxMouseEvent& event)
{
    if (!view) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    wxClientDC dc(this);
    PrepareDC(dc);
    dc.SetFont(font);
    wxPoint pt(event.GetLogicalPosition(dc));


    bool dorefresh = false;

    if
    (
        WindowState == EditingDiscontinuity ||
        WindowState == InsertingVLineWaitingFirstPoint ||
        WindowState == InsertingVLineWaitingSecondPoint ||
        WindowState == InsertingHArrowWaitingFirstPoint ||
        WindowState == InsertingHArrowWaitingSecondPoint ||
        WindowState == SelectingText
    )
        SetCursor(*wxCROSS_CURSOR);

    if ( WindowState == TextFieldSelected )
    {
        caret->Show(false);
        WindowState = SelectingText;
        editingValA = -1;
        editingValB = -1;
        editingValC = -1;
        this->Refresh();
        return;
    }

    if ( WindowState != Neutral ) dorefresh = true;
    SetNeutralState();

    editingNumber = -1;
    for ( wxInt32 n = 0 ; n < doc->signals.size() ; ++n )
        if ( pt.x >= manipXoffset - 2*manipRadius && pt.y >= heightOffsets[n]+MinimumSignalDistance/2 + SignalHeight/2 + doc->signals[n].prespace - 2*manipRadius &&
            pt.x <=  manipXoffset + 2*manipRadius && pt.y <= heightOffsets[n]+MinimumSignalDistance/2 + SignalHeight/2 + doc->signals[n].prespace + 2*manipRadius)
        {
            editingNumber = n;
            WindowState = SignalIsSelected;

            wxMenu menu;//(_T("Add/Remove function Menue"));
            if ( doc->signals[n].IsClock )
                menu.Append(TIMING_ID_CHANGECLOCK, _T("Change clock settings"));
            //menu.Append(TIMING_ID_DELETE_SIGNAL, _T("Delet signal"));
            menu.Append(wxID_DELETE, _T("Delet signal"));


            PopupMenu(&menu);
        }

    if ( dorefresh ) this->Refresh();
    return;

}

void TimingWindow::OnMouseMove(wxMouseEvent &event)
{
    if ( !view ) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    wxClientDC dc(this);
    PrepareDC(dc);
    dc.SetFont(font);
    wxPoint pt(event.GetLogicalPosition(dc));

    bool dorefresh = false;

    wxPoint textOffset(0,0);

    states newstate = WindowState;
    switch ( WindowState )
    {
        default:
        case Neutral:
        case AxisIsMarked:
        case EditingDiscontinuity:
        case InsertingVLineWaitingFirstPoint:
        case InsertingHArrowWaitingFirstPoint:
        case SelectingText:
            break;
        case EditAxisLeft:
            scrollingleft = false;
            scrollingright = false;
            scrolltimer.Stop();
            if ( event.LeftIsDown() )
            {
                wxCoord p = pt.x - axisStart;
                if ( pt.x > axisStop )
                    editingValB = editingValA;
                else
                    editingValB = editingValA - p / (transitionWidth * transitionsPerGridStep);
            }
            else
                newstate = AxisIsMarked;
            break;
        /// the user is changing the length of the time axis
        case EditAxisRight:
            scrollingleft = false;
            scrollingright = false;
            scrolltimer.Stop();
            if ( event.LeftIsDown()  )
            {
                wxCoord p = pt.x-axisStart;
                if ( pt.x < axisStart )
                    editingValB = editingValA;
                else
                    editingValB = p / (transitionWidth * transitionsPerGridStep);
                wxSize clientSize;
                clientSize = this->GetClientSize();
                wxCoord startx, starty;
                this->GetViewStart(&startx, &starty);
                wxPoint pos(event.GetPosition());
                if ( pos.x <=5 )
                {
                    wxCoord dx, dy;//, desc;
                    dx = dc.GetCharWidth();
                    dy = dc.GetCharHeight();
                    Scroll(startx-dx/4, starty);
                    scrollingleft = true;
                    scrolltimer.Start(400, wxTIMER_ONE_SHOT);
                }
                else if ( (pos.x - clientSize.x) >=-5 )
                {
                    wxCoord dx, dy;//, desc;
                    dx = dc.GetCharWidth();
                    dy = dc.GetCharHeight();
                    Scroll(startx+dx/4, starty);
                    scrollingright = true;
                    scrolltimer.Start(400, wxTIMER_ONE_SHOT);
                }
            }
            else
                newstate = AxisIsMarked;
            break;
        case EditSignal:
            if ( event.LeftIsDown() )
            {
                wxInt32 editingSignalPosStart = editingValA;
                wxInt32 editingSignalPosStop = editingValB;
                if ( pt.y >= heightOffsets[editingNumber] + doc->signals[editingNumber].prespace && pt.x >= axisStart &&
                    pt.y <= heightOffsets[editingNumber] + doc->signals[editingNumber].prespace + SignalHeight && pt.x <= axisStop )
                {
                    wxCoord p = pt.x - axisStart;
                    editingSignalPosStop = p / (transitionWidth * transitionsPerGridStep);
                    vals val = doc->signals[editingNumber].values[editingSignalPosStart];
                    vals nval;
                    if ( true /*!editingSignal.IsBus */ )
                    {
                        switch ( val )
                        {
                            case zero: nval = one; break;
                            case one: nval = hz; break;
                            case hz: nval = u; break;
                            default:
                            case u: nval = zero; break;
                        }

                        editingSignal.values = doc->signals[editingNumber].values;
                        if ( editingSignalPosStop >= editingSignalPosStart )
                            for ( wxInt32 n = editingSignalPosStart ; n <= editingSignalPosStop ; ++n )
                                editingSignal.values[n] = nval;
                        else
                            for ( wxInt32 n = editingSignalPosStart ; n >= editingSignalPosStop ; --n )
                                editingSignal.values[n] = nval;
                    }
                }
                else
                    editingSignalPosStop = -1;
                editingValA = editingSignalPosStart;
                editingValB = editingSignalPosStop;
            }
            break;
        case SignalIsSelected:
            if ( event.LeftIsDown() )
                newstate = MovingSignal;
            break;
        case MovingSignal:
            /// moving signal
            if ( event.LeftIsDown() )
            {
                wxInt32 n;
                for ( n = 0 ; n < doc->signals.size() ; ++n)
                    if ( pt.y < (heightOffsets[n] + heightOffsets[n+1])/2 )
                        break;

                if ( editingValA != n )
                    dorefresh = true;
                editingValA = n;
            }
            break;
        case ChangingUpperSpace:
            if ( event.LeftIsDown() )
            {
                wxCoord p = pt.y;
                if ( p >= ( heightOffsets[editingNumber] + doc->signals[editingNumber].prespace ) )
                    editingValA = 0;
                else
                    editingValA = ( heightOffsets[editingNumber] + doc->signals[editingNumber].prespace) - p;
            }
            break;
        case ChangingLowerSpace:
            if ( event.LeftIsDown() )
            {
                wxCoord p = pt.y;
                if ( p <= (heightOffsets[editingNumber] + doc->signals[editingNumber].prespace +
                                SignalHeight + MinimumSignalDistance ) )
                        editingValA = 0;
                    else
                        editingValA = p - (heightOffsets[editingNumber] + doc->signals[editingNumber].prespace +
                                SignalHeight + MinimumSignalDistance );
            }
            break;
        case InsertingVLineWaitingSecondPoint:
            {
                wxInt32 n = doc->signals.size();
                if ( n //&&  some signals in the document
                    //pt.x > axisStart &&
                    //pt.x < axisStop /*&&
                    //pt.y > heightOffsets[0] &&
                    //pt.y < heightOffsets[n]
                )
                {
                    wxCoord p = pt.x-axisStart - (transitionWidth * transitionsPerGridStep)/2;
                    editingPoint[1].x = p / (transitionWidth * transitionsPerGridStep);

                    wxInt32 k;
                    for ( k = 0 ; k < n ; ++k )
                        if ( pt.y < (heightOffsets[k] + heightOffsets[k+1])/2 )
                            break;

                    if ( editingPoint[1].y != k )
                        dorefresh = true;
                    editingPoint[1].y = k;
                }
            }
            break;
        case ChangingVLineLengthUpper:
        case ChangingVLineLengthLower:
            {
                wxInt32 n = doc->signals.size();
                if ( n && // some signals in the document
                    pt.x > axisStart &&
                    pt.x < axisStop )
                {
                    wxCoord p = pt.x-axisStart + (transitionWidth * transitionsPerGridStep)/2;
                    editingPoint[1].x = p / (transitionWidth * transitionsPerGridStep);

                    wxInt32 kmax = n, kmin = 0;
                    // for every harrow in the doc check
                    for ( wxInt32 j = 0 ; j < doc->harrows.size() ; ++j )
                    {
                        // if it is connected to the changing vline
                        if ( editingNumber == doc->harrows[j].fromVLine ||
                             editingNumber == doc->harrows[j].toVLine )
                        {
                            wxInt32 off =
                            //heightOffsets[
                            //    doc->vertlines[doc->harrows[j].fromVLine].StartPos
                            //] +
                            DistanceToTimeline + DistanceFromTimeline +
                            doc->harrows[j].pos;

                            wxInt32 k;
                            for ( k = 0 ; k < n ; ++k )
                            {
                                if ( off >= heightOffsets[k] &&
                                     off <  heightOffsets[k+1] )
                                {
                                    if ( kmax > k ) kmax = k;
                                    if ( kmin < k+1 ) kmin = k+1;
                                }
                            }
                        }
                    }

                    wxInt32 k;
                    for ( k = 0 ; k < n ; ++k )
                        if ( pt.y < (heightOffsets[k] + heightOffsets[k+1])/2 )
                            break;

                    bool AllowedMove = true;
                    if ( WindowState == ChangingVLineLengthUpper )
                    {
                        if ( k > kmax ) AllowedMove = false;
                    }
                    else
                    {
                        if ( k < kmin ) AllowedMove = false;
                    }

                    if ( editingValA != k && AllowedMove )
                    {
                        dorefresh = true;
                        editingValA = k;
                    }
                }
            }
            break;
        case VLineIsMarked:
            if ( event.LeftIsDown() )
                newstate = MovingVLine;
        case MovingVLine:
            if ( event.LeftIsDown() )
            {
                wxCoord p = pt.x-axisStart + (transitionWidth * transitionsPerGridStep)/2;
                if ( pt.x < axisStart || pt.x > axisStop )
                    editingValA = doc->vertlines[editingNumber].vpos;
                else
                    editingValA = p / (transitionWidth * transitionsPerGridStep);
            }
            break;
        case HArrowIsMarked:
            if ( event.LeftIsDown() )
                newstate = MovingHArrow;
        case MovingHArrow:
            if ( event.LeftIsDown() )
            {
                wxCoord top, bottom, tmp;
                top = heightOffsets[doc->vertlines[doc->harrows[editingNumber].fromVLine].StartPos];
                tmp = heightOffsets[doc->vertlines[doc->harrows[editingNumber].toVLine].StartPos];
                if ( tmp > top ) top = tmp;
                bottom = heightOffsets[doc->vertlines[doc->harrows[editingNumber].fromVLine].EndPos+1];
                tmp = heightOffsets[doc->vertlines[doc->harrows[editingNumber].toVLine].EndPos+1];
                if ( tmp < bottom ) bottom = tmp;

                if ( pt.y < top )
                {
                    editingValA = top - DistanceToTimeline - DistanceFromTimeline;
                }
                else if ( pt.y > bottom )
                {
                    editingValA = bottom - DistanceToTimeline - DistanceFromTimeline;
                }
                else
                {
                    editingValA = pt.y - DistanceToTimeline - DistanceFromTimeline;
                }
            }
            break;
        case InsertingHArrowWaitingSecondPoint:
            /// check if user mover over a vertical line
            for ( wxInt32 k = 0 ; k < VLineOffsets.size() ; ++k )
            {
                wxInt32 pos = DistanceToTimeline + DistanceFromTimeline + editingValA;
                if (pt.x >= VLineOffsets[k].x - 3 &&
                    pt.x <= VLineOffsets[k].x + 3 &&
                    pt.y >= VLineOffsets[k].y &&
                    pt.y <= VLineToOffset[k] &&
                    pos >= VLineOffsets[k].y &&
                    pos <= VLineToOffset[k] )
                {
                    //newstate = InsertingHArrowWaitingSecondPoint;
                    //editingNumber = k;
                    //editingValA = pt.y - DistanceToTimeline - DistanceFromTimeline;
                    editingValB = k;

                    break;
                }
                else
                    editingValB = -1;
            }
            dorefresh = true;
            break;

        case ChangingHArrowLengthLeft:
        case ChangingHArrowLengthRight:
            for ( wxInt32 k = 0 ; k < VLineOffsets.size() ; ++k )
            {
                wxInt32 pos = doc->harrows[editingNumber].pos + DistanceToTimeline + DistanceFromTimeline;
                if (pt.x >= VLineOffsets[k].x - 3 &&
                    pt.x <= VLineOffsets[k].x + 3 &&
                    pt.y >= VLineOffsets[k].y &&
                    pt.y <= VLineToOffset[k] &&
                    pos >= VLineOffsets[k].y &&
                    pos <= VLineToOffset[k] )
                {
                    if ( WindowState == ChangingHArrowLengthLeft )
                        editingValA = k;
                    else
                        editingValB = k;
                    break;
                }
                else
                    if ( WindowState == ChangingHArrowLengthLeft )
                        editingValA = -1;
                    else
                        editingValB = -1;
            }
            dorefresh = true;
            break;
        case HArrowMovingText:
            if ( event.LeftIsDown() )
            {
                editingValA = pt.x - (HArrowOffsets[editingNumber].x + HArrowToOffset[editingNumber])/2;
                editingValB = pt.y - HArrowOffsets[editingNumber].y;
            }
            break;
        case TextFieldSelected:
            if ( event.LeftIsDown() )
            {
                wxString text = editingText;
                /// calc width of the texts
                wxArrayInt widths;
                dc.GetPartialTextExtents(text, widths);
                widths.Insert(0, 0);
                if ( editingValB == -1 )
                    editingValB = editingValA;

                ///get position of cursor in the text and move the cursor to this place
                wxInt32 &k = editingValA;
                for ( k = 0 ; k < widths.GetCount() - 1 ; ++k )
                    if ( pt.x <= textOffsets[editingNumber].x + (widths[k] + widths[k+1] )/2 )
                        break;
                editingValC = editingValA;
                dorefresh = true;
            }
            break;
    }
    WindowState = newstate;

    if ( pt != cursorpos )
    {
        cursorpos = pt;
        dorefresh = true;
    }

    if ( dorefresh ) this->Refresh();
    return;
}

void TimingWindow::OnMouseWheel(wxMouseEvent& event)
{
    if (!view)
        return;

    wxClientDC dc(this);
    PrepareDC(dc);

    //wxFont font(fontsize, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);//, const bool underline = false, const wxString& faceName = _T(""), wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    //dc.SetFont(font);
    dc.SetFont(font);
    //dc.SetPen(*wxBLACK_PEN);
    wxCoord dx, dy;//, desc;
    //dc.GetTextExtent(_T("H"), &dx, &dy, &desc);
    dx = dc.GetCharWidth();
    dy = dc.GetCharHeight();

    if ( ! event.IsPageScroll() )
    {
        wxInt32 nWheelRotation = event.GetWheelRotation();
        wxInt32 x, y;
        GetViewStart(&x, &y);
        if (nWheelRotation < 0)
            y += dy/4;
        else
            y -= dy/4;
        Scroll(x, y);
    }
}
/// //////////////unused event methods//////////////////////////////////////////
void TimingWindow::OnMouseRightUp(wxMouseEvent& event){}
//void MyCanvas::OnSetFocus(wxFocusEvent &event){}
//void MyCanvas::OnKillFocus(wxFocusEvent &event){}
/// ////////////////////////////////////////////////////////////////////////////

void TimingWindow::OnChar(wxKeyEvent &event)
{
    if ( !view ) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    switch ( WindowState )
    {
        case EditingDiscontinuity:
        case InsertingVLineWaitingSecondPoint:
        case InsertingVLineWaitingFirstPoint:
        case InsertingHArrowWaitingFirstPoint:
        case InsertingHArrowWaitingSecondPoint:
            if ( event.GetKeyCode() == WXK_ESCAPE )
                SetCursor(*wxCROSS_CURSOR);
        default:
        case Neutral:
        case VLineIsMarked:
        case ChangingVLineLengthLower:
        case ChangingVLineLengthUpper:
        case AxisIsMarked:
        case EditAxisLeft:
        case EditAxisRight:
        case EditSignal:
        case MovingSignal:
        case ChangingUpperSpace:
        case ChangingLowerSpace:
        case HArrowIsMarked:
        case ChangingHArrowLengthLeft:
        case ChangingHArrowLengthRight:
        case HArrowMovingText:
        case MovingHArrow:
        case MovingVLine:
            if ( event.GetKeyCode() == WXK_ESCAPE )
            {
                SetNeutralState();
                Refresh();
            }
            break;
        case SignalIsSelected:
            {
                wxInt32 n = doc->signals.size();
                switch (event.GetKeyCode())
                {
                    case WXK_HOME:
                        if (editingNumber != 0 )
                        {
                            editingNumber = 0;
                            this->Refresh();
                        }
                        break;
                    case WXK_END:
                        if ( editingNumber != n-1 )
                        {
                            editingNumber = n-1;
                            this->Refresh();
                        }
                        break;
                    case WXK_UP:
                        if ( editingNumber != 0)
                        {
                            --editingNumber;
                            this->Refresh();
                        }
                        break;
                    case WXK_DOWN:
                        if ( editingNumber < n-1 )
                        {
                            ++editingNumber;
                            this->Refresh();
                        }
                        break;
                    case WXK_ESCAPE:
                        WindowState = Neutral;
                        scrollingleft = false;
                        scrollingright = false;
                        scrolltimer.Stop();
                        editingValA = -1;
                        editingValB = -1;
                        this->Refresh();
                        break;
                    default:
                        break;
                }
            }
            break;
        case SelectingText:
            if ( event.GetKeyCode() == WXK_ESCAPE )
            {
                SetCursor(*wxCROSS_CURSOR);
                SetNeutralState();
                Refresh();
            }
            break;
        case TextFieldSelected:
            {
                //MyCommandProcessor *cmdproc = (MyCommandProcessor *)doc->GetCommandProcessor();
                wxString ch;
                switch (event.GetKeyCode())
                {
                    case WXK_TAB:
                    {
                        ch = _T("    ");
                        break;
                    }
                    case WXK_BACK:
                    {
                        if ( editingValB == -1 || editingValC == -1 )
                        {
                            if ( editingValA > 0 )
                            {
                                //wxString chtd = text.Mid(editingValA-1, 1);
                                editingText = editingText.Mid(0, editingValA-1) + editingText.Mid(editingValA);
                                editingValA--;
                            }
                        }
                        else
                        {
                            if ( editingValB < editingValC )
                            {
                                editingText = editingText.Mid(0, editingValB) + editingText.Mid(editingValC);
                                editingValA = editingValB;
                            }
                            else
                            {
                                editingText = editingText.Mid(0, editingValC) + editingText.Mid(editingValB);
                                editingValA = editingValC;
                            }
                            editingValB = -1;
                            editingValC = -1;
                        }
                        Refresh();
                        return;
                    }
                    case WXK_HOME:
                    {
                        if ( event.ShiftDown() && editingValA != 0 )
                        {
                            if ( editingValB == -1 || editingValC == -1)
                            {
                                editingValB = editingValA;
                                editingValC = 0;
                            }
                            else
                            {
                                editingValC = 0;
                            }
                        }
                        else
                        {
                            editingValB = -1;
                            editingValC = -1;
                        }
                        editingValA = 0;
                        this->Refresh();
                        return;
                    }
                    case WXK_END:
                    {
                        if ( event.ShiftDown() && editingValA < editingText.Length() )
                        {
                            if ( editingValB == -1 || editingValC == -1 )
                            {
                                editingValB = editingValA;
                                editingValC = editingText.Length();
                            }
                            else
                            {
                                editingValC = editingText.Length();
                            }
                        }
                        else
                        {
                            editingValB = -1;
                            editingValC = -1;
                        }
                        editingValA = editingText.Length();
                        Refresh();
                        return;
                    }
                    case WXK_LEFT:
                    {
                        if ( editingValA > 0 )
                        {
                            if ( event.ShiftDown() )
                            {
                                if ( editingValB == -1 || editingValC == -1)
                                {
                                    editingValB = editingValA;
                                    editingValC = editingValA-1;
                                }
                                else
                                {
                                    editingValC--;
                                }
                            }
                            else
                            {
                                editingValB = -1;
                                editingValC = -1;
                            }
                            editingValA--;
                            Refresh();
                        }
                        return;
                    }
                    case WXK_RIGHT:
                    {
                        if ( editingValA < editingText.Length() )
                        {
                            if ( event.ShiftDown() )
                            {
                                if ( editingValB == -1 || editingValC == -1 )
                                {
                                    editingValB = editingValA;
                                    editingValC = editingValA+1;
                                }
                                else
                                {
                                    editingValC++;
                                }
                            }
                            else
                            {
                                editingValB = -1;
                                editingValC = -1;
                            }
                            editingValA++;
                            Refresh();
                        }
                        return;
                    }
                    case WXK_UP: return;
                    case WXK_DOWN: return;
                    case WXK_RETURN:
                    case WXK_NUMPAD_ENTER:
                    {
                        /// generate the command to change the text
                        // TODO (daniel#1#): command to change text
                        wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                        cmdproc->Submit(
                            new ChangeTextCommand(doc, editingNumber, editingText)
                        );
                        editingNumber = -1;
                        editingValA = -1;
                        editingValB = -1;
                        editingValC = -1;
                        caret->Show(false);
                        WindowState = SelectingText;
                        return;
                    }
                    case WXK_ESCAPE:
                    {
                        editingValA = -1;
                        editingValB = -1;
                        editingValC = -1;
                        if ( caret->IsVisible() ) caret->Hide();
                        //EditMode = false;
                        editingNumber = -1;
                        /// /////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        WindowState = SelectingText;
                        Refresh();
                        return;
                    }
                    default:
                    {
                        if ( !event.AltDown() && wxIsprint(event.GetKeyCode()) )
                        {
                            ch = wxString( (wxChar)event.GetKeyCode() );
                            break;
                        }
                        else
                        {
                            event.Skip();
                            return;
                        }
                    }
                }
                if ( editingValB == -1 || editingValC == -1 )
                {
                    editingText = editingText.Mid(0, editingValA) + ch + editingText.Mid(editingValA);
                    editingValA += ch.Length();
                }
                else
                {
                    if ( editingValB < editingValC )
                    {
                        ch = editingText.Mid( 0, editingValB ) + ch;
                        editingText = ch + editingText.Mid(editingValC);
                    }
                    else
                    {
                        ch = editingText.Mid(0, editingValC) + ch;
                        editingText = ch + editingText.Mid(editingValB);
                    }
                    editingValA = ch.Length();
                }
                editingValB = -1;
                editingValC = -1;
                this->Refresh();
            }
            break;
        //case ...
    }
}

void TimingWindow::OnKeyDown(wxKeyEvent &event)
{
    ///special key-handling here
    /// most of the time EVT_CHAR will call OnChar, thanks to Skip()

    //if ( WindowState == MovingVLine )
    //    if ( event.GetModifiers() == wxMOD_CONTROL )

    if (!view) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

//    if ( WindowState == TextFieldSelected )
//    {
//        int ch = event.GetKeyCode();
//        if ( ch == WXK_RETURN ||
//             ch == WXK_NUMPAD_ENTER )
//        {
//            /// generate the command to change the text
//            // TODO (daniel#1#): command to change text
//            wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
//            cmdproc->Submit(
//                new ChangeTextCommand(doc, editingNumber, editingText)
//            );
//            editingNumber = -1;
//            editingValA = -1;
//            editingValB = -1;
//            editingValC = -1;
//            WindowState = SelectingText;
//            return;
//        }
//    }
    event.Skip();
}

void TimingWindow::OnSelectInsertDiscontTool(void)
{
    if (!view) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    wxBitmap cursBitmap(tri_cur_xpm);
    wxImage cursImage = cursBitmap.ConvertToImage();
    cursImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 5);
    cursImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
    wxCursor newcursor(cursImage);
    //#ifdef __WXMSW__
    //#else
    //#endif

    SetCursor(newcursor);

    WindowState = EditingDiscontinuity;
    editingNumber = -1;
    scrollingleft = false;
    scrollingright = false;
    scrolltimer.Stop();

    this->Refresh();
}

void TimingWindow::OnSelectRulerTool(void)
{
    if (!view) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    wxBitmap cursBitmap(ruler_cur_xpm);
    wxImage cursImage = cursBitmap.ConvertToImage();
    cursImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 8);
    cursImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 2);
    wxCursor newcursor(cursImage);
    //#ifdef __WXMSW__
    //#else
    //#endif

    SetCursor(newcursor);

    WindowState = InsertingVLineWaitingFirstPoint;
    editingNumber = -1;
    scrollingleft = false;
    scrollingright = false;
    scrolltimer.Stop();

    this->Refresh();
}
void TimingWindow::OnSelectHArrowTool(void)
{
    if (!view) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    wxBitmap cursBitmap(harrow_cur_xpm);
    wxImage cursImage = cursBitmap.ConvertToImage();
    cursImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 7);
    cursImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 6);
    wxCursor newcursor(cursImage);
    //#ifdef __WXMSW__
    //#else
    //#endif

    SetCursor(newcursor);

    WindowState = InsertingHArrowWaitingFirstPoint;
    editingNumber = -1;
    scrollingleft = false;
    scrollingright = false;
    scrolltimer.Stop();

    this->Refresh();
}
void TimingWindow::OnSelectTextTool(void)
{
    if (!view) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    wxBitmap cursBitmap(textedit_cur_xpm);
    wxImage cursImage = cursBitmap.ConvertToImage();
    cursImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 7);
    cursImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 6);
    wxCursor newcursor(cursImage);
    //#ifdef __WXMSW__
    //#else
    //#endif

    SetCursor(newcursor);

    WindowState = SelectingText;
    editingNumber = -1;
    scrollingleft = false;
    scrollingright = false;
    scrolltimer.Stop();

    this->Refresh();
}
void TimingWindow::OnSelectNeutralTool(void)
{
    if (!view) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;


    switch ( WindowState )
    {
        case EditingDiscontinuity:
        case InsertingVLineWaitingSecondPoint:
        case InsertingVLineWaitingFirstPoint:
        case InsertingHArrowWaitingFirstPoint:
        case InsertingHArrowWaitingSecondPoint:
        case SelectingText:
        case TextFieldSelected:
            SetCursor(*wxCROSS_CURSOR);
            SetNeutralState();
            editingNumber = -1;
            scrollingleft = false;
            scrollingright = false;
            scrolltimer.Stop();
            break;
        default:
        case Neutral:
        case VLineIsMarked:
        case ChangingVLineLengthLower:
        case ChangingVLineLengthUpper:
        case AxisIsMarked:
        case EditAxisLeft:
        case EditAxisRight:
        case EditSignal:
        case MovingSignal:
        case ChangingUpperSpace:
        case ChangingLowerSpace:
        case HArrowIsMarked:
        case ChangingHArrowLengthLeft:
        case ChangingHArrowLengthRight:
        case HArrowMovingText:
        case MovingHArrow:
        case MovingVLine:
        case SignalIsSelected:
            break;
    }

    Refresh();

}

/*void TimingWindow::InsertDroppedText(wxString str)
{
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    wxString text = *droppedStringPtr;
    doc->GetCommandProcessor()->Submit(
        new ChangeText(doc, droppedStringPtr, text.Mid(0, droppos) + str + text.Mid(droppos) )
    );
    strpos = droppos + str.Length();
    selpos[0] = droppos;
    selpos[1] = strpos;
    //selnmbr = dropnmbr;
    currentlyEditedTextPtr = droppedStringPtr;
    droppedStringPtr = NULL;
    droppos = -1;
    this->Refresh();
}*/

void TimingWindow::InsertText(wxString str)
{
    if ( WindowState == TextFieldSelected && editingValA != -1 )
    {
        if ( editingValB == -1 || editingValC == -1 )
        {
            editingText = editingText.Mid(0, editingValA) + str + editingText.Mid(editingValA);
            editingValA += str.Length();
        }
        else
        {
            if ( editingValB < editingValC )
            {
                editingText = editingText.Mid(0, editingValB) + str + editingText.Mid(editingValC);
                editingValA = editingValB;
            }
            else
            {
                editingText = editingText.Mid(0, editingValC) + str + editingText.Mid(editingValB);
                editingValA = editingValC;
            }
            editingValA += str.Length();
            editingValB = -1;
            editingValC = -1;
        }
        Refresh();
    }
}
wxString TimingWindow::GetText(void)
{
    if ( !view )
        return wxString(_(""));
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return wxString(_(""));


    if ( ! IsTextSelected() ) return wxString(_(""));

    wxString text = editingText, str;

    if ( editingValB < editingValC )
        str = text.Mid(editingValB, editingValC - editingValB);
    else
        str = text.Mid(editingValC, editingValB - editingValC);
    return str;
}
void TimingWindow::DeleteText(void)
{
    if ( !view ) return;

    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    if ( WindowState != TextFieldSelected) return;

    if ( IsTextSelected() )
    {
        if ( editingValB < editingValC )
        {
            editingText = editingText.Mid(0, editingValB) + editingText.Mid(editingValC);
            editingValA = editingValB;
        }
        else
        {
            editingText = editingText.Mid(0, editingValC) + editingText.Mid(editingValB);
            editingValA = editingValC;
        }

        editingValB = editingValC = -1;
    }
    else
        editingText = editingText.Mid(0, editingValA) + editingText.Mid(editingValA+1);

    this->Refresh();
}
void TimingWindow::SelectAll(void)
{
    if ( WindowState != TextFieldSelected ) return;

    editingValB = 0;
    editingValC = editingText.Length();
    this->Refresh();
}
bool TimingWindow::CanDeleteText(void)
{
    if (!view) return false;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return false;

    if ( IsTextSelected() ) return true;

    if ( WindowState == TextFieldSelected  &&
            editingValA < editingText.Len() )
        return true;

    return false;
}
void TimingWindow::DeleteSignal(void)
{
    if (!view) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    if ( WindowState == SignalIsSelected )
    {
        wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
        cmdproc->Submit(
            new DeleteSignalCommand(doc, editingNumber)
        );
        editingNumber = -1;
        SetNeutralState();
        Refresh();
    }
}
void TimingWindow::DeleteVLine(void)
{
    if (!view) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    if ( WindowState == VLineIsMarked )
    {
        wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
        cmdproc->Submit(
            new DeleteVLineCommand(doc, editingNumber)
        );
        editingNumber = -1;
        SetNeutralState();
        Refresh();
    }
}
void TimingWindow::DeleteHArrow(void)
{
    if (!view) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    if ( WindowState == HArrowIsMarked )
    {
        wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
        cmdproc->Submit(
            new DeleteHArrowCommand(doc, editingNumber)
        );
        editingNumber = -1;
        editingValA = -1;
        editingValB = -1;
        SetNeutralState();
        Refresh();
    }
}
void TimingWindow::DeleteSelection(void)
{
    if ( !view ) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;


    switch ( WindowState )
    {
        case SignalIsSelected:
            DeleteSignal();
            break;
        case VLineIsMarked:
            DeleteVLine();
            break;
        case HArrowIsMarked:
            DeleteHArrow();
            break;
        case TextFieldSelected:
            DeleteText();
            break;
        default:
            break;
    }
}

void TimingWindow::OnDragEnter(void)
{
    /*drop = true;
    dropcaret->Show();
    Refresh();*/
}
void TimingWindow::OnDragLeave(void)
{
    /*drop = false;
    dropcaret->Hide();
    Refresh();*/
}
wxDragResult TimingWindow::OnDragOver(wxPoint p, wxDragResult def)
{
    if ( !view )
        return (wxDragNone);
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc )
        return (wxDragNone);
/*
    wxClientDC dc(this);
    PrepareDC(dc);
    wxCoord xx, yy;
    CalcUnscrolledPosition( p.x, p.y, &xx, &yy);
    dc.SetFont(font);
    wxCoord dx, dy;
    //dc.GetTextExtent(_T("H"), &dx, &dy);
    dx = dc.GetCharWidth();
    dy = dc.GetCharHeight();

    wxPoint pt(xx, yy);

    if ( dndsource && def == wxDragCopy )
        dropcopy = true;
    else
        dropcopy = false;

/// ////////////////////////////////////////////////////////////////////////////
    /// cursor over text area?
    //bool overText = false;

    for (unsigned int n = 0 ; n < textSizes.size() ; ++n )
    {
        wxPoint offset( textOffsets[n] );
        wxPoint size ( textSizes[n] );
        if( pt.x > offset.x &&
            pt.x < offset.x + size.x &&
            pt.y > offset.y &&
            pt.y < offset.y + size.y
            )
        {
            //textOffset = offset;
            droppedStringPtr = textStringPtrs[n];
            /// calc with of the texts
            wxArrayInt widths;
            wxString text = *droppedStringPtr;
            dc.SetFont(font); /// change to font of selected text
            dc.GetPartialTextExtents(text, widths);
            widths.Insert(0, 0);

            /// get position of cursor in the text
            wxInt32 n;
            for ( n = 0 ; n < widths.GetCount()-1; n++ )
                if ( pt.x <= 10 + (widths[n] + widths[n+1] )/2 ) break;
            /// set size of dropcaret mathing to text
            ///move the cursor to this place
            if ( !dropcaret->IsVisible() ) dropcaret->Show();
            dropcaret->SetSize(1, dc.GetCharHeight() );
            droppos = n;
            this->Refresh();
            return def;
        }
        else
        {
            droppos = -1;
            droppedStringPtr = NULL;
            if ( dropcaret->IsVisible() ) dropcaret->Hide();
        }
    }
    /// check that a wxCaret is shown where needed and refresh
    this->Refresh();
*/
/// ////////////////////////////////////////////////////////////////////////////
    return(wxDragNone);
}
bool TimingWindow::OnDrop(wxPoint pt, wxString str )
{
    if (!view)
        return( false );

    /*dropcaret->Hide();
    if ( dndsource )
    {
        DroppedText = str;
    }
    else
    {
        drop = false;
        InsertDroppedText(str);
    }
*/
    Refresh();
    /// /////////////////////////////////////////////////////////////////////////////////////////////////
    return true;
}
void TimingWindow::OnTimer(wxTimerEvent& event)
{
    wxClientDC dc(this);
    PrepareDC(dc);

    if ( scrollingleft || scrollingright )
    {
        wxCoord startx, starty;
        this->GetViewStart(&startx, &starty);
        wxCoord dx;//, desc;
        dx = dc.GetCharWidth();
        if (scrollingleft)
            Scroll(startx-dx, starty);
        else
        {
            Scroll(startx+dx, starty);
            cursorpos.x += dx;
        }

        scrolltimer.Start(200,wxTIMER_ONE_SHOT);


        /*wxCoord p = cursorpos.x-axisStart+2;
        if ( cursorpos.x < axisStart )
            newLength = oldlength;
        else
            newLength = p / (transitionWidth * transitionsPerGridStep);
        */


        this->Refresh();
    }
    else
        scrolltimer.Stop();

}

bool TimingWindow::CanZoomTicksOut(void)
{
    if ( transitionWidth > 2 || transitionsPerGridStep > 3)
        return true;
    return false;
}
bool TimingWindow::CanZoomTicksIn(void)
{
    if ( transitionWidth < 40 )
        return true;
    return false;
}

void TimingWindow::ZoomTicksOut(void)
{
    if ( transitionWidth > 2 )
        --transitionWidth;
    else
    {
        transitionWidth = 2;
        if ( transitionsPerGridStep > 3)
        {
            --transitionsPerGridStep;
        }
    }
    Refresh();
}
void TimingWindow::ZoomTicksIn(void)
{
    if ( transitionsPerGridStep < 5 )
    {
        ++transitionsPerGridStep;
    }
    else
    {
        if ( transitionWidth < 40 )
            ++transitionWidth;
        else
            transitionWidth = 40;
    }
    Refresh();
}

bool TimingWindow::IsSignalSelected(void)
{
    if ( WindowState == SignalIsSelected )
        return true;
    return false;
}
bool TimingWindow::VLineIsSelected(void)
{
    if ( WindowState == VLineIsMarked )
        return true;
    return false;
}
bool TimingWindow::HArrowIsSelected(void)
{
    if ( WindowState == HArrowIsMarked )
        return true;
    return false;
}





































