#include "DiagramWaveWindow.h"

#include <wx/dcbuffer.h>

#include "DiagramAxisWindow.h"
#include "DiagramLabelsWindow.h"

#include "TimingDoc.h"
#include "TimingView.h"

IMPLEMENT_DYNAMIC_CLASS(DiagramWaveWindow, wxPanel)

BEGIN_EVENT_TABLE(DiagramWaveWindow, wxPanel)
    EVT_PAINT(            DiagramWaveWindow::OnPaint)
    EVT_ERASE_BACKGROUND( DiagramWaveWindow::OnEraseBackground)
END_EVENT_TABLE()

DiagramWaveWindow::DiagramWaveWindow(TimingView *view, wxWindow* parent, wxScrolledWindow *scrollowner, DiagramAxisWindow *axis, DiagramLabelsWindow *labels, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: wxPanel( parent, id, pos, size, style )
{
    //ctor
    m_view = view;
    m_owner  = scrollowner;
    m_axis   = axis;
    m_labels = labels;

    //(void)new wxTextCtrl( this, wxID_ANY, _T("Text I"), wxPoint(0,100), wxSize(100,25), wxBORDER_NONE );

    //SetBackgroundColour( wxT("WHITE") );

    SetCursor(*wxCROSS_CURSOR);

}
DiagramWaveWindow::DiagramWaveWindow(){}
DiagramWaveWindow::~DiagramWaveWindow(){}

void DiagramWaveWindow::Update()
{
    Refresh();
}

wxColour DiagramWaveWindow::GetBackgroundColour() const
{
    //return wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE );
    return *wxWHITE;
}
wxColour DiagramWaveWindow::GetLineColour() const
{
    return *wxBLACK;
}
wxColour DiagramWaveWindow::GetTextColour() const
{
    return *wxBLACK;
}
wxColour DiagramWaveWindow::GetShadowColour() const
{
    return wxColour(0xf0,0xf0,0xf0);
}

wxColour DiagramWaveWindow::GetUndefinedSignalColour() const
{
    return *wxLIGHT_GREY;
}

void DiagramWaveWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxBufferedPaintDC dc( this );
    PaintBackground(dc);

    m_owner->PrepareDC( dc );

    // OK, let's assume we are a grid control and we have two
    // grid cells. Here in OnPaint we want to know which cell
    // to redraw so that we prevent redrawing cells that don't
    // need to get redrawn. We have one cell at (0,0) and one
    // more at (200,0), both having a size of (100,25).

    // We can query how much the window has been scrolled
    // by calling CalcUnscrolledPosition()

    int scroll_x = 0;
    int scroll_y = 0;
    m_owner->CalcUnscrolledPosition( scroll_x, scroll_y, &scroll_x, &scroll_y );

    // We also need to know the size of the window to see which
    // cells are completely hidden and not get redrawn

    Draw(dc);
}
void DiagramWaveWindow::OnEraseBackground(wxEraseEvent &WXUNUSED(event) ){}
void DiagramWaveWindow::ScrollWindow( int dx, int dy, const wxRect *rect )
{
    wxPanel::ScrollWindow( dx, dy, rect );
    m_axis->ScrollWindow( dx, 0, rect );
    m_labels->ScrollWindow( 0, dy );
}
void DiagramWaveWindow::PaintBackground(wxDC &dc)
{
    dc.SetBrush(wxBrush(GetBackgroundColour()));
    dc.SetPen(wxPen(GetBackgroundColour(), 1));
    wxRect windowRect(wxPoint(0,0), GetClientSize());
    dc.DrawRectangle(windowRect);
    dc.SetBrush(wxNullBrush);
    dc.SetPen(wxNullPen);
}

void DiagramWaveWindow::Draw(wxDC &dc)
{
    if (!m_view) return;
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    if ( !doc ) return;

    wxCoord start = 10;
    wxCoord stop = start + (m_view->GridStepWidth)*(m_view->VisibleTicks.size() - 1);
    SetVirtualSize(2*start+stop, m_view->heightOffsets[m_view->heightOffsets.size()-1]);


    dc.SetFont(m_view->GetFont());

    wxPoint offset;



    /// drawing background of clock signal
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(GetShadowColour());
    std::vector<wxUint32> positions;
    std::vector<wxString> texts;
    for ( wxUint32 n = 0 ; n < doc->signals.size() ; ++n )
    {
        offset.x = start;
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
            for (wxUint32 k = 0; k < m_view->VisibleTicks.size()-1 ; )
            {
                bool hascompressor = false;
                for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
                    if ( doc->compressors[indx].pos == m_view->VisibleTicks[k] && doc->compressors[indx].enabled )
                    {
                        if ( n < 2*sig.ticks )// drawing a bar in the first half of the tick
                            dc.DrawRectangle( offset.x, 0,
                                (m_view->GridStepWidth)/2, m_view->heightOffsets[m_view->heightOffsets.size()-1]
                            );

                        wxInt32 len = (doc->compressors[indx].length) % (4*sig.ticks);
                        n += len - 2;
                        if ( n < 0 ) n += (4*sig.ticks);
                        n %= (4*sig.ticks);
                        if ( n < 2*sig.ticks )// drawing a bar over the second half of the tick
                            dc.DrawRectangle(
                                offset.x + (m_view->GridStepWidth)/2, 0,
                                (m_view->GridStepWidth)/2+1  , m_view->heightOffsets[m_view->heightOffsets.size()-1]
                            );
                        ++n;
                        hascompressor = true;
                    }
                if ( !hascompressor );
                {
                    if ( n < 2*sig.ticks ){// drawing a bar over the whole tick
                        dc.DrawRectangle( offset.x, 0,
                            (m_view->GridStepWidth)+1, m_view->heightOffsets[m_view->heightOffsets.size()-1]
                        );
                    }
                    if ( sig.ShowPeriodCount &&
                        (n == sig.ticks || n == 3*sig.ticks))
                    {
                        wxUint32 per = m_view->VisibleTicks[k]/2/sig.ticks+1;
                        positions.push_back(offset.x);
                        texts.push_back( wxString::Format(_T("%d"),per) );
                        //dc.DrawText( , , heightOffsets[0]);
                    }
                    ++n;
                }
                n %= (4*sig.ticks);
                offset.x += (m_view->GridStepWidth);
                ++k;
            }
        }
    }
    for ( wxUint32 n = 0; n < positions.size() ; ++n )
        dc.DrawText( texts[n] , positions[n], 0);
    dc.SetPen(wxNullPen);
    dc.SetBrush(wxNullBrush);




    /// drawing the signals
    wxPen boldPen(GetLineColour(), 2);
    dc.SetPen(boldPen);
    /*if (  !exporting && ( WindowState == SelectingText ||
                  WindowState == TextFieldSelected) )
    {
        dc.SetBackgroundMode(wxSOLID);
        dc.SetTextBackground(col);
    }*/
    for ( wxUint32 n = 0 ; n < doc->signals.size() ; ++n )
    {
        wxPoint offset;
        offset.x = start;
        offset.y = m_view->heightOffsets[n]+doc->MinimumSignalDistance/2+ doc->signals[n].prespace;

        wxCoord transwidth = (m_view->GridStepWidth)/(100.0/doc->TransitWidth);
        //{
        wxCoord wo = transwidth;
        wxCoord wt = (m_view->GridStepWidth)/2;
        wxCoord ho[6] = {0,0,doc->SignalHeight/2,doc->SignalHeight/2,doc->SignalHeight,doc->SignalHeight};
        wxCoord ht[6];
        ht[4] = ho[5]*50/doc->TransitWidth;
        ht[1] = ho[5]-ht[4];
        ht[2] = ht[4]/2;
        ht[3] = ho[5]-ht[2];
        ht[5] = ht[2]+ho[5]/2;
        ht[0] = ho[5]-ht[5];//}

        Signal sig = doc->signals[n];
//        if ( WindowState == EditSignal && editingNumber>=0 && (wxUint32)editingNumber == n && editingValB != -1 ) sig = editingSignal;
//        else sig = doc->signals[n];
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
            for (wxUint32 k = 0; k < m_view->VisibleTicks.size()-1 ; )
            {
                bool hascompressor = false;
                for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
                    if ( doc->compressors[indx].pos == m_view->VisibleTicks[k] && doc->compressors[indx].enabled )
                    {
                        if ( n < sig.ticks )// drawing a one
                            dc.DrawLine(
                                offset.x                  , offset.y,
                                offset.x + (m_view->GridStepWidth)/2, offset.y
                            );
                        else// drawing a zero
                            dc.DrawLine(
                                offset.x                  , offset.y + doc->SignalHeight,
                                offset.x + (m_view->GridStepWidth)/2, offset.y + doc->SignalHeight
                            );

                        wxInt32 len = (doc->compressors[indx].length) % (2*sig.ticks);
                        n += len -1;
                        if ( n < 0 ) n += (2*sig.ticks);
                        n %= (2*sig.ticks);
                        if ( n < sig.ticks )// drawing a one
                            dc.DrawLine(
                                offset.x + (m_view->GridStepWidth)/2, offset.y,
                                offset.x + (m_view->GridStepWidth)  , offset.y
                            );
                        else// drawing a zero
                            dc.DrawLine(
                                offset.x + (m_view->GridStepWidth)/2, offset.y + doc->SignalHeight,
                                offset.x + (m_view->GridStepWidth)  , offset.y + doc->SignalHeight
                            );
                        ++n;
                        hascompressor = true;
                    }
                if ( !hascompressor )
                {
                    if ( n < sig.ticks )// drawing a one
                        dc.DrawLine(
                            offset.x                , offset.y,
                            offset.x + (m_view->GridStepWidth), offset.y
                        );
                    else// drawing a zero
                        dc.DrawLine(
                            offset.x                , offset.y + doc->SignalHeight,
                            offset.x + (m_view->GridStepWidth), offset.y + doc->SignalHeight
                        );
                    ++n;
                }

                if( n == sig.ticks || n == 2*sig.ticks ) // draw the edge
                    dc.DrawLine(
                        offset.x + (m_view->GridStepWidth), offset.y + doc->SignalHeight,
                        offset.x + (m_view->GridStepWidth), offset.y
                    );
                n %= (2*sig.ticks);
                offset.x += (m_view->GridStepWidth);
                ++k;
            }
        }
        else if ( !sig.IsBus )
        {
            vals oldval = sig.values[0];// first tick can not start with a transition

            for (wxUint32 k = 0; k < m_view->VisibleTicks.size()-1 ; ++k)
            {
                wxInt32 tick = m_view->VisibleTicks[k];
                wxInt32 len = 1;
                vals val = sig.values[tick];
                for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
                if ( doc->compressors[indx].pos == m_view->VisibleTicks[k] && doc->compressors[indx].enabled )
                    len = doc->compressors[indx].length;


                /// drawing the transitions:
                wxCoord w1 = wo;
                wxCoord h[6] = {ho[0], ho[1], ho[2], ho[3], ho[4], ho[5]};
                wxCoord h0 = ho[5];
                if ( len > 1 && doc->TransitWidth >= 50 ) // time compressor and transition "breiter" than the half tick
                {
                    w1 = wt;
                    for ( int i = 0 ; i < 6 ; i++) h[i] = ht[i];
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
                                {
                                    dc.SetBrush(wxBrush(GetUndefinedSignalColour()));
                                    dc.SetPen(wxPen(GetUndefinedSignalColour(),1));
                                    wxPoint rec[4];
                                    rec[0] = wxPoint(offset.x   , offset.y);
                                    rec[1] = wxPoint(offset.x+w1, offset.y + h[4]);
                                    rec[2] = wxPoint(offset.x+w1, offset.y + h0);
                                    rec[3] = wxPoint(offset.x   , offset.y + h0);
                                    dc.DrawPolygon(4, rec);
                                    dc.SetBrush(wxNullBrush);
                                    dc.SetPen(boldPen);
                                }
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
                            case u:{
                                    dc.SetBrush(wxBrush(GetUndefinedSignalColour()));
                                    dc.SetPen(wxPen(GetUndefinedSignalColour(),1));
                                    wxPoint rec[4];
                                    rec[0] = wxPoint(offset.x   , offset.y+h0);
                                    rec[1] = wxPoint(offset.x+w1, offset.y+h[1]);
                                    rec[2] = wxPoint(offset.x+w1, offset.y);
                                    rec[3] = wxPoint(offset.x   , offset.y);
                                    dc.DrawPolygon(4, rec);
                                    dc.SetBrush(wxNullBrush);
                                    dc.SetPen(boldPen);
                                }
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
                                {   dc.SetBrush(wxBrush(GetUndefinedSignalColour()));
                                    dc.SetPen(wxPen(GetUndefinedSignalColour(),1));
                                    wxPoint rec[4];
                                    rec[0] = wxPoint(offset.x   , offset.y);
                                    rec[1] = wxPoint(offset.x+w1, offset.y + h[2]);
                                    rec[2] = wxPoint(offset.x+w1, offset.y + h[3]);
                                    rec[3] = wxPoint(offset.x   , offset.y + h0);
                                    dc.DrawPolygon(4, rec);
                                    dc.SetBrush(wxNullBrush);
                                    dc.SetPen(boldPen);
                                }
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
                                {
                                    dc.SetBrush(wxBrush(GetUndefinedSignalColour()));
                                    dc.SetPen(wxPen(GetUndefinedSignalColour(),1));
                                    wxPoint rec[3];
                                    rec[0] = wxPoint(offset.x   , offset.y+h0);
                                    rec[1] = wxPoint(offset.x+w1, offset.y+h0);
                                    rec[2] = wxPoint(offset.x+w1, offset.y+h[1]);
                                    dc.DrawPolygon(3, rec);
                                    dc.SetBrush(wxNullBrush);
                                    dc.SetPen(boldPen);
                                }
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h0);
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h[1]);
                                break;
                            case one:
                                {
                                    dc.SetBrush(wxBrush(GetUndefinedSignalColour()));
                                    dc.SetPen(wxPen(GetUndefinedSignalColour(),1));
                                    wxPoint rec[3];
                                    rec[0] = wxPoint(offset.x   , offset.y);
                                    rec[1] = wxPoint(offset.x+w1, offset.y);
                                    rec[2] = wxPoint(offset.x+w1, offset.y+h[4]);
                                    dc.DrawPolygon(3, rec);
                                    dc.SetBrush(wxNullBrush);
                                    dc.SetPen(boldPen);
                                }
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y+h[4]);
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y);
                                break;
                            case hz:
                                {
                                    dc.SetBrush(wxBrush(GetUndefinedSignalColour()));
                                    dc.SetPen(wxPen(GetUndefinedSignalColour(),1));
                                    wxPoint rec[3];
                                    rec[0] = wxPoint(offset.x   , offset.y+h0/2);
                                    rec[1] = wxPoint(offset.x+w1, offset.y+h[5]);
                                    rec[2] = wxPoint(offset.x+w1, offset.y+h[0]);
                                    dc.DrawPolygon(3, rec);
                                    dc.SetBrush(wxNullBrush);
                                    dc.SetPen(boldPen);
                                }
                                dc.DrawLine(offset.x   , offset.y+h0/2,
                                            offset.x+w1, offset.y+h[5]);
                                dc.DrawLine(offset.x   , offset.y+h0/2,
                                            offset.x+w1, offset.y+h[0]);
                                break;
                            default:
                            case u:
                                {
                                    dc.SetBrush(wxBrush(GetUndefinedSignalColour()));
                                    dc.SetPen(wxPen(GetUndefinedSignalColour(),1));
                                    wxPoint rec[4];
                                    rec[0] = wxPoint(offset.x   , offset.y);
                                    rec[1] = wxPoint(offset.x+w1, offset.y);
                                    rec[2] = wxPoint(offset.x+w1, offset.y + h0);
                                    rec[3] = wxPoint(offset.x   , offset.y + h0);
                                    dc.DrawPolygon(4, rec);
                                    dc.SetBrush(wxNullBrush);
                                    dc.SetPen(boldPen);
                                }
                                dc.DrawLine(offset.x   , offset.y+h0,
                                            offset.x+w1, offset.y+h0);
                                dc.DrawLine(offset.x   , offset.y,
                                            offset.x+w1, offset.y);
                                break;
                            case dc1:
                            case dc2:
                                {
                                    dc.SetBrush(wxBrush(GetUndefinedSignalColour()));
                                    dc.SetPen(wxPen(GetUndefinedSignalColour(),1));
                                    wxPoint rec[3];
                                    rec[0] = wxPoint(offset.x+w1  , offset.y+h[1]);
                                    rec[1] = wxPoint(offset.x+wo/2, offset.y+h0/2);
                                    rec[2] = wxPoint(offset.x+w1  , offset.y+h[4]);
                                    dc.DrawPolygon(3, rec);
                                    dc.SetBrush(wxNullBrush);
                                    dc.SetPen(boldPen);
                                }
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
                                {
                                    dc.SetBrush(wxBrush(GetUndefinedSignalColour()));
                                    dc.SetPen(wxPen(GetUndefinedSignalColour(),1));
                                    wxPoint rec[3];
                                    rec[0] = wxPoint(offset.x     , offset.y);
                                    rec[1] = wxPoint(offset.x+wo/2, offset.y+h0/2);
                                    rec[2] = wxPoint(offset.x     , offset.y+h0);
                                    dc.DrawPolygon(3, rec);
                                    dc.SetBrush(wxNullBrush);
                                    dc.SetPen(boldPen);
                                }
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
                wxCoord w2 = (m_view->GridStepWidth);
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
                        {
                            dc.SetBrush(wxBrush(GetUndefinedSignalColour()));
                            dc.SetPen(wxPen(GetUndefinedSignalColour(),1));
                            wxPoint rec[4];
                            rec[0] = wxPoint(offset.x+w1, offset.y);
                            rec[1] = wxPoint(offset.x+w2, offset.y);
                            rec[2] = wxPoint(offset.x+w2, offset.y+h0);
                            rec[3] = wxPoint(offset.x+w1, offset.y+h0);
                            dc.DrawPolygon(4, rec);
                            dc.SetBrush(wxNullBrush);
                            dc.SetPen(boldPen);
                        }
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
                    w1 = (m_view->GridStepWidth)/2;
                    w2 = (m_view->GridStepWidth);
                    val = sig.values[m_view->VisibleTicks[k+1]-1];
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
                            {
                                dc.SetBrush(wxBrush(GetUndefinedSignalColour()));
                                dc.SetPen(wxPen(GetUndefinedSignalColour(),1));
                                wxPoint rec[4];
                                rec[0] = wxPoint(offset.x+w1, offset.y);
                                rec[1] = wxPoint(offset.x+w2, offset.y);
                                rec[2] = wxPoint(offset.x+w2, offset.y+h0);
                                rec[3] = wxPoint(offset.x+w1, offset.y+h0);
                                dc.DrawPolygon(4, rec);
                                dc.SetBrush(wxNullBrush);
                                dc.SetPen(boldPen);
                            }
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
                offset.x+= (m_view->GridStepWidth);
            }
        }
        else // is bus
        {
            vals oldval = sig.values[0];// first tick can not start with a transition

            for (wxUint32 k = 0; k < m_view->VisibleTicks.size()-1 ; ++k)
            {
                wxInt32 tick = m_view->VisibleTicks[k];
                wxInt32 len = 1;
                vals val = sig.values[tick];
                for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
                if ( doc->compressors[indx].pos == m_view->VisibleTicks[k] && doc->compressors[indx].enabled )
                    len = doc->compressors[indx].length;

                /// drawing the transitions:
                wxCoord w1 = wo;
                wxCoord h[6] = {ho[0], ho[1], ho[2], ho[3], ho[4], ho[5]};
                wxCoord h0 = ho[5];
                if ( len > 1 && doc->TransitWidth >= 50 ) // time compressor and transition "breiter" than the half tick
                {
                    w1 = wt;
                    for ( int i = 0 ; i < 6 ; i++) h[i] = ht[i];
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
                            case u:
                                {
                                    dc.SetBrush(wxBrush(GetUndefinedSignalColour()));
                                    dc.SetPen(wxPen(GetUndefinedSignalColour(),1));
                                    wxPoint rec[3];
                                    rec[0] = wxPoint(offset.x     , offset.y);
                                    rec[1] = wxPoint(offset.x+wo/2, offset.y + h0/2);
                                    rec[2] = wxPoint(offset.x     , offset.y + h0);
                                    dc.DrawPolygon(3, rec);
                                    dc.SetBrush(wxNullBrush);
                                    dc.SetPen(boldPen);
                                }
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
                            case u:
                                {
                                    dc.SetBrush(wxBrush(GetUndefinedSignalColour()));
                                    dc.SetPen(wxPen(GetUndefinedSignalColour(),1));
                                    wxPoint rec[4];
                                    rec[0] = wxPoint(offset.x   , offset.y);
                                    rec[1] = wxPoint(offset.x+w1, offset.y + h[2]);
                                    rec[2] = wxPoint(offset.x+w1, offset.y + h[3]);
                                    rec[3] = wxPoint(offset.x   , offset.y + h0);
                                    dc.DrawPolygon(4, rec);
                                    dc.SetBrush(wxNullBrush);
                                    dc.SetPen(boldPen);
                                }
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
                            case hz:
                                {
                                    dc.SetBrush(wxBrush(GetUndefinedSignalColour()));
                                    dc.SetPen(wxPen(GetUndefinedSignalColour(),1));
                                    wxPoint rec[3];
                                    rec[0] = wxPoint(offset.x+w1, offset.y +h[0]);
                                    rec[1] = wxPoint(offset.x+w1, offset.y +h[5]);
                                    rec[2] = wxPoint(offset.x   , offset.y +h0/2);
                                    dc.DrawPolygon(3,rec);
                                    dc.SetBrush(wxNullBrush);
                                    dc.SetPen(boldPen);
                                }
                                dc.DrawLine(offset.x   , offset.y +h0/2,
                                            offset.x+w1, offset.y +h[0]);
                                dc.DrawLine(offset.x   , offset.y +h0/2,
                                            offset.x+w1, offset.y +h[5]);
                                break;
                            default:
                            case u:
                                {
                                    dc.SetBrush(wxBrush(GetUndefinedSignalColour()));
                                    dc.SetPen(wxPen(GetUndefinedSignalColour(),1));
                                    wxPoint rec[4];
                                    rec[0] = wxPoint(offset.x   , offset.y);
                                    rec[1] = wxPoint(offset.x+w1, offset.y);
                                    rec[2] = wxPoint(offset.x+w1, offset.y + h0);
                                    rec[3] = wxPoint(offset.x   , offset.y + h0);
                                    dc.DrawPolygon(4, rec);
                                    dc.SetBrush(wxNullBrush);
                                    dc.SetPen(boldPen);
                                }
                                dc.DrawLine(offset.x, offset.y, offset.x+w1, offset.y);
                                dc.DrawLine(offset.x, offset.y+h0, offset.x+w1, offset.y+h0);
                                break;
                            case zero:
                            case one:
                                {
                                    dc.SetBrush(wxBrush(GetUndefinedSignalColour()));
                                    dc.SetPen(wxPen(GetUndefinedSignalColour(),1));
                                    wxPoint rec[3];
                                    rec[0] = wxPoint(offset.x+w1  , offset.y+h[1]);
                                    rec[1] = wxPoint(offset.x+wo/2, offset.y+h0/2);
                                    rec[2] = wxPoint(offset.x+w1  , offset.y+h[4]);
                                    dc.DrawPolygon(3, rec);
                                    dc.SetBrush(wxNullBrush);
                                    dc.SetPen(boldPen);
                                }
                                dc.DrawLine(offset.x, offset.y   , offset.x+w1, offset.y+h[4]);
                                dc.DrawLine(offset.x, offset.y+h0, offset.x+w1, offset.y+h[1]);
                                break;
                        }
                        break;
                }

                /// drawing the state
                wxCoord w2 = (m_view->GridStepWidth);
                if ( len > 1 )
                    w2 /= 2;
                if ( len == 1 || doc->TransitWidth < 50 ) switch ( val )
                {
                    case hz:
                        dc.DrawLine(offset.x+w1, offset.y+h0/2,
                                    offset.x+w2, offset.y+h0/2);
                        break;
                    default:
                    case u:
                        {
                            dc.SetBrush(wxBrush(GetUndefinedSignalColour()));
                            dc.SetPen(wxPen(GetUndefinedSignalColour(),1));
                            wxPoint rec[4];
                            rec[0] = wxPoint(offset.x+w1, offset.y);
                            rec[1] = wxPoint(offset.x+w2, offset.y);
                            rec[2] = wxPoint(offset.x+w2, offset.y+h0);
                            rec[3] = wxPoint(offset.x+w1, offset.y+h0);
                            dc.DrawPolygon(4, rec);
                            dc.SetBrush(wxNullBrush);
                            dc.SetPen(boldPen);
                        }
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
                    w1 = (m_view->GridStepWidth)/2;
                    w2 = (m_view->GridStepWidth);
                    val = sig.values[m_view->VisibleTicks[k+1]-1];
                    switch ( val )
                    {
                        case hz:
                            dc.DrawLine(offset.x+w1, offset.y+h0/2,
                                        offset.x+w2, offset.y+h0/2);
                            break;
                        default:
                        case u:
                            {
                                dc.SetBrush(wxBrush(GetUndefinedSignalColour()));
                                dc.SetPen(wxPen(GetUndefinedSignalColour(),1));
                                wxPoint rec[4];
                                rec[0] = wxPoint(offset.x+w1, offset.y);
                                rec[1] = wxPoint(offset.x+w2, offset.y);
                                rec[2] = wxPoint(offset.x+w2, offset.y+h0);
                                rec[3] = wxPoint(offset.x+w1, offset.y+h0);
                                dc.DrawPolygon(4, rec);
                                dc.SetBrush(wxNullBrush);
                                dc.SetPen(boldPen);
                            }
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
                offset.x += (m_view->GridStepWidth);
            }

            //drawing the texts
            offset.x = start;
            oldval = sig.values[0];
            for ( wxUint32 k = 0; k < m_view->VisibleTicks.size()-1 ; ++k )
            {
                wxInt32 tick = m_view->VisibleTicks[k];
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
                            dc.DrawText(sig.TextValues[ti], offset);//DrawEditableText(dc, sig.TextValues[ti], offset, false);
                    }
                    hascompressor = true;
                    break;
                }
                if ( !hascompressor )
                {
                    if ( (tick == 0 || oldval != val) && (val == one || val == zero ))
                    {
                        wxPoint textoff(
                            wo + offset.x + k*(m_view->GridStepWidth),
                            offset.y + doc->SignalHeight/2 -
                            dc.GetCharHeight()/2
                        );
                        //bool viz = true;
                        //if ( textoff.y + dc.GetCharHeight()/2 < 0) viz = false;
                        //if ( textoff.x                        < start - 5 ) viz = false;
                        dc.DrawText(sig.TextValues[tick], textoff);//DrawEditableText(dc, sig.TextValues[tick], textoff, viz);
                    }
                }
                oldval = val;
            }
        }
        if ( /* !exporting && */ n < doc->signals.size() )
        {
            dc.SetPen(wxPen(GetShadowColour(), 1, wxDOT));
            dc.DrawLine(start, m_view->heightOffsets[n],
                        stop, m_view->heightOffsets[n]);
            if ( n + 1 == doc->signals.size() )
                dc.DrawLine(start, m_view->heightOffsets[n+1],
                            stop, m_view->heightOffsets[n+1]);
            dc.SetPen(boldPen);
        }
    }
    dc.SetPen(wxNullPen);
    //dc.SetTextBackground(bgcol);
    //dc.SetBackgroundMode(wxTRANSPARENT);
    //dc.SetPen(defPen);



    dc.SetPen(wxPen(GetLineColour(),1));
    /// drawing discontinuities
    for ( wxUint32 k = 0 ; k < m_view->VisibleTicks.size()-1 ; ++k )
    {
        wxInt32 tick = m_view->VisibleTicks[k];
        wxInt32 len = 1;
        for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
	        if ( doc->compressors[indx].pos == tick && doc->compressors[indx].enabled )
	            len = doc->compressors[indx].length;

        if ( len > 1 )
        {
            /// splines over signal
            for ( wxUint32 n = 0 ; n < doc->signals.size() ; ++n)
            {
                offset.x = start  + (0.5 + k)*(m_view->GridStepWidth) - 2;
                offset.y = m_view->heightOffsets[n]+doc->MinimumSignalDistance/2 + doc->signals[n].prespace;
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
                    bool drawbg =  n != 0 && n != 3;
                    if ( drawbg )
                        dc.SetPen(wxPen(GetBackgroundColour(),1));
                    dc.DrawSpline(4, points);
                    if ( drawbg )
                        dc.SetPen(wxNullPen);

                }
            }
        }
    }
    dc.SetPen(wxNullPen);

    DrawVerticalLines(dc);
    DrawHorizontalArrows(dc);
}

void DiagramWaveWindow::DrawVerticalLines(wxDC &dc)
{
    if (!m_view) return;
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    if ( !doc ) return;

    wxCoord start = 10;

    dc.SetPen(wxPen(GetLineColour(),1));
    for ( wxUint32 k = 0 ; k < doc->vertlines.size() ; ++k)
    {
        bool found = false;
        wxInt32 vpos;
        for ( wxUint32 i = 0 ; i < m_view->VisibleTicks.size() ; ++i)
        {
            if (m_view->VisibleTicks[i] == doc->vertlines[k].vpos)
            {
                found = true;
                vpos = i;
                break;
            }
        }
        if ( !found )
            continue;

        wxPoint offset(start + vpos * m_view->GridStepWidth,
                       m_view->heightOffsets[doc->vertlines[k].StartPos]);

        if ( doc->en50 && doc->vertlines[k].vposoffset == 1 )
            offset.x += m_view->GridStepWidth/(100.0/(doc->TransitWidth/2.0));
        if ( doc->en90 && doc->vertlines[k].vposoffset == 2 )
            offset.x += m_view->GridStepWidth/(100.0/(doc->TransitWidth));

        dc.DrawLine(
            offset.x, offset.y,
            offset.x, m_view->heightOffsets[doc->vertlines[k].EndPos + 1]
        );
    }
    dc.SetPen(wxNullPen);


}
void DiagramWaveWindow::DrawHorizontalArrows(wxDC &dc)
{
    if (!m_view) return;
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    if ( !doc ) return;

    wxCoord start = 10;

    dc.SetPen(wxPen(GetLineColour(),1));
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

        for ( wxUint32 i = 0 ; i < m_view->VisibleTicks.size() ; ++i)
        {
            if ( !fromVlineVisible && m_view->VisibleTicks[i] == fromvpos )
            {
                fromvpos = i;
                fromVlineVisible = true;
                //break;
            }
            if ( !toVlineVisible && m_view->VisibleTicks[i] == tovpos )
            {
                tovpos = i;
                toVlineVisible = true;
            }
        }
        if ( fromVlineVisible && toVlineVisible)
        {
            /// calc offset based on vline to start from
            wxPoint offset(start + fromvpos * m_view->GridStepWidth,
                    ha.pos + m_view->heightOffsets[ha.signalnmbr]);
            if ( doc->en50 && fromvposoffset == 1 )
                offset.x += m_view->GridStepWidth/(100.0/(doc->TransitWidth/2.0));
            else if ( doc->en90 && fromvposoffset == 2 )
                offset.x += m_view->GridStepWidth/(100.0/(doc->TransitWidth));

            /// calc offset based on vline where harrow will end
            wxPoint tooffset(start + tovpos * m_view->GridStepWidth,
                offset.y);
            if ( doc->en50 && tovposoffset == 1 )
                tooffset.x += m_view->GridStepWidth/(100.0/(doc->TransitWidth/2.0));
            else if ( doc->en90 && tovposoffset == 2 )
                tooffset.x += m_view->GridStepWidth/(100.0/(doc->TransitWidth));


            if ( offset.x > tooffset.x ) // swap
            {
                wxCoord t = offset.x;
                offset.x = tooffset.x;
                tooffset.x = t;
            }

            dc.DrawLine(  offset.x,   offset.y, tooffset.x,   tooffset.y);
            /// arrowhead:
            dc.DrawLine(  offset.x,   offset.y,   offset.x+5,   offset.y-3);
            dc.DrawLine(  offset.x,   offset.y,   offset.x+5,   offset.y+3);
            dc.DrawLine(tooffset.x, tooffset.y, tooffset.x-5, tooffset.y-3);
            dc.DrawLine(tooffset.x, tooffset.y, tooffset.x-5, tooffset.y+3);

            ///the text:
            wxPoint textoff;

            textoff.x  = (offset.x + tooffset.x)/2;
            if ( ha.textoffset.x > m_view->GridStepWidth )
                textoff.x += m_view->GridStepWidth;
            else
                textoff.x += ha.textoffset.x;
            textoff.x += ha.textgridoffset*m_view->GridStepWidth;
            textoff.y  = offset.y + ha.textoffset.y - dc.GetCharHeight();

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
                str = wxString::Format(m_view->GetFloatFormatStr(), t);
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
            dc.DrawText( text, textoff );
        }
    }
    dc.SetPen(wxNullPen);
}
