#include "DiagramAxisWindow.h"

#include <wx/dcbuffer.h>
#include "TimingDoc.h"
#include "TimingView.h"

IMPLEMENT_DYNAMIC_CLASS(DiagramAxisWindow,wxPanel)

BEGIN_EVENT_TABLE(DiagramAxisWindow, wxPanel)
  EVT_PAINT(               DiagramAxisWindow::OnPaint)
  EVT_ERASE_BACKGROUND(    DiagramAxisWindow::OnEraseBackground)
END_EVENT_TABLE()

DiagramAxisWindow::DiagramAxisWindow(TimingView *view, wxWindow* parent, wxScrolledWindow *scrollowner, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: wxPanel( parent, id, pos, size, style )
{
    //ctor
    m_owner = scrollowner;
    m_view = view;
}
DiagramAxisWindow::~DiagramAxisWindow(){}
DiagramAxisWindow::DiagramAxisWindow(){}

void DiagramAxisWindow::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    PaintBackground(dc);

    // DoPrepareDC(dc);  it will translate both x and y if the
    // window is scrolled, the label windows are active in one
    // direction only.  Do the action below instead -- RL.
    //m_owner->PrepareDC( dc );

    int xScrollUnits, xOrigin;

    m_owner->GetViewStart( &xOrigin, 0 );
    m_owner->GetScrollPixelsPerUnit( &xScrollUnits, 0 );
    dc.SetDeviceOrigin( -xOrigin*xScrollUnits, 0 );

    Draw(dc);
}

const int DistanceToTicksLine    = 25;
const int DistanceFromTicksLine  = 5;
const int DistanceToAxis        = 25;
const int DistanceFromAxis      = 20;


void DiagramAxisWindow::OnEraseBackground(wxEraseEvent &WXUNUSED(event) ){}

wxColour DiagramAxisWindow::GetBackgroundColour() const
{
    return wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE );
    //*wxWHITE;
}
wxColour DiagramAxisWindow::GetLineColour() const
{
    return *wxBLACK;
}

void DiagramAxisWindow::PaintBackground(wxDC &dc)
{
    dc.SetBrush(wxBrush(GetBackgroundColour()));
    dc.SetPen(wxPen(GetBackgroundColour(), 1));
    wxRect windowRect(wxPoint(0,0), GetClientSize());
    dc.DrawRectangle(windowRect);
    dc.SetBrush(wxNullBrush);
    dc.SetPen(wxNullPen);
}

void DiagramAxisWindow::Update()
{
    Refresh();
}
void DiagramAxisWindow::Draw(wxDC & dc)
{
    bool exporting = false;

    if (!m_view) return;
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    if ( !doc ) return;

    wxCoord axispos =  DistanceToAxis;
    if ( ! exporting )
        axispos += DistanceToTicksLine + DistanceFromTicksLine;
    wxCoord start = 10;
    wxCoord stop = start + (m_view->GridStepWidth)*(m_view->VisibleTicks.size() - 1);



    dc.SetFont(m_view->GetFont());
    dc.SetPen(wxPen(GetLineColour(), 1));

    /// drawing the time axis
    dc.DrawLine(start , axispos, stop, axispos);

    for ( wxUint32 n = 0 ; n < m_view->VisibleTicks.size() ; ++n )
    {
        wxInt32 ticks = m_view->VisibleTicks[n];
        if ( ((ticks + doc->timeOffset) % (doc->MarkerLength)) == 0 )
        {
            dc.DrawLine(start + n*(m_view->GridStepWidth), axispos - 2,
                        start + n*(m_view->GridStepWidth), axispos + 3);

            double t = (ticks + doc->timeOffset) * doc->TickLength;
            wxInt8 u = doc->TickLengthUnit;
            while ( u < 3 && (t >= 1000.0 || t <= -1000.0))
            {
                u++;
                t /= 1000.0;
            }
            wxString str(wxString::Format(m_view->GetFloatFormatStr(), t));
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
            dc.DrawText(str, start + n*(m_view->GridStepWidth) - w/2, axispos - h - 4);
        }
    }

    /// drawing the ticks
    dc.DrawLine(start - 3, DistanceToTicksLine, stop + 3, DistanceToTicksLine);
    for (unsigned int n = 0 ; n <= m_view->VisibleTicks.size()-1; ++n)
        dc.DrawLine(start + n*(m_view->GridStepWidth), DistanceToTicksLine - 2,
                    start + n*(m_view->GridStepWidth), DistanceToTicksLine + 3);


    /// drawing the time compressors
    for ( wxUint32 k = 0 ; k < m_view->VisibleTicks.size()-1 ; ++k )
    {
        wxInt32 tick = m_view->VisibleTicks[k];
        wxInt32 len = 1;
        for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
            if ( doc->compressors[indx].pos == tick && doc->compressors[indx].enabled )
                len = doc->compressors[indx].length;
        if ( len > 1 )
        {
            /// triangle
            wxPoint offset;
            offset.x = start + (0.5 + k)*(m_view->GridStepWidth) - 3;
            offset.y = DistanceToTicksLine-3;
            wxPoint points[] =
            {
                offset + wxPoint(0, 0),
                offset + wxPoint(8, 0),
                offset + wxPoint(4, 6)
            };
            dc.SetBrush(wxBrush(GetLineColour()));
            dc.DrawPolygon(3, points);
            dc.SetBrush(wxNullBrush);


            /// split the axis
            offset.x = start + (0.5 + k) * (m_view->GridStepWidth) - 1;
            offset.y += DistanceToTicksLine + DistanceFromTicksLine;
            //if ( exporting) offset.y = DistanceToAxis - 3;
            wxPen bgpen = wxPen(GetBackgroundColour(),1);
            for ( wxInt32 n = 0 ; n < 4 ; ++n )
            {
                bool drawbg =  n != 0 && n != 3;
                if ( drawbg )
                    dc.SetPen( bgpen );
                dc.DrawLine(offset.x + n-1 , offset.y,
                            offset.x + n-1 , offset.y + 6);
                if ( drawbg )
                    dc.SetPen( wxNullPen );

            }
        }
        for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
        if ( doc->compressors[indx].pos == tick && !doc->compressors[indx].enabled )
        {

            wxPoint offset;
        	len =  doc->compressors[indx].length;
            offset.x = start  + (0.5 + k) * (m_view->GridStepWidth);
            offset.y = DistanceToTicksLine-4 ;
            wxPoint points[] =
            {
                offset + wxPoint(-1, 0),
                offset + wxPoint( 4, 4),
                offset + wxPoint(-1, 8)
            };
            //if ( !exporting )
            //{
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
            if ( offset.x + (rlen-0.75)*(m_view->GridStepWidth) < stop )
            {
                dc.DrawLine(offset.x, offset.y, offset.x + (rlen-0.75)*(m_view->GridStepWidth), offset.y);
                dc.DrawLine(offset.x + (rlen-0.75)*(m_view->GridStepWidth), offset.y, offset.x + (rlen-0.75)*(m_view->GridStepWidth), offset.y+3);
            }
            else
                dc.DrawLine(offset.x, offset.y, stop, offset.y);
            dc.SetBrush(*wxWHITE_BRUSH);
            dc.DrawPolygon(3, points);

            dc.SetPen(wxNullPen);
            dc.SetBrush(wxNullBrush);
            //}
        }
    }

}
