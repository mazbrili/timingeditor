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
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/svg/dcsvg.h>
#include <wx/dcps.h>
#include <wx/filename.h>

#include "TimingView.h"
#include "TimingWindow.h"
#include "TimingMainFrame.h"
#include "TimingApp.h"
#include "cmd.h"
#include "enumers.h"


#include "DiagramSplitterWindow.h"
#include "DiagramRightWindow.h"


#include "GraphBusSignal.h"
#include "GraphClockSignal.h"
#include "GraphNormalSignal.h"

#include "Task.h"
#include "AddVerticalLineTask.h"


IMPLEMENT_DYNAMIC_CLASS(TimingView, wxView)
BEGIN_EVENT_TABLE(TimingView, wxView)
    EVT_MENU(TIMING_ID_DELETE,          TimingView::OnDelete)
    EVT_MENU(wxID_SELECTALL,            TimingView::OnSelectAll)
    EVT_MENU(wxID_COPY,                 TimingView::OnCopy)
    EVT_MENU(wxID_CUT,                  TimingView::OnCut)
    EVT_MENU(wxID_PASTE,                TimingView::OnPaste)

    EVT_MENU(TIMING_ID_GLASS_N,         TimingView::OnZoomOut)
    EVT_MENU(TIMING_ID_GLASS_P,         TimingView::OnZoomIn)

    //EVT_MENU(TIMING_ID_CHANGECLOCK,     TimingView::OnEditClock)
    EVT_MENU(TIMING_ID_ADD_CLOCK,       TimingView::OnAddClock)
    EVT_MENU(TIMING_ID_ADD_SIGNAL,      TimingView::OnAddSignal)
    EVT_MENU(TIMING_ID_ADD_BUS,         TimingView::OnAddBus)

    EVT_MENU(TIMING_ID_DISCONTINUITY,   TimingView::OnDiscontinuityTool)
    EVT_MENU(TIMING_ID_RULER,           TimingView::OnRulerTool)
    EVT_MENU(TIMING_ID_HARROW,          TimingView::OnHArrowTool)
    EVT_MENU(TIMING_ID_EDIT,            TimingView::OnEditTool)
    //EVT_MENU(TIMING_ID_EDITTEXT,        TimingView::OnSelectTextTool)
    EVT_MENU(TIMING_ID_EXPORT_BITMAP,   TimingView::OnExportBitmap)
    EVT_MENU(TIMING_ID_EXPORT_SVG,      TimingView::OnExportSVG)
    EVT_MENU(TIMING_ID_EXPORT_PS,       TimingView::OnExportPS)
END_EVENT_TABLE()


/// //////////////////////////////////// construction and destruction
TimingView::TimingView()
{
    splitterwindow = (DiagramSplitterWindow *)NULL;
    frame = (wxDocMDIChildFrame *)NULL;
    GridStepWidth = 15;
    ClkSetPanel = (ClockSettingsPanel *)NULL;
    TranSetPanel = (TransitionSettingsPanel *)NULL;
    AxisSetPanel = (AxisSettingsPanel *)NULL;
    TmeCmprssrPanel = (TimeCompressorSettingsPanel *)NULL;
    task = NULL;
    defaultTask = NULL;
}
bool TimingView::OnCreate(wxDocument *doc, long WXUNUSED(flags))
{
    frame = wxGetApp().CreateChildFrame(doc, this);
    splitterwindow = wxGetApp().GetMainFrame()->CreateWindow(this, frame);

#ifdef __X__
    int x, y;
    frame->GetSize(&x, &y);
    frame->SetSize(wxDefaultCoord, wxDefaultCoord, x, y);
#endif

    DiagramRightWindow *rwnd = splitterwindow->GetRightWindow();
    defaultTask = new Task(this, splitterwindow->GelLabelsWindow(), rwnd->GetAxisWindow(), rwnd->GetWavesWindow());
    if (!defaultTask)
        return false;

    task = defaultTask;

    UpdateVisibelTicksContainer();
    AttachPanels();
    Activate(true);

    frame->Show(true);

    return true;
}
// Clean up windows used for displaying the view.
bool TimingView::OnClose(bool deleteWindow)
{
    if (!GetDocument()->Close()) return false;

    DetachPanels();

    splitterwindow->view = (TimingView *)NULL;
    splitterwindow = (DiagramSplitterWindow *)NULL;

    SetFrame((wxFrame*)NULL);
    Activate(false);

    // Remove file menu from those managed by the command history
    wxMenu* fileMenu = frame->GetMenuBar()->GetMenu(0);
    wxGetApp().GetDocManager()->FileHistoryRemoveMenu(fileMenu);

    if (deleteWindow)
        delete frame;
    return true;
}

void TimingView::SetPanels(ClockSettingsPanel *csp, TransitionSettingsPanel *tsp, AxisSettingsPanel *asp, TimeCompressorSettingsPanel *tcsp)
{
    ClkSetPanel = csp;
    TranSetPanel = tsp;
    AxisSetPanel = asp;
    TmeCmprssrPanel = tcsp;
}

/// ///////////////////////////////////////////////////////////////// colours fonts spaces
wxColour TimingView::GetBackgroundColour() const
{
    //return wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE );
    return *wxWHITE;
}
wxColour TimingView::GetLineColour() const{return *wxBLACK;}
wxColour TimingView::GetTextColour() const{return *wxBLACK;}
wxColour TimingView::GetShadowColour() const{return wxColour(0xf0,0xf0,0xf0);}
wxColour TimingView::GetWaveSeparatorColour() const{return wxColour(0xe0, 0xe0, 0xe0);}
wxColour TimingView::GetAxisBackgroundColour()const{
    return wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE );}
wxColour TimingView::GetAxisLineColour()const{return *wxBLACK;}
wxColour TimingView::GetLabelsBackgroundColour()const
{
    return wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE );
}
wxColour TimingView::GetLabelsTextColour()const{return *wxBLACK;}
wxColour TimingView::GetLabelsLineColour()const
{
    return wxSystemSettings::GetColour( wxSYS_COLOUR_BTNSHADOW );
}
wxColour TimingView::GetUndefinedSignalColour() const{return *wxLIGHT_GREY;}
wxColour TimingView::GetCompressorColour()const
{
    //return *wxGREY;
    return wxTheColourDatabase->Find(_T("GREY"));
}
wxColour TimingView::GetGraphCaretColour()const
{
    return *wxBLUE;
}
int TimingView::GetWavesLeftSpace()const{return 10;}
wxString TimingView::GetFloatFormatStr() const
{
    unsigned char digitsAfterDecimalpoint = 2;
    return wxString::Format(_T("\%.%df "), digitsAfterDecimalpoint);
}
wxString TimingView::GetTimeString(wxInt32 ticks)
{
    TimingDocument *doc = (TimingDocument *) GetDocument();
    double t = (ticks + doc->timeOffset) * doc->TickLength;
    wxInt8 u = doc->TickLengthUnit;
    while ( u < 3 && (t >= 1000.0 || t <= -1000.0))
    {
        u++;
        t /= 1000.0;
    }
    wxString str(wxString::Format(GetFloatFormatStr(), t));
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
    return str;
}
wxFont TimingView::GetFont() const
{
    return wxFont(10, wxFONTFAMILY_SWISS,  wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
}
wxCoord TimingView::GetHeightOfAxisWindow()const
{
    return 60;
}

/// ////////////////////////////////////////////////////////////  panels
void TimingView::OnActivateView(bool activate, wxView *activeView, wxView *deactiveView)
{
    if ( activate && this == activeView )
        AttachPanels();
}
void TimingView::AttachPanels()
{
    TimingDocument *doc = (TimingDocument *)GetDocument();

    if ( !doc )
    {
        DetachPanels();
        return;
    }

    ClkSetPanel->view = this;
    if ( IsSelectedSignalClock() )
        UpdateClockPanel();

    TranSetPanel->view = this;
    UpdateTransitionPanel();

    AxisSetPanel->view = this;
    UpdateAxisPanel();

    TmeCmprssrPanel->view = this;
    UpdateTimeCompressorPanel();
}
void TimingView::DetachPanels()
{
    ClkSetPanel->view = (TimingView *)NULL;
    TranSetPanel->view = (TimingView *)NULL;
    AxisSetPanel->view = (TimingView *)NULL;
    TmeCmprssrPanel->view = (TimingView *)NULL;
}
void TimingView::UpdateClockPanel()
{
    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( !doc ) return;

    if ( IsSelectedSignalClock() )
    {
//        wxInt32 n;
//        wxString str;

////        n = doc->signals[editingNumber].delay;
//        str = wxString::Format ( _( "%d" ) , n);
//        ClkSetPanel->SetDelayText(str);
//
//        n = doc->signals[editingNumber].ticks;
//        str = wxString::Format ( _( "%d" ) , n);
//        ClkSetPanel->SetTicksText(str);
//
//        ClkSetPanel->SetShadowed(doc->signals[editingNumber].GenerateBackground);
//        ClkSetPanel->SetShowPeriod(doc->signals[editingNumber].ShowPeriodCount);
    }
}
void TimingView::UpdateTransitionPanel()
{
    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( !doc ) return;

    TranSetPanel->SetTransitionWidth(doc->TransitWidth);
    TranSetPanel->Set50(doc->en50);
    TranSetPanel->Set90(doc->en90);

}
void TimingView::UpdateAxisPanel()
{
    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( !doc ) return;

    AxisSetPanel->SetTickLength(doc->TickLength);
    AxisSetPanel->SetLengthUnit(doc->TickLengthUnit + 5);
    AxisSetPanel->SetMarkerLength(doc->MarkerLength);
    AxisSetPanel->SetOffset(doc->timeOffset);
    AxisSetPanel->SetTotalLengt(doc->length);
}
void TimingView::UpdateTimeCompressorPanel(void)
{
    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( !doc ) return;

    if ( !IsDiscontinuitySelected() ) return;

//    for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx )
//        if ( doc->compressors[indx].pos == editingValA )
//            TmeCmprssrPanel->SetTimeText(
//                wxString::Format( _("%d"), doc->compressors[indx].length )
//            );
}


/// ////////////////////////////////////////////////////////// interface for panels
void TimingView::SetAxis(wxInt8 unit, wxInt32 ticklength, wxInt32 markerlength, wxInt32 offset, wxInt32 totallength)
{
    ///SetFocus();
    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( !doc ) return;
    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
    cmdproc->Submit(
        new ChangeAxisSettings(doc, unit-5, ticklength, markerlength, offset, totallength )
    );
    UpdateAxisPanel();
}
void TimingView::SetClock(wxInt32 delay, wxInt32 ticks, bool shadow, bool DrawPeriod)
{
    ///SetFocus();
    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( !doc ) return;
    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();

    if ( IsSelectedSignalClock() )
    {
        cmdproc->Submit(
            new ChangeClockParamCommand(doc, GetSelectedSignalNumber() , ticks, delay, shadow, DrawPeriod)
        );
    }
}
void TimingView::SetTransition(wxInt8 width, bool en50, bool en90)
{
    ///SetFocus();
    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( !doc ) return;
    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
    cmdproc->Submit(
        new ChangeTransitionWidth(doc, width, en50, en90)
    );
    UpdateTransitionPanel();
}
void TimingView::SetTimeCompressor(wxInt32 time)
{
    ///SetFocus();
    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( !doc ) return;
    if ( !IsDiscontinuitySelected() ) return;

    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();
    wxInt32 index = GetSelectedDiscontinuity();
    if ( doc->compressors.find(index) != doc->compressors.end())
        cmdproc->Submit(
            new ChangeTimeCompressor(doc, index, time, doc->compressors[index].enabled)
        );

    UpdateTimeCompressorPanel();
}

/// //////////////////////////////////////////////////////////// drawing
void TimingView::OnDraw(wxDC *dc){} // is virtual and has nothing to do (flicker free mechanism is walking on another way)

/// ///////////////////////////////////////////////////////////  update internal structure after changes in the document
void TimingView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint))
{
    TimingDocument *doc = (TimingDocument *)m_viewDocument;
    wxFileName fn( doc->GetFilename() );
    wxString fname = fn.GetName();

    if ( fname.IsEmpty() )
        fname = doc->GetTitle();

    if (frame)
    {
        if ( doc->IsModified() )
            frame->SetTitle( _("*") + fname );
        else
            frame->SetTitle( fname );
    }


    UpdateVisibelTicksContainer();
    UpdateHeightsContainer();

    UpdateVerticalLines();
    UpdateHorizontalArrows();

    UpdateSignals();



    AttachPanels();
    if (splitterwindow)
        splitterwindow->Update();
}
void TimingView::UpdateHeightsContainer()
{
    TimingDocument *doc = (TimingDocument *)m_viewDocument;

    heightOffsets.clear();
    wxCoord yoffset = 20;
    for ( unsigned int k = 0 ; k < doc->signals.size() ; ++k )
    {
        /// remember position
        heightOffsets.push_back(yoffset);
        /// update position for next signal
        yoffset += doc->SignalHeight;
        yoffset += doc->MinimumSignalDistance;
        yoffset += doc->signals[k].space;
        yoffset += doc->signals[k].prespace;
    }
    heightOffsets.push_back(yoffset);// remember last offset
}
void TimingView::UpdateVisibelTicksContainer()
{
    TimingDocument *doc = (TimingDocument *)m_viewDocument;

    VisibleTicks.clear();
    wxInt32 n = 0;
    while ( n <= doc->length )
    {
        VisibleTicks.push_back(n);

        bool hascompressor = false;
        if ( doc->compressors.find(n) != doc->compressors.end() && doc->compressors[n].enabled )
        {
            n += doc->compressors[n].length;
            if ( n > doc->length )
                VisibleTicks.push_back(n);
            hascompressor = true;
        }
        if ( !hascompressor )
            ++n;

    }
}
void TimingView::UpdateVerticalLines()
{
    TimingDocument *doc = (TimingDocument *)m_viewDocument;

    m_vertlines.clear();


    for ( wxUint32 k = 0 ; k < doc->vertlines.size() ; ++k)
    {
        bool found = false;
        wxInt32 vpos;
        for ( wxUint32 i = 0 ; i < VisibleTicks.size() ; ++i)
        {
            if (VisibleTicks[i] == doc->vertlines[k].vpos)
            {
                found = true;
                vpos = i;
                break;
            }
        }
        if ( !found )
            continue;

        wxPoint offset(GetWavesLeftSpace() + vpos * GridStepWidth,
                       heightOffsets[doc->vertlines[k].StartPos]);

        if ( doc->en50 && doc->vertlines[k].vposoffset == 1 )
            offset.x += GridStepWidth/(100.0/(doc->TransitWidth/2.0));
        if ( doc->en90 && doc->vertlines[k].vposoffset == 2 )
            offset.x +=  GridStepWidth/(100.0/(doc->TransitWidth));

//        dc.DrawLine(
//            offset.x, offset.y,
//            offset.x,  heightOffsets[doc->vertlines[k].EndPos + 1]
//        );
        m_vertlines.push_back(GraphVerticalLine(offset, wxPoint(offset.x,  heightOffsets[doc->vertlines[k].EndPos + 1])));
    }

}
void TimingView::UpdateHorizontalArrows()
{
    TimingDocument *doc = (TimingDocument *)m_viewDocument;

    m_horizontalarrows.clear();


    wxCoord leftSpace = GetWavesLeftSpace();

    /// drawing the horizontal arrows
    //HArrowOffsets.clear();
    //HArrowToOffset.clear();
    for ( wxUint32 n = 0 ; n < doc->harrows.size() ; ++n )
    {
        HArrow &ha = doc->harrows[n];
        bool fromVlineVisible = false, toVlineVisible = false;
        wxInt32 fromvpos = doc->vertlines[ha.fromVLine].vpos;
        wxInt32 fromvposoffset = doc->vertlines[ha.fromVLine].vposoffset;
        wxInt32 tovpos = doc->vertlines[ha.toVLine].vpos;
        wxInt32 tovposoffset = doc->vertlines[ha.toVLine].vposoffset;

        for ( wxUint32 i = 0 ; i < VisibleTicks.size() ; ++i)
        {
            if ( !fromVlineVisible && VisibleTicks[i] == fromvpos )
            {
                fromvpos = i;
                fromVlineVisible = true;
                //break;
            }
            if ( !toVlineVisible && VisibleTicks[i] == tovpos )
            {
                tovpos = i;
                toVlineVisible = true;
            }
        }
        if ( fromVlineVisible && toVlineVisible)
        {
            /// calc offset based on vline to start from
            wxPoint offset(leftSpace + fromvpos * GridStepWidth,
                    ha.pos + heightOffsets[ha.signalnmbr]);
            if ( doc->en50 && fromvposoffset == 1 )
                offset.x += GridStepWidth/(100.0/(doc->TransitWidth/2.0));
            else if ( doc->en90 && fromvposoffset == 2 )
                offset.x += GridStepWidth/(100.0/(doc->TransitWidth));

            /// calc offset based on vline where harrow will end
            wxPoint tooffset(leftSpace + tovpos * GridStepWidth,
                offset.y);
            if ( doc->en50 && tovposoffset == 1 )
                tooffset.x += GridStepWidth/(100.0/(doc->TransitWidth/2.0));
            else if ( doc->en90 && tovposoffset == 2 )
                tooffset.x += GridStepWidth/(100.0/(doc->TransitWidth));


            if ( offset.x > tooffset.x ) // swap
            {
                wxCoord t = offset.x;
                offset.x = tooffset.x;
                tooffset.x = t;
            }
            ///the text:
            wxPoint textoff;

            textoff.x  = (offset.x + tooffset.x)/2;
            if ( ha.textoffset.x > GridStepWidth )
                textoff.x += GridStepWidth;
            else
                textoff.x += ha.textoffset.x;
            textoff.x += ha.textgridoffset*GridStepWidth;
            textoff.y  = offset.y + ha.textoffset.y;

            wxString text = ha.text;
            wxInt32 pos = text.Find( _T("$t$"));
            if ( pos != wxNOT_FOUND )
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
                str = wxString::Format(GetFloatFormatStr(), t);
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
            m_horizontalarrows.push_back(GraphHorizontalArrow(offset, tooffset, text, textoff));
        }
    }


}
void TimingView::UpdateSignals()
{
    TimingDocument *doc = (TimingDocument *)m_viewDocument;

    m_graphsignals.clear();

    for ( unsigned int i = 0  ; i < doc->signals.size(); i++ )
    {
        if ( doc->signals[i].IsClock )
            m_graphsignals.push_back(new GraphClockSignal(this, &doc->signals[i], heightOffsets[i]));
        else if ( doc->signals[i].IsBus )
            m_graphsignals.push_back(new GraphBusSignal(this, &doc->signals[i], heightOffsets[i]));
        else
            m_graphsignals.push_back(new GraphNormalSignal(this, &doc->signals[i], heightOffsets[i]));
    }

}

/// //////////////////////////////////////////////////////////  make internal structures accessible for the windows
const VerticalLines &TimingView::GetVerticalLines()const
{
    return m_vertlines;
}
const HorizontalArrows &TimingView::GetHorizontalArrows()const
{
    return m_horizontalarrows;
}
const GraphSignals &TimingView::GetGraphSignals()const
{
    return m_graphsignals;
}

/// ////////////////////////////////////////////////////////// adding signals
void TimingView::OnAddClock(wxCommandEvent& event)
{
    TimingDocument *doc = (TimingDocument*)m_viewDocument;
    wxInt32 n = GetSelectedSignalNumber();

    Signal sig;
    sig.delay=1;
    sig.ticks=2;
    sig.IsClock = true;
    sig.IsBus = false;
    sig.name = _("newClock");
    sig.space = 0;
    sig.prespace = 0;

    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();

    SetTask(NULL);

    cmdproc->Submit( new AddSignalCommand(doc, n, sig) );

}
void TimingView::OnAddSignal(wxCommandEvent& event)
{
    TimingDocument *doc = (TimingDocument*)m_viewDocument;
    wxInt32 n = GetSelectedSignalNumber();

    Signal sig;
    sig.delay = 1;
    sig.ticks = 2;
    sig.IsClock = false;
    sig.IsBus = false;
    sig.name = _("newSignal");
    sig.space = 0;
    sig.prespace = 0;
    for (wxInt32 n = 0 ; n < doc->length; ++n)
        sig.values.push_back(zero);

    SetTask(NULL);

    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();

    cmdproc->Submit( new AddSignalCommand(doc, n, sig) );

}
void TimingView::OnAddBus(wxCommandEvent& event)
{
    TimingDocument *doc = (TimingDocument*)m_viewDocument;
    wxInt32 n = GetSelectedSignalNumber();

    Signal sig;
    sig.delay = 1;
    sig.ticks = 2;
    sig.IsClock = false;
    sig.IsBus = true;
    sig.name = _("newBus");
    sig.space = 0;
    sig.prespace = 0;
    for (wxInt32 n = 0 ; n < doc->length; ++n)
        sig.values.push_back(zero);
    sig.TextValues[0] = _("Data");

    SetTask(NULL);

    wxCommandProcessor *cmdproc = doc->GetCommandProcessor();

    cmdproc->Submit( new AddSignalCommand(doc, n, sig) );
}


/// ///////////////////////////////////////////////////////////////////// tools selected
void TimingView::OnDiscontinuityTool(wxCommandEvent& event)
{
}
void TimingView::OnRulerTool(wxCommandEvent& event)
{
    DiagramRightWindow *rwnd = splitterwindow->GetRightWindow();
    AddVerticalLineTask *newtask = new AddVerticalLineTask(this,splitterwindow->GelLabelsWindow(), rwnd->GetAxisWindow(), rwnd->GetWavesWindow());

    if ( task != defaultTask )
        delete task;
    SetTask(newtask);
}
void TimingView::OnHArrowTool(wxCommandEvent& event)
{
}
//void TimingView::OnSelectTextTool(wxCommandEvent& event)

void TimingView::OnEditTool(wxCommandEvent& event)
{
}

///  ///////////////////////////////////////////////////////// zooming
bool TimingView::CanZoomIn(void)
{
    return GridStepWidth < 150;
}
bool TimingView::CanZoomOut(void)
{
    return GridStepWidth > 1;
}
void TimingView::OnZoomOut(wxCommandEvent& WXUNUSED(event) )
{
    if ( GridStepWidth > 1 )
        GridStepWidth  *= 0.7;
    if ( GridStepWidth < 1 ) GridStepWidth = 1;
    if ( splitterwindow )
        splitterwindow->Update();
}
void TimingView::OnZoomIn(wxCommandEvent& WXUNUSED(event) )
{
    if ( GridStepWidth < 150 )
        GridStepWidth  *= 1.4;
    if ( GridStepWidth > 150 ) GridStepWidth = 150;
    if ( splitterwindow )
        splitterwindow->Update();
}


/// ////////////////////////////////////////////////////////////////////// clipboard, cut copy paste (select)
void TimingView::OnCopy(wxCommandEvent& WXUNUSED(event) )
{
    task->Copy();
}
void TimingView::OnCut(wxCommandEvent& WXUNUSED(event) )
{
    task->Cut();
}
void TimingView::OnPaste(wxCommandEvent& WXUNUSED(event) )
{
    task->Paste();
}
void TimingView::OnDelete(wxCommandEvent& WXUNUSED(event) )
{
    task->Delete();
}
void TimingView::OnSelectAll(wxCommandEvent& WXUNUSED(event) )
{
    task->SelectAll();
}
bool TimingView::CanPaste(void)
{
    return task->CanPaste();
}
bool TimingView::CanDelete(void)
{
    return task->CanDelete();
}
bool TimingView::HasActiveSelection(void)
{
    return task->HasActiveSelection();
}
//bool TimingView::IsTextSelected(void){return false;}
wxInt32 TimingView::GetSelectedSignalNumber()
{
    return task->GetSelectedSignalNumber();
}
wxInt32 TimingView::GetSelectedDiscontinuity()
{
    return task->GetSelectedDiscontinuity();
}




void TimingView::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    task->LabelsMouse(event, pos);
}
void TimingView::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    task->WavesMouse(event, pos);
}
void TimingView::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    task->AxisMouse(event, pos);
}
void TimingView::SetTask(Task *newtask)
{
    if ( !newtask )
    {
        defaultTask->InitTask();
        newtask = defaultTask;
    }
    task = newtask;
}
void TimingView::LabelsKey(const wxKeyEvent &event, bool down)
{
    task->LabelsKey(event, down);
}
void TimingView::WavesKey(const wxKeyEvent &event, bool down)
{
    task->WavesKey(event, down);
}
void TimingView::AxisKey(const wxKeyEvent &event, bool down)
{
    task->AxisKey(event, down);
}




bool TimingView::IsDiscontinuitySelected()
{
    return GetSelectedDiscontinuity() != -1;
}
bool TimingView::IsSelectedSignalClock()
{
    wxInt32 selsig = GetSelectedSignalNumber();

    if ( selsig < 0 )
        return false;

    TimingDocument *doc = (TimingDocument *)GetDocument();
    if ( doc->signals.size() < (unsigned int)selsig)
        return false;

    return doc->signals[selsig].IsClock;
}
bool TimingView::IsSignalSelected(void)
{
    return GetSelectedSignalNumber() != -1;
}



/// ///////////////////// graphics export

void TimingView::OnExportBitmap(wxCommandEvent& event)
{
//    wxFileDialog dlg( wxGetApp().GetMainFrame(), _T("Choose a file for exporting into it"),_T(""),_T(""),_T("PNG files (*.png)|*.png"),wxSAVE | wxOVERWRITE_PROMPT );
//    if ( dlg.ShowModal() != wxID_OK )
//        return;
//
//    wxString filename = dlg.GetPath();
//    if ( filename.empty() )
//        return;
//
//    /// calc size
//    //wxBitmap bm(10,10);
//    wxMemoryDC *memdc = new wxMemoryDC();
//    //memdc->SelectObject(bm);
//
//    wxPoint s = window->GetBitmapSize();
//
//
//    wxBitmap bitmap(s.x, s.y);
//    memdc->SelectObject(bitmap);
//
//    /// draw using the memdc
//    //memdc->SetBackground(*wxWHITE_BRUSH);
//    //window->PaintBackground(*memdc);
//    memdc->SetBrush(*wxWHITE_BRUSH);
//    memdc->SetPen(*wxWHITE_PEN);
//    memdc->DrawRectangle(0, 0, s.x, s.y);
//    memdc->SetPen(*wxBLACK_PEN);
//    window->Draw(*memdc, true);
//
//
//    memdc->SelectObject(wxNullBitmap);
//    bitmap.SaveFile(filename, wxBITMAP_TYPE_PNG); //wxBITMAP_TYPE_JPEG);

}

void TimingView::OnExportSVG(wxCommandEvent& event)
{
//    wxFileDialog dlg( wxGetApp().GetMainFrame(), _T("Choose a file for exporting into it"), _T(""), _T(""), _T("SVG files (*.svg)|*.svg"), wxSAVE | wxOVERWRITE_PROMPT);
//    if ( dlg.ShowModal() != wxID_OK )
//        return;
//    wxString filename = dlg.GetPath();
//    if ( filename.empty() )
//        return;
//
//    wxPoint s = window->GetBitmapSize();
//    wxSVGFileDC *svgdc = new wxSVGFileDC(filename, s.x, s.y);
//    svgdc->SetBrush(*wxWHITE_BRUSH);
//    svgdc->SetPen(*wxBLACK_PEN);
//    //svgdc->DrawRectangle(0, 0, s.x, s.y);
//    window->Draw(*svgdc, true);
//
//    delete svgdc;
}

void TimingView::OnExportPS(wxCommandEvent& event)
{
//    wxFileDialog dlg( wxGetApp().GetMainFrame(), _T("Choose a file for exporting into it"), _T(""), _T(""), _T("PostScrip files (*.ps)|*.ps"), wxSAVE | wxOVERWRITE_PROMPT);
//    if ( dlg.ShowModal() != wxID_OK )
//        return;
//    wxString filename = dlg.GetPath();
//    if ( filename.empty() )
//        return;
//
//    wxPoint s = window->GetBitmapSize();
//    //wxPostScriptDC *psdc = new wxPostScriptDC(filename);
//    wxPrintData g_printData;
//    g_printData.SetFilename(filename);
//    wxPostScriptDC *psdc = new wxPostScriptDC(g_printData);
//
//    psdc->StartDoc(_T("Printing PS"));
//    //psdc->SetUserScale(1.0,1.0);
//    //psdc->SetBackground(*wxTRANSPARENT_BRUSH);
//    psdc->Clear();
//    psdc->SetBackgroundMode(wxTRANSPARENT);
//    psdc->SetBrush(*wxWHITE_BRUSH);
//    psdc->SetPen(*wxBLACK_PEN);
//    //svgdc->DrawRectangle(0, 0, s.x, s.y);
//    window->Draw(*psdc, true);
//
//    delete psdc;
}


