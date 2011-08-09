#include "DiagramWavesWindow.h"

#include <wx/dcbuffer.h>

#include "DiagramAxisWindow.h"
#include "DiagramLabelsWindow.h"

#include "TimingDoc.h"
#include "TimingView.h"
#include "GraphSignal.h"
#include "HorizontalArrowText.h"

#include "HoverDrawlet.h"

IMPLEMENT_DYNAMIC_CLASS(DiagramWavesWindow, wxPanel)

BEGIN_EVENT_TABLE(DiagramWavesWindow, wxPanel)
    EVT_PAINT(            DiagramWavesWindow::OnPaint)
    EVT_ERASE_BACKGROUND( DiagramWavesWindow::OnEraseBackground)

    EVT_MOUSE_EVENTS(     DiagramWavesWindow::OnMouse)
//    EVT_MOTION(           DiagramWavesWindow::OnMouse)
//    EVT_LEFT_DOWN(        DiagramWavesWindow::OnMouse)
//    EVT_LEFT_UP(          DiagramWavesWindow::OnMouse)
//    EVT_RIGHT_DOWN(       DiagramWavesWindow::OnMouse)
//    EVT_RIGHT_UP(         DiagramWavesWindow::OnMouse)
//    EVT_RIGHT_DCLICK(     DiagramWavesWindow::OnMouse)
//
//    EVT_ENTER_WINDOW(     DiagramWavesWindow::OnMouseEnter)
//    EVT_LEAVE_WINDOW(     DiagramWavesWindow::OnMouseLeave)

    //EVT_SIZE(           DiagramWavesWindow::OnSize)
  EVT_KEY_DOWN(           DiagramWavesWindow::OnKeyDown)
  EVT_KEY_UP(             DiagramWavesWindow::OnKeyUp)
END_EVENT_TABLE()

DiagramWavesWindow::DiagramWavesWindow(TimingView *view, wxWindow* parent, wxScrolledWindow *scrollowner, DiagramAxisWindow *axis, DiagramLabelsWindow *labels, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: wxPanel( parent, id, pos, size, style ),
m_owner(scrollowner),
m_axis(axis),
m_labels(labels),
m_view(view),
m_drawlet(NULL)
{
//    textctrls.push_back(new wxTextCtrl( this, wxID_ANY, _T("Text I"), wxPoint(0,100), wxSize(80,25), wxBORDER_NONE ));
//    textctrls.push_back(new wxTextCtrl( this, wxID_ANY, _T("Text II"), wxPoint(0,150), wxSize(80,25), wxBORDER_NONE ));
//    textctrls.push_back(new wxTextCtrl( this, wxID_ANY, _T("Text III"), wxPoint(0,200), wxSize(80,25), wxBORDER_NONE ));
    m_horizontalArrowTextCtrl = new HorizontalArrowText(this,m_view);
    m_horizontalArrowTextCtrl->Hide();
}
DiagramWavesWindow::DiagramWavesWindow(){}
DiagramWavesWindow::~DiagramWavesWindow(){}

void DiagramWavesWindow::DoUpdate()
{
    wxCoord leftSpace = m_view->GetWavesLeftSpace();
    wxCoord stop = leftSpace + (m_view->GridStepWidth)*(m_view->VisibleTicks.size() - 1);

    SetVirtualSize(2*leftSpace+stop, m_view->heightOffsets[m_view->heightOffsets.size()-1]);
    m_owner->AdjustScrollbars();

    RemoveDrawlet();

    Refresh();
}

void DiagramWavesWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
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
void DiagramWavesWindow::OnEraseBackground(wxEraseEvent &WXUNUSED(event) ){}
void DiagramWavesWindow::ScrollWindow( int dx, int dy, const wxRect *rect )
{
    wxPanel::ScrollWindow( dx, dy, rect );
    m_axis->ScrollWindow( dx, 0, rect );
    m_labels->ScrollWindow( 0, dy, rect );
}

void DiagramWavesWindow::PaintBackground(wxDC &dc)
{
    dc.SetBrush(wxBrush(m_view->GetWavesBackgroundColour()));
    dc.SetPen(wxPen(m_view->GetWavesBackgroundColour(), 1));
    wxRect windowRect(wxPoint(0,0), GetClientSize());
    dc.DrawRectangle(windowRect);
    dc.SetBrush(wxNullBrush);
    dc.SetPen(wxNullPen);
}
void DiagramWavesWindow::Draw(wxDC &dc)
{
    DrawSignals(dc);

    DrawSignalSeparators(dc);

    DrawDiscontinuities(dc);

    DrawVerticalLines(dc);
    DrawHorizontalArrows(dc);

    if ( m_drawlet)
        m_drawlet->Draw(dc);
}
void DiagramWavesWindow::DrawSignalSeparators(wxDC &dc)
{
    if (!m_view) return;

    wxCoord stop = m_view->GetWavesLeftSpace() + (m_view->GridStepWidth)*(m_view->VisibleTicks.size() - 1);

    dc.SetPen(wxPen(m_view->GetWaveSeparatorColour(), 1, wxDOT));
    for ( wxUint32 n = 0 ; n < m_view->heightOffsets.size() ; ++n )
        dc.DrawLine(m_view->GetWavesLeftSpace(), m_view->heightOffsets[n],
                    stop, m_view->heightOffsets[n]);

    dc.SetPen(wxNullPen);
}
void DiagramWavesWindow::DrawDiscontinuities(wxDC &dc)
{
    if (!m_view) return;
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    if ( !doc ) return;

    dc.SetPen(wxPen(m_view->GetLineColour(),1));

    wxPoint offset;

    /// drawing discontinuities
    for ( wxUint32 k = 0 ; k < m_view->VisibleTicks.size()-1 ; ++k )
    {
        wxInt32 tick = m_view->VisibleTicks[k];
        wxInt32 len = 1;
        if ( doc->compressors.find(tick) != doc->compressors.end() && doc->compressors[tick].enabled )
            len = doc->compressors[tick].length;

        if ( len > 1 )
        {
            /// splines over signal
            for ( wxUint32 n = 0 ; n < doc->signals.size() ; ++n)
            {
                offset.x = m_view->GetWavesLeftSpace()  + (0.5 + k)*(m_view->GridStepWidth) - 2;
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
                    if ( n != 0 && n != 3 )
                        dc.SetPen(wxPen(m_view->GetWavesBackgroundColour(),1));
                    else
                        dc.SetPen(wxPen(*wxBLACK, 1));
                    dc.DrawSpline(4, points);

                }
            }
        }
    }
    dc.SetPen(wxNullPen);
}
void DiagramWavesWindow::DrawVerticalLines(wxDC &dc)
{
    if (!m_view) return;

    dc.SetPen(wxPen(m_view->GetLineColour(),1));

    GraphVerticalLines vls = m_view->GetGraphVerticalLines();

    for (unsigned int i = 0;i < vls.size(); ++i)
        vls[i].Draw(dc);

    dc.SetPen(wxNullPen);
}
void DiagramWavesWindow::DrawHorizontalArrows(wxDC &dc)
{
    if (!m_view) return;

    dc.SetPen(wxPen(m_view->GetLineColour(),1));

    GraphHorizontalArrows arrows = m_view->GetGraphHorizontalArrows();

    for ( unsigned int i = 0 ; i <  arrows.size() ; ++i )
        arrows[i].Draw(dc);

    dc.SetPen(wxNullPen);
}
void DiagramWavesWindow::DrawSignals(wxDC &dc)
{
    if (!m_view) return;

    dc.SetPen(wxPen(m_view->GetLineColour(),1));

    for ( unsigned int i = 0 ; i < m_view->GetGraphSignals().size(); i++)
        m_view->GetGraphSignals()[i]->DrawBackground(dc);

    for ( unsigned int i = 0 ; i < m_view->GetGraphSignals().size(); i++)
        m_view->GetGraphSignals()[i]->Draw(dc);

    dc.SetBrush(wxNullBrush);
    dc.SetPen(wxNullPen);
}


void DiagramWavesWindow::SetDrawlet(HoverDrawlet *drawlet)
{
    wxClientDC dc( this );
    m_owner->PrepareDC( dc );

    if (m_drawlet)
    {
        m_drawlet->UnDraw(dc);
        delete m_drawlet;
        m_drawlet = NULL;
    }

    if ( drawlet->Draw(dc) )
        m_drawlet = drawlet;
    else
        delete drawlet;

}
void DiagramWavesWindow::RemoveDrawlet()
{
    wxClientDC dc( this );
    m_owner->PrepareDC( dc );

    if (m_drawlet)
    {
        m_drawlet->UnDraw(dc);
        delete m_drawlet;
        m_drawlet = NULL;
    }
}

void DiagramWavesWindow::OnSize(wxSizeEvent &event)
{
    //
}

void DiagramWavesWindow::OnMouse(wxMouseEvent &event)
{
    int scroll_x = event.m_x;
    int scroll_y = event.m_y;
    m_owner->CalcUnscrolledPosition( scroll_x, scroll_y, &scroll_x, &scroll_y );
    wxPoint pt(scroll_x, scroll_y);
    m_view->WavesMouse(event, pt);
    if (event.ButtonDown(/*wxMOUSE_BTN_LEFT*/))
        SetFocusIgnoringChildren();
    event.Skip();
}
void DiagramWavesWindow::CalcControlPos(wxPoint &point)const
{
    int scroll_x = 0;
    int scroll_y = 0;
    m_owner->CalcUnscrolledPosition( scroll_x, scroll_y, &scroll_x, &scroll_y );
    point.x -= scroll_x;
    point.y -= scroll_y;
}
void DiagramWavesWindow::OnKeyDown(wxKeyEvent &event)
{
    m_view->WavesKey(event, true);
}
void DiagramWavesWindow::OnKeyUp(wxKeyEvent &event)
{
    m_view->WavesKey(event, false);
}
void DiagramWavesWindow::OnMouseEnter(wxMouseEvent &event)
{
    wxClientDC dc( this );
    m_owner->PrepareDC( dc );

    if (m_drawlet)
        m_drawlet->Draw(dc);
}
void DiagramWavesWindow::OnMouseLeave(wxMouseEvent &event)
{
    RemoveDrawlet();
}
bool DiagramWavesWindow::SetCursor(const wxCursor &cursor, bool forchilds )
{
    bool ret = wxWindow::SetCursor(cursor);

    //m_horizontalArrowTextCtrl->SetCursor(cursor);

    for (unsigned int i = 0 ; i < textctrls.size(); i++)
        if (!forchilds)
            textctrls[i]->SetCursor(wxCURSOR_IBEAM);
        else
            textctrls[i]->SetCursor(cursor);

    return ret;
}

