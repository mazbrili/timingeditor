#include "DiagramLabelsWindow.h"

#include <wx/dcbuffer.h>

#include "TimingDoc.h"
#include "TimingView.h"


IMPLEMENT_DYNAMIC_CLASS(DiagramLabelsWindow,wxScrolledWindow)

BEGIN_EVENT_TABLE(DiagramLabelsWindow, wxScrolledWindow)
  EVT_PAINT(               DiagramLabelsWindow::OnPaint)
  EVT_ERASE_BACKGROUND(    DiagramLabelsWindow::OnEraseBackground)
  //EVT_SIZE(                DiagramLabelsWindow::OnSize)
  //EVT_MOUSEWHEEL(          DiagramLabelsWindow::OnMousewheel)
END_EVENT_TABLE()

const int pixelsPerUnitX = 10;

DiagramLabelsWindow::DiagramLabelsWindow(TimingView *view, wxWindow* parent, wxScrolledWindow *scrollowner, wxWindowID id , const wxPoint& pos, const wxSize& size, long style)
: wxScrolledWindow(  parent, id, pos, size, style/* | wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN*/ ),
m_owner(scrollowner),
m_view(view)
{
    //ctor
    SetVirtualSize(300, -1);

    SetScrollRate( pixelsPerUnitX, 0);

    //std::vector<TimingTextCtrl *> SignalLabels;
    //std::vector<wxStaticLine *> Divisors;
}
DiagramLabelsWindow::~DiagramLabelsWindow(){}
DiagramLabelsWindow::DiagramLabelsWindow(){}


void DiagramLabelsWindow::Update()
{
    Refresh();
}
//void DiagramLabelsWindow::UpdateSizeToMatchLabels()
//{
//    wxCoord width = 0;//GetClientSize().x;
//    for ( unsigned int k = 0 ; k < SignalLabels.size() ; ++k )
//    {
//        int x, y;
//        SignalLabels[k]->GetSize(&x, &y);
//        if (width < x+10 ) width = x+10;
//    }
//    SetVirtualSize(width , -1);
//}
//void DiagramLabelsWindow::UpdateSizeOfLines()
//{
//    wxCoord width = GetVirtualSize().x;
//    if ( GetClientSize().x > width )
//        width = GetClientSize().x;
//    for(unsigned int k = 0; k < Divisors.size() ; ++k )
//        Divisors[k]->SetSize(width, 1);
//
//}

void DiagramLabelsWindow::OnEraseBackground(wxEraseEvent &WXUNUSED(event) )
{}
void DiagramLabelsWindow::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxBufferedPaintDC dc(this);
    PaintBackground(dc);

    // DoPrepareDC(dc); is wrong..  it will translate both x and y if the
    // window is scrolled, the label windows are active in one
    // direction only.  Do the action below instead -- RL.
    //m_owner->PrepareDC( dc );
    int xScrollUnits, xOrigin;
    GetViewStart(&xOrigin, 0);
    GetScrollPixelsPerUnit( &xScrollUnits, 0);

    int yScrollUnits, yOrigin;
    m_owner->GetViewStart( 0, &yOrigin );
    m_owner->GetScrollPixelsPerUnit( 0, &yScrollUnits );

    dc.SetDeviceOrigin( -xOrigin*xScrollUnits, -yOrigin * yScrollUnits );


    Draw(dc);
}
void DiagramLabelsWindow::PaintBackground(wxDC &dc)
{
    dc.SetBrush(wxBrush(GetBackgroundColour()));
    dc.SetPen(wxPen(GetBackgroundColour(), 1));
    wxRect windowRect(wxPoint(0,0), GetClientSize());
    dc.DrawRectangle(windowRect);
    dc.SetBrush(wxNullBrush);
    dc.SetPen(wxNullPen);
}
wxColour DiagramLabelsWindow::GetBackgroundColour() const
{
    //*wxWHITE;
    return wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE );
}
wxColour DiagramLabelsWindow::GetLineColour() const
{
    return wxSystemSettings::GetColour( wxSYS_COLOUR_BTNSHADOW  );
}
wxColour DiagramLabelsWindow::GetTextColour() const
{
    return *wxBLACK;
}
void DiagramLabelsWindow::OnSize( wxSizeEvent &WXUNUSED(event) )
{

}
//void DiagramLabelsWindow::OnMousewheel(wxMouseEvent &event)
//{
//}

void DiagramLabelsWindow::Draw(wxDC &dc)
{
    if (!m_view) return;
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    if ( !doc ) return;

    wxCoord width = 0;
    wxCoord w, h;


    // draw the text to show the signal names
    dc.SetPen(wxPen(GetTextColour(), 1));
    dc.SetFont(m_view->GetFont());
    for ( unsigned int k = 0 ; k < doc->signals.size() ; ++k )
    {
        Signal &sig = doc->signals[k];
        wxString str = sig.name;

        if ( sig.IsBus )
            str += _T(" [") + sig.buswidth + _T("]");

        GetTextExtent(str, &w, &h);

        wxCoord additionaloffset = doc->SignalHeight/2 +
                                   doc->MinimumSignalDistance/2 +
                                   sig.prespace-
                                   h/2;
        dc.DrawText(str, 10, m_view->heightOffsets[k] + additionaloffset);
        if ( width < w )
            width = w;

    }
    width += 30;
    SetVirtualSize(width, -1);
    dc.SetPen(wxNullPen);


    // draw the horizontal lines
    if ( width < GetClientSize().x)
        width = GetClientSize().x;
    dc.SetPen(wxPen(GetLineColour(), 1));
    for ( unsigned int k = 0 ; k < m_view->heightOffsets.size() ; ++k )
        dc.DrawLine(0, m_view->heightOffsets[k], width+pixelsPerUnitX, m_view->heightOffsets[k]);
    dc.SetPen(wxNullPen);
}

