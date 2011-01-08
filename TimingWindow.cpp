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

const int DistanceToTicksLine    = 25;
const int DistanceFromTicksLine  = 5;
const int manipXoffset          = 20;
const int manipRadius           = 4;

const int DistanceToAxis        = 25;
const int DistanceFromAxis      = 20;

const wxString FloatFormatStr = _("%.2f ");


wxPoint TimingWindow::GetBitmapSize()
{
    return wxPoint(
        axisStop-40,
        heightOffsets[heightOffsets.size()-1] - DistanceToTicksLine - DistanceFromTicksLine
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

    /// change position of caret
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
    InitTextDrawing();

    VisibleTicks.clear();

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
        offset = wxPoint(40, DistanceToTicksLine + DistanceFromTicksLine );
    else
        offset = wxPoint( 0, 0);

    offset.y += DistanceToAxis + DistanceFromAxis;

    std::set<wxInt32>::iterator it;
    wxPoint unscrolledPosition(0,0);
    if ( !exporting )
        CalcUnscrolledPosition(0, 0, &unscrolledPosition.x, &unscrolledPosition.y);

    wxPoint clientsize;
    GetClientSize(&clientsize.x, &clientsize.y);
    dc.DestroyClippingRegion();

    /// calc vertical grid
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
    if ( virtsize.y < offset.y ) virtsize.y = offset.y;

    /// drawing signal names:
    if ( !exporting )
        dc.SetClippingRegion(unscrolledPosition.x, heightOffsets[0]+unscrolledPosition.y,clientsize.x, clientsize.y);
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

    /// points to manipulate the signal (through right click) will be removed in the future
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
    axisStart = signalNamesWidth;
    wxInt32 n = 0;
    while ( n <= doc->length )
    {
        VisibleTicks.push_back(n);
        if ( WindowState != AddTime || n != editingValA || editingValC < 1 )
        {
            bool hascompressor = false;
            for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
            if ( doc->compressors[indx].pos == n && doc->compressors[indx].enabled )
            {
                n += doc->compressors[indx].length;
                if ( n > doc->length )
                    VisibleTicks.push_back(n);
                hascompressor = true;
            }
            if ( !hascompressor )
                ++n;
        }
        else
        {
            for ( wxInt32 adding = 0 ; adding < editingValC ; ++adding )
                VisibleTicks.push_back(n);
            ++n;
        }
    }
    axisStop = signalNamesWidth + GridStepWidth*(VisibleTicks.size() - 1);
    if ( virtsize.x < (axisStop + 500) ) virtsize.x = (axisStop + 500);
    if ( virtsize.x < clientsize.x ) virtsize.x = clientsize.x;
    if ( virtsize.y < clientsize.y ) virtsize.y = clientsize.y;
    SetVirtualSize(virtsize);// change the size of the scrollable window

    /// drawing a graphical partitioning of the signal names
    if ( !exporting )
    {
        //wxPen pen(wxColour(0xf0,0xf0,0xf0));
        dc.SetPen(*wxGREY_PEN);
        dc.DrawLine(unscrolledPosition.x + signalNamesWidth-6, unscrolledPosition.y + DistanceToTicksLine + DistanceFromTicksLine + DistanceToAxis + DistanceFromAxis-5,
                    unscrolledPosition.x + signalNamesWidth-6, heightOffsets[heightOffsets.size()-1]);
        dc.SetPen(*wxBLACK_PEN);
    }

    /// drawing the time axis
    if ( WindowState != EditAxisLeft && !exporting )
        dc.SetClippingRegion(unscrolledPosition.x+signalNamesWidth-5,unscrolledPosition.y, virtsize.x, virtsize.y+10);
    {
        wxCoord axispos =  DistanceToAxis + unscrolledPosition.y;
        wxCoord start = signalNamesWidth;
        wxCoord stop = start + axisStop-axisStart;
        if ( ! exporting )
            axispos += DistanceToTicksLine + DistanceFromTicksLine;

        dc.DrawLine(start , axispos, stop, axispos);
        wxInt32 markerlen = doc->MarkerLength;
        wxInt32 adding = 0;
        for ( wxUint32 n = 0 ; n < VisibleTicks.size() ; ++n )
        {
            wxInt32 ticks = VisibleTicks[n];
            if ( WindowState == AddTime && editingValC > 0 && ticks > editingValA )
                ticks += editingValC;
            if ( WindowState == AddTime && editingValC > 0 && ticks == editingValA )
            {
                ticks += adding;
                adding++;
            }
            if ( ((ticks + doc->timeOffset) % markerlen) == 0 )
            {
                dc.DrawLine(signalNamesWidth + n*GridStepWidth, axispos - 2,
                    signalNamesWidth + n*GridStepWidth, axispos + 3);

                double t = (ticks + doc->timeOffset) * doc->TickLength;
                wxInt8 u = doc->TickLengthUnit;
                while ( u < 3 && (t >= 1000.0 || t <= -1000.0))
                {
                    u++;
                    t /= 1000.0;
                }
                wxString str(wxString::Format(FloatFormatStr, t));
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
                dc.DrawText(str, signalNamesWidth + n*GridStepWidth - w/2, axispos - h - 4);
            }
        }
    }

    if ( WindowState != EditAxisLeft && !exporting  )
        dc.SetClippingRegion(unscrolledPosition.x+signalNamesWidth-5,unscrolledPosition.y, virtsize.x, virtsize.y+10);
    /// drawing the ticks
    if ( !exporting )
    {
        wxCoord drawstartx, drawy, drawstopx;
        wxInt32 drawsteps;
        bool drawBoxes;

        drawstartx = axisStart;
        drawstopx = axisStop;
        drawy = DistanceToTicksLine;
        drawsteps = VisibleTicks.size()-1;

        if ( WindowState != EditAxisLeft && WindowState != EditAxisRight )
            drawBoxes = WindowState == AxisIsMarked;
        else
        {
            drawBoxes = true;
            drawsteps = editingValB;
            if ( WindowState == EditAxisRight )
            {
                if ( editingValB < doc->length )
                    for ( wxUint32 i = 0 ; i < VisibleTicks.size() ; ++i )
                    {
                        if ( editingValB == VisibleTicks[i] )
                        {
                            drawsteps = i;
                            break;
                        }
                    }
                else
                    drawsteps = editingValB - (doc->length - VisibleTicks.size()+1);
                axisStop = signalNamesWidth + GridStepWidth*drawsteps;
            }
            else // EditAxisLeft
            {
                drawsteps = VisibleTicks.size()-1;
                if ( editingValB > doc->length )
                    drawsteps += (editingValB-doc->length);
                else
                {
                    wxInt32 diff = doc->length - editingValB;
                    for ( wxInt32 i = 0 ; i < (wxInt32)VisibleTicks.size() ;++i )
                    {
                        if ( i == diff )
                        {
                            diff = VisibleTicks[i];
                            break;
                        }
                    }
                    drawsteps -= diff;
                }
                drawstartx = axisStop - GridStepWidth*drawsteps;
            }
            drawstopx = axisStop;
        }

        if ( WindowState == AddTime && editingValC > 0)
        {
            drawBoxes = false;

        }

        drawy += unscrolledPosition.y;
        dc.DrawLine(drawstartx - 3, drawy, drawstopx + 3, drawy);
        for (wxInt32 n = 0 ; n <= drawsteps ; ++n)
            dc.DrawLine(drawstartx + n*GridStepWidth, drawy - 2, drawstartx + n*GridStepWidth, drawy + 3);
        if ( drawBoxes )
        {
            dc.SetBrush(*wxBLUE_BRUSH);
            dc.DrawRectangle(drawstartx - 3, drawy - 3, 7, 7);
            dc.DrawRectangle(drawstopx - 3, drawy - 3, 7, 7);
        }
    }

    if ( !exporting )
        dc.SetClippingRegion(unscrolledPosition.x+signalNamesWidth-5,heightOffsets[0]-5+unscrolledPosition.y, virtsize.x, virtsize.y+10);

    /// drawing background of clock signal
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxColour(0xf0,0xf0,0xf0));
    std::vector<wxUint32> positions;
    std::vector<wxString> texts;
    for ( wxUint32 n = 0 ; n < doc->signals.size() ; ++n )
    {
        offset.x = signalNamesWidth;
        Signal sig = doc->signals[n];
        if ( sig.IsClock && sig.GenerateBackground ) // is clock with background generation on?
        {
            if ( sig.ShowPeriodCount )
            {
                positions.clear();
                texts.clear();
            }
            // drawing the background if needed
            wxInt32 n = 0;
            for (wxInt32 k = -((4*sig.ticks)- (sig.delay % (4*sig.ticks))) ; k < 0 ; ++k )
                if ( ++n == 4*sig.ticks )
                    n = 0;
            for (wxUint32 k = 0; k < VisibleTicks.size()-1 ; )
            {
                bool hascompressor = false;
                for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
                if ( doc->compressors[indx].pos == VisibleTicks[k] && doc->compressors[indx].enabled )
                {
                    if ( n < 2*sig.ticks )// drawing a bar in the first half of the tick
                        dc.DrawRectangle(
                            offset.x, heightOffsets[0],
                            GridStepWidth/2, heightOffsets[heightOffsets.size()-1] - heightOffsets[0]
                        );

                    wxInt32 len = (doc->compressors[indx].length) % (4*sig.ticks);
                    n += len - 2;
                    if ( n < 0 ) n += (4*sig.ticks);
                    n %= (4*sig.ticks);
                    if ( n < 2*sig.ticks )// drawing a bar over the second half of the tick
                        dc.DrawRectangle(
                            offset.x + GridStepWidth/2, heightOffsets[0],
                            GridStepWidth/2+1  , heightOffsets[heightOffsets.size()-1] - heightOffsets[0]
                        );
                    ++n;
                    hascompressor = true;
                }
                if ( !hascompressor );
                {
                    if ( n < 2*sig.ticks ){// drawing a bar over the whole tick
                        dc.DrawRectangle(
                            offset.x, heightOffsets[0],
                            GridStepWidth+1, heightOffsets[heightOffsets.size()-1] - heightOffsets[0]
                        );
                    }
                    if ( sig.ShowPeriodCount &&
                        (n == sig.ticks || n == 3*sig.ticks))
                    {
                        wxUint32 per = VisibleTicks[k]/2/sig.ticks+1;
                        positions.push_back(offset.x);
                        texts.push_back( wxString::Format(_T("%d"),per) );
                        //dc.DrawText( , , heightOffsets[0]);
                    }
                    ++n;
                }
                n %= (4*sig.ticks);
                offset.x += GridStepWidth;
                ++k;
            }
        }
    }
    for ( wxUint32 n = 0; n < positions.size() ; ++n )
        dc.DrawText( texts[n] , positions[n], heightOffsets[0]);
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);


    /// indicate "inserting" of time
    if ( !exporting && WindowState == AddTime && editingValC >= 0 )
    {
        dc.SetBrush(*wxMEDIUM_GREY_BRUSH);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle( axisStart + editingValB * GridStepWidth, heightOffsets[0],
                        editingValC * GridStepWidth, -heightOffsets[0] + heightOffsets[heightOffsets.size()-1]);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(*wxBLACK_PEN);
    }

    /// indicate "removal" of time
    if ( !exporting &&WindowState == RemoveTime && editingValC != -1 )
    {
        dc.SetBrush(*wxMEDIUM_GREY_BRUSH);
        dc.SetPen(*wxTRANSPARENT_PEN);
        wxInt32 a, b; //a left ,  b right
        a = editingValA;
        b = editingValC;
        if ( b < a ) { a = b; b = editingValA;} //swap
        for ( wxInt32 n = 0 ; n < (wxInt32)VisibleTicks.size() ; ++n )
            if ( VisibleTicks[n] == a )
            {
                a = n;
                break;
            }
        for ( wxInt32 n = 0 ; n < (wxInt32)VisibleTicks.size() ; ++n )
            if ( VisibleTicks[n] == b )
            {
                b = n;
                break;
            }
        dc.DrawRectangle( axisStart + a * GridStepWidth, heightOffsets[0],
                        (b-a) * GridStepWidth, -heightOffsets[0] + heightOffsets[heightOffsets.size()-1]);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(*wxBLACK_PEN);
    }


    /// indicate target position of moving signals
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

        wxCoord transwidth = GridStepWidth*doc->TransitWidth/100.0;
        //{
        wxCoord wo = transwidth;
        wxCoord wt = GridStepWidth/2;
        wxCoord ho[6] = {0, 0, doc->SignalHeight/2, doc->SignalHeight/2, doc->SignalHeight, doc->SignalHeight};
        wxCoord transitionheights[6];
        if ( doc->TransitWidth != 0 )
            transitionheights[4] = ho[5]*50/doc->TransitWidth;
        transitionheights[1] = ho[5]-transitionheights[4];
        transitionheights[2] = transitionheights[4]/2;
        transitionheights[3] = ho[5]-transitionheights[2];
        transitionheights[5] = transitionheights[2]+ho[5]/2;
        transitionheights[0] = ho[5]-transitionheights[5];//}

        Signal sig;
        if ( WindowState == EditSignal && editingNumber>=0 && (wxUint32)editingNumber == n && editingValB != -1 ) sig = editingSignal;
        else sig = doc->signals[n];
        if ( sig.IsClock )
        {
            wxInt32 n = 0;
            for (wxInt32 k = -((2*sig.ticks)- (sig.delay % (2*sig.ticks)))
             ; k < 0 ; ++k ) if ( ++n == 2*sig.ticks ) n = 0;
            if ( n == 0 || n == sig.ticks ) // draw an edge at the left of the diagram
                dc.DrawLine(
                    offset.x, offset.y + doc->SignalHeight,
                    offset.x, offset.y
                );
            for (wxUint32 k = 0; k < VisibleTicks.size()-1 ; )
            {
                bool hascompressor = false;
                for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
                    if ( doc->compressors[indx].pos == VisibleTicks[k] && doc->compressors[indx].enabled )
                    {
                        if ( n < sig.ticks )// drawing a one
                            dc.DrawLine(
                                offset.x                  , offset.y,
                                offset.x + GridStepWidth/2, offset.y
                            );
                        else// drawing a zero
                            dc.DrawLine(
                                offset.x                  , offset.y + doc->SignalHeight,
                                offset.x + GridStepWidth/2, offset.y + doc->SignalHeight
                            );

                        wxInt32 len = (doc->compressors[indx].length) % (2*sig.ticks);
                        n += len -1;
                        if ( n < 0 ) n += (2*sig.ticks);
                        n %= (2*sig.ticks);
                        if ( n < sig.ticks )// drawing a one
                            dc.DrawLine(
                                offset.x + GridStepWidth/2, offset.y,
                                offset.x + GridStepWidth  , offset.y
                            );
                        else// drawing a zero
                            dc.DrawLine(
                                offset.x + GridStepWidth/2, offset.y + doc->SignalHeight,
                                offset.x + GridStepWidth  , offset.y + doc->SignalHeight
                            );
                        ++n;
                        hascompressor = true;
                    }
                if ( !hascompressor )
                {
                    if ( n < sig.ticks )// drawing a one
                        dc.DrawLine(
                            offset.x                , offset.y,
                            offset.x + GridStepWidth, offset.y
                        );
                    else// drawing a zero
                        dc.DrawLine(
                            offset.x                , offset.y + doc->SignalHeight,
                            offset.x + GridStepWidth, offset.y + doc->SignalHeight
                        );
                    ++n;
                }

                if( n == sig.ticks || n == 2*sig.ticks ) // draw the edge
                    dc.DrawLine(
                        offset.x + GridStepWidth, offset.y + doc->SignalHeight,
                        offset.x + GridStepWidth, offset.y
                    );
                n %= (2*sig.ticks);
                offset.x += GridStepWidth;
                ++k;
            }
        }
        else if ( !sig.IsBus )
        {
            offset.x = signalNamesWidth;
            vals oldval = sig.values[0];// first tick can not start with a transition

            for (wxUint32 k = 0; k < VisibleTicks.size()-1 ; ++k)
            {
                wxInt32 tick = VisibleTicks[k];
                wxInt32 len = 1;
                vals val = sig.values[tick];
                for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
                if ( doc->compressors[indx].pos == VisibleTicks[k] && doc->compressors[indx].enabled )
                    len = doc->compressors[indx].length;


                /// drawing the transitions:
                wxCoord w1 = wo;
                wxCoord h[6] = {ho[0], ho[1], ho[2], ho[3], ho[4], ho[5]};
                wxCoord h0 = ho[5];
                if ( len > 1 && doc->TransitWidth >= 50 ) // time compressor and transition "breiter" than the half tick
                {
                    w1 = wt;
                    for ( int i = 0 ; i < 6 ; i++) h[i] = transitionheights[i];
                }
                switch(val)
                {
                    case zero:
                        switch ( oldval )
                        {
                            case zero:
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h0);
                                break;
                            case one:
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y+h[4]);
                                break;
                            case hz:
                                dc.DrawLine(offset.x   , offset.y+h0/2,
                                            offset.x+w1, offset.y+h[5]);
                                break;
                            default:
                            case u:
                                {dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                dc.SetPen(*wxLIGHT_GREY_PEN);
                                wxPoint rec[4];
                                rec[0] = wxPoint(offset.x   , offset.y);
                                rec[1] = wxPoint(offset.x+w1, offset.y + h[4]);
                                rec[2] = wxPoint(offset.x+w1, offset.y + h0);
                                rec[3] = wxPoint(offset.x   , offset.y + h0);
                                dc.DrawPolygon(4, rec);}
                                dc.SetPen(boldPen);
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h0);
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y+h[4]);
                                break;
                            case dc1:
                            case dc2:
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h0);
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y+h[4]);
                                break;
                        }
                        break;
                    case one:
                        switch ( oldval )
                        {
                            case zero:
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h[1]);
                                break;
                            case one:
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y);
                                break;
                            case hz:
                                dc.DrawLine(offset.x   , offset.y+h0/2,
                                            offset.x+w1, offset.y+h[0]);
                                break;
                            default:
                            case u:{dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                dc.SetPen(*wxLIGHT_GREY_PEN);
                                wxPoint rec[4];
                                rec[0] = wxPoint(offset.x   , offset.y+h0);
                                rec[1] = wxPoint(offset.x+w1, offset.y+h[1]);
                                rec[2] = wxPoint(offset.x+w1, offset.y);
                                rec[3] = wxPoint(offset.x   , offset.y);
                                dc.DrawPolygon(4, rec);}
                                dc.SetPen(boldPen);
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h[1]);
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y);
                                break;
                            case dc1:
                            case dc2:
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h[1]);
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y);
                                break;
                        }
                        break;
                    case hz:
                        switch ( oldval )
                        {
                            case zero:
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h[3]);
                                break;
                            case one:
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y+h[2]);
                                break;
                            case hz:
                                dc.DrawLine(offset.x   , offset.y+h0/2,
                                            offset.x+w1, offset.y+h0/2);
                                break;
                            default:
                            case u:
                                {dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                dc.SetPen(*wxLIGHT_GREY_PEN);
                                wxPoint rec[4];
                                rec[0] = wxPoint(offset.x   , offset.y);
                                rec[1] = wxPoint(offset.x+w1, offset.y + h[2]);
                                rec[2] = wxPoint(offset.x+w1, offset.y + h[3]);
                                rec[3] = wxPoint(offset.x   , offset.y + h0);
                                dc.DrawPolygon(4, rec);}
                                dc.SetPen(boldPen);
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h[3]);
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y+h[2]);
                                break;
                            case dc1:
                            case dc2:
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h[3]);
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y+h[2]);
                                break;
                        }
                        break;
                    default:
                    case u:
                        switch ( oldval )
                        {
                            case zero:
                                {dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                dc.SetPen(*wxLIGHT_GREY_PEN);
                                wxPoint rec[3];
                                rec[0] = wxPoint(offset.x   , offset.y+h0);
                                rec[1] = wxPoint(offset.x+w1, offset.y+h0);
                                rec[2] = wxPoint(offset.x+w1, offset.y+h[1]);
                                dc.DrawPolygon(3, rec);}
                                dc.SetPen(boldPen);
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h0);
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h[1]);
                                break;
                            case one:
                                {dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                dc.SetPen(*wxLIGHT_GREY_PEN);
                                wxPoint rec[3];
                                rec[0] = wxPoint(offset.x   , offset.y);
                                rec[1] = wxPoint(offset.x+w1, offset.y);
                                rec[2] = wxPoint(offset.x+w1, offset.y+h[4]);
                                dc.DrawPolygon(3, rec);}
                                dc.SetPen(boldPen);
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y+h[4]);
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y);
                                break;
                            case hz:
                                {dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                dc.SetPen(*wxLIGHT_GREY_PEN);
                                wxPoint rec[3];
                                rec[0] = wxPoint(offset.x   , offset.y+h0/2);
                                rec[1] = wxPoint(offset.x+w1, offset.y+h[5]);
                                rec[2] = wxPoint(offset.x+w1, offset.y+h[0]);
                                dc.DrawPolygon(3, rec);}
                                dc.SetPen(boldPen);
                                dc.DrawLine(offset.x   , offset.y+h0/2,
                                            offset.x+w1, offset.y+h[5]);
                                dc.DrawLine(offset.x   , offset.y+h0/2,
                                            offset.x+w1, offset.y+h[0]);
                                break;
                            default:
                            case u:
                                {dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                dc.SetPen(*wxLIGHT_GREY_PEN);
                                wxPoint rec[4];
                                rec[0] = wxPoint(offset.x   , offset.y);
                                rec[1] = wxPoint(offset.x+w1, offset.y);
                                rec[2] = wxPoint(offset.x+w1, offset.y + h0);
                                rec[3] = wxPoint(offset.x   , offset.y + h0);
                                dc.DrawPolygon(4, rec);}
                                dc.SetPen(boldPen);
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h0);
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y);
                                break;
                            case dc1:
                            case dc2:
                                {dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                dc.SetPen(*wxLIGHT_GREY_PEN);
                                wxPoint rec[3];
                                rec[0] = wxPoint(offset.x+w1  , offset.y+h[1]);
                                rec[1] = wxPoint(offset.x+wo/2, offset.y+h0/2);
                                rec[2] = wxPoint(offset.x+w1  , offset.y+h[4]);
                                dc.DrawPolygon(3, rec);}
                                dc.SetPen(boldPen);
                                dc.DrawLine(offset.x   , offset.y ,
                                            offset.x+w1, offset.y+h[4]);
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h[1]);

                                break;
                        }
                        break;
                    case dc1:
                    case dc2:
                        switch( oldval )
                        {
                            case one:
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y);
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y+h[4]);
                                break;
                            case zero:
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h0);
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h[1]);
                                break;
                            case hz:
                                dc.DrawLine(offset.x   , offset.y+h0/2,
                                            offset.x+w1, offset.y+h[5]);
                                dc.DrawLine(offset.x   , offset.y+h0/2,
                                            offset.x+w1, offset.y+h[0]);
                                break;
                            default:
                            case u:
                                {dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                dc.SetPen(*wxLIGHT_GREY_PEN);
                                wxPoint rec[3];
                                rec[0] = wxPoint(offset.x     , offset.y);
                                rec[1] = wxPoint(offset.x+wo/2, offset.y+h0/2);
                                rec[2] = wxPoint(offset.x     , offset.y+h0);
                                dc.DrawPolygon(3, rec);}
                                dc.SetPen(boldPen);
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y+h[4]);
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h[1]);
                                break;
                            case dc1:
                            case dc2:
                                if ( val != oldval )
                                {
                                    dc.DrawLine(offset.x   , offset.y,
                                                offset.x+w1, offset.y+h[4]);
                                    dc.DrawLine(offset.x   , offset.y+h0,
                                                offset.x+w1, offset.y+h[1]);
                                }
                                else
                                {
                                    dc.DrawLine(offset.x   , offset.y,
                                                offset.x+w1, offset.y);
                                    dc.DrawLine(offset.x   , offset.y+h0,
                                                offset.x+w1, offset.y+h0);
                                }
                                break;
                        }
                        break;
                }

                /// drawing the state
                wxCoord w2 = GridStepWidth;
                if ( len > 1 )
                    w2 /= 2;
                if ( len == 1 || doc->TransitWidth < 50 ) switch ( val )
                {
                    case one:
                        dc.DrawLine(offset.x+w1, offset.y,
                                    offset.x+w2, offset.y);
                        break;
                    case zero:
                        dc.DrawLine(offset.x+w1, offset.y+h0,
                                    offset.x+w2, offset.y+h0);
                        break;
                    case hz:
                        dc.DrawLine(offset.x+w1, offset.y+h0/2,
                                    offset.x+w2, offset.y+h0/2);
                        break;
                    default:
                    case u:
                        {dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                        dc.SetPen(*wxLIGHT_GREY_PEN);
                        wxPoint rec[4];
                        rec[0] = wxPoint(offset.x+w1, offset.y);
                        rec[1] = wxPoint(offset.x+w2, offset.y);
                        rec[2] = wxPoint(offset.x+w2, offset.y+h0);
                        rec[3] = wxPoint(offset.x+w1, offset.y+h0);
                        dc.DrawPolygon(4, rec);}
                        dc.SetPen(boldPen);
                        dc.DrawLine(offset.x+w1, offset.y,
                                    offset.x+w2, offset.y);
                        dc.DrawLine(offset.x+w1, offset.y+h0,
                                    offset.x+w2, offset.y+h0);
                        break;
                    case dc1:
                    case dc2:
                        dc.DrawLine(offset.x+w1, offset.y,
                                    offset.x+w2, offset.y);
                        dc.DrawLine(offset.x+w1, offset.y+h0,
                                    offset.x+w2, offset.y+h0);
                        break;
                }

                if ( len > 1 )
                {
                    w1 = GridStepWidth/2;
                    w2 = GridStepWidth;
                    val = sig.values[VisibleTicks[k+1]-1];
                    switch ( val )
                    {
                        case one:
                            dc.DrawLine(offset.x+w1, offset.y,
                                        offset.x+w2, offset.y);
                            break;
                        case zero:
                            dc.DrawLine(offset.x+w1, offset.y+h0,
                                        offset.x+w2, offset.y+h0);
                            break;
                        case hz:
                            dc.DrawLine(offset.x+w1, offset.y+h0/2,
                                        offset.x+w2, offset.y+h0/2);
                            break;
                        default:
                        case u:
                            {dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                            dc.SetPen(*wxLIGHT_GREY_PEN);
                            wxPoint rec[4];
                            rec[0] = wxPoint(offset.x+w1, offset.y);
                            rec[1] = wxPoint(offset.x+w2, offset.y);
                            rec[2] = wxPoint(offset.x+w2, offset.y+h0);
                            rec[3] = wxPoint(offset.x+w1, offset.y+h0);
                            dc.DrawPolygon(4, rec);}
                            dc.SetPen(boldPen);
                            dc.DrawLine(offset.x+w1, offset.y,
                                        offset.x+w2, offset.y);
                            dc.DrawLine(offset.x+w1, offset.y+h0,
                                        offset.x+w2, offset.y+h0);
                            break;
                        case dc1:
                        case dc2:
                            dc.DrawLine(offset.x+w1, offset.y,
                                        offset.x+w2, offset.y);
                            dc.DrawLine(offset.x+w1, offset.y+h0,
                                        offset.x+w2, offset.y+h0);
                            break;
                    }
                }
                oldval = val;
                offset.x+= GridStepWidth;
            }
        }
        else // is bus
        {
            offset.x = signalNamesWidth;
            vals oldval = sig.values[0];// first tick can not start with a transition

            for (wxUint32 k = 0; k < VisibleTicks.size()-1 ; ++k)
            {
                wxInt32 tick = VisibleTicks[k];
                wxInt32 len = 1;
                vals val = sig.values[tick];
                for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
                if ( doc->compressors[indx].pos == VisibleTicks[k] && doc->compressors[indx].enabled )
                    len = doc->compressors[indx].length;

                /// drawing the transitions:
                wxCoord w1 = wo;
                wxCoord h[6] = {ho[0], ho[1], ho[2], ho[3], ho[4], ho[5]};
                wxCoord h0 = ho[5];
                if ( len > 1 && doc->TransitWidth >= 50 ) // time compressor and transition "breiter" than the half tick
                {
                    w1 = wt;
                    for ( int i = 0 ; i < 6 ; i++) h[i] = transitionheights[i];
                }
                switch(val)
                {
                    case zero:
                    case one:
                        switch ( oldval )
                        {
                            case zero:
                            case one:
                                if ( oldval != val )
                                {
                                    dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h[1]);
                                    dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y+h[4]);
                                }
                                else
                                {
                                    dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h0);
                                    dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y);
                                }
                                break;
                            case hz:
                                dc.DrawLine(offset.x   , offset.y+h0/2,
                                            offset.x+w1, offset.y+h[5]);
                                dc.DrawLine(offset.x   , offset.y+h0/2,
                                            offset.x+w1, offset.y+h[0]);
                                break;
                            default:
                            case u:{
                                dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                dc.SetPen(*wxLIGHT_GREY_PEN);
                                wxPoint rec[3];
                                rec[0] = wxPoint(offset.x     , offset.y);
                                rec[1] = wxPoint(offset.x+wo/2, offset.y + h0/2);
                                rec[2] = wxPoint(offset.x     , offset.y + h0);
                                dc.DrawPolygon(3, rec);}
                                dc.SetPen(boldPen);
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y+h[4]);
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h[1]);
                                break;
                        }
                        break;
                    case hz:
                        switch ( oldval )
                        {
                            case hz:
                                dc.DrawLine(offset.x   , offset.y+h0/2,
                                            offset.x+w1, offset.y+h0/2);
                                break;
                            default:
                            case u:{
                                dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                dc.SetPen(*wxLIGHT_GREY_PEN);
                                wxPoint rec[4];
                                rec[0] = wxPoint(offset.x   , offset.y);
                                rec[1] = wxPoint(offset.x+w1, offset.y + h[2]);
                                rec[2] = wxPoint(offset.x+w1, offset.y + h[3]);
                                rec[3] = wxPoint(offset.x   , offset.y + h0);
                                dc.DrawPolygon(4, rec);}
                                dc.SetPen(boldPen);
                                // no break here
                            case zero:
                            case one:
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h[3]);
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y+h[2]);
                                break;
                        }
                        break;
                    default:
                    case u:
                        switch ( oldval )
                        {
                            case hz:{
                                dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                dc.SetPen(*wxLIGHT_GREY_PEN);
                                wxPoint rec[3];
                                rec[0] = wxPoint(offset.x+w1, offset.y +h[0]);
                                rec[1] = wxPoint(offset.x+w1, offset.y +h[5]);
                                rec[2] = wxPoint(offset.x   , offset.y +h0/2);
                                dc.DrawPolygon(3,rec);
                                dc.SetPen(boldPen);
                                dc.DrawLine(offset.x   , offset.y +h0/2,
                                            offset.x+w1, offset.y +h[0]);
                                dc.DrawLine(offset.x   , offset.y +h0/2,
                                            offset.x+w1, offset.y +h[5]);}
                                break;
                            default:
                            case u:{
                                dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                dc.SetPen(*wxLIGHT_GREY_PEN);
                                wxPoint rec[4];
                                rec[0] = wxPoint(offset.x   , offset.y);
                                rec[1] = wxPoint(offset.x+w1, offset.y);
                                rec[2] = wxPoint(offset.x+w1, offset.y + h0);
                                rec[3] = wxPoint(offset.x   , offset.y + h0);
                                dc.DrawPolygon(4, rec);}
                                dc.SetPen(boldPen);
                                dc.DrawLine(offset.x, offset.y, offset.x+w1, offset.y);
                                dc.DrawLine(offset.x, offset.y+h0, offset.x+w1, offset.y+h0);
                                break;
                            case zero:
                            case one:{
                                dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                                dc.SetPen(*wxLIGHT_GREY_PEN);
                                wxPoint rec[3];
                                rec[0] = wxPoint(offset.x+w1  , offset.y+h[1]);
                                rec[1] = wxPoint(offset.x+wo/2, offset.y+h0/2);
                                rec[2] = wxPoint(offset.x+w1  , offset.y+h[4]);
                                dc.DrawPolygon(3, rec);}
                                dc.SetPen(boldPen);
                                dc.DrawLine(offset.x, offset.y   , offset.x+w1, offset.y+h[4]);
                                dc.DrawLine(offset.x, offset.y+h0, offset.x+w1, offset.y+h[1]);
                                break;
                        }
                        break;
                }

                /// drawing the state
                wxCoord w2 = GridStepWidth;
                if ( len > 1 )
                    w2 /= 2;
                if ( len == 1 || doc->TransitWidth < 50 ) switch ( val )
                {
                    case hz:
                        dc.DrawLine(offset.x+w1, offset.y+h0/2,
                                    offset.x+w2, offset.y+h0/2);
                        break;
                    default:
                    case u:{
                        dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                        dc.SetPen(*wxLIGHT_GREY_PEN);
                        wxPoint rec[4];
                        rec[0] = wxPoint(offset.x+w1, offset.y);
                        rec[1] = wxPoint(offset.x+w2, offset.y);
                        rec[2] = wxPoint(offset.x+w2, offset.y+h0);
                        rec[3] = wxPoint(offset.x+w1, offset.y+h0);
                        dc.DrawPolygon(4, rec);}
                        dc.SetPen(boldPen);
                    case one:
                    case zero:
                        dc.DrawLine(offset.x+w1, offset.y,
                                    offset.x+w2, offset.y);
                        dc.DrawLine(offset.x+w1, offset.y+h0,
                                    offset.x+w2, offset.y+h0);
                        break;
                }
                if ( len > 1 )
                {
                    w1 = GridStepWidth/2;
                    w2 = GridStepWidth;
                    val = sig.values[VisibleTicks[k+1]-1];
                    switch ( val )
                    {
                        case hz:
                            dc.DrawLine(offset.x+w1, offset.y+h0/2,
                                        offset.x+w2, offset.y+h0/2);
                            break;
                        default:
                        case u:{
                            dc.SetBrush(*wxLIGHT_GREY_BRUSH);
                            dc.SetPen(*wxLIGHT_GREY_PEN);
                            wxPoint rec[4];
                            rec[0] = wxPoint(offset.x+w1, offset.y);
                            rec[1] = wxPoint(offset.x+w2, offset.y);
                            rec[2] = wxPoint(offset.x+w2, offset.y+h0);
                            rec[3] = wxPoint(offset.x+w1, offset.y+h0);
                            dc.DrawPolygon(4, rec);}
                            dc.SetPen(boldPen);
                        case one:
                        case zero:
                            dc.DrawLine(offset.x+w1, offset.y+h0,
                                        offset.x+w2, offset.y+h0);
                            dc.DrawLine(offset.x+w1, offset.y,
                                        offset.x+w2, offset.y);
                            break;

                    }
                }
                oldval = val;
                offset.x += GridStepWidth;
            }

            //drawing the texts
            offset.x = signalNamesWidth;
            oldval = sig.values[0];
            for ( wxUint32 k = 0; k < VisibleTicks.size()-1 ; ++k )
            {
                wxInt32 tick = VisibleTicks[k];
                vals val = sig.values[tick];
                wxInt32 invislen = 0;

                bool hascompressor = false;
                for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
                if ( doc->compressors[indx].pos == tick && doc->compressors[indx].enabled )
                {
                    invislen = doc->compressors[indx].length;
                    for ( wxInt32 i = 0 ; i < invislen ; ++i )
                    {
                        wxInt32 ti = tick + i;
                        val = sig.values[ti];
                        if ( ( ti == 0 || (sig.values[ti-1] != val)) && (val == one || val == zero ))
                            DrawEditableText(dc, sig.TextValues[ti], offset, false);
                    }
                    hascompressor = true;
                    break;
                }
                if ( !hascompressor )
                {
                    if ( (tick == 0 || oldval != val) && (val == one || val == zero ))
                    {
                        wxPoint textoff(
                            wo + offset.x + k*GridStepWidth,
                            offset.y + doc->SignalHeight/2 -
                            dc.GetCharHeight()/2
                        );
                        bool viz = true;
                        if ( textoff.y + dc.GetCharHeight()/2 < heightOffsets[0]+unscrolledPosition.y) viz = false;
                        if ( textoff.x                        < signalNamesWidth - 5 + unscrolledPosition.x) viz = false;
                        DrawEditableText(dc, sig.TextValues[tick], textoff, viz);
                    }
                }
                oldval = val;
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
    bool addingdone = false;
    for ( wxUint32 k = 0 ; k < VisibleTicks.size()-1 ; ++k )
    {
        wxInt32 tick = VisibleTicks[k];
        wxInt32 len = 1;
        for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
	        if ( doc->compressors[indx].pos == tick && doc->compressors[indx].enabled )
	            len = doc->compressors[indx].length;

        if ( WindowState == AddTime && tick == editingValA )
        {
            if ( !addingdone )
                addingdone = true;
            else
                len = 0;
        }
        if ( len > 1 )
        {
            /// splines over signal
            for ( wxUint32 n = 0 ; n < doc->signals.size() ; ++n)
            {
                offset.x = signalNamesWidth  + (0.5 + k)*GridStepWidth - 2;
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

                    dc.DrawSpline(4, points);
                }
            }
        }
    }

    /// drawing the vertical lines
    dc.DestroyClippingRegion();
    if ( !exporting )
        dc.SetClippingRegion(unscrolledPosition.x+signalNamesWidth-5,heightOffsets[0]+unscrolledPosition.y-3, virtsize.x, virtsize.y+3);
    VLineOffsets.clear();
    VLineToOffset.clear();
    for ( wxUint32 k = 0 ; k < doc->vertlines.size() ; ++k)
    {
        bool found = false;
        wxInt32 vpos;
        for ( wxUint32 i = 0 ; i < VisibleTicks.size() ; ++i)
            if (VisibleTicks[i] == doc->vertlines[k].vpos)
            {
                found = true;
                vpos = i;
                break;
            }
        if(!found)
        {
            VLineOffsets.push_back(wxPoint(-10,-10));
            VLineToOffset.push_back(-10);
            continue;
        }
        wxPoint offset(signalNamesWidth + vpos * GridStepWidth,
                       heightOffsets[doc->vertlines[k].StartPos]);

        if ( doc->en50 && doc->vertlines[k].vposoffset == 1 )
            offset.x += GridStepWidth*(doc->TransitWidth/2.0)/100.0;
        if ( doc->en90 && doc->vertlines[k].vposoffset == 2 )
            offset.x += GridStepWidth*(doc->TransitWidth)/100.0;

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
                bool found = false;
                wxInt32 vpos;
                for ( wxUint32 i = 0 ; i < VisibleTicks.size() ; ++i)
                    if (VisibleTicks[i] == editingValA)
                    {
                        found = true;
                        vpos = i;
                        break;
                    }
                if(!found)
                {
                    VLineOffsets.push_back(wxPoint(-10,-10));
                    VLineToOffset.push_back(-10);
                    continue;
                }
                else
                {
                    offset.x = signalNamesWidth + vpos * GridStepWidth;
                    if ( doc->en50 &&  editingValB == 1 )
                        offset.x += GridStepWidth*(doc->TransitWidth/2.0)/100.0;
                    if ( doc->en90 && editingValB == 2 )
                        offset.x += GridStepWidth*(doc->TransitWidth)/100.0;

                    dc.DrawLine(
                        offset.x, offset.y,
                        offset.x, heightOffsets[doc->vertlines[k].EndPos + 1]
                    );
                    dc.SetBrush(*wxBLUE_BRUSH);
                    dc.DrawRectangle(offset.x-3, offset.y -                                    3, 7, 7);
                    dc.DrawRectangle(offset.x-3, heightOffsets[doc->vertlines[k].EndPos + 1] - 3, 7, 7);
                    dc.SetBrush(wxNullBrush);
                }
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
        wxInt32 vpos = editingPoint[0].x;
        for ( wxUint32 i = 0 ; i < VisibleTicks.size() ; ++i)
            if (VisibleTicks[i] == vpos)
                vpos = i;

        wxPoint offset(signalNamesWidth + vpos * GridStepWidth,
        offset.y = heightOffsets[editingPoint[0].y]);

        if ( doc->en50 && editingValB == 1 )
            offset.x += GridStepWidth*(doc->TransitWidth/2.0)/100.0;
        if ( doc->en90 && editingValB == 2 )
            offset.x += GridStepWidth*(doc->TransitWidth)/100.0;

        dc.DrawLine(
            offset.x, offset.y,
            offset.x, heightOffsets[editingPoint[1].y]
        );
        dc.SetBrush(*wxBLUE_BRUSH);
        dc.DrawRectangle(offset.x-3, offset.y -                         3, 7, 7);
        dc.DrawRectangle(offset.x-3, heightOffsets[editingPoint[1].y] - 3, 7, 7);
        dc.SetBrush(wxNullBrush);
    }

    /// drawing the horizontal arrows
    HArrowOffsets.clear();
    HArrowToOffset.clear();
    if ( !exporting && ( WindowState == SelectingText ||
                         WindowState == TextFieldSelected ) )
    {
        dc.SetBackgroundMode(wxSOLID);
        dc.SetTextBackground(col);
    }
    wxInt32 distancfortimeline = 0;
    if ( !exporting )
        distancfortimeline = DistanceToTicksLine + DistanceFromTicksLine;
    for ( wxUint32 n = 0 ; n < doc->harrows.size() ; ++n )
    {
        HArrow &ha = doc->harrows[n];
        bool fromfound = false, tofound = false;
        wxInt32 fromvpos = doc->vertlines[ha.fromVLine].vpos;
        wxInt32 fromvposoffset = doc->vertlines[ha.fromVLine].vposoffset;
        wxInt32 tovpos = doc->vertlines[ha.toVLine].vpos;
        wxInt32 tovposoffset = doc->vertlines[ha.toVLine].vposoffset;


        if ( !exporting && editingNumber >= 0 && (wxUint32)editingNumber == n &&
        (WindowState == ChangingHArrowLengthStart || WindowState == ChangingHArrowLengthEnd))
        {
            if ( editingValA != -1 )
            {
                fromvpos = doc->vertlines[editingValA].vpos;
                fromvposoffset = doc->vertlines[editingValA].vposoffset;
            }
            if ( editingValB != -1 )
            {
                tovpos = doc->vertlines[editingValB].vpos;
                tovposoffset = doc->vertlines[editingValB].vposoffset;
            }
        }

        for ( wxUint32 i = 0 ; i < VisibleTicks.size() ; ++i)
        {
            if ( !fromfound && VisibleTicks[i] == fromvpos )
            {
                fromvpos = i;
                fromfound = true;
                //break;
            }
            if ( !tofound && VisibleTicks[i] == tovpos )
            {
                tovpos = i;
                tofound = true;
            }
        }
        if ( fromfound && tofound)
        {
            /// calc offset based on vline to start from
            wxPoint offset(signalNamesWidth + fromvpos * GridStepWidth,
                    ha.pos + heightOffsets[ha.signalnmbr]);
            if ( doc->en50 && fromvposoffset == 1 )
                offset.x += GridStepWidth*(doc->TransitWidth/2.0)/100.0;
            else if ( doc->en90 && fromvposoffset == 2 )
                offset.x += GridStepWidth*(doc->TransitWidth)/100.0;

            if ( editingNumber >= 0 && (wxUint32)editingNumber == n && WindowState == MovingHArrow )
                offset.y = editingValA + heightOffsets[editingValB];

            /// calc offset based on vline where harrow will end
            wxPoint tooffset(signalNamesWidth + tovpos * GridStepWidth,
                offset.y);
            if ( doc->en50 && tovposoffset == 1 )
                tooffset.x += GridStepWidth*(doc->TransitWidth/2.0)/100.0;
            else if ( doc->en90 && tovposoffset == 2 )
                tooffset.x += GridStepWidth*(doc->TransitWidth)/100.0;


            if ( !exporting && editingNumber >= 0 && (wxUint32)editingNumber == n &&
                (WindowState == ChangingHArrowLengthStart || WindowState == ChangingHArrowLengthEnd))
            {
                if (editingValA == -1 && WindowState == ChangingHArrowLengthStart )
                    offset.x = cursorpos.x;

                if (editingValB == -1 && WindowState == ChangingHArrowLengthEnd)
                    tooffset.x = cursorpos.x;
            }

            HArrowOffsets.push_back(offset);
            HArrowToOffset.push_back(tooffset.x);
            if ( offset.x > tooffset.x ) // swap
            {
                wxCoord t = offset.x;
                offset.x = tooffset.x;
                tooffset.x = t;
            }
            if ( !exporting && editingNumber>=0 && (wxUint32)editingNumber == n &&
                (WindowState == ChangingHArrowLengthEnd ||
                 WindowState == ChangingHArrowLengthStart    ) &&
                (editingValA == -1 || editingValB == -1))
            {
                wxPen pen(*wxBLACK, 1, wxDOT);
                dc.SetPen(pen);
                dc.DrawLine(  offset.x,   offset.y, tooffset.x,   tooffset.y);
                dc.SetPen(*wxBLACK_PEN);
            }
            else
                dc.DrawLine(  offset.x,   offset.y, tooffset.x,   tooffset.y);

            /// arrowhead:
            dc.DrawLine(  offset.x,   offset.y,   offset.x+5,   offset.y-3);
            dc.DrawLine(  offset.x,   offset.y,   offset.x+5,   offset.y+3);
            dc.DrawLine(tooffset.x, tooffset.y, tooffset.x-5, tooffset.y-3);
            dc.DrawLine(tooffset.x, tooffset.y, tooffset.x-5, tooffset.y+3);

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
            wxString text = ha.text;
            wxInt32 pos = text.Find( _T("$t$"));
            if ( WindowState != SelectingText && WindowState != TextFieldSelected && pos != wxNOT_FOUND )
            {
                wxInt32 l = 0;
                wxInt32 s = 0;

                if ( doc->vertlines[ha.fromVLine].vpos < doc->vertlines[ha.toVLine].vpos )
                {
                    l = doc->vertlines[ha.toVLine].vpos - doc->vertlines[ha.fromVLine].vpos;
                    if ( doc->en50 && doc->vertlines[ha.fromVLine].vposoffset == 1)
                        s -= 50;
                    if ( doc->en90 && doc->vertlines[ha.fromVLine].vposoffset == 2)
                        s -= 100;

                    if ( doc->en50 && doc->vertlines[ha.toVLine].vposoffset == 1)
                        s += 50;
                    if ( doc->en90 && doc->vertlines[ha.toVLine].vposoffset == 2)
                        s += 100;
                }
                else
                {
                    l = doc->vertlines[ha.fromVLine].vpos - doc->vertlines[ha.toVLine].vpos;
                    if ( doc->en50 && doc->vertlines[ha.fromVLine].vposoffset == 1)
                        s += 50;
                    if ( doc->en90 && doc->vertlines[ha.fromVLine].vposoffset == 2)
                        s += 100;

                    if ( doc->en50 && doc->vertlines[ha.toVLine].vposoffset == 1)
                        s -= 50;
                    if ( doc->en90 && doc->vertlines[ha.toVLine].vposoffset == 2)
                        s -= 100;
                }

                wxString str;
                double t = l*doc->TickLength + (s*doc->TransitWidth)/10000.0*doc->TickLength;
                wxInt8 u = doc->TickLengthUnit;
                while ( u < 3 && (t >= 1000.0 || t <= -1000.0) )
                {
                    u++;
                    t /= 1000.0;
                }
                str = wxString::Format(FloatFormatStr, t);
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
                text.Replace( _T("$t$"), str);
            }
            bool viz = true;
            if ( textoff.y + dc.GetCharHeight()/2 < heightOffsets[0]+unscrolledPosition.y) viz = false;
            if ( textoff.x                        < signalNamesWidth - 5 + unscrolledPosition.x) viz = false;
            DrawEditableText( dc, text, textoff, viz );

            /// blue boxes to manipulate positions
            if ( !exporting && editingNumber>=0 && (wxUint32)editingNumber == n && ( WindowState == HArrowIsMarked ||
                   WindowState == ChangingHArrowLengthStart ||
                   WindowState == ChangingHArrowLengthEnd ))
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
                  WindowState == ChangingHArrowLengthStart ||
                  WindowState == ChangingHArrowLengthEnd ||
                  WindowState == SelectingText))
            {
                wxPen pen(*wxBLACK, 1, wxSHORT_DASH );
                dc.SetPen(pen);
                dc.DrawLine(  (offset.x + tooffset.x)/2,   offset.y, textoff.x,   textoff.y+dc.GetCharHeight());
                dc.SetPen(*wxBLACK_PEN);
            }
        }
        else
        {
            HArrowOffsets.push_back(wxPoint(-20,-20));
            HArrowToOffset.push_back(-40);
            wxString str(wxEmptyString);
            wxPoint pt(-20,-20);
            DrawEditableText( dc, str, pt, false );
        }
    }
    if ( WindowState == InsertingHArrowWaitingSecondPoint )
    {
        bool fromfound = false, tofound = false;
        wxInt32 fromvpos = doc->vertlines[editingNumber].vpos;
        wxInt32 tovpos = 0;
        if (editingValB == -1 )
            tofound = true;
        else
            tovpos = doc->vertlines[editingValB].vpos;
        for ( wxUint32 i = 0 ; i < VisibleTicks.size() ; ++i)
        {
            if ( !fromfound && VisibleTicks[i] == fromvpos )
            {
                fromvpos = i;
                fromfound = true;
                //break;
            }
            if ( !tofound && VisibleTicks[i] == tovpos )
            {
                tovpos = i;
                tofound = true;
            }
        }

        if ( fromfound && tofound )
        {
            wxPoint offset(signalNamesWidth + fromvpos * GridStepWidth,
                    editingValA + heightOffsets[editingValC]);
            if ( doc->vertlines[editingNumber].vposoffset == 1 )
                offset.x += GridStepWidth*(doc->TransitWidth/2.0)/100.0;
            else if ( doc->vertlines[editingNumber].vposoffset == 2 )
                offset.x += GridStepWidth*(doc->TransitWidth)/100.0;
            wxPoint tooffset;
            tooffset.y = offset.y;
            if ( editingValB == -1 )
                tooffset.x = cursorpos.x;
            else
            {
                tooffset.x = signalNamesWidth + tovpos * GridStepWidth;
                if ( doc->vertlines[editingValB].vposoffset == 1 )
                    tooffset.x += GridStepWidth*(doc->TransitWidth/2.0)/100.0;
                else if ( doc->vertlines[editingValB].vposoffset == 2 )
                    tooffset.x += GridStepWidth*(doc->TransitWidth)/100.0;
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
    }
    dc.SetTextBackground(bgcol);
    dc.SetBackgroundMode(wxTRANSPARENT);


    /// drawing around the selected signal
    dc.DestroyClippingRegion();
    if ( !exporting )
        dc.SetClippingRegion(0, heightOffsets[0]+unscrolledPosition.y-3,virtsize.x, virtsize.y+12 );
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

    /// drawing discontinuities triangles and time line interrupts
    dc.DestroyClippingRegion();
    if ( !exporting )
        dc.SetClippingRegion(unscrolledPosition.x+signalNamesWidth-5,unscrolledPosition.y, virtsize.x, virtsize.y);
    for ( wxUint32 k = 0 ; k < VisibleTicks.size()-1 ; ++k )
    {
        wxInt32 tick = VisibleTicks[k];
        wxInt32 len = 1;
        for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
            if ( doc->compressors[indx].pos == tick && doc->compressors[indx].enabled )
                len = doc->compressors[indx].length;
        if ( len > 1 )
        {
            /// triangle
            offset.x = signalNamesWidth  +
                        (0.5 + k) * GridStepWidth
                        - 3
                        ;
            offset.y = DistanceToTicksLine-3;
            wxPoint points[] =
            {
                offset + wxPoint(0, unscrolledPosition.y),
                offset + wxPoint(8, unscrolledPosition.y),
                offset + wxPoint(4, unscrolledPosition.y + 6)
            };
            dc.SetBrush(*wxBLACK_BRUSH);
            if (  !exporting )
            {
                if ( WindowState == DisconSelected && tick == editingValA && editingValA != -1)
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
                        (0.5 + k) * GridStepWidth - 1
                        ;
            offset.y = DistanceToAxis - 3 + unscrolledPosition.y;
            if ( !exporting )
                offset.y += DistanceToTicksLine + DistanceFromTicksLine;
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
        for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
        if ( doc->compressors[indx].pos == tick && !doc->compressors[indx].enabled )
        {
        	len =  doc->compressors[indx].length;
            offset.x = signalNamesWidth  + (0.5 + k) * GridStepWidth;
            offset.y = DistanceToTicksLine-4 + unscrolledPosition.y;
            wxPoint points[] =
            {
                offset + wxPoint(-1, 0),
                offset + wxPoint( 4, 4),
                offset + wxPoint(-1, 8)
            };
            if ( !exporting )
            {
                wxInt32 rlen = len;
                for ( wxInt32 i = 1 ; i <= len ; ++i )
                {
                    for ( wxUint32 indx2 = 0 ; indx2 < doc->compressors.size() ; ++indx2)
                    if ( doc->compressors[indx2].pos == tick+i && doc->compressors[indx2].enabled )
                    {
                        if ( rlen > i +(doc->compressors[indx2].length-1))
                            rlen -= (doc->compressors[indx2].length-1);
                        else
                        {
                            rlen = i+1;
                            i = len;//break outer loop
                        }
                    }
                }

                dc.SetPen(*wxLIGHT_GREY_PEN);
                if ( offset.x + (rlen-0.75)*GridStepWidth < axisStop )
                {
                    dc.DrawLine(offset.x, offset.y, offset.x + (rlen-0.75)*GridStepWidth, offset.y);
                    dc.DrawLine(offset.x + (rlen-0.75)*GridStepWidth, offset.y, offset.x + (rlen-0.75)*GridStepWidth, offset.y+3);
                }
                else
                    dc.DrawLine(offset.x, offset.y, axisStop, offset.y);
                dc.SetPen(*wxBLACK_PEN);
                dc.SetBrush(*wxWHITE_BRUSH);

                if ( WindowState == DisconSelected && tick == editingValA && editingValA != -1)
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
        }
        dc.SetBrush(*wxBLACK_BRUSH);
        dc.SetPen(*wxBLACK_PEN);
    }

    /// cursor cross over the whole canvas:
    dc.DestroyClippingRegion();
    if (!exporting && mouseoverwindow &&
        WindowState != SelectingText &&
        WindowState != TextFieldSelected
        )
    {
        dc.SetPen(*wxGREY_PEN);
        dc.CrossHair(cursorpos.x, cursorpos.y);

        /// show current(cursors) position in ticks and time
        wxString str;
        if ( WindowState == AddTime )
        {
            double t = editingValC*doc->TickLength;
            wxInt8 u = doc->TickLengthUnit;
            str = wxString::Format(_T("Adding %d ticks = "), editingValC);
            while ( u < 3 && (t >= 1000.0 || t <= -1000.0) )
            {
                u++;
                t /= 1000.0;
            }
            str += wxString::Format(FloatFormatStr, t);
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
        }
        else if ( WindowState == RemoveTime)
        {
            wxInt32 r = editingValC - editingValB;
            if ( r < 0 ) r *= -1.0;
            double t = r*doc->TickLength;
            wxInt8 u = doc->TickLengthUnit;
            str = wxString::Format(_T("Removing %d ticks = "), r);
            while ( u < 3 && (t >= 1000.0 || t <= -1000.0) )
            {
                u++;
                t /= 1000.0;
            }
            str += wxString::Format(FloatFormatStr, t);
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
        }
        else
        {
            wxInt32 p = (cursorpos.x - axisStart)/GridStepWidth;
            double r  = 100.0*(cursorpos.x - axisStart -p*GridStepWidth)/GridStepWidth;
            if ( p >= 0 && p < (wxInt32)VisibleTicks.size() )
            {
                wxInt32 p1 = VisibleTicks[p];
                wxInt32 p2 = p1+1;
                if ( p+1 < (wxInt32)VisibleTicks.size() )
                    p2 = VisibleTicks[p+1];
                if ( r > 50.0 && p1 + 1 != p2)
                    p1 = p2;
                else
                    p1 = p1+1;
                str = wxString::Format(_T("tick: %d, t:"), p1);

                double t = (p1 + doc->timeOffset-1)*doc->TickLength;
                wxInt8 u = doc->TickLengthUnit;
                while ( u < 3 && (t >= 1000.0 || t <= -1000.0) )
                {
                    u++;
                    t /= 1000.0;
                }
                str += wxString::Format(FloatFormatStr, t);
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
            }
        }


        dc.DrawText(str, cursorpos.x+5,  unscrolledPosition.y + 5);
    }

    /// show where to "insert"/"remove" some time
    if ( !exporting && WindowState == Neutral )
    {
        if ( (cursorpos.x - axisStart ) > 0 && (cursorpos.x - axisStart ) < axisStop )
        {
            wxInt32 p = (cursorpos.x - axisStart )/GridStepWidth;
            bool hascompressor = false;
            for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
            if ( doc->compressors[indx].pos == p && doc->compressors[indx].enabled )
                hascompressor = true;
            if ( !hascompressor )
            {
                if (
                    cursorpos.y - unscrolledPosition.y > DistanceToTicksLine + DistanceFromTicksLine + DistanceToAxis - 10 &&
                    cursorpos.y - unscrolledPosition.y < DistanceToTicksLine + DistanceFromTicksLine + DistanceToAxis
                )
                {
                    dc.SetBrush(*wxWHITE_BRUSH);
                    dc.SetPen(*wxBLACK_PEN);
                    dc.DrawRectangle(cursorpos.x+2,      cursorpos.y+2 , 11, 11);
                    dc.DrawLine(     cursorpos.x+2 + 5,  cursorpos.y+2  + 2,
                                     cursorpos.x+2 + 5,  cursorpos.y+2  + 9);
                    dc.DrawLine(     cursorpos.x+2 + 2,  cursorpos.y+2  + 5,
                                     cursorpos.x+2 + 9,  cursorpos.y+2  + 5);
                }
                if (
                    cursorpos.y - unscrolledPosition.y > DistanceToTicksLine + DistanceFromTicksLine + DistanceToAxis  &&
                    cursorpos.y - unscrolledPosition.y < DistanceToTicksLine + DistanceFromTicksLine + DistanceToAxis + 10
                )
                {
                    dc.SetBrush(*wxWHITE_BRUSH);
                    dc.SetPen(*wxBLACK_PEN);
                    dc.DrawRectangle(cursorpos.x+2,      cursorpos.y+2 , 11, 11);
                    dc.DrawLine(     cursorpos.x+2 + 2,  cursorpos.y+2  + 5,
                                     cursorpos.x+2 + 9,  cursorpos.y+2  + 5);
                }
            }
        }
    }

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
            {
                SetNeutralState();
                WindowState = AxisIsMarked;
            }
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
        case AddTime:
        case RemoveTime:
            if ( ! event.LeftIsDown() )
                SetNeutralState();
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
                p.y > DistanceToTicksLine - 5 &&
                p.y < DistanceToTicksLine + 5 )
            {
                wxCoord p = pt.x-axisStart;
                wxInt32 discontpos = VisibleTicks[p / (GridStepWidth)];
                bool hascompressor = false;
                for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
                if ( doc->compressors[indx].pos == discontpos )
                {
                    newstate = DisconSelected;
                    editingValA = discontpos;
                    dorefresh = true;
                    hascompressor = true;
                }
                if ( hascompressor ) break;
            }
            /// check click on axis
            if (p.x > signalNamesWidth-10 &&
                pt.x > axisStart &&
                pt.x < axisStop &&
                p.y > DistanceToTicksLine - 5 &&
                p.y < DistanceToTicksLine + 5)
            {
                newstate = AxisIsMarked;
                dorefresh = true;
                if ( p.y <= DistanceToTicksLine )
                    editingValC = -2;
                else
                    editingValC = -3;
                break;
                editingValA = VisibleTicks[pt.x-axisStart / (GridStepWidth)];
                editingValB = -1;
            }
            /// check click on left points of the ticks line
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
                        editingNumber = n;
                        editingValA = VisibleTicks[(pt.x - axisStart) / (GridStepWidth)];
                        editingSignal = doc->signals[n];
                        editingValB = editingValA;

                        vals val = editingSignal.values[editingValA];
                        vals nval;
                        vals valr = u, vall = u;
                        if ( editingValA > 0 ) vall = editingSignal.values[editingValA - 1];
                        if ( editingValA <= doc->length ) valr = editingSignal.values[editingValA + 1];
                        if (  !editingSignal.IsBus ) //-- "normal" signal
                        {
                            if ( (vall == dc1 || vall == dc2 ) || ( valr == dc1 || valr == dc2 ))
                                switch ( val )
                                {
                                    case zero: nval = one; break;
                                    case one: nval = hz; break;
                                    case hz: nval = u; break;
                                    default:
                                    case u: nval = dc1; break;
                                    case dc1: nval = dc2; break;
                                    case dc2: nval = zero; break;
                                }
                            else
                                switch ( val )
                                {
                                    case zero: nval = one; break;
                                    case one: nval = hz; break;
                                    case hz: nval = u; break;
                                    default:
                                    case u: nval = dc1; break;
                                    case dc1:
                                    case dc2: nval = zero; break;
                                }
                            editingSignal.values[editingValA] = nval;
                        }
                        else // IsBus
                        {
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
                     pt.y >= HArrowOffsets[k].y - 3 &&
                     pt.y <= HArrowOffsets[k].y + 3 &&
                     ((HArrowOffsets[k].x < HArrowToOffset[k] && pt.x >= HArrowOffsets[k].x && pt.x <= HArrowToOffset[k])||
                      (HArrowOffsets[k].x > HArrowToOffset[k] && pt.x <= HArrowOffsets[k].x && pt.x >= HArrowToOffset[k]))
                     )
                {
                    newstate = HArrowIsMarked;
                    editingNumber = k;
                    editingValA = -1;
                    editingValB = -1;
                    dorefresh = true;
                    break;
                }
            }
            /// check click to "insert" time
            if ( p.x > signalNamesWidth-10 &&
                pt.x > axisStart &&
                pt.x < axisStop &&
                p.y > DistanceToTicksLine + DistanceFromTicksLine + DistanceToTicksLine - 10 &&
                p.y < DistanceToTicksLine + DistanceFromTicksLine + DistanceToTicksLine)
            {
                bool hascompressor = false;
                for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
                if ( doc->compressors[indx].pos == ((pt.x - axisStart )/GridStepWidth) )
                    hascompressor = true;
                if ( !hascompressor )
                {
                    newstate = AddTime;
                    editingValB = (pt.x - axisStart) / (GridStepWidth);
                    editingValA = VisibleTicks[editingValB];
                    editingValC = 0;
                    dorefresh = true;
                }
                break;
            }
            /// check click to "remove" time
            if ( p.x > signalNamesWidth-10 &&
                pt.x > axisStart &&
                pt.x < axisStop &&
                p.y > DistanceToTicksLine + DistanceFromTicksLine + DistanceToTicksLine &&
                p.y < DistanceToTicksLine + DistanceFromTicksLine + DistanceToTicksLine + 10 )
            {
                newstate = RemoveTime;
                editingValB = (pt.x - axisStart) / (GridStepWidth);
                editingValA = VisibleTicks[editingValB];
                editingValC = -1;
                dorefresh = true;
                break;
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
                p.y >= DistanceToTicksLine - 5 &&
                pt.x <= axisStop+5 &&
                p.y <= DistanceToTicksLine + 5 )
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
                p.y >= DistanceToTicksLine - 5 &&
                pt.x <= axisStart+5 &&
                p.y <= DistanceToTicksLine + 5)
            {
                newstate = EditAxisLeft;
                editingValA = doc->length;
                editingValB = editingValA;
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
               )
            {
                wxCoord p = pt.x-axisStart;
                wxInt32 discontpos = VisibleTicks[p / (GridStepWidth)];
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                bool hascompressor = false;
                for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
                if ( doc->compressors[indx].pos == discontpos )
                    hascompressor = true;
                if ( !hascompressor )
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
                if ( n &&
                    p.x > signalNamesWidth-10 && p.y > heightOffsets[0]-5 &&
                    pt.x > axisStart &&
                    pt.x < axisStop )
                {
                    wxInt32 rvpos = (pt.x - axisStart)/GridStepWidth;
                    editingPoint[0].x = VisibleTicks[rvpos];
                    wxCoord p = pt.x - axisStart - rvpos*GridStepWidth;
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
            /// check click on start/end of harrow
            if (p.x > signalNamesWidth-10 && p.y > heightOffsets[0]-5
                && pt.y>=HArrowOffsets[editingNumber].y - 3
                && pt.y<=HArrowOffsets[editingNumber].y + 3)
            {
                if (pt.x>=HArrowOffsets[editingNumber].x - 3 &&
                     pt.x<=HArrowOffsets[editingNumber].x + 3) /// the start of the harrow
                {
                    newstate = ChangingHArrowLengthStart;
                    editingValA = doc->harrows[editingNumber].fromVLine;
                    editingValB = doc->harrows[editingNumber].toVLine;
                }
                else if (pt.x>=HArrowToOffset[editingNumber] - 3 &&
                     pt.x<=HArrowToOffset[editingNumber] + 3) /// the end of the harrow
                {
                    newstate = ChangingHArrowLengthEnd;
                    editingValA = doc->harrows[editingNumber].fromVLine;
                    editingValB = doc->harrows[editingNumber].toVLine;
                }
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
            else // click beside the text
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
        case ChangingHArrowLengthStart:
        case ChangingHArrowLengthEnd:
        case MovingVLine:
        case MovingHArrow:
        case AddTime:
        case RemoveTime:
            /// should not happen in this states: MouseLeftDown
            SetNeutralState();
            break;
    }
    WindowState = newstate;
//    if ( newstate == SignalIsSelected && doc->signals[editingNumber].IsClock )
//        UpdateClockPanel();
//    if ( newstate == DisconSelected )
//        UpdateTimeCompressorPanel();

    if ( dorefresh ) this->Refresh(true);
    return;
}

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
    else if ( WindowState == DisconSelected )
    {
        wxCommandProcessor *cmdproc = doc->GetCommandProcessor();

        for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx )
        if ( doc->compressors[indx].pos == editingValA )
            cmdproc->Submit(
                new ChangeTimeCompressor(doc, editingValA, doc->compressors[indx].length, !doc->compressors[indx].enabled)
            );
    }
    else if ( WindowState == Neutral )
    {
        OnMouseLeftDown(event);
    }


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

    scrollingleft = false;
    scrollingright = false;
    scrollingup = false;
    scrollingdown = false;
    scrolltimer.Stop();

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
        case EditAxisRight:
            if ( editingValB > 0 && editingValB != editingValA )
            {
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new ChangeLength(doc, editingValB) );
            }
            SetNeutralState();
            newstate = AxisIsMarked;
            break;
        case EditAxisLeft:
            if ( editingValB > 0 && editingValB != editingValA )
            {
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new ChangeLengthLeft(doc, editingValB) );
            }
            SetNeutralState();
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
            newstate = Neutral;
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
                newha.text = _("$t$");
                newha.textoffset.x = 0;
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
        case ChangingHArrowLengthStart:
        case ChangingHArrowLengthEnd:
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
            {
                wxCoord gridoff = editingValA / GridStepWidth;
                wxCoord off     = editingValA - GridStepWidth * gridoff;
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new ChangeHArrowTextPosCommand(doc, editingNumber,
                    off,// the new x offset
                    editingValB, // the new y offset
                    gridoff
                ));
            }
            newstate = HArrowIsMarked;
            break;
        case AddTime:
            if ( editingValC > 0 )
            {
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                cmdproc->Submit(new AddTimeCommand(doc, editingValA, editingValC));
            }
            newstate = Neutral;
            break;
        case RemoveTime:
            if (editingValC  != -1 )
            {
                wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
                if ( editingValC != editingValA )
                    cmdproc->Submit(new RemoveTimeCommand(doc, editingValA, editingValC));
            }
            newstate = Neutral;
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
    scrollingdown = false;
    scrollingup = false;
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
//            if ( doc->signals[n].IsClock ) UpdateClockPanel();

            wxMenu menu;
            menu.Append(TIMING_ID_DELETE, _T("Delet signal"));
            PopupMenu(&menu);
            break;
        }
    }

    if ( dorefresh ) this->Refresh(true);
    return;

}

void TimingWindow::CheckStartHScroll(wxPoint pos)
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

void TimingWindow::CheckStartVScroll(wxPoint pos)
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
        case EditingDiscontinuity:
        case InsertingVLineWaitingFirstPoint:
        case InsertingHArrowWaitingFirstPoint:
        case DisconSelected:
        case SelectingText:
            break;
        case AxisIsMarked:

            break;
        case EditAxisRight:
            if ( event.LeftIsDown() )
            {
                wxCoord p = pt.x-axisStart;
                if ( pt.x < axisStart )
                    editingValB = editingValA;
                else
                {
                    if ( (p / (GridStepWidth)) >= (wxInt32)VisibleTicks.size() )
                        editingValB = editingValA + (p / (GridStepWidth)) - VisibleTicks.size()+1;
                    else
                        editingValB = VisibleTicks[ p / (GridStepWidth) ];
                }
                CheckStartHScroll(pos);
            }
            else
            {
                SetNeutralState();
                newstate = AxisIsMarked;
            }
            break;
        case EditAxisLeft:
            if ( event.LeftIsDown() )
            {
                wxCoord p = pt.x - axisStart;
                if ( pt.x > axisStop )
                    editingValB = editingValA;
                else
                {
                    if ( pt.x <= axisStart )
                        editingValB = editingValA - p / (GridStepWidth);
                    else
                    {
                        if( VisibleTicks[p / (GridStepWidth)] != p / (GridStepWidth) )
                            editingValB  = editingValA;
                        else
                            editingValB = editingValA - VisibleTicks[p / (GridStepWidth)];
                    }
                }
                CheckStartHScroll(pos);
            }
            else
            {
                SetNeutralState();
                newstate = AxisIsMarked;
            }
            break;
        case EditSignal:
            if ( event.LeftIsDown() )
            {
                wxInt32 editingSignalPosStart = editingValA;
                wxInt32 editingSignalPosStop = editingValB;
                if (   pt.y >= heightOffsets[editingNumber]
                    && pt.x >= axisStart
                    && pt.y <= heightOffsets[editingNumber+1]
                    && pt.x <= axisStop )
                {
                    wxCoord p = pt.x - axisStart;
                    editingSignalPosStop = VisibleTicks[p / (GridStepWidth)];
                    vals val = doc->signals[editingNumber].values[editingSignalPosStart];
                    vals nval;
                    if ( doc->signals[editingNumber].IsBus )
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
                            case zero: nval = one; break;
                            case one: nval = hz; break;
                            case hz: nval = u; break;
                            default:
                            case u: nval = dc1; break;
                            case dc1: nval = dc2; break;
                            case dc2: nval = zero; break;
                        }

                    editingSignal.values = doc->signals[editingNumber].values;
                    if ( editingSignalPosStop >= editingSignalPosStart )
                        for ( wxInt32 n = editingSignalPosStart ; n <= editingSignalPosStop ; ++n )
                            editingSignal.values[n] = nval;
                    else
                        for ( wxInt32 n = editingSignalPosStart ; n >= editingSignalPosStop ; --n )
                            editingSignal.values[n] = nval;
                }
                else
                    editingSignalPosStop = -1;
                editingValA = editingSignalPosStart;
                editingValB = editingSignalPosStop;
                CheckStartHScroll(pos);
            }
            break;
        case SignalIsSelected:
            if ( event.LeftIsDown() )
                newstate = MovingSignal;
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
                CheckStartVScroll(pos);
            }
            break;
        case ChangingUpperSpace:
            if ( event.LeftIsDown() )
            {
                if ( pt.y >= ( heightOffsets[editingNumber] + doc->signals[editingNumber].prespace ) )
                    editingValA = 0;
                else
                    editingValA = ( heightOffsets[editingNumber] + doc->signals[editingNumber].prespace) - pt.y;
                CheckStartVScroll(pos);
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
                CheckStartVScroll(pos);
            }
            break;
        case InsertingVLineWaitingSecondPoint:
            {
                wxInt32 n = doc->signals.size();
                if ( n ) //some signals in the document
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
                    CheckStartVScroll(pos);
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
                CheckStartVScroll(pos);
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
                    editingValA = VisibleTicks[(pt.x - axisStart)/GridStepWidth];
                    wxCoord p = pt.x - axisStart - ((pt.x - axisStart)/GridStepWidth)*GridStepWidth;
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
                        {
                            if ( (((pt.x - axisStart)/GridStepWidth) + 1) < (wxInt32)VisibleTicks.size() )
                                editingValA = VisibleTicks[(((pt.x - axisStart)/GridStepWidth) + 1)];
                            else
                                editingValA = doc->vertlines[editingNumber].vpos;

                        }
                        editingValB = 0;
                    }
                }
                CheckStartHScroll(pos);
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
                CheckStartVScroll(pos);
            }
            break;
        case InsertingHArrowWaitingSecondPoint:
            /// check if user moves over a vertical line
            for ( wxUint32 k = 0 ; k < VLineOffsets.size() ; ++k )
            {
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
            CheckStartHScroll(pos);
            dorefresh = true;
            break;
        case ChangingHArrowLengthStart:
        case ChangingHArrowLengthEnd:
            for ( wxUint32 k = 0 ; k < VLineOffsets.size() ; ++k )
            {
                wxInt32  pos = heightOffsets[doc->harrows[editingNumber].signalnmbr] + doc->harrows[editingNumber].pos;
                if ( pt.x >= VLineOffsets[k].x - 3 &&
                     pt.x <= VLineOffsets[k].x + 3 &&
                     pt.y >= VLineOffsets[k].y &&
                     pt.y <= VLineToOffset[k] &&
                     pos >= VLineOffsets[k].y &&
                     pos <= VLineToOffset[k]
                   )
                {
                    if ( WindowState == ChangingHArrowLengthStart )
                    {
                        if ( (wxInt32)k != editingValB )
                            editingValA = k;
                        else
                            editingValA = -1;
                    }
                    else
                    {
                        if ( (wxInt32)k != editingValA )
                            editingValB = k;
                        else
                            editingValB = -1;
                    }
                    break;
                }
                else
                    if ( WindowState == ChangingHArrowLengthStart )
                        editingValA = -1;
                    else
                        editingValB = -1;
            }
            dorefresh = true;
            CheckStartHScroll(pos);
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
            CheckStartHScroll(pos);
            CheckStartVScroll(pos);
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
        case AddTime:
            if ( event.LeftIsDown() && pt.x > axisStart && pt.x < axisStop)
            {
                //wxCoord p = pt.x-axisStart;

                //editingValB = ;
                //editingValA = VisibleTicks[editingValB];
                editingValC = (pt.x - axisStart) / (GridStepWidth) - editingValB;
                if ( editingValC < 0 ) editingValC = 0;
            }
            else
                editingValC = 0;
            break;
        case RemoveTime:
            if ( event.LeftIsDown() && pt.x > axisStart && pt.x < axisStop)
                editingValC = VisibleTicks[(pt.x - axisStart) / (GridStepWidth)] ;
            else
                editingValC = -1;
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
    dx = 10;
    dy = 10;

    if ( ! event.m_controlDown )
    {
        if ( ! event.IsPageScroll() )
        {
            wxPoint ViewStart; GetViewStart(&ViewStart.x, &ViewStart.y);
            if (event.GetWheelRotation() >= 0)
                dy = -dy;
            Scroll(ViewStart.x, ViewStart.y + dy/4);
            wxPoint newViewStart; GetViewStart(&newViewStart.x, &newViewStart.y);
            wxPoint ppu; GetScrollPixelsPerUnit(&ppu.x, &ppu.y);
            cursorpos.x += ((newViewStart.x - ViewStart.x)*ppu.x);
            cursorpos.y += ((newViewStart.y - ViewStart.y)*ppu.y);
            this->Refresh();
        }
    }
    else
    {
        if ( event.GetWheelRotation() >= 0 )
            ZoomTicksOut();
        else
            ZoomTicksIn();
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
        case ChangingHArrowLengthStart:
        case ChangingHArrowLengthEnd:
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
                            //UpdateClockPanel();
                            this->Refresh(true);
                        }
                        break;
                    case WXK_END:
                        if ( editingNumber != n-1 )
                        {
                            editingNumber = n-1;
                            //UpdateClockPanel();
                            this->Refresh(true);
                        }
                        break;
                    case WXK_UP:
                        if ( editingNumber != 0)
                        {
                            --editingNumber;
                            //UpdateClockPanel();
                            this->Refresh(true);
                        }
                        break;
                    case WXK_DOWN:
                        if ( editingNumber < n-1 )
                        {
                            ++editingNumber;
                            //UpdateClockPanel();
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

    SetCursor(newcursor);

    SetNeutralState();
    WindowState = EditingDiscontinuity;
    editingNumber = -1;
    scrollingleft = false;
    scrollingright = false;
    scrollingdown = false;
    scrollingup = false;
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
    scrollingdown = false;
    scrollingup = false;
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
    scrollingdown = false;
    scrollingup = false;
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
    scrollingdown = false;
    scrollingup = false;
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
        case ChangingHArrowLengthStart:
        case ChangingHArrowLengthEnd:
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

        //wxCoord startx, starty;
        wxPoint ViewStart;
        this->GetViewStart(&ViewStart.x, &ViewStart.y);

        wxCoord dx = 0, dy = 0;
        if ( scrollingleft || scrollingright ) dx = 5;//dx = dc.GetCharWidth();
        if ( scrollingup || scrollingdown ) dy = 5;//dy = dc.GetCharHeight();

        if (scrollingleft) dx = -dx;
        if (scrollingup) dy = -dy;

        //GetScrollPixelsPerUnit(&xu, &yu);

        if ( cursorpos.x < signalNamesWidth || cursorpos.x >= virtsize.x )
            dx = 0;
        if ( cursorpos.y < 0 || cursorpos.y > virtsize.y)
            dy = 0;

        Scroll(ViewStart.x+dx, ViewStart.y+dy);
        wxPoint newViewStart; GetViewStart(&newViewStart.x, &newViewStart.y);
        wxPoint ppu; GetScrollPixelsPerUnit(&ppu.x, &ppu.y);
        cursorpos.x += ((newViewStart.x - ViewStart.x)*ppu.x);
        cursorpos.y += ((newViewStart.y - ViewStart.y)*ppu.y);


        wxPoint &pt = cursorpos;
        switch ( WindowState )
        {
            case EditAxisRight:
                editingValB = (cursorpos.x-axisStart) / GridStepWidth;
                break;
            case EditAxisLeft:
                if ( cursorpos.x < axisStop )
                    editingValB = editingValA - (cursorpos.x-axisStart) / (GridStepWidth);
                else
                    editingValB = editingValA;
                break;
            case ChangingUpperSpace:
                if ( cursorpos.y >= ( heightOffsets[editingNumber] + doc->signals[editingNumber].prespace ) )
                    editingValA = 0;
                else
                    editingValA = ( heightOffsets[editingNumber] + doc->signals[editingNumber].prespace) - cursorpos.y;
                break;
            case ChangingLowerSpace:
                if ( cursorpos.y <= (heightOffsets[editingNumber] + doc->signals[editingNumber].prespace +
                                doc->SignalHeight + doc->MinimumSignalDistance ) )
                    editingValA = 0;
                else
                    editingValA = cursorpos.y - (heightOffsets[editingNumber] + doc->signals[editingNumber].prespace +
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
                        wxCoord p = cursorpos.x-axisStart - (GridStepWidth)/2;
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
                        wxCoord p = cursorpos.x-axisStart + (GridStepWidth)/2;
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
                            if ( cursorpos.y < (heightOffsets[k] + heightOffsets[k+1])/2 )
                                break;
                        bool AllowedMove = true;
                        if ( WindowState == ChangingVLineLengthUpper ){
                            if ( k > kmax ) AllowedMove = false;}
                        else{
                            if ( k < kmin ) AllowedMove = false;}
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
                    //wxInt32 pos = DistanceToTicksLine + DistanceFromTicksLine + editingValA;
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
            case ChangingHArrowLengthStart:
            case ChangingHArrowLengthEnd:
                for ( wxUint32 k = 0 ; k < VLineOffsets.size() ; ++k )
                {
                    //wxInt32 pos = doc->harrows[editingNumber].pos + DistanceToTicksLine + DistanceFromTicksLine;
                    wxInt32 pos = heightOffsets[doc->harrows[editingNumber].signalnmbr] + doc->harrows[editingNumber].pos;
                    if (pt.x >= VLineOffsets[k].x - 3 &&
                        pt.x <= VLineOffsets[k].x + 3 &&
                        pt.y >= VLineOffsets[k].y &&
                        pt.y <= VLineToOffset[k] &&
                        pos >= VLineOffsets[k].y &&
                        pos <= VLineToOffset[k] )
                    {
                        if ( WindowState == ChangingHArrowLengthStart )
                        {
                            if ( editingValB != (wxInt32)k )
                                editingValA = k;
                            else
                                editingValA = -1;
                        }
                        else
                        {
                            if ( editingValA != (wxInt32)k )
                                editingValB = k;
                            else
                                editingValB = -1;
                        }
                        break;
                    }
                    else
                        if ( WindowState == ChangingHArrowLengthStart )
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
        scrolltimer.Start(200,wxTIMER_ONE_SHOT);
        this->Refresh(true);
    }
    else
    {
        scrollingleft = false;
        scrollingright = false;
        scrollingdown = false;
        scrollingup = false;
        scrolltimer.Stop();
    }
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

//void TimingWindow::DetachPanels()
//{
//    ClkSetPanel->wnd = (TimingWindow *)NULL;
//    TranSetPanel->wnd = (TimingWindow *)NULL;
//    AxisSetPanel->wnd = (TimingWindow *)NULL;
//    TmeCmprssrPanel->wnd = (TimingWindow *)NULL;
//}

//void TimingWindow::AttachPanels()
//{
//    ClkSetPanel->wnd = this;
//    TimingDocument *doc = (TimingDocument *)view->GetDocument();
//    if ( !doc ) return;
//
//    if ( IsSelectedSignalClock() )
//        UpdateClockPanel();
//
//    TranSetPanel->wnd = this;
//    UpdateTransitionPanel();
//
//    AxisSetPanel->wnd = this;
//    UpdateAxisPanel();
//
//    TmeCmprssrPanel->wnd = this;
//    UpdateTimeCompressorPanel();
//}

//void TimingWindow::UpdateClockPanel()
//{
//    TimingDocument *doc = (TimingDocument *)view->GetDocument();
//    if ( !doc ) return;
//
//    if ( IsSelectedSignalClock() )
//    {
//        wxInt32 n;
//        wxString str;
//
//        n = doc->signals[editingNumber].delay;
//        str = wxString::Format ( _( "%d" ) , n);
//        ClkSetPanel->SetDelayText(str);
//
//        n = doc->signals[editingNumber].ticks;
//        str = wxString::Format ( _( "%d" ) , n);
//        ClkSetPanel->SetTicksText(str);
//
//        ClkSetPanel->SetShadowed(doc->signals[editingNumber].GenerateBackground);
//        ClkSetPanel->SetShowPeriod(doc->signals[editingNumber].ShowPeriodCount);
//    }
//}

//void TimingWindow::SetClock(wxInt32 delay, wxInt32 ticks, bool shadow, bool DrawPeriod)
//{
//    SetFocus();
//    TimingDocument *doc = (TimingDocument *)view->GetDocument();
//    if ( !doc ) return;
//    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
//
//    if ( IsSelectedSignalClock() )
//    {
//        cmdproc->Submit(
//            new ChangeClockParamCommand(doc, editingNumber , ticks, delay, shadow, DrawPeriod)
//        );
//    }
//}
//
//void TimingWindow::UpdateAxisPanel()
//{
//    TimingDocument *doc = (TimingDocument *)view->GetDocument();
//    if ( !doc ) return;
//
//    AxisSetPanel->SetTickLength(doc->TickLength);
//    AxisSetPanel->SetLengthUnit(doc->TickLengthUnit + 5);
//    AxisSetPanel->SetMarkerLength(doc->MarkerLength);
//    AxisSetPanel->SetOffset(doc->timeOffset);
//    AxisSetPanel->SetTotalLengt(doc->length);
//}

//void TimingWindow::SetAxis(wxInt8 unit, wxInt32 ticklength, wxInt32 markerlength, wxInt32 offset, wxInt32 totallength)
//{
//    SetFocus();
//
//    TimingDocument *doc = (TimingDocument *)view->GetDocument();
//    if ( !doc ) return;
//    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
//    cmdproc->Submit(
//        new ChangeAxisSettings(doc, unit-5, ticklength, markerlength, offset, totallength )
//    );
//    UpdateAxisPanel();
//}

//void TimingWindow::UpdateTransitionPanel()
//{
//    TimingDocument *doc = (TimingDocument *)view->GetDocument();
//    if ( !doc ) return;
//
//    TranSetPanel->SetTransitionWidth(doc->TransitWidth);
//    TranSetPanel->Set50(doc->en50);
//    TranSetPanel->Set90(doc->en90);
//
//}

wxInt8 TimingWindow::GetTransitionWidth()
{
    TimingDocument *doc = (TimingDocument *)view->GetDocument();
    if ( !doc ) return 25;

    return doc->TransitWidth;
}

//void TimingWindow::SetTransition(wxInt8 width, bool en50, bool en90)
//{
//    SetFocus();
//
//    TimingDocument *doc = (TimingDocument *)view->GetDocument();
//    if ( !doc ) return;
//    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
//    cmdproc->Submit(
//        new ChangeTransitionWidth(doc, width, en50, en90)
//    );
//    UpdateTransitionPanel();
//}
//
//void TimingWindow::UpdateTimeCompressorPanel(void)
//{
//    TimingDocument *doc = (TimingDocument *)view->GetDocument();
//    if ( !doc ) return;
//
//    if ( !DiscontSelected() ) return;
//
//    for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx )
//    if ( doc->compressors[indx].pos == editingValA )
//    {
//        TmeCmprssrPanel->SetTimeText(
//            wxString::Format( _("%d"), doc->compressors[indx].length )
//        );
//    }
//}

//void TimingWindow::SetTimeCompressor(wxInt32 time)
//{
//    SetFocus();
//    TimingDocument *doc = (TimingDocument *)view->GetDocument();
//    if ( !doc ) return;
//    if ( !DiscontSelected() ) return;
//
//    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
//    for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
//    if ( doc->compressors[indx].pos == editingValA )
//        cmdproc->Submit(
//            new ChangeTimeCompressor(doc, editingValA, time, doc->compressors[indx].enabled)
//        );
//
//    UpdateTimeCompressorPanel();
//}

void TimingWindow::OnScroll(wxScrollWinEvent &event)
{
    //wxPoint res(0,0);
    int nScrollInc = CalcScrollInc(event);
    if ( nScrollInc == 0 )// can't scroll further
    {
        event.Skip();
        return;
    }

//    int dx = 0,
//        dy = 0;
//    int orient = event.GetOrientation();
//    if (orient == wxHORIZONTAL)
//       if ( m_xScrollingEnabled )
//           dx = -m_xScrollPixelsPerLine * nScrollInc;
//    else
//        if ( m_yScrollingEnabled )
//            dy = -m_yScrollPixelsPerLine * nScrollInc;

    if (event.GetOrientation() == wxHORIZONTAL)
    {
        m_xScrollPosition += nScrollInc;
        m_win->SetScrollPos(wxHORIZONTAL, m_xScrollPosition);
    }
    else
    {
        m_yScrollPosition += nScrollInc;
        m_win->SetScrollPos(wxVERTICAL, m_yScrollPosition);
    }

    this->Refresh(true, GetScrollRect());
}

void TimingWindow::OnSize(wxSizeEvent &event)
{
    event.Skip();
    this->Refresh();
}

void TimingWindow::Scroll( int x_pos, int y_pos )
{

    wxPoint res(0,0);

    if (((x_pos == -1) || (x_pos == m_xScrollPosition)) &&
        ((y_pos == -1) || (y_pos == m_yScrollPosition))) return;

    int w = 0, h = 0;
    GetTargetSize(&w, &h);

    // compute new position:
    int new_x = m_xScrollPosition;
    int new_y = m_yScrollPosition;

    if ((x_pos != -1) && (m_xScrollPixelsPerLine))
    {
        new_x = x_pos;

        // Calculate page size i.e. number of scroll units you get on the
        // current client window
        int noPagePositions = w/m_xScrollPixelsPerLine;
        if (noPagePositions < 1) noPagePositions = 1;

        // Correct position if greater than extent of canvas minus
        // the visible portion of it or if below zero
        new_x = wxMin( m_xScrollLines-noPagePositions, new_x );
        new_x = wxMax( 0, new_x );
    }
    if ((y_pos != -1) && (m_yScrollPixelsPerLine))
    {
        new_y = y_pos;

        // Calculate page size i.e. number of scroll units you get on the
        // current client window
        int noPagePositions = h/m_yScrollPixelsPerLine;
        if (noPagePositions < 1) noPagePositions = 1;

        // Correct position if greater than extent of canvas minus
        // the visible portion of it or if below zero
        new_y = wxMin( m_yScrollLines-noPagePositions, new_y );
        new_y = wxMax( 0, new_y );
    }

    if ( new_x == m_xScrollPosition && new_y == m_yScrollPosition )
        return; // nothing to do, the position didn't change

    // flush all pending repaints before we change m_{x,y}ScrollPosition, as
    // otherwise invalidated area could be updated incorrectly later when
    // ScrollWindow() makes sure they're repainted before scrolling them
    //m_targetWindow->Update();

    Refresh();
    // update the position and scroll the window now:
    if (m_xScrollPosition != new_x)
    {
        int old_x = m_xScrollPosition;
        m_xScrollPosition = new_x;
        SetScrollPos( wxHORIZONTAL, new_x );
        ScrollWindow( (old_x-new_x)*m_xScrollPixelsPerLine, 0,
                                      GetScrollRect() );
    }

    if (m_yScrollPosition != new_y)
    {
        int old_y = m_yScrollPosition;
        m_yScrollPosition = new_y;
        SetScrollPos( wxVERTICAL, new_y );
        ScrollWindow( 0, (old_y-new_y)*m_yScrollPixelsPerLine,
                                      GetScrollRect() );
    }
    //Refresh();
}
