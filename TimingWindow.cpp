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

#include "ClockSettingsPanel.h"
#include "TransitionSettingsPanel.h"
#include "AxisSettingsPanel.h"
#include "TimeCompressorSettingsPanel.h"

BEGIN_EVENT_TABLE(TimingWindow, wxScrolledWindow)
    EVT_PAINT           (TimingWindow::OnPaint)
    EVT_ERASE_BACKGROUND(TimingWindow::OnEraseBackground)
    EVT_LEFT_DOWN       (TimingWindow::OnMouseLeftDown)
    EVT_LEFT_DCLICK     (TimingWindow::OnMouseLeftDClick)
    EVT_LEFT_UP         (TimingWindow::OnMouseLeftUp)
    EVT_RIGHT_DOWN      (TimingWindow::OnMouseRightDown)
    EVT_RIGHT_UP        (TimingWindow::OnMouseRightUp)
    EVT_MOTION          (TimingWindow::OnMouseMove)
    EVT_MOUSEWHEEL      (TimingWindow::OnMouseWheel)
    EVT_CHAR            (TimingWindow::OnChar)
    //EVT_KEY_DOWN        (TimingWindow::OnKeyDown)
    EVT_LEAVE_WINDOW    (TimingWindow::OnLeaveWindow)
    EVT_ENTER_WINDOW    (TimingWindow::OnEnterWindow)
    EVT_TIMER           (-1, TimingWindow::OnTimer)
    EVT_SCROLLWIN       (TimingWindow::OnScroll)
    EVT_SIZE            (TimingWindow::OnSize)
END_EVENT_TABLE()


TimingWindow::TimingWindow(wxView *v, wxWindow *parent,
    ClockSettingsPanel *clkpanel,
    TransitionSettingsPanel *trnpanel,
    AxisSettingsPanel *axspanel,
    TimeCompressorSettingsPanel *tcpanel
    )
:wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxWANTS_CHARS),
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
    scrollingup(false),
    scrollingdown(false),
    scrolltimer(this),
    //transitionWidth(3),
    //transitionsPerGridStep(5),
    GridStepWidth(15),

    editingNumber(-1),

    editingTextIsVisible(false),
    WindowState(Neutral),

    ClkSetPanel(clkpanel),
    TranSetPanel(trnpanel),
    AxisSetPanel(axspanel),
    TmeCmprssrPanel(tcpanel),


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
    Refresh(true);
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

const int DistanceToTimeline    = 25;
const int DistanceFromTimeline  = 5;
const int manipXoffset          = 20;
const int manipRadius           = 4;

const int DistanceToAxis        = 25;
const int DistanceFromAxis      = 20;


wxPoint TimingWindow::GetBitmapSize()
{
    return wxPoint(
        axisStop-40,
        heightOffsets[heightOffsets.size()-1] - DistanceToTimeline - DistanceFromTimeline
    );
}
void TimingWindow::InitTextDrawing()
{
    textOffsets.clear();
    textSizes.clear();
    texts.clear();
    textNumber = 0;
    editingTextIsVisible = false;
    textvisible.clear();
}

wxPoint TimingWindow::DrawEditableText(wxDC &dc, wxString str, wxPoint &offset, bool visible = true)
{
    bool empty = false;
    texts.push_back(str);
    textvisible.push_back(visible);
    wxCoord w, h;//, desc;
    if ( WindowState == TextFieldSelected  &&  editingNumber == textNumber )
    {
        str = editingText;
    }
    else
    {
        if ( str.length() == 0 )
        {
            empty = true;
            str = _T(" ");
            dc.GetTextExtent(str, &w, &h);
            offset.x -= w/2;
        }
    }
    if ( visible ) dc.DrawText(str, offset.x, offset.y);
    textOffsets.push_back( offset );
    dc.GetTextExtent(str, &w, &h);
    wxPoint size(w, h);
    textSizes.push_back( size );

    /// chang position of caret
    if ( WindowState == TextFieldSelected && editingNumber == textNumber )
    {
        if ( visible )
        {
            editingTextIsVisible = true;
            wxCoord w, h;//, desc;

            if ( editingValA < 0 || (wxUint32)editingValA > str.Length() )
                 editingValA = str.Length();
            dc.GetTextExtent(str.Mid(0, editingValA), &w, &h);
            wxCoord xx, yy;
            CalcScrolledPosition( offset.x+w, offset.y, &xx, &yy);
            caret->Move(xx, yy);
            if ( !caret->IsVisible() ) caret->Show();
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
                dx += offset.x;
                dy = dc.GetCharHeight();
                dc.Blit(dx, offset.y, lx, dy, &dc, 0, 0, wxINVERT);
            }
        }
        else
        {
            editingTextIsVisible = false;
            if ( caret->IsVisible() ) caret->Hide();
        }
    }
    ++textNumber;

    if ( empty )
        return wxPoint(0, h);
    return wxPoint(w, h);
}

void TimingWindow::Draw( wxDC& dc, bool exporting )
{
    if (!view) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    dc.SetFont(font);

    wxSize virtsize(0,0);
    //textStringPtrs.clear();
    InitTextDrawing();

    wxColour bgcol = dc.GetTextBackground();
    wxColour col( 0xe0, 0xe0, 0xe0 );
    if ( !exporting &&
        ( WindowState == SelectingText ||  WindowState == TextFieldSelected) )
    {
        dc.SetBackgroundMode(wxSOLID);
        dc.SetTextBackground(col);
    }
    dc.SetPen(*wxBLACK_PEN);

    wxCoord width = 0;
    wxPoint offset(0, 0);
    if ( !exporting )
        offset = wxPoint(40, DistanceToTimeline + DistanceFromTimeline );
    else
        offset = wxPoint( 0, 0);

    offset.y += DistanceToAxis + DistanceFromAxis;

    std::set<wxInt32>::iterator it;
    wxPoint unscrolledPosition;
    CalcUnscrolledPosition(0, 0, &unscrolledPosition.x, &unscrolledPosition.y);
    if ( exporting ) unscrolledPosition = wxPoint(0,0);
    wxPoint clientsize;
    GetClientSize(&clientsize.x, &clientsize.y);
    dc.DestroyClippingRegion();

    /// calc grid
    heightOffsets.clear();
    for ( unsigned int k = 0 ; k < doc->signals.size() ; ++k )
    {
        /// remember position
        heightOffsets.push_back(offset.y);
        /// update position for next signal
        offset.y += doc->SignalHeight;
        offset.y += doc->MinimumSignalDistance;
        offset.y += doc->signals[k].space;
        offset.y += doc->signals[k].prespace;

    }
    heightOffsets.push_back(offset.y);// remember last offset

    /// drawing signal names:
    if ( !exporting )
    dc.SetClippingRegion(0, heightOffsets[0]+unscrolledPosition.y,clientsize.x, clientsize.y);
    for ( unsigned int k = 0 ; k < doc->signals.size() ; ++k )
    {
        wxString str = doc->signals[k].name;
        wxPoint textoff(offset.x + unscrolledPosition.x,
            heightOffsets[k] +
            doc->SignalHeight/2 +
            doc->MinimumSignalDistance/2 +
            doc->signals[k].prespace -
            dc.GetCharHeight()/2
        );
        bool viz = true;
        if ( !exporting && textoff.y + dc.GetCharHeight()/2 < heightOffsets[0]+unscrolledPosition.y) viz = false;
        wxPoint siz = DrawEditableText(dc, str, textoff, viz);

        if ( doc->signals[k].IsBus )
        {
            dc.SetTextBackground(bgcol);
            dc.SetBackgroundMode(wxTRANSPARENT);
            wxString str = _T(" [");
            dc.DrawText(str, textoff.x + siz.x, textoff.y);
            wxCoord w, h;
            dc.GetTextExtent(str, &w, &h);
            if ( !exporting && ( WindowState == SelectingText || WindowState == TextFieldSelected) )
            {
                dc.SetBackgroundMode(wxSOLID);
                dc.SetTextBackground(col);
            }

            str = doc->signals[k].buswidth;
            wxPoint bwoff(textoff.x + siz.x+w+2, textoff.y);
            wxPoint s = DrawEditableText(dc, str, bwoff, viz);

            dc.SetTextBackground(bgcol);
            dc.SetBackgroundMode(wxTRANSPARENT);
            str = _T(" ]");
            dc.DrawText(str, bwoff.x + s.x, bwoff.y);
            if ( !exporting && ( WindowState == SelectingText || WindowState == TextFieldSelected) )
            {
                dc.SetBackgroundMode(wxSOLID);
                dc.SetTextBackground(col);
            }

            siz.x += s.x;
        }
        /// remember maximum width
        if ( width < siz.x ) width = siz.x;
    }
    dc.SetTextBackground(bgcol);
    dc.SetBackgroundMode(wxTRANSPARENT);

    /// points to manipulate the signal (through right click)
    if ( !exporting )
    {
        dc.SetBrush(*wxBLACK_BRUSH);
        for ( wxUint32 n = 0 ; n < doc->signals.size() ; ++n )
        {
            if ( WindowState == SignalIsSelected && editingNumber>=0 && ((wxUint32)editingNumber == n ) )
            {
                dc.SetBrush(*wxBLUE_BRUSH);
                dc.SetPen(*wxLIGHT_GREY_PEN);
            }
            dc.DrawCircle(manipXoffset + unscrolledPosition.x,
                heightOffsets[n] + doc->SignalHeight/2+doc->MinimumSignalDistance/2+ doc->signals[n].prespace,
                manipRadius
            );
            if ( WindowState == SignalIsSelected  && editingNumber>=0 && (wxUint32)editingNumber == n )
            {
                dc.SetBrush(*wxBLACK_BRUSH);
                //dc.SetPen(wxNullPen);
                dc.SetPen(*wxBLACK_PEN);
            }
        }
        dc.SetBrush(wxNullBrush);
    }
    dc.DestroyClippingRegion();


    /// where to start the waves and how big will the virtual window be
    signalNamesWidth = offset.x + width + 25;
    if ( virtsize.y < offset.y ) virtsize.y = offset.y;// change the size of the scrollable window
    width += 20 + GridStepWidth * doc->length;
    if ( virtsize.x < (width + 500) ) virtsize.x = (width + 500);
    SetVirtualSize(virtsize);



    if ( WindowState != EditAxisLeft && !exporting  )
        dc.SetClippingRegion(unscrolledPosition.x+signalNamesWidth-5,unscrolledPosition.y, virtsize.x, virtsize.y+10);
    /// drawing the ticks
    if ( !exporting )
    {
        wxInt32 gridWidth = GridStepWidth;
        wxInt32 gridSteps = doc->length;
        axisStart = signalNamesWidth;
        axisStop = signalNamesWidth + gridWidth*gridSteps;
        wxCoord drawstartx, drawy, drawstopx;
        wxInt32 drawsteps;
        bool drawBoxes;

        drawstartx = axisStart;
        drawstopx = axisStop;
        drawy = DistanceToTimeline;
        drawsteps = gridSteps;

        if ( WindowState != EditAxisLeft && WindowState != EditAxisRight )
            drawBoxes = WindowState == AxisIsMarked;
        else
        {
            drawBoxes = true;
            drawsteps = editingValB;
            if ( WindowState == EditAxisRight )
                axisStop  = signalNamesWidth + gridWidth*editingValB;
            else
                drawstartx = axisStop - gridWidth*editingValB;
            drawstopx = axisStop;
        }
        drawy += unscrolledPosition.y;
        dc.DrawLine(drawstartx - 3, drawy, drawstopx + 3, drawy);
        for (wxInt32 n = 0 ; n <= drawsteps ; ++n)
            dc.DrawLine(drawstartx + n*gridWidth, drawy - 2, drawstartx + n*gridWidth, drawy + 3);
        if ( drawBoxes )
        {
            dc.SetBrush(*wxBLUE_BRUSH);
            dc.DrawRectangle(drawstartx - 3, drawy - 3, 7, 7);
            dc.DrawRectangle(drawstopx - 3, drawy - 3, 7, 7);
        }
    }

    /// drawing the axis
    if ( !exporting )
        dc.SetClippingRegion(unscrolledPosition.x+signalNamesWidth-5,unscrolledPosition.y, virtsize.x, virtsize.y+10);
    {
        wxInt32 gridWidth = GridStepWidth;
        wxInt32 gridSteps = doc->length;

        wxCoord axispos =  DistanceToAxis + unscrolledPosition.y;
        wxCoord start = signalNamesWidth, stop = start + axisStop-axisStart;
        if ( ! exporting )
            axispos += DistanceToTimeline + DistanceFromTimeline;

        dc.DrawLine(start , axispos, stop, axispos);
        //for ( wxInt32 i = timeOffset ; (i - timeOffset) < gridSteps ; i += TackLength )
        wxInt32 ticks = doc->timeOffset;
        wxInt32 tacklen = doc->TackLength;
        for ( wxInt32 n = 0 ; n <= gridSteps ; n++ )
        {
            if ( (ticks % tacklen) == 0 )
            {
                dc.DrawLine(signalNamesWidth + n*gridWidth, axispos - 2,
                    signalNamesWidth + n*gridWidth, axispos + 3);

                double t = ticks * doc->TickLength;
                wxInt8 u = doc->TickLengthUnit;
                while ( u < 3 && t >= 1000.0)
                {
                    u++; t /= 1000.0;
                }
                wxString str(wxString::Format(_("%.1f "), t));
                switch (u)
                {
                    case -5: str += _("fs"); break;
                    case -4: str += _("ps"); break;
                    case -3: str += _("ns"); break;
                    case -2: str += _("us"); break;
                    case -1: str += _("ms"); break;
                    case  0: str += _("s"); break;
                    case  1: str += _("ks"); break;
                    case  2: str += _("Ms"); break;
                    default:
                    case  3: str += _("Gs"); break;
                }
                wxCoord w, h;
                dc.GetTextExtent(str, &w, &h);
                dc.DrawText(str, signalNamesWidth + n*gridWidth - w/2, axispos - h - 4);
            }


             if ( doc->discontinuities.find(n) != doc->discontinuities.end() )
                 ticks += doc->discontlength[n];
             else
                ++ticks;
        }
    }

    /// indicate target position of moving signals
    if ( !exporting )
        dc.SetClippingRegion(unscrolledPosition.x+signalNamesWidth-5,heightOffsets[0]-5+unscrolledPosition.y, virtsize.x, virtsize.y+10);
    if ( !exporting && WindowState == MovingSignal )
    {
        wxPen pen(*wxBLACK, 2 );
        dc.SetPen(pen);
        dc.DrawLine(
            manipXoffset, heightOffsets[editingValA],
            axisStop,  heightOffsets[editingValA]
        );
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
    for ( wxUint32 n = 0 ; n < doc->signals.size() ; ++n )
    {
        offset.x = signalNamesWidth;
        offset.y = heightOffsets[n]+doc->MinimumSignalDistance/2+ doc->signals[n].prespace;
        Signal sig;
        if ( WindowState == EditSignal && editingNumber>=0 && (wxUint32)editingNumber == n && editingValB != -1 ) sig = editingSignal;
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
                            offset.x, offset.y + doc->SignalHeight,
                            offset.x, offset.y
                        );
                    }
                    if ( val ) // one
                    {
                        dc.DrawLine(
                            offset.x, offset.y + doc->SignalHeight,
                            offset.x + GridStepWidth, offset.y + doc->SignalHeight
                        );
                        if ( ++n == sig.ticks )
                        {
                            dc.DrawLine(
                                offset.x + GridStepWidth, offset.y + doc->SignalHeight,
                                offset.x + GridStepWidth, offset.y
                            );
                            val = !val;
                            n = 0;
                        }
                    }
                    else
                    {
                        dc.DrawLine(
                            offset.x, offset.y,
                            offset.x + GridStepWidth, offset.y
                        );
                        if ( ++n == sig.ticks )
                        {
                            dc.DrawLine(
                                offset.x + GridStepWidth, offset.y,
                                offset.x + GridStepWidth, offset.y + doc->SignalHeight
                            );
                            val = !val;
                            n = 0;
                        }
                    }
                    offset.x += GridStepWidth;
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
            offset.x = signalNamesWidth;
            wxCoord transwidth = GridStepWidth/(100.0/doc->TransitWidth);
            if ( doc->length > 0 ) switch(sig.values[0])
            {
                case zero:
                    dc.DrawLine(
                        offset.x,              offset.y+doc->SignalHeight,
                        offset.x + transwidth, offset.y+doc->SignalHeight
                    );
                    break;
                case one:
                    dc.DrawLine(
                        offset.x,            offset.y,
                        offset.x+transwidth, offset.y
                    );
                    break;
                case hz :
                    dc.DrawLine(
                        offset.x,            offset.y + doc->SignalHeight/2,
                        offset.x+transwidth, offset.y + doc->SignalHeight/2
                    );
                    break;
                case u:
                default:
                    dc.DrawLine(
                        offset.x,            offset.y+doc->SignalHeight,
                        offset.x+transwidth, offset.y+doc->SignalHeight
                    );
                    dc.DrawLine(
                        offset.x,            offset.y,
                        offset.x+transwidth, offset.y
                    );
                    break;
            }
            offset.x += transwidth;//1*transitionWidth;
            for ( wxInt32 k = 0 ; k < doc->length ; ++k )
            {
                switch ( sig.values[k])
                {
                    case zero:
                        dc.DrawLine(
                            offset.x, offset.y+doc->SignalHeight,
                            offset.x + GridStepWidth/(100.0/(100-doc->TransitWidth))/*GridStepWidth/(100.0/(100-doc->TransitWidth))*/, offset.y+doc->SignalHeight
                        );

                        if ( k != doc->length -1 )
                        {
                            switch ( sig.values[k+1])
                            {
                                case zero:
                                    dc.DrawLine(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight,
                                        offset.x+GridStepWidth, offset.y+doc->SignalHeight);
                                    break;
                                case one:
                                    dc.DrawLine(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight,
                                        offset.x+GridStepWidth, offset.y);
                                    break;
                                case hz :
                                    dc.DrawLine(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight,
                                        offset.x+GridStepWidth, offset.y+doc->SignalHeight/2);
                                    break;
                                case u:
                                default:
                                    dc.DrawLine(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight,
                                        offset.x+GridStepWidth, offset.y+doc->SignalHeight);
                                    dc.DrawLine(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight,
                                        offset.x+GridStepWidth, offset.y);
                            }
                        }
                        break;
                    case one:
                        dc.DrawLine(
                            offset.x, offset.y,
                            offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y
                        );
                        if ( k != doc->length -1 )
                        {
                            switch ( sig.values[k+1])
                            {
                                case zero:
                                    dc.DrawLine(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y,
                                        offset.x+GridStepWidth, offset.y+doc->SignalHeight);
                                    break;
                                case one:
                                    dc.DrawLine(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y,
                                        offset.x+GridStepWidth, offset.y);
                                    break;
                                case hz :
                                    dc.DrawLine(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y,
                                        offset.x+GridStepWidth, offset.y+doc->SignalHeight/2);
                                    break;
                                case u:
                                default:
                                    dc.DrawLine(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y,
                                        offset.x+GridStepWidth, offset.y);
                                    dc.DrawLine(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y,
                                        offset.x+GridStepWidth, offset.y+doc->SignalHeight);
                            }
                        }
                        break;
                    case hz :
                        dc.DrawLine(
                            offset.x, offset.y+doc->SignalHeight/2,
                            offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight/2
                        );
                        if ( k != doc->length -1 )
                        {
                            switch ( sig.values[k+1])
                            {
                                case zero:
                                    dc.DrawLine(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight/2,
                                        offset.x+GridStepWidth, offset.y+doc->SignalHeight);
                                    break;
                                case one:
                                    dc.DrawLine(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight/2,
                                        offset.x+GridStepWidth, offset.y);
                                    break;
                                case hz :
                                    dc.DrawLine(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight/2,
                                        offset.x+GridStepWidth, offset.y+doc->SignalHeight/2);
                                    break;
                                case u:
                                default:
                                    dc.DrawLine(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight/2,
                                        offset.x+GridStepWidth, offset.y+doc->SignalHeight);
                                    dc.DrawLine(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight/2,
                                        offset.x+GridStepWidth, offset.y);
                            }
                        }
                        break;
                    case u:
                    default:
                        dc.DrawLine(
                            offset.x, offset.y+doc->SignalHeight,
                            offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight
                        );
                        dc.DrawLine(
                            offset.x, offset.y,
                            offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y
                        );
                        if ( k != doc->length -1 )
                        {
                            switch ( sig.values[k+1])
                            {
                                case zero:
                                    dc.DrawLine(
                                        offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight,
                                        offset.x+GridStepWidth, offset.y+doc->SignalHeight
                                    );
                                    dc.DrawLine(
                                        offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y,
                                        offset.x+GridStepWidth, offset.y+doc->SignalHeight
                                    );
                                    break;
                                case one:
                                    dc.DrawLine(
                                        offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight,
                                        offset.x+GridStepWidth, offset.y
                                    );
                                    dc.DrawLine(
                                        offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y,
                                        offset.x+GridStepWidth, offset.y
                                    );
                                    break;
                                case hz:
                                    dc.DrawLine(
                                        offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight,
                                        offset.x+GridStepWidth, offset.y+doc->SignalHeight/2
                                    );
                                    dc.DrawLine(
                                        offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y,
                                        offset.x+GridStepWidth, offset.y+doc->SignalHeight/2
                                    );
                                    break;
                                case u:
                                default:
                                    dc.DrawLine(
                                        offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight,
                                        offset.x+GridStepWidth, offset.y+doc->SignalHeight
                                    );
                                    dc.DrawLine(
                                        offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y,
                                        offset.x+GridStepWidth, offset.y
                                    );
                                    break;
                            }
                        }
                        break;
                }
                offset.x+= GridStepWidth;
            }
        }
        else // is bus
        {
            offset.x = signalNamesWidth;
            wxCoord transwidth = GridStepWidth/(100.0/doc->TransitWidth);
            if ( doc->length > 0 ) switch(sig.values[0])
            {
                case zero:
                case one:
                    dc.DrawLine(
                        offset.x,            offset.y + doc->SignalHeight,
                        offset.x+transwidth, offset.y + doc->SignalHeight
                    );
                    dc.DrawLine(
                        offset.x,            offset.y,
                        offset.x+transwidth, offset.y
                    );
                break;
                case hz :
                    dc.DrawLine(
                        offset.x,            offset.y+doc->SignalHeight/2,
                        offset.x+transwidth, offset.y+doc->SignalHeight/2
                    );
                    break;
                case u:
                default:
                    {
                        dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                        dc.SetPen(*wxLIGHT_GREY_PEN);
                        wxPoint rec[4];
                        rec[0] = wxPoint(offset.x+1, offset.y+doc->SignalHeight);
                        rec[1] = wxPoint(offset.x+transwidth, offset.y+doc->SignalHeight);
                        rec[2] = wxPoint(offset.x+transwidth, offset.y);
                        rec[3] = wxPoint(offset.x+1, offset.y);
                        dc.DrawPolygon(4, rec);
                        //dc.SetPen(wxNullPen);
                        //dc.SetPen(*wxBLACK_PEN);
                        dc.SetPen(boldPen);
                    }
                    dc.DrawLine(
                        offset.x,            offset.y+doc->SignalHeight,
                        offset.x+transwidth, offset.y+doc->SignalHeight
                    );
                    dc.DrawLine(
                        offset.x,            offset.y,
                        offset.x+transwidth, offset.y
                    );
                break;
            }
            offset.x += transwidth;
            //wxInt32 i = 0;
            for ( wxInt32 k = 0 ; k < doc->length ; ++k )
            {
                switch ( sig.values[k] )
                {
                    case zero:
                    case one:
                        dc.DrawLine(
                            offset.x, offset.y + doc->SignalHeight,
                            offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y + doc->SignalHeight
                        );
                        dc.DrawLine(
                            offset.x, offset.y,
                            offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y
                        );
                        if ( k != doc->length -1 )
                        {
                            if ( sig.values[k+1] == sig.values[k])
                            {
                                dc.DrawLine(
                                    offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y + doc->SignalHeight,
                                    offset.x+GridStepWidth, offset.y + doc->SignalHeight
                                );
                                dc.DrawLine(
                                    offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y,
                                    offset.x+GridStepWidth, offset.y
                                );
                            }
                            else if ( sig.values[k+1] == hz )
                            {
                                dc.DrawLine(
                                    offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y + doc->SignalHeight,
                                    offset.x+GridStepWidth, offset.y + doc->SignalHeight/2
                                );
                                dc.DrawLine(
                                    offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y,
                                    offset.x+GridStepWidth, offset.y+ doc->SignalHeight/2
                                );
                            }
                            else if ( sig.values[k+1] == u )
                            {
                                dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                dc.SetPen(*wxLIGHT_GREY_PEN);
                                wxPoint rec[3];
                                rec[0] = wxPoint(offset.x+GridStepWidth, offset.y + doc->SignalHeight);
                                rec[1] = wxPoint(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth/2.0))/*transitionWidth*(+transitionsPerGridStep-0.5)*/, offset.y + doc->SignalHeight/2);
                                rec[2] = wxPoint(offset.x+GridStepWidth, offset.y);
                                dc.DrawPolygon(3, rec);
                                //dc.SetPen(wxNullPen);
                                //dc.SetPen(*wxBLACK_PEN);
                                dc.SetPen(boldPen);
                                dc.DrawLine(
                                    offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y + doc->SignalHeight,
                                    offset.x+GridStepWidth, offset.y
                                );
                                dc.DrawLine(
                                    offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y,
                                    offset.x+GridStepWidth, offset.y + doc->SignalHeight
                                );
                            }
                            else
                            {
                                dc.DrawLine(
                                    offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y + doc->SignalHeight,
                                    offset.x+GridStepWidth, offset.y
                                );
                                dc.DrawLine(
                                    offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y,
                                    offset.x+GridStepWidth, offset.y + doc->SignalHeight
                                );
                            }

                        }
                        if ( k == 0 || sig.values[k] != sig.values[k-1] )
                        {
                            /// the text:
                            wxPoint textoff(
                                2 + offset.x,
                                offset.y + doc->SignalHeight/2 -
                                dc.GetCharHeight()/2
                            );
                            bool viz = true;
                            if ( textoff.y + dc.GetCharHeight()/2 < heightOffsets[0]+unscrolledPosition.y) viz = false;
                            if ( textoff.x                        < signalNamesWidth - 5 + unscrolledPosition.x) viz = false;
                            DrawEditableText(dc, sig.TextValues[k], textoff, viz);
                        }
                        break;
                    case hz :
                        dc.DrawLine(
                            offset.x, offset.y+doc->SignalHeight/2,
                            offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight/2
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
                                    rec[0] = wxPoint(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight/2);
                                    rec[1] = wxPoint(offset.x+GridStepWidth,     offset.y+doc->SignalHeight);
                                    rec[2] = wxPoint(offset.x+GridStepWidth,     offset.y);
                                    dc.DrawPolygon(3, rec);

                                    //dc.SetPen(wxNullPen);
                                    //dc.SetPen(*wxBLACK_PEN);
                                    dc.SetPen(boldPen);
                                }
                                case one:
                                case zero:
                                    dc.DrawLine(
                                        offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight/2,
                                        offset.x+GridStepWidth,    offset.y+doc->SignalHeight
                                    );
                                    dc.DrawLine(
                                        offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight/2,
                                        offset.x+GridStepWidth,    offset.y
                                    );
                                    break;
                                case hz :
                                    dc.DrawLine(
                                        offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight/2,
                                        offset.x+GridStepWidth,     offset.y+doc->SignalHeight/2
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
                            rec[0] = wxPoint(offset.x+1, offset.y+doc->SignalHeight);
                            rec[1] = wxPoint(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight);
                            rec[2] = wxPoint(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y);
                            rec[3] = wxPoint(offset.x+1, offset.y);
                            dc.DrawPolygon(4, rec);
                            //dc.SetPen(wxNullPen);
                            //dc.SetPen(*wxBLACK_PEN);
                            dc.SetPen(boldPen);
                        }
                        dc.DrawLine(
                            offset.x, offset.y+doc->SignalHeight,
                            offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight
                        );
                        dc.DrawLine(
                            offset.x, offset.y,
                            offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y
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
                                        rec[0] = wxPoint(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight);
                                        rec[1] = wxPoint(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth/2.0))/*transitionWidth*(-0.5+transitionsPerGridStep)*/, offset.y+doc->SignalHeight/2);
                                        rec[2] = wxPoint(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y);
                                        dc.DrawPolygon(3, rec);
                                        //dc.SetPen(wxNullPen);
                                        //dc.SetPen(*wxBLACK_PEN);
                                        dc.SetPen(boldPen);
                                    }
                                    dc.DrawLine(
                                        offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight,
                                        offset.x+GridStepWidth,   offset.y
                                    );
                                    dc.DrawLine(
                                        offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y,
                                        offset.x+GridStepWidth,   offset.y+doc->SignalHeight
                                    );
                                    break;
                                case hz:
                                    {
                                        dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                        dc.SetPen(*wxLIGHT_GREY_PEN);
                                        wxPoint rec[3];
                                        rec[0] = wxPoint(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight);
                                        rec[1] = wxPoint(offset.x+GridStepWidth, offset.y+doc->SignalHeight/2);
                                        rec[2] = wxPoint(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y);
                                        dc.DrawPolygon(3, rec);
                                        //dc.SetPen(wxNullPen);
                                        //dc.SetPen(*wxBLACK_PEN);
                                        dc.SetPen(boldPen);
                                    }
                                    dc.DrawLine(
                                        offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight,
                                        offset.x+GridStepWidth, offset.y+doc->SignalHeight/2
                                    );
                                    dc.DrawLine(
                                        offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y,
                                        offset.x+GridStepWidth, offset.y+doc->SignalHeight/2
                                    );
                                    break;
                                case u:
                                default:
                                    {
                                        dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                        dc.SetPen(*wxLIGHT_GREY_PEN);
                                        wxPoint rec[4];
                                        rec[0] = wxPoint(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight);
                                        rec[1] = wxPoint(offset.x+GridStepWidth, offset.y+doc->SignalHeight);
                                        rec[2] = wxPoint(offset.x+GridStepWidth, offset.y);
                                        rec[3] = wxPoint(offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y);
                                        dc.DrawPolygon(4, rec);
                                        //dc.SetPen(wxNullPen);
                                        //dc.SetPen(*wxBLACK_PEN);
                                        dc.SetPen(boldPen);
                                    }
                                    dc.DrawLine(
                                        offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y+doc->SignalHeight,
                                        offset.x+GridStepWidth,   offset.y+doc->SignalHeight
                                    );
                                    dc.DrawLine(
                                        offset.x+GridStepWidth/(100.0/(100-doc->TransitWidth)), offset.y,
                                        offset.x+GridStepWidth,   offset.y
                                    );
                                    break;
                            }
                        }
                        break;
                }
                offset.x+= GridStepWidth;
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
    dc.SetPen(defPen);

    /// drawing discontinuities
    for ( it = doc->discontinuities.begin() ; it != doc->discontinuities.end() ; it++)
    {
        /// splines over signal
        for ( wxUint32 n = 0 ; n < doc->signals.size() ; ++n)
        {
            //wxPoint sploff(15, 10);
            offset.x = signalNamesWidth  +
                    (0.5 + *it) * GridStepWidth
                    - 2
                    ;
            offset.y = heightOffsets[n]+doc->MinimumSignalDistance/2 + doc->signals[n].prespace;
            for ( wxInt32 n = 0 ; n < 4 ; ++n )
            {
                wxPoint ind(n,0);
                wxPoint points[] =
                {
                    offset + ind + wxPoint(+3, -0.2*doc->SignalHeight),
                    offset + ind + wxPoint(+0, +0.2*doc->SignalHeight),
                    offset + ind + wxPoint(+7, +0.8*doc->SignalHeight),
                    offset + ind + wxPoint(+0, +1.2*doc->SignalHeight)
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


    /// drawing the horizontal arrows
    if ( !exporting && ( WindowState == SelectingText ||
                         WindowState == TextFieldSelected ) )
    {
        dc.SetBackgroundMode(wxSOLID);
        dc.SetTextBackground(col);
    }
    HArrowOffsets.clear();
    HArrowToOffset.clear();
    for ( wxUint32 n = 0 ; n < doc->harrows.size() ; ++n )
    {
        wxInt32 distancfortimeline = 0;
        if ( !exporting )
            distancfortimeline = DistanceToTimeline + DistanceFromTimeline;
        HArrow &ha = doc->harrows[n];
        wxPoint offset(signalNamesWidth + doc->vertlines[ha.fromVLine].vpos * GridStepWidth,
                ha.pos + heightOffsets[ha.signalnmbr]);
        if ( doc->vertlines[ha.fromVLine].vposoffset == 1 )
            offset.x += GridStepWidth/(100.0/(doc->TransitWidth/2.0));
        else if ( doc->vertlines[ha.fromVLine].vposoffset == 2 )
            offset.x += GridStepWidth/(100.0/(doc->TransitWidth));

        if ( editingNumber >= 0 && (wxUint32)editingNumber == n && WindowState == MovingHArrow )
            offset.y = editingValA + heightOffsets[editingValB];
        wxPoint tooffset(signalNamesWidth + doc->vertlines[ha.toVLine].vpos * GridStepWidth,
            offset.y);
        if ( doc->vertlines[ha.toVLine].vposoffset == 1 )
            tooffset.x += GridStepWidth/(100.0/(doc->TransitWidth/2.0));
        else if ( doc->vertlines[ha.toVLine].vposoffset == 2 )
            tooffset.x += GridStepWidth/(100.0/(doc->TransitWidth));


        if ( !exporting && editingNumber >= 0 && (wxUint32)editingNumber == n && WindowState == ChangingHArrowLengthLeft )
        {
            if (editingValA == -1 )
                offset.x = cursorpos.x;
            else
            {
                offset.x = signalNamesWidth + doc->vertlines[editingValA].vpos * GridStepWidth;
                if ( doc->vertlines[editingValA].vposoffset == 1 )
                    tooffset.x += GridStepWidth/(100.0/(doc->TransitWidth/2.0));
                else if ( doc->vertlines[editingValA].vposoffset == 2 )
                    tooffset.x += GridStepWidth/(100.0/(doc->TransitWidth));
            }
        }
        if ( !exporting && editingNumber>=0 && (wxUint32)editingNumber == n && WindowState == ChangingHArrowLengthRight )
        {
            if (editingValB == -1 )
                tooffset.x = cursorpos.x;
            else
            {
                tooffset.x = signalNamesWidth + doc->vertlines[editingValB].vpos * GridStepWidth;
                if ( doc->vertlines[editingValB].vposoffset == 1 )
                    tooffset.x += GridStepWidth/(100.0/(doc->TransitWidth/2.0));
                else if ( doc->vertlines[editingValB].vposoffset == 2 )
                    tooffset.x += GridStepWidth/(100.0/(doc->TransitWidth));
            }
        }

        if ( offset.x > tooffset.x ) // swap
        {
            wxCoord t = offset.x;
            offset.x = tooffset.x;
            tooffset.x = t;
        }
        if ( !exporting && editingNumber>=0 && (wxUint32)editingNumber == n &&
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

        ///the text:
        wxPoint textoff;
        if ( editingNumber>=0 && (wxUint32)editingNumber == n && WindowState == HArrowMovingText )
        {
            textoff.x  = (offset.x + tooffset.x)/2 + editingValA;
            textoff.y  = offset.y + editingValB - dc.GetCharHeight();
        }
        else
        {
            textoff.x  = (offset.x + tooffset.x)/2;
            if ( ha.textoffset.x > GridStepWidth ) textoff.x += GridStepWidth;
            else                                   textoff.x += ha.textoffset.x;
            textoff.x += ha.textgridoffset*GridStepWidth;
            textoff.y  = offset.y + ha.textoffset.y - dc.GetCharHeight();
        }
        bool viz = true;
        if ( textoff.y + dc.GetCharHeight()/2 < heightOffsets[0]+unscrolledPosition.y) viz = false;
        if ( textoff.x                        < signalNamesWidth - 5 + unscrolledPosition.x) viz = false;
        DrawEditableText( dc, ha.text, textoff, viz );

        /// blue boxes to manipulate positions
        if ( !exporting && editingNumber>=0 && (wxUint32)editingNumber == n && ( WindowState == HArrowIsMarked ||
               WindowState == ChangingHArrowLengthLeft ||
               WindowState == ChangingHArrowLengthRight ))
        {
            dc.SetBrush(*wxBLUE_BRUSH);
            dc.DrawRectangle(  offset.x-3, offset.y - 3, 7, 7);
            dc.DrawRectangle(tooffset.x-3, offset.y - 3, 7, 7);
            dc.SetBrush(wxNullBrush);
        }
        if ( !exporting && editingNumber>=0 && (wxUint32)editingNumber == n &&
            ( WindowState == HArrowIsMarked ||
              WindowState == HArrowMovingText ))
        {
            dc.SetBrush(*wxBLUE_BRUSH);
            dc.DrawRectangle(
                textoff.x - 3,
                textoff.y + dc.GetCharHeight() - 3,
                7, 7);
            dc.SetBrush(wxNullBrush);
        }
        if ( !exporting && editingNumber>=0 && (wxUint32)editingNumber == n &&
            ( WindowState == HArrowIsMarked ||
              WindowState == HArrowMovingText ||
              WindowState == ChangingHArrowLengthLeft ||
              WindowState == ChangingHArrowLengthRight ||
              WindowState == SelectingText))
        {
            wxPen pen(*wxBLACK, 1, wxSHORT_DASH );
            dc.SetPen(pen);
            dc.DrawLine(  (offset.x + tooffset.x)/2,   offset.y, textoff.x,   textoff.y+dc.GetCharHeight());
            dc.SetPen(*wxBLACK_PEN);
        }
    }
    if ( WindowState == InsertingHArrowWaitingSecondPoint )
    {
        wxPoint offset(signalNamesWidth + doc->vertlines[editingNumber].vpos * GridStepWidth,
        //        heightOffsets[doc->vertlines[ha.fromVLine].StartPos] + ha.pos);
                editingValA + heightOffsets[editingValC]);
        if ( doc->vertlines[editingNumber].vposoffset == 1 )
            offset.x += GridStepWidth/(100.0/(doc->TransitWidth/2.0));
        else if ( doc->vertlines[editingNumber].vposoffset == 2 )
            offset.x += GridStepWidth/(100.0/(doc->TransitWidth));
        wxPoint tooffset;
        tooffset.y = offset.y;
        if ( editingValB == -1 )
            tooffset.x = cursorpos.x;
        else
        {
            tooffset.x = signalNamesWidth + doc->vertlines[editingValB].vpos * GridStepWidth;
            if ( doc->vertlines[editingValB].vposoffset == 1 )
                tooffset.x += GridStepWidth/(100.0/(doc->TransitWidth/2.0));
            else if ( doc->vertlines[editingValB].vposoffset == 2 )
                tooffset.x += GridStepWidth/(100.0/(doc->TransitWidth));
        }
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


    /// drawing the vertical lines
    dc.DestroyClippingRegion();
    if ( !exporting )
        dc.SetClippingRegion(unscrolledPosition.x+signalNamesWidth-5,heightOffsets[0]+unscrolledPosition.y-3, virtsize.x, virtsize.y+3);
    VLineOffsets.clear();
    VLineToOffset.clear();
    for ( wxUint32 k = 0 ; k < doc->vertlines.size() ; ++k)
    {
        wxPoint offset(signalNamesWidth + doc->vertlines[k].vpos * GridStepWidth,
        /*offset.y =*/ heightOffsets[doc->vertlines[k].StartPos]);

        if ( doc->en50 && doc->vertlines[k].vposoffset == 1 )
            offset.x += GridStepWidth/(100.0/(doc->TransitWidth/2.0));
        if ( doc->en90 && doc->vertlines[k].vposoffset == 2 )
            offset.x += GridStepWidth/(100.0/(doc->TransitWidth));

        wxInt32 tolen = heightOffsets[doc->vertlines[k].EndPos + 1];
        if ( !exporting && editingNumber>=0 && (wxUint32)editingNumber == k )
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
                offset.x = signalNamesWidth + editingValA * GridStepWidth;
                if ( doc->en50 &&  editingValB == 1 )
                    offset.x += GridStepWidth/(100.0/(doc->TransitWidth/2.0));
                if ( doc->en90 && editingValB == 2 )
                    offset.x += GridStepWidth/(100.0/(doc->TransitWidth));

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
        wxPoint offset(signalNamesWidth + editingPoint[0].x * GridStepWidth,
        offset.y = heightOffsets[editingPoint[0].y]);

        if ( doc->en50 && editingValB == 1 )
            offset.x += GridStepWidth/(100.0/(doc->TransitWidth/2.0));
        if ( doc->en90 && editingValB == 2 )
            offset.x += GridStepWidth/(100.0/(doc->TransitWidth));

        dc.DrawLine(
            offset.x, offset.y,
            offset.x, heightOffsets[editingPoint[1].y]
        );
        dc.SetBrush(*wxBLUE_BRUSH);
        dc.DrawRectangle(offset.x-3, offset.y -                         3, 7, 7);
        dc.DrawRectangle(offset.x-3, heightOffsets[editingPoint[1].y] - 3, 7, 7);
        dc.SetBrush(wxNullBrush);
    }


    /// drawing around the selected signal
    dc.DestroyClippingRegion();
    if ( !exporting )
        dc.SetClippingRegion(0, heightOffsets[0]+unscrolledPosition.y-3,virtsize.x, virtsize.y+6);
    if ( !exporting && (WindowState == SignalIsSelected ||
        WindowState == ChangingLowerSpace ||
        WindowState == ChangingUpperSpace ||
        WindowState == MovingSignal))
    {
        dc.SetPen(*wxBLACK_PEN);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        wxInt32 newSpace = editingValA;
        wxCoord xstart, ystart, width, height;
        xstart = manipXoffset-2*manipRadius;
        width = axisStop-xstart+10;
        ystart=heightOffsets[editingNumber];
        height = doc->SignalHeight + doc->MinimumSignalDistance +
                 doc->signals[editingNumber].space +
                 doc->signals[editingNumber].prespace;

        if ( WindowState != SignalIsSelected && WindowState != MovingSignal )
        {
            if ( WindowState == ChangingLowerSpace )
            {
                height += newSpace - doc->signals[editingNumber].space;
                editingPoint[0].y += newSpace - doc->signals[editingNumber].space;
            }
            else
            {
                ystart += doc->signals[editingNumber].prespace - newSpace;
                height += newSpace - doc->signals[editingNumber].prespace;
                editingPoint[1].y +=  doc->signals[editingNumber].prespace - newSpace;
            }
        }

        width-=unscrolledPosition.x;
        xstart+=unscrolledPosition.x;
        editingPoint[0].x = xstart + width/2;
        if ( clientsize.x + unscrolledPosition.x - 5 < editingPoint[0].x)
            editingPoint[0].x = clientsize.x + unscrolledPosition.x - 5;
        editingPoint[0].y = ystart+height;
        editingPoint[1].x = editingPoint[0].x;
        editingPoint[1].y = ystart;
        if ( width > signalNamesWidth )
        {
            dc.DrawRoundedRectangle(xstart, ystart, width, height, 10.0);
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
    }

    /// drawing discontinuities triangles and time line
    dc.DestroyClippingRegion();
    if ( !exporting )
        dc.SetClippingRegion(unscrolledPosition.x+signalNamesWidth-5,unscrolledPosition.y, virtsize.x, virtsize.y);
    for ( it = doc->discontinuities.begin() ; it != doc->discontinuities.end() ; it++)
    {
        /// triangle
        offset.x = signalNamesWidth  +
                    (0.5 + *it) * GridStepWidth
                    - 3
                    ;
        offset.y = DistanceToTimeline-3;
        wxPoint points[] =
        {
            offset + wxPoint(0, unscrolledPosition.y),
            offset + wxPoint(8, unscrolledPosition.y),
            offset + wxPoint(4, unscrolledPosition.y + 6)
        };
        dc.SetBrush(*wxBLACK_BRUSH);
        if (  !exporting )
        {
            if ( WindowState == DisconSelected && *it == editingValA )
            {
                wxPen pen(*wxBLUE);
                dc.SetBrush(*wxBLUE_BRUSH);
                dc.SetPen(pen);
                dc.DrawPolygon(3, points);
                dc.SetBrush(*wxBLACK_BRUSH);
                dc.SetPen(*wxBLACK_PEN);
            }
            else
                dc.DrawPolygon(3, points);
        }

        /// over the axis
        offset.x = signalNamesWidth  +
                    (0.5 + *it) * GridStepWidth - 1
                    ;
        offset.y = DistanceToAxis - 3 + unscrolledPosition.y;
        if ( !exporting )
            offset.y += DistanceToTimeline + DistanceFromTimeline;
        for ( wxInt32 n = 0 ; n < 4 ; ++n )
        {
            if ( n == 0 || n == 3 )
                dc.SetPen(*wxBLACK_PEN);
            else
                dc.SetPen( *wxWHITE_PEN );
            dc.DrawLine(offset.x + n-1 , offset.y,
                        offset.x + n-1 , offset.y + 6);
        }
    }

    /// cursor cross over the whole canvas:
    dc.DestroyClippingRegion();
    if ( mouseoverwindow &&
        WindowState != SelectingText &&
        WindowState != TextFieldSelected
        )
    {
        dc.SetPen(*wxGREY_PEN);
        dc.CrossHair(cursorpos.x, cursorpos.y);
    }

    //dropcaret->Draw(dc);
    //dc.DrawText(wxString::Format(_T("cursor x: %d, virtsize.x: %d"), cursorpos.x, virtsizex),
    //unscrolledPosition.x+5, unscrolledPosition.y+5 );
}

void TimingWindow::OnEraseBackground(wxEraseEvent &event){/* must be empty */}

void TimingWindow::OnPaint(wxPaintEvent &event)
{
    wxBufferedPaintDC dc(this);
    PaintBackground(dc);
    DoPrepareDC(dc);
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
    dc.DrawRectangle(windowRect);
    dc.SetBrush(wxNullBrush);
    dc.SetPen(wxNullPen);
}

void TimingWindow::OnLeaveWindow(wxMouseEvent &event)
{
    mouseoverwindow = false;
    this->Refresh(true);
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
        case DisconSelected:
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

    Refresh(true);
}
void TimingWindow::OnMouseLeftDown(wxMouseEvent &event)
{
    if (!view) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    SetFocus();

    wxClientDC dc(this);
    DoPrepareDC(dc);
    dc.SetFont(font);
    wxPoint pt(event.GetLogicalPosition(dc));

    wxPoint p(event.GetPosition());

    bool dorefresh = false;

    wxPoint unscrolledPosition;
    CalcUnscrolledPosition(0, 0, &unscrolledPosition.x, &unscrolledPosition.y);

    states newstate = WindowState;
    switch ( WindowState )
    {
        case Neutral:
            /// check if user clicked onto discont triangle
            if (p.x > signalNamesWidth-10 &&
                pt.x > axisStart &&
                pt.x < axisStop &&
                p.y > DistanceToTimeline - 5 &&
                p.y < DistanceToTimeline + 5 )
            {
                wxCoord p = pt.x-axisStart;
                wxInt32 discontpos = p / (GridStepWidth);
                //wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                if ( doc->discontinuities.find(discontpos) != doc->discontinuities.end() )
                {
                    newstate = DisconSelected;
                    editingValA = discontpos;
                    dorefresh = true;
                    break;
                }
            }
            /// check click on axis
            if (p.x > signalNamesWidth-10 &&
                pt.x > axisStart &&
                pt.x < axisStop &&
                p.y > DistanceToTimeline - 5 &&
                p.y < DistanceToTimeline + 5)
            {
                newstate = AxisIsMarked;
                dorefresh = true;
                break;
            }
            /// check click on left points
            editingNumber = -1;
            for ( wxUint32 n = 0 ; n < doc->signals.size() ; ++n )
            {
                if (p.y >   heightOffsets[0] &&
                    p.x >=  manipXoffset - 2*manipRadius &&
                    pt.y >= heightOffsets[n]+doc->MinimumSignalDistance/2 + doc->SignalHeight/2 + doc->signals[n].prespace - 2*manipRadius &&
                    p.x <=  manipXoffset + 2*manipRadius &&
                    pt.y <= heightOffsets[n]+doc->MinimumSignalDistance/2 + doc->SignalHeight/2 + doc->signals[n].prespace + 2*manipRadius)
                {
                    newstate = SignalIsSelected;
                    editingNumber = n;
                    dorefresh = true;
                    break;
                }
            }
            /// check if user clicked into a waveform
            for ( wxUint32 n = 0 ; n < doc->signals.size() ; ++n )
            {
                if ( ! doc->signals[n].IsClock && p.x > signalNamesWidth && p.y > heightOffsets[0] )
                    if ( pt.y >= heightOffsets[n]+doc->MinimumSignalDistance/2+ doc->signals[n].prespace && pt.x >= axisStart &&
                        pt.y <= heightOffsets[n]+doc->MinimumSignalDistance/2 + doc->signals[n].prespace + doc->SignalHeight && pt.x <= axisStop )
                    {
                        newstate = EditSignal;
                        //editingSignalNr
                        //editingSignalPosStart
                        //editingSignalPosStop
                        editingNumber = n;
                        wxCoord p = pt.x - axisStart;
                        editingValA = p / (GridStepWidth);
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
            for ( wxUint32 k = 0 ; k < VLineOffsets.size() ; ++k )
            {
                if (p.x > signalNamesWidth-10 &&
                    p.y > heightOffsets[0]-5 &&
                    pt.x >= VLineOffsets[k].x - 3 &&
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
            for ( wxUint32 k = 0 ; k < HArrowOffsets.size() ; ++k)
            {
                if ( p.x > signalNamesWidth-10 &&
                     p.y > heightOffsets[0]-5 &&
                     pt.x >= HArrowOffsets[k].x &&
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
        case DisconSelected:
            SetNeutralState();
            OnMouseLeftDown(event);
            return;
        case AxisIsMarked:
            /// check if user clicked the end of marked axis
            if (p.x > signalNamesWidth-10 &&
                pt.x >= axisStop-5 &&
                p.y >= DistanceToTimeline - 5 &&
                pt.x <= axisStop+5 &&
                p.y <= DistanceToTimeline + 5 )
            {
                newstate = EditAxisRight;
                editingValA = doc->length;
                editingValB = editingValA;
                dorefresh = true;
                break;
            }
            /// check if user clicked the start of marked axis
            if (p.x > signalNamesWidth-10 &&
                pt.x >= axisStart-5 &&
                p.y >= DistanceToTimeline - 5 &&
                pt.x <= axisStart+5 &&
                p.y <= DistanceToTimeline + 5)
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
            if (p.x > signalNamesWidth-10 &&
                p.y > heightOffsets[0]-5 &&
                pt.x >= VLineOffsets[editingNumber].x - 3 &&
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
            if (p.x > signalNamesWidth-10 &&
                p.y > heightOffsets[0]-5 &&
                pt.x >= VLineOffsets[editingNumber].x - 3 &&
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
            if (p.x > signalNamesWidth-10 &&
                p.y > heightOffsets[0]-5 &&
                pt.x >= VLineOffsets[editingNumber].x - 3 &&
                pt.x <= VLineOffsets[editingNumber].x + 3 &&
                pt.y >= VLineOffsets[editingNumber].y + 3 &&
                pt.y <= VLineToOffset[editingNumber] - 3 )
            {
                newstate = MovingVLine;
                editingValA = doc->vertlines[editingNumber].vpos;
                if ( doc->en50 || doc->en90 )
                    editingValB = doc->vertlines[editingNumber].vposoffset;
                else
                    editingValB = 0;
                dorefresh = true;
                break;
            }
            /// check another click on vline
            SetNeutralState();
            OnMouseLeftDown(event);
            return;
        case SignalIsSelected:
            /// check change of space around signal
            if (p.x > signalNamesWidth-10 &&
                p.y > heightOffsets[0]-5 &&
                pt.x >= editingPoint[0].x - 4 &&
                pt.y >= editingPoint[0].y - 4 &&
                pt.x <= editingPoint[0].x + 4 &&
                pt.y <= editingPoint[0].y + 4)
            {
                newstate = ChangingLowerSpace;
                editingValA = doc->signals[editingNumber].space;
                break;
            }
            if (p.x > signalNamesWidth-10 &&
                p.y > heightOffsets[0]-5 &&
                pt.x >= editingPoint[1].x - 4 &&
                pt.y >= editingPoint[1].y - 4 &&
                pt.x <= editingPoint[1].x + 4 &&
                pt.y <= editingPoint[1].y + 4)
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
            if (p.x > signalNamesWidth-10 &&
                pt.x > axisStart && pt.x < axisStop
                //&& pt.y > DistanceToTimeline - 5 && pt.y < DistanceToTimeline + 5
                //&& pt.y < heightOffsets[heightOffsets.size()-1] + 10
               )
            {
                wxCoord p = pt.x-axisStart;
                wxInt32 discontpos = p / (GridStepWidth);
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                if ( doc->discontinuities.find(discontpos) == doc->discontinuities.end() )
                    cmdproc->Submit(new AddDiscontCommand(doc, discontpos) );
                else
                {
                    editingValA = discontpos;
                    DeleteDiscont();
                }
                dorefresh = true;
            }
            break;
        case InsertingVLineWaitingFirstPoint:
            {
                wxInt32 n = doc->signals.size();
                if ( n && // some signals in the document
                    p.x > signalNamesWidth-10 && p.y > heightOffsets[0]-5 &&
                    pt.x > axisStart &&
                    pt.x < axisStop )
                {
                    editingPoint[0].x = (pt.x - axisStart)/GridStepWidth;
                    wxCoord p = pt.x - axisStart - editingPoint[0].x*GridStepWidth;
                    if ( doc->en50 && doc->en90 )
                    {
                        if      ( p < GridStepWidth*(doc->TransitWidth/4.0)/100.0 )
                            editingValB = 0;
                        else if ( p < GridStepWidth*((3.0*doc->TransitWidth/4.0)/100.0) )
                            editingValB = 1;
                        else if ( p < GridStepWidth*(50.0+doc->TransitWidth/2.0)/100.0 )
                            editingValB = 2;
                        else
                        {
                            editingPoint[0].x++;
                            editingValB = 0;
                        }
                    }
                    else if ( doc->en50 )
                    {
                        if ( p < GridStepWidth*(doc->TransitWidth/4.0)/100.0 )
                            editingValB = 0;
                        else if ( p < GridStepWidth *((50.0+doc->TransitWidth/4.0)/100.0) )
                            editingValB = 1;
                        else
                        {
                            editingPoint[0].x++;
                            editingValB = 0;
                        }
                    }
                    else if ( doc->en90 )
                    {
                        if ( p < GridStepWidth*(doc->TransitWidth/2.0)/100.0 )
                            editingValB = 0;
                        else if ( p < GridStepWidth * (50.0+doc->TransitWidth/2.0) / 100.0 )
                            editingValB = 2;
                        else
                        {
                            editingPoint[0].x++;
                            editingValB = 0;
                        }
                    }
                    else
                    {

                        if ( p > GridStepWidth/2 )
                            editingPoint[0].x++;
                        editingValB = 0;
                    }

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
            for ( wxUint32 k = 0 ; k < VLineOffsets.size() ; ++k )
            {
                if (p.x > signalNamesWidth-10 && p.y > heightOffsets[0]-5 &&
                    pt.x >= VLineOffsets[k].x - 3 &&
                    pt.x <= VLineOffsets[k].x + 3 &&
                    pt.y >= VLineOffsets[k].y &&
                    pt.y <= VLineToOffset[k] )
                {
                    newstate = InsertingHArrowWaitingSecondPoint;
                    editingNumber = k;
                    //editingValA = pt.y - DistanceToTimeline - DistanceFromTimeline;
                    for ( editingValC = 0 ; editingValC < (wxInt32)heightOffsets.size() ; ++editingValC )
                    {
                        if ( pt.y < heightOffsets[editingValC] )
                        {
                            editingValC--;
                            editingValA = pt.y - heightOffsets[editingValC];
                            break;
                        }
                    }
                    editingValB = - 1;
                    dorefresh = true;
                    break;
                }
            }
            break;
        case HArrowIsMarked:
            /// check click on start of harrow (left)
            if (p.x > signalNamesWidth-10 && p.y > heightOffsets[0]-5 &&
                pt.x >= HArrowOffsets[editingNumber].x - 3 &&
                pt.x <= HArrowOffsets[editingNumber].x + 3 &&
                pt.y >= HArrowOffsets[editingNumber].y - 3 &&
                pt.y <= HArrowOffsets[editingNumber].y + 3)
            {
                if ( HArrowOffsets[editingNumber].x ==
                    signalNamesWidth + doc->vertlines[doc->harrows[editingNumber].fromVLine].vpos *
                        GridStepWidth )
                    newstate = ChangingHArrowLengthLeft;
                else
                    newstate = ChangingHArrowLengthRight;
                editingValA = doc->harrows[editingNumber].fromVLine;
                editingValB = doc->harrows[editingNumber].toVLine;



                dorefresh = true;
                break;
            }
            /// check click on end of harrow (right)
            if (p.x > signalNamesWidth-10 && p.y > heightOffsets[0]-5 &&
                pt.x >= HArrowToOffset[editingNumber] - 3 &&
                pt.x <= HArrowToOffset[editingNumber] + 3 &&
                pt.y >= HArrowOffsets[editingNumber].y - 3 &&
                pt.y <= HArrowOffsets[editingNumber].y + 3)
            {
                if ( HArrowToOffset[editingNumber] ==
                    signalNamesWidth + doc->vertlines[doc->harrows[editingNumber].toVLine].vpos *
                        GridStepWidth )
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
                wxCoord x = (HArrowOffsets[editingNumber].x + HArrowToOffset[editingNumber])/2;
                if ( doc->harrows[editingNumber].textoffset.x > GridStepWidth ) x += GridStepWidth;
                else  x += doc->harrows[editingNumber].textoffset.x;
                x += doc->harrows[editingNumber].textgridoffset*GridStepWidth;
                wxCoord y = HArrowOffsets[editingNumber].y + doc->harrows[editingNumber].textoffset.y;
                if ( p.x > signalNamesWidth-10 && p.y > heightOffsets[0]-5 &&
                     pt.x >=  x  - 3 &&
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
            if ( p.x > signalNamesWidth-10 && p.y > heightOffsets[0]-5 &&
                 pt.x >= HArrowOffsets[editingNumber].x - 3 &&
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
            if (//p.x > signalNamesWidth-10 && p.y > heightOffsets[0]-5 &&
                pt.x > textOffsets[editingNumber].x &&
                pt.x < textOffsets[editingNumber].x + textSizes[editingNumber].x &&
                pt.y > textOffsets[editingNumber].y &&
                pt.y < textOffsets[editingNumber].y + textSizes[editingNumber].y &&
                textvisible[editingNumber]
               )
            {
                /// calc with of the texts
                wxArrayInt widths;
                dc.GetPartialTextExtents(editingText, widths);
                widths.Insert(0, 0);

                /// serch position of click in text
                wxUint32 k;
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
                    if ( //p.x > signalNamesWidth-10 && p.y > heightOffsets[0]-5 &&
                         pt.x > textOffsets[n].x &&
                         pt.x < textOffsets[n].x + textSizes[n].x &&
                         pt.y > textOffsets[n].y &&
                         pt.y < textOffsets[n].y + textSizes[n].y &&
                         textvisible[n] )
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
                        wxUint32 k;
                        for ( k = 0 ; k < widths.GetCount()-1; ++k )
                            if ( pt.x <= textOffsets[n].x + (widths[k] + widths[k+1] )/2 )
                                break;
                        editingValA = (wxInt32)k;
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
    if ( newstate == SignalIsSelected && doc->signals[editingNumber].IsClock )
        UpdateClockPanel();

    if ( dorefresh ) this->Refresh(true);
    return;
}
//p.x > signalNamesWidth-10 && p.y > heightOffsets[0]-5 &&
void TimingWindow::OnMouseLeftDClick(wxMouseEvent &event)
{

    if (!view) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    SetFocus();

    wxClientDC dc(this);
    DoPrepareDC(dc);
    dc.SetFont(font);
    wxPoint pt(event.GetLogicalPosition(dc));

    bool dorefresh = false;

    if ( WindowState == TextFieldSelected )
    {
        bool clickedText = false;
        for ( wxInt32 n = textOffsets.size() - 1 ; n >= 0 ; n-- )
        {
            if ( pt.x > textOffsets[n].x &&
                pt.x < textOffsets[n].x + textSizes[n].x &&
                pt.y > textOffsets[n].y &&
                pt.y < textOffsets[n].y + textSizes[n].y &&
                textvisible[n])
            {
                // clicked into textfield
                editingNumber = n;
                editingText = texts[n];
                dorefresh = true;
                clickedText = true;

                if ( !caret->IsVisible() ) caret->Show();
                caret->SetSize(1, dc.GetCharHeight());
                //select all text
                editingValA = 0;
                editingValC = editingText.Len();
                editingValB = 0;
                break;
            }
        }
    }
    // text field is selected after first half of dclick
    //else if ( WindowState == SelectingText ){}
//    else
//    {
//        OnMouseLeftDown(event);
//        return;
//    }

    if ( dorefresh ) this->Refresh(true);
    return;
}
/// moving of objects or changing length (moving start/end) of objects
void TimingWindow::OnMouseLeftUp(wxMouseEvent &event)
{
    if (!view) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    SetFocus();

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
        case DisconSelected:
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
            if (editingValA != -1 &&
                editingNumber != editingValA &&
                editingNumber +1 != editingValA)
            {
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new MoveSignalPosCommand(
                    doc, editingNumber, editingValA)
                );
            }
            ///newstate = SignalIsSelected;
            newstate = Neutral;
            if (editingValA <=  editingNumber )
                editingNumber = editingValA;
            else if ( editingValA > editingNumber + 1)
                editingNumber = editingValA - 1;
            //SendEvent(_T("SignalSelected"));
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
                if ( !doc->en50 && editingValB == 1 ) editingValB = 0;
                if ( !doc->en90 && editingValB == 2 ) editingValB = 0;
                newline.vposoffset = editingValB;
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
                    doc->vertlines[editingNumber].EndPos,
                    doc->vertlines[editingNumber].vposoffset)
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
                    editingValA-1,
                    doc->vertlines[editingNumber].vposoffset)
                );
            }
            newstate = VLineIsMarked;
            break;
        case MovingVLine:
            if ( editingValA != doc->vertlines[editingNumber].vpos ||
                 editingValB != doc->vertlines[editingNumber].vposoffset )
            {
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                if ( !doc->en50 && editingValB == 1 ) editingValB = 0;
                if ( !doc->en90 && editingValB == 2 ) editingValB = 0;
                cmdproc->Submit(new ChangeVLineCommand(doc, editingNumber,
                    editingValA,
                    doc->vertlines[editingNumber].StartPos,
                    doc->vertlines[editingNumber].EndPos,
                    editingValB)
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
                newha.signalnmbr = editingValC;

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
                    editingValA, editingValB, doc->harrows[editingNumber].fromVLine, doc->harrows[editingNumber].toVLine)
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
                    doc->harrows[editingNumber].signalnmbr,
                    editingValA, editingValB)
                );
            }
            newstate = HArrowIsMarked;
            break;
        case HArrowMovingText:
            //if ( editingValA != -1 && editingValB != -1 )
            {
                wxCoord gridoff = editingValA / GridStepWidth;
                wxCoord off     = editingValA - GridStepWidth * gridoff;
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new ChangeHArrowTextPosCommand(doc, editingNumber,
                    off,//editingValA, // the new x offset
                    editingValB, // the new y offset
                    gridoff
                ));
            }
            newstate = HArrowIsMarked;
            break;
    }
    WindowState = newstate;
    if ( newstate == Neutral )
        SetNeutralState();
    Refresh(true);
}

void TimingWindow::SetNeutralState(void)
{
    WindowState = Neutral;
    if ( caret->IsVisible() ) caret->Show(false);
    editingTextIsVisible = false;
    scrollingleft = false;
    scrollingright = false;
    scrolltimer.Stop();
    editingNumber = -1;
    editingValA = -1;
    editingValB = -1;
    editingValC = -1;
}

void TimingWindow::OnMouseRightDown(wxMouseEvent& event)
{
    if (!view) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    SetFocus();

    wxClientDC dc(this);
    DoPrepareDC(dc);
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
        if ( caret->IsVisible() ) caret->Show(false);
        WindowState = SelectingText;
        editingValA = -1;
        editingValB = -1;
        editingValC = -1;
        this->Refresh(true);
        return;
    }

    if ( WindowState != Neutral ) dorefresh = true;
    SetNeutralState();

    editingNumber = -1;
    wxPoint unscrolledPosition;
    CalcUnscrolledPosition(0, 0, &unscrolledPosition.x, &unscrolledPosition.y);
    for ( wxUint32 n = 0 ; n < doc->signals.size() ; ++n )
    {
        if ( pt.x >= manipXoffset - 2*manipRadius + unscrolledPosition.x &&
            pt.y >= heightOffsets[n]+doc->MinimumSignalDistance/2 + doc->SignalHeight/2 + doc->signals[n].prespace - 2*manipRadius &&
            pt.x <=  manipXoffset + 2*manipRadius + unscrolledPosition.x &&
            pt.y <= heightOffsets[n]+doc->MinimumSignalDistance/2 + doc->SignalHeight/2 + doc->signals[n].prespace + 2*manipRadius)
        {
            editingNumber = n;
            WindowState = SignalIsSelected;
            if ( doc->signals[n].IsClock ) UpdateClockPanel();

            wxMenu menu;
//            if ( doc->signals[n].IsClock )
//                menu.Append(TIMING_ID_CHANGECLOCK, _T("Change clock settings"));
            menu.Append(TIMING_ID_DELETE, _T("Delet signal"));
            PopupMenu(&menu);
            break;
        }
    }

    if ( dorefresh ) this->Refresh(true);
    return;

}

void TimingWindow::CheckHScroll(wxPoint pos)
{
    wxSize clientSize;
    clientSize = this->GetClientSize();

    if ( pos.x <= signalNamesWidth )
    {
        scrollingleft = true;
        scrolltimer.Start(200, wxTIMER_ONE_SHOT);
    }
    else if ( (pos.x - clientSize.x) >=-5 )
    {
        scrollingright = true;
        scrolltimer.Start(200, wxTIMER_ONE_SHOT);
    }
}
void TimingWindow::CheckVScroll(wxPoint pos)
{
    wxSize clientSize;
    clientSize = this->GetClientSize();

    if ( pos.y <= heightOffsets[0] )
    {
        scrollingup = true;
        scrolltimer.Start(200, wxTIMER_ONE_SHOT);
    }
    else if ( (pos.y -clientSize.y) >= -5 )
    {
        scrollingdown = true;
        scrolltimer.Start(200, wxTIMER_ONE_SHOT);
    }
}
void TimingWindow::OnMouseMove(wxMouseEvent &event)
{
    if ( !view ) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    wxClientDC dc(this);
    DoPrepareDC(dc);
    dc.SetFont(font);
    wxPoint pt(event.GetLogicalPosition(dc));

    bool dorefresh = false;

    wxPoint textOffset(0,0);

    states newstate = WindowState;
    scrollingleft = false;
    scrollingright = false;
    scrollingup = false;
    scrollingdown = false;
    scrolltimer.Stop();
    wxPoint pos(event.GetPosition());
    switch ( WindowState )
    {
        default:
        case Neutral:
        case AxisIsMarked:
        case EditingDiscontinuity:
        case InsertingVLineWaitingFirstPoint:
        case InsertingHArrowWaitingFirstPoint:
        case DisconSelected:
        case SelectingText:
            break;
        case EditAxisLeft:
            if ( event.LeftIsDown() )
            {
                wxCoord p = pt.x - axisStart;
                if ( pt.x > axisStop )
                    editingValB = editingValA;
                else
                    editingValB = editingValA - p / (GridStepWidth);
                CheckHScroll(pos);
            }
            else
                newstate = AxisIsMarked;
            break;
        /// the user is changing the length of the time axis
        case EditAxisRight:
            if ( event.LeftIsDown()  )
            {
                wxCoord p = pt.x-axisStart;
                if ( pt.x < axisStart )
                    editingValB = editingValA;
                else
                    editingValB = p / (GridStepWidth);
                CheckHScroll(pos);
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
                    pt.y <= heightOffsets[editingNumber] + doc->signals[editingNumber].prespace + doc->SignalHeight && pt.x <= axisStop )
                {
                    wxCoord p = pt.x - axisStart;
                    editingSignalPosStop = p / (GridStepWidth);
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
            if ( event.LeftIsDown() )
            {
                wxUint32 n;
                for ( n = 0 ; n < doc->signals.size() ; ++n)
                    if ( pt.y < (heightOffsets[n] + heightOffsets[n+1])/2 )
                        break;
                if ( editingNumber>=0 && (wxUint32)editingNumber != n && (wxUint32)editingNumber +1 != n )
                {
                    if ( editingValA != (wxInt32)n )
                        dorefresh = true;
                    editingValA = (wxInt32)n;
                }
                else
                {
                    if ( editingValA != -1 )
                        dorefresh = true;
                    editingValA = -1;
                }
                CheckVScroll(pos);
            }
            break;
        case ChangingUpperSpace:
            if ( event.LeftIsDown() )
            {
                if ( pt.y >= ( heightOffsets[editingNumber] + doc->signals[editingNumber].prespace ) )
                    editingValA = 0;
                else
                    editingValA = ( heightOffsets[editingNumber] + doc->signals[editingNumber].prespace) - pt.y;
                CheckVScroll(pos);
            }
            break;
        case ChangingLowerSpace:
            if ( event.LeftIsDown() )
            {
                if ( pt.y <= (heightOffsets[editingNumber] + doc->signals[editingNumber].prespace +
                                doc->SignalHeight + doc->MinimumSignalDistance ) )
                    editingValA = 0;
                else
                    editingValA = pt.y - (heightOffsets[editingNumber] + doc->signals[editingNumber].prespace +
                                doc->SignalHeight + doc->MinimumSignalDistance );
                CheckVScroll(pos);
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
                    wxCoord p = pt.x-axisStart - (GridStepWidth)/2;
                    editingPoint[1].x = p / (GridStepWidth);

                    wxInt32 k;
                    for ( k = 0 ; k < n ; ++k )
                        if ( pt.y < (heightOffsets[k] + heightOffsets[k+1])/2 )
                            break;

                    if ( editingPoint[1].y != k )
                        dorefresh = true;
                    editingPoint[1].y = k;
                    CheckVScroll(pos);
                }
            }
            break;
        case ChangingVLineLengthUpper:
        case ChangingVLineLengthLower:
            {
                wxInt32 n = doc->signals.size();
                if ( n && pt.x > axisStart && pt.x < axisStop )
                {
                    wxCoord p = pt.x-axisStart + (GridStepWidth)/2;
                    editingPoint[1].x = p / (GridStepWidth);

                    wxInt32 kmax = n, kmin = 0;
                    for ( wxUint32 j = 0 ; j < doc->harrows.size() ; ++j )
                    {
                        if ( editingNumber == doc->harrows[j].fromVLine ||
                             editingNumber == doc->harrows[j].toVLine )
                        {
                            wxInt32 off =
                            doc->harrows[j].pos + heightOffsets[doc->harrows[j].signalnmbr];

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
                CheckVScroll(pos);
            }
            break;
        case VLineIsMarked:
            if ( event.LeftIsDown() )
                newstate = MovingVLine;
        case MovingVLine:
            if ( event.LeftIsDown() )
            {

                if ( pt.x < axisStart || pt.x > axisStop )
                {
                    editingValA = doc->vertlines[editingNumber].vpos;
                    editingValB = doc->vertlines[editingNumber].vposoffset;
                }
                else
                {
                    editingValA = (pt.x - axisStart)/GridStepWidth;
                    wxCoord p = pt.x - axisStart - editingValA*GridStepWidth;
                    if ( doc->en50 && doc->en90 )
                    {
                        if ( p < GridStepWidth*(doc->TransitWidth/4.0)/100.0 )
                            editingValB = 0;
                        else if ( p < GridStepWidth*((3.0*doc->TransitWidth/4.0)/100.0) )
                            editingValB = 1;
                        else if ( p < GridStepWidth*(50.0+doc->TransitWidth/2.0)/100.0 )
                            editingValB = 2;
                        else
                        {
                            editingValA++;
                            editingValB = 0;
                        }
                    }
                    else if ( doc->en50 )
                    {
                        if ( p < GridStepWidth*(doc->TransitWidth/4.0)/100.0 )
                            editingValB = 0;
                        else if ( p < GridStepWidth *((50.0+doc->TransitWidth/4.0)/100.0) )
                            editingValB = 1;
                        else
                        {
                            editingValA++;
                            editingValB = 0;
                        }
                    }
                    else if ( doc->en90 )
                    {
                        if ( p < GridStepWidth*(doc->TransitWidth/2.0)/100.0 )
                            editingValB = 0;
                        else if ( p < GridStepWidth * (50.0+doc->TransitWidth/2.0) / 100.0 )
                            editingValB = 2;
                        else
                        {
                            editingValA++;
                            editingValB = 0;
                        }
                    }
                    else
                    {
                        if ( p > GridStepWidth/2 )
                            editingValA++;
                        editingValB = 0;
                    }
                }
                CheckHScroll(pos);
            }
            break;
        case HArrowIsMarked:
            if ( event.LeftIsDown() )
                newstate = MovingHArrow;
        case MovingHArrow:
            if ( event.LeftIsDown() )
            {
                wxCoord top, bottom, tmp;
                wxInt32 topline;
                wxInt32 bottomline;
                top = heightOffsets[doc->vertlines[doc->harrows[editingNumber].fromVLine].StartPos];
                topline = doc->vertlines[doc->harrows[editingNumber].fromVLine].StartPos;
                tmp = heightOffsets[doc->vertlines[doc->harrows[editingNumber].toVLine].StartPos];
                if ( tmp > top )
                {
                    top = tmp;
                    topline = doc->vertlines[doc->harrows[editingNumber].toVLine].StartPos;
                }
                bottom = heightOffsets[doc->vertlines[doc->harrows[editingNumber].fromVLine].EndPos+1];
                bottomline = doc->vertlines[doc->harrows[editingNumber].fromVLine].EndPos;
                tmp = heightOffsets[doc->vertlines[doc->harrows[editingNumber].toVLine].EndPos+1];
                if ( tmp < bottom )
                {
                    bottom = tmp;
                    bottomline = doc->vertlines[doc->harrows[editingNumber].toVLine].EndPos;
                }

                if ( pt.y < top )
                {
                    editingValA = 0;
                    editingValB = topline;
                }
                else if ( pt.y > bottom )
                {
                    editingValA = bottom-heightOffsets[bottomline];
                    editingValB = bottomline;
                }
                else
                {
                    wxInt32 k = 0;
                    for ( k = topline; k <= bottomline ; k++)
                    {
                        if ( heightOffsets[k+1] > pt.y )
                            break;
                    }
                    editingValB = k;
                    editingValA = pt.y - heightOffsets[editingValB];
                }
                CheckVScroll(pos);
            }
            break;
        case InsertingHArrowWaitingSecondPoint:
            /// check if user mover over a vertical line
            for ( wxUint32 k = 0 ; k < VLineOffsets.size() ; ++k )
            {
                //wxInt32 pos = DistanceToTimeline + DistanceFromTimeline + editingValA;
                wxInt32 pos = heightOffsets[editingValC] + editingValA;
                if (pt.x >= VLineOffsets[k].x - 3 &&
                    pt.x <= VLineOffsets[k].x + 3 &&
                    pt.y >= VLineOffsets[k].y &&
                    pt.y <= VLineToOffset[k] &&
                    pos >= VLineOffsets[k].y &&
                    pos <= VLineToOffset[k] )
                {
                    editingValB = k;
                    break;
                }
                else
                    editingValB = -1;
            }
            CheckHScroll(pos);
            dorefresh = true;
            break;

        case ChangingHArrowLengthLeft:
        case ChangingHArrowLengthRight:
            for ( wxUint32 k = 0 ; k < VLineOffsets.size() ; ++k )
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
            CheckHScroll(pos);
            break;
        case HArrowMovingText:
            if ( event.LeftIsDown() )
            {
                if ( pt.x > axisStart && pt.x < axisStop &&
                     pt.y > heightOffsets[0]+dc.GetCharHeight() && pt.y < heightOffsets[heightOffsets.size()-1]
                   )
                {
                    editingValA = pt.x - (HArrowOffsets[editingNumber].x + HArrowToOffset[editingNumber])/2;
                    editingValB = pt.y - HArrowOffsets[editingNumber].y;
                }
            }
            CheckHScroll(pos);
            CheckVScroll(pos);
            break;
        case TextFieldSelected:
            if ( event.LeftIsDown() && textvisible[editingNumber] )
            {
                wxString text = editingText;
                /// calc width of the texts
                wxArrayInt widths;
                dc.GetPartialTextExtents(text, widths);
                widths.Insert(0, 0);
                if ( editingValB == -1 )
                    editingValB = editingValA;

                ///get position of cursor in the text and move the cursor to this place
                wxUint32 k = (wxUint32)editingValA;
                for ( k = 0 ; k < widths.GetCount() - 1 ; ++k )
                    if ( pt.x <= textOffsets[editingNumber].x + (widths[k] + widths[k+1] )/2 )
                        break;
                editingValA = (wxInt32)k;
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

    if ( dorefresh ) this->Refresh(true);
    return;
}

void TimingWindow::OnMouseWheel(wxMouseEvent& event)
{
    if (!view)
        return;

    wxClientDC dc(this);
    DoPrepareDC(dc);

    dc.SetFont(font);
    wxCoord dx, dy;
    dx = dc.GetCharWidth();
    dy = dc.GetCharHeight();

    if ( ! event.IsPageScroll() )
    {
        wxInt32 nWheelRotation = event.GetWheelRotation();
        wxInt32 x, y,yy;
        GetViewStart(&x, &y);
        if (nWheelRotation < 0)
            yy = dy/4;
        else
            yy = -dy/4;
        y += yy;
        Scroll(x, y);
        //cursorpos.y += yy;
        this->Refresh();
    }
}
/// //////////////unused event methods//////////////////////////////////////////
void TimingWindow::OnMouseRightUp(wxMouseEvent& event){}
//void TimingWindow::OnSetFocus(wxFocusEvent &event){}
//void TimingWindow::OnKillFocus(wxFocusEvent &event){}
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
        case ChangingVLineLengthLower:
        case ChangingVLineLengthUpper:
        case EditAxisLeft:
        case EditAxisRight:
        case EditSignal:
        case MovingSignal:
        case ChangingUpperSpace:
        case ChangingLowerSpace:
        case ChangingHArrowLengthLeft:
        case ChangingHArrowLengthRight:
        case HArrowMovingText:
        case MovingHArrow:
        case MovingVLine:
            if ( event.GetKeyCode() == WXK_ESCAPE )
            {
                SetNeutralState();
                Refresh(true);
            }
            break;
        case DisconSelected:
        case VLineIsMarked:
        case AxisIsMarked:
        case HArrowIsMarked:
            if ( event.GetKeyCode() == WXK_ESCAPE )
            {
                SetNeutralState();
                Refresh(true);
            }
            else if ( event.GetKeyCode() == WXK_DELETE )
            {
                DeleteSelection();
                Refresh(true);
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
                            UpdateClockPanel();
                            this->Refresh(true);
                        }
                        break;
                    case WXK_END:
                        if ( editingNumber != n-1 )
                        {
                            editingNumber = n-1;
                            UpdateClockPanel();
                            this->Refresh(true);
                        }
                        break;
                    case WXK_UP:
                        if ( editingNumber != 0)
                        {
                            --editingNumber;
                            UpdateClockPanel();
                            this->Refresh(true);
                        }
                        break;
                    case WXK_DOWN:
                        if ( editingNumber < n-1 )
                        {
                            ++editingNumber;
                            UpdateClockPanel();
                            this->Refresh(true);
                        }
                        break;
                    case WXK_DELETE:
                        DeleteSelection();
                    case WXK_ESCAPE:
                        WindowState = Neutral;
                        scrollingleft = false;
                        scrollingright = false;
                        scrolltimer.Stop();
                        editingValA = -1;
                        editingValB = -1;
                        this->Refresh(true);
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
                Refresh(true);
            }
            break;
        case TextFieldSelected:
            {
                //MyCommandProcessor *cmdproc = (MyCommandProcessor *)doc->GetCommandProcessor();
                wxString ch;
                if ( editingTextIsVisible )
                {
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
                            Refresh(true);
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
                            this->Refresh(true);
                            return;
                        }
                        case WXK_END:
                        {
                            if ( event.ShiftDown() && (wxUint32)editingValA < editingText.Length() )
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
                            Refresh(true);
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
                                Refresh(true);
                            }
                            return;
                        }
                        case WXK_RIGHT:
                        {
                            if ( (wxUint32)editingValA < editingText.Length() )
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
                                Refresh(true);
                            }
                            return;
                        }
                        case WXK_DELETE:
                        {
                            DeleteSelection();
                            return;
                        }
                        case WXK_UP: return;
                        case WXK_DOWN: return;
                        case WXK_RETURN:
                        case WXK_NUMPAD_ENTER:
                        {
                            /// generate the command to change the text
                            wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                            cmdproc->Submit(
                                new ChangeTextCommand(doc, editingNumber, editingText)
                            );
                            editingNumber = -1;
                            editingValA = -1;
                            editingValB = -1;
                            editingValC = -1;
                            if ( caret->IsVisible() ) caret->Show(false);
                            WindowState = SelectingText;
                            return;
                        }
                        case WXK_ESCAPE:
                        {
                            editingValA = -1;
                            editingValB = -1;
                            editingValC = -1;
                            if ( caret->IsVisible() ) caret->Hide();
                            editingTextIsVisible = false;
                            editingNumber = -1;
                            /// /////////////////////////////////////////////////////////////////////////////////////////////////////////////
                            WindowState = SelectingText;
                            Refresh(true);
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
                    this->Refresh(true);
                }
                else{
                    if ( event.GetKeyCode() == WXK_ESCAPE )
                    {
                        editingValA = -1;
                        editingValB = -1;
                        editingValC = -1;
                        if ( caret->IsVisible() ) caret->Hide();
                        editingTextIsVisible = false;
                        editingNumber = -1;
                        /// /////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        WindowState = SelectingText;
                        Refresh(true);
                        return;
                    }
                }
            }
            break;
        //case ...
    }
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

    SetNeutralState();
    WindowState = EditingDiscontinuity;
    editingNumber = -1;
    scrollingleft = false;
    scrollingright = false;
    scrolltimer.Stop();

    this->Refresh(true);
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

    SetNeutralState();
    WindowState = InsertingVLineWaitingFirstPoint;
    editingNumber = -1;
    scrollingleft = false;
    scrollingright = false;
    scrolltimer.Stop();

    this->Refresh(true);
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

    SetNeutralState();
    WindowState = InsertingHArrowWaitingFirstPoint;
    editingNumber = -1;
    scrollingleft = false;
    scrollingright = false;
    scrolltimer.Stop();

    this->Refresh(true);
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

    SetNeutralState();
    WindowState = SelectingText;
    editingNumber = -1;
    scrollingleft = false;
    scrollingright = false;
    scrolltimer.Stop();

    this->Refresh(true);
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

    Refresh(true);

}

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
        Refresh(true);
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

    if ( WindowState != TextFieldSelected || !editingTextIsVisible) return;

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

    this->Refresh(true);
}
void TimingWindow::SelectAll(void)
{
    if ( WindowState != TextFieldSelected ) return;

    editingValB = 0;
    editingValC = editingText.Length();
    this->Refresh(true);
}
bool TimingWindow::CanDeleteText(void)
{
    if (!view) return false;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return false;

    if ( IsTextSelected() ) return true;

    if ( WindowState == TextFieldSelected  && (wxUint32)editingValA < editingText.Len() && editingValA >= 0 )
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
        Refresh(true);
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
        Refresh(true);
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
        Refresh(true);
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
            SetNeutralState();
            break;
        case VLineIsMarked:
            DeleteVLine();
            SetNeutralState();
            break;
        case HArrowIsMarked:
            DeleteHArrow();
            SetNeutralState();
            break;
        case DisconSelected:
            DeleteDiscont();
            SetNeutralState();
            break;
        case TextFieldSelected:
            DeleteText();
            break;
        default:
            break;
    }
}

void TimingWindow::DeleteDiscont(void)
{
    if (!view) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    if ( editingValA != -1 )
    {
        wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
        cmdproc->Submit(new RemoveDiscontCommand(doc, editingValA));
    }
}

void TimingWindow::OnDragEnter(void)
{
    /*drop = true;
    dropcaret->Show();
    Refresh(true);*/
}
void TimingWindow::OnDragLeave(void)
{
    /*drop = false;
    dropcaret->Hide();
    Refresh(true);*/
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
    DoPrepareDC(dc);
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
            this->Refresh(true);
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
    this->Refresh(true);
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
    Refresh(true);
    /// /////////////////////////////////////////////////////////////////////////////////////////////////
    return true;
}
void TimingWindow::OnTimer(wxTimerEvent& event)
{
    if ( !view ) return;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    wxClientDC dc(this);
    DoPrepareDC(dc);

    if ( scrollingleft || scrollingright || scrollingup || scrollingdown )
    {
        wxSize virtsize = GetVirtualSize();

        wxCoord startx, starty;
        this->GetViewStart(&startx, &starty);

        wxCoord dx = 0, dy = 0;
        if ( scrollingleft || scrollingright ) dx = 5;//dx = dc.GetCharWidth();
        if ( scrollingup || scrollingdown ) dy = 5;//dy = dc.GetCharHeight();

        if (scrollingleft) dx = -dx;
        if (scrollingup) dy = -dy;

        int xu,yu;
        GetScrollPixelsPerUnit(&xu, &yu);
        xu *= dx;
        yu *= dy;
        wxPoint newcursorpos = cursorpos;
        newcursorpos.x += xu;
        newcursorpos.y += yu;

        if ( newcursorpos.x < signalNamesWidth || newcursorpos.x >= virtsize.x )
        {
            newcursorpos.x = cursorpos.x;
            dx = 0;
        }
        if ( newcursorpos.y < 0 || newcursorpos.y > virtsize.y)
        {
            newcursorpos.y = cursorpos.y;
            dy = 0;
        }

        Scroll(startx+dx, starty+dy);

        wxPoint &pt = newcursorpos;
        switch ( WindowState )
        {
            case EditAxisRight:
                editingValB = (newcursorpos.x-axisStart) / GridStepWidth;
                break;
            case EditAxisLeft:
                if ( newcursorpos.x < axisStop )
                    editingValB = editingValA - (newcursorpos.x-axisStart) / (GridStepWidth);
                else
                    editingValB = editingValA;
                break;
            case ChangingUpperSpace:
                if ( newcursorpos.y >= ( heightOffsets[editingNumber] + doc->signals[editingNumber].prespace ) )
                    editingValA = 0;
                else
                    editingValA = ( heightOffsets[editingNumber] + doc->signals[editingNumber].prespace) - newcursorpos.y;
                break;
            case ChangingLowerSpace:
                if ( newcursorpos.y <= (heightOffsets[editingNumber] + doc->signals[editingNumber].prespace +
                                doc->SignalHeight + doc->MinimumSignalDistance ) )
                    editingValA = 0;
                else
                    editingValA = newcursorpos.y - (heightOffsets[editingNumber] + doc->signals[editingNumber].prespace +
                                doc->SignalHeight + doc->MinimumSignalDistance );
                break;
            case MovingSignal:
                {
                    wxUint32 n;
                    for ( n = 0 ; n < doc->signals.size() ; ++n)
                        if ( pt.y < (heightOffsets[n] + heightOffsets[n+1])/2 )
                            break;
                    if ( editingNumber>=0 && (wxUint32)editingNumber != n && (wxUint32)editingNumber +1 != n )
                        editingValA = (wxInt32)n;
                    else
                        editingValA = -1;
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
                        wxCoord p = newcursorpos.x-axisStart - (GridStepWidth)/2;
                        editingPoint[1].x = p / (GridStepWidth);

                        wxInt32 k;
                        for ( k = 0 ; k < n ; ++k )
                            if ( pt.y < (heightOffsets[k] + heightOffsets[k+1])/2 )
                                break;
                        editingPoint[1].y = k;
                    }
                }
                break;
            case ChangingVLineLengthUpper:
            case ChangingVLineLengthLower:
                {
                    wxInt32 n = doc->signals.size();
                    if ( n && pt.x > axisStart && pt.x < axisStop )
                    {
                        wxCoord p = newcursorpos.x-axisStart + (GridStepWidth)/2;
                        editingPoint[1].x = p / (GridStepWidth);

                        wxInt32 kmax = n, kmin = 0;
                        for ( wxUint32 j = 0 ; j < doc->harrows.size() ; ++j )
                        {
                            if ( editingNumber == doc->harrows[j].fromVLine ||
                                 editingNumber == doc->harrows[j].toVLine )
                            {
                                wxInt32 off =
                                doc->harrows[j].pos + heightOffsets[doc->harrows[j].signalnmbr];

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
                            if ( newcursorpos.y < (heightOffsets[k] + heightOffsets[k+1])/2 )
                                break;
                        bool AllowedMove = true;
                        if ( WindowState == ChangingVLineLengthUpper )
                            if ( k > kmax ) AllowedMove = false;
                        else
                            if ( k < kmin ) AllowedMove = false;
                        if ( editingValA != k && AllowedMove )
                            editingValA = k;
                    }
                }
                break;
            case MovingVLine:
                if ( pt.x < axisStart || pt.x > axisStop )
                {
                    editingValA = doc->vertlines[editingNumber].vpos;
                    editingValB = doc->vertlines[editingNumber].vposoffset;
                }
                else
                {
                    editingValA = (pt.x - axisStart)/GridStepWidth;
                    wxCoord p = pt.x - axisStart - editingValA*GridStepWidth;
                    if ( doc->en50 && doc->en90 )
                    {
                        if ( p < GridStepWidth*(doc->TransitWidth/4.0)/100.0 )
                            editingValB = 0;
                        else if ( p < GridStepWidth*((3.0*doc->TransitWidth/4.0)/100.0) )
                            editingValB = 1;
                        else if ( p < GridStepWidth*(50.0+doc->TransitWidth/2.0)/100.0 )
                            editingValB = 2;
                        else
                        {
                            editingValA++;
                            editingValB = 0;
                        }
                    }
                    else if ( doc->en50 )
                    {
                        if ( p < GridStepWidth*(doc->TransitWidth/4.0)/100.0 )
                            editingValB = 0;
                        else if ( p < GridStepWidth *((50.0+doc->TransitWidth/4.0)/100.0) )
                            editingValB = 1;
                        else
                        {
                            editingValA++;
                            editingValB = 0;
                        }
                    }
                    else if ( doc->en90 )
                    {
                        if ( p < GridStepWidth*(doc->TransitWidth/2.0)/100.0 )
                            editingValB = 0;
                        else if ( p < GridStepWidth * (50.0+doc->TransitWidth/2.0) / 100.0 )
                            editingValB = 2;
                        else
                        {
                            editingValA++;
                            editingValB = 0;
                        }
                    }
                    else
                    {
                        if ( p > GridStepWidth/2 )
                            editingValA++;
                        editingValB = 0;
                    }
                }
            case MovingHArrow:
                {
                    wxCoord top, bottom, tmp;
                    wxInt32 topline;
                    wxInt32 bottomline;
                    top = heightOffsets[doc->vertlines[doc->harrows[editingNumber].fromVLine].StartPos];
                    topline = doc->vertlines[doc->harrows[editingNumber].fromVLine].StartPos;
                    tmp = heightOffsets[doc->vertlines[doc->harrows[editingNumber].toVLine].StartPos];
                    if ( tmp > top )
                    {
                        top = tmp;
                        topline = doc->vertlines[doc->harrows[editingNumber].toVLine].StartPos;
                    }
                    bottom = heightOffsets[doc->vertlines[doc->harrows[editingNumber].fromVLine].EndPos+1];
                    bottomline = doc->vertlines[doc->harrows[editingNumber].fromVLine].EndPos;
                    tmp = heightOffsets[doc->vertlines[doc->harrows[editingNumber].toVLine].EndPos+1];
                    if ( tmp < bottom )
                    {
                        bottom = tmp;
                        bottomline = doc->vertlines[doc->harrows[editingNumber].toVLine].EndPos;
                    }

                    if ( pt.y < top )
                    {
                        editingValA = 0;
                        editingValB = topline;
                    }
                    else if ( pt.y > bottom )
                    {
                        editingValA = bottom-heightOffsets[bottomline];
                        editingValB = bottomline;
                    }
                    else
                    {
                        wxInt32 k = 0;
                        for ( k = topline; k <= bottomline ; k++)
                        {
                            if ( heightOffsets[k+1] > pt.y )
                                break;
                        }
                        editingValB = k;
                        editingValA = pt.y - heightOffsets[editingValB];
                    }
                }
                break;
            case InsertingHArrowWaitingSecondPoint:
                /// check if user mover over a vertical line
                for ( wxUint32 k = 0 ; k < VLineOffsets.size() ; ++k )
                {
                    //wxInt32 pos = DistanceToTimeline + DistanceFromTimeline + editingValA;
                    wxInt32 pos = heightOffsets[editingValC] + editingValA;
                    if (pt.x >= VLineOffsets[k].x - 3 &&
                        pt.x <= VLineOffsets[k].x + 3 &&
                        pt.y >= VLineOffsets[k].y &&
                        pt.y <= VLineToOffset[k] &&
                        pos >= VLineOffsets[k].y &&
                        pos <= VLineToOffset[k] )
                    {
                        editingValB = k;
                        break;
                    }
                    else
                        editingValB = -1;
                }
                break;
            case ChangingHArrowLengthLeft:
            case ChangingHArrowLengthRight:
                for ( wxUint32 k = 0 ; k < VLineOffsets.size() ; ++k )
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
                break;
            case HArrowMovingText:
                if ( pt.x > axisStart && pt.x < axisStop &&
                     pt.y > heightOffsets[0]+dc.GetCharHeight() && pt.y < heightOffsets[heightOffsets.size()-1]
                   )
                {
                    editingValA = pt.x - (HArrowOffsets[editingNumber].x + HArrowToOffset[editingNumber])/2;
                    editingValB = pt.y - HArrowOffsets[editingNumber].y;
                }
                break;
            default:
                break;
        }

        cursorpos = newcursorpos;

        scrolltimer.Start(200,wxTIMER_ONE_SHOT);
        this->Refresh(true);
    }
    else
        scrolltimer.Stop();
}

bool TimingWindow::CanZoomTicksOut(void)
{
    if ( GridStepWidth > 6 ) return true;
    return false;
}
bool TimingWindow::CanZoomTicksIn(void)
{
    if ( GridStepWidth < 150 )
        return true;
    return false;
}

void TimingWindow::ZoomTicksOut(void)
{
    if ( GridStepWidth > 6 )
        GridStepWidth  *= 0.7;
    if ( GridStepWidth < 6 ) GridStepWidth = 6;
    Refresh(true);
}
void TimingWindow::ZoomTicksIn(void)
{
    if ( GridStepWidth < 150 )
        GridStepWidth  *= 1.4;
    if ( GridStepWidth > 150 ) GridStepWidth = 150;
    Refresh(true);
}

bool TimingWindow::DiscontSelected(void)
{
    if ( WindowState == DisconSelected )
        return true;
    return false;
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

wxInt32 TimingWindow::GetSelectedSignalNr()
{
    if ( IsSignalSelected() )
        return editingNumber;
    return -1;
}
bool TimingWindow::IsSelectedSignalClock(void)
{
    if ( !IsSignalSelected() ) return false;

    if (!view) return false;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return false;

    return doc->signals[editingNumber].IsClock;
}

void TimingWindow::DetachPanels()
{
    ClkSetPanel->wnd = (TimingWindow *)NULL;
    TranSetPanel->wnd = (TimingWindow *)NULL;
    AxisSetPanel->wnd = (TimingWindow *)NULL;
    TmeCmprssrPanel->wnd = (TimingWindow *)NULL;
}
void TimingWindow::AttachPanels()
{
    ClkSetPanel->wnd = this;
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    if ( IsSelectedSignalClock() )
        UpdateClockPanel();

    TranSetPanel->wnd = this;
    UpdateTransitionPanel();

    AxisSetPanel->wnd = this;
    UpdateAxisPanel();

    TmeCmprssrPanel->wnd = this;
    UpdateTimeCompressorPanel();
}
void TimingWindow::UpdateClockPanel()
{
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    if ( IsSelectedSignalClock() )
    {
        wxInt32 n;
        wxString str;

        n = doc->signals[editingNumber].delay;
        str = wxString::Format ( _( "%d" ) , n);
        ClkSetPanel->SetDelayText(str);

        n = doc->signals[editingNumber].ticks;
        str = wxString::Format ( _( "%d" ) , n);
        ClkSetPanel->SetTicksText(str);
    }
}
void TimingWindow::SetClock(wxInt32 delay, wxInt32 ticks)
{
    SetFocus();
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;
    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();

    if ( IsSelectedSignalClock() )
    {
        cmdproc->Submit(
            new ChangeClockParamCommand(doc, editingNumber , ticks, delay)
        );
    }
}
void TimingWindow::UpdateAxisPanel()
{
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    AxisSetPanel->SetTickLength(doc->TickLength);
    AxisSetPanel->SetLengthUnit(doc->TickLengthUnit + 5);
    AxisSetPanel->SetTackLength(doc->TackLength);
    AxisSetPanel->SetOffset(doc->timeOffset);
}
void TimingWindow::SetAxis(wxInt8 unit, wxInt32 ticklength, wxInt32 tacklength, wxInt32 offset)
{
    SetFocus();

    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;
    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
    cmdproc->Submit(
        new ChangeAxisSettings(doc, unit-5, ticklength, tacklength, offset)
    );
    UpdateAxisPanel();
}
void TimingWindow::UpdateTransitionPanel()
{
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    TranSetPanel->SetTransitionWidth(doc->TransitWidth);
    TranSetPanel->Set50(doc->en50);
    TranSetPanel->Set90(doc->en90);

}
wxInt8 TimingWindow::GetTransitionWidth()
{
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return 25;

    return doc->TransitWidth;
}
void TimingWindow::SetTransition(wxInt8 width, bool en50, bool en90)
{
    SetFocus();

    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;
    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
    cmdproc->Submit(
        new ChangeTransitionWidth(doc, width, en50, en90)
    );
    UpdateTransitionPanel();
}
void TimingWindow::UpdateTimeCompressorPanel(void)
{
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;

    if ( !DiscontSelected() ) return;
    wxInt32 n = doc->discontlength[editingValA];
    TmeCmprssrPanel->SetTimeText(wxString::Format( _("%d"), n) );
}
void TimingWindow::SetTimeCompressor(wxInt32 time)
{
    SetFocus();
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return;
    if ( !DiscontSelected() ) return;
    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
    cmdproc->Submit(
        new ChangeTimeCompressor(doc, editingValA, time)
    );

    UpdateTimeCompressorPanel();
}

void TimingWindow::OnScroll(wxScrollWinEvent &event)
{
    int nScrollInc = CalcScrollInc(event);
    if ( nScrollInc == 0 )// can't scroll further
    {
        event.Skip();
        return;
    }

    int dx = 0,
        dy = 0;
    int orient = event.GetOrientation();
    if (orient == wxHORIZONTAL)
       if ( m_xScrollingEnabled )
           dx = -m_xScrollPixelsPerLine * nScrollInc;
    else
        if ( m_yScrollingEnabled )
            dy = -m_yScrollPixelsPerLine * nScrollInc;

    if (orient == wxHORIZONTAL)
    {
        m_xScrollPosition += nScrollInc;
        m_win->SetScrollPos(orient, m_xScrollPosition);
    }
    else
    {
        m_yScrollPosition += nScrollInc;
        m_win->SetScrollPos(orient, m_yScrollPosition);
    }

    this->Refresh(true, GetScrollRect());
}

void TimingWindow::OnSize(wxSizeEvent &event)
{
    event.Skip();
    this->Refresh();
}
