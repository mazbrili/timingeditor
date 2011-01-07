#include "DiagramLabelsWindow.h"

#include <wx/dcbuffer.h>

#include "TimingDoc.h"
#include "TimingView.h"

#include "HoverDrawlet.h"


IMPLEMENT_DYNAMIC_CLASS(DiagramLabelsWindow,wxScrolledWindow)

BEGIN_EVENT_TABLE(DiagramLabelsWindow, wxScrolledWindow)
  EVT_PAINT(               DiagramLabelsWindow::OnPaint)
  EVT_ERASE_BACKGROUND(    DiagramLabelsWindow::OnEraseBackground)
  ////EVT_SIZE(                DiagramLabelsWindow::OnSize)
  ////EVT_MOUSEWHEEL(          DiagramLabelsWindow::OnMousewheel)

    EVT_MOUSE_EVENTS(      DiagramLabelsWindow::OnMouse)
//  EVT_MOTION(              DiagramLabelsWindow::OnMouse)
//  EVT_LEFT_DOWN(           DiagramLabelsWindow::OnMouse)
//  EVT_LEFT_UP(             DiagramLabelsWindow::OnMouse)
//  EVT_RIGHT_DOWN(          DiagramLabelsWindow::OnMouse)
//  EVT_RIGHT_UP(            DiagramLabelsWindow::OnMouse)
//  EVT_RIGHT_DCLICK(        DiagramLabelsWindow::OnMouse)
//
//  EVT_ENTER_WINDOW(        DiagramLabelsWindow::OnMouseEnter)
//  EVT_LEAVE_WINDOW(        DiagramLabelsWindow::OnMouseLeave)

  //EVT_CHILD_FOCUS(         DiagramLabelsWindow::OnChildFocus)
  EVT_KEY_DOWN(            DiagramLabelsWindow::OnKeyDown)
  EVT_KEY_UP(              DiagramLabelsWindow::OnKeyUp)
END_EVENT_TABLE()


DiagramLabelsWindow::DiagramLabelsWindow(TimingView *view, wxWindow* parent, wxScrolledWindow *scrollowner, wxWindowID id , const wxPoint& pos, const wxSize& size, long style)
: wxScrolledWindow(  parent, id, pos, size, style/* | wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN*/ ),
m_owner(scrollowner),
m_view(view),
m_drawlet(NULL)
{
    //ctor
    SetVirtualSize(300, -1);

    SetScrollRate( m_view->GetScrollPixelsPerUnit(), 0);

    //(void*)new wxTextCtrl(this, wxID_ANY, _T("TEST"), wxPoint(20,20), wxSize(50,20), wxBORDER_SIMPLE ); //wxBORDER_NONE| wxTE_DONTWRAP

}
DiagramLabelsWindow::~DiagramLabelsWindow(){}
DiagramLabelsWindow::DiagramLabelsWindow(){}


void DiagramLabelsWindow::Update()
{
    if (!m_view) return;
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    if ( !doc ) return;

    for( unsigned int i = 0; i < textctrls.size(); i++)
        textctrls[i]->Destroy();
    textctrls.clear();


    wxCoord width = 0;
    wxCoord w, h;
    // draw the text to show the signal names
    //dc.SetPen(wxPen(GetTextColour(), 1));
    //dc.SetFont(m_view->GetFont());
    for ( unsigned int k = 0 ; k < doc->signals.size() ; ++k )
    {
        Signal &sig = doc->signals[k];
        wxString str = sig.name;

        if ( sig.IsBus )
            str += _T(" [") + sig.buswidth + _T("]");

        //str += _T("Really loooooooooooong label");

        GetTextExtent(str, &w, &h);

        wxCoord additionaloffset = doc->SignalHeight/2 +
                                   doc->MinimumSignalDistance/2 +
                                   sig.prespace-
                                   h/2;

        //dc.DrawText(str, 10, m_view->heightOffsets[k] + additionaloffset);
        textctrls.push_back(new wxTextCtrl(this, wxID_ANY, str,
                                           wxPoint(GetOffsetToLabelTextCtrl(), m_view->heightOffsets[k] + additionaloffset),
                                           wxSize(w+10,h+5), wxBORDER_NONE )); //wxBORDER_NONE| wxTE_DONTWRAP | wxBORDER_SIMPLE
        if ( width < w )
            width = w;

    }
    width += 30;
    SetVirtualSize(width, -1);



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

wxColour DiagramLabelsWindow::GetBackgroundColour() const
{
    return m_view->GetLabelsBackgroundColour();
}
wxColour DiagramLabelsWindow::GetLineColour() const
{
    return m_view->GetLabelsLineColour();
}
wxColour DiagramLabelsWindow::GetTextColour() const
{
    return m_view->GetLabelsTextColour();
}
const unsigned int DiagramLabelsWindow::GetOffsetToLabelTextCtrl()const
{
    return 10;
}
//void DiagramLabelsWindow::OnChildFocus(wxChildFocusEvent &event)
//{
//
//}
void DiagramLabelsWindow::ScrollWindow(int dx, int dy, const wxRect* rect)
{
    wxScrolledWindow::ScrollWindow(dx, dy, rect);

    int xScrollUnits, xOrigin;
    wxScrolledWindow::GetViewStart(&xOrigin, 0);
    wxScrolledWindow::GetScrollPixelsPerUnit( &xScrollUnits, 0);

    for( unsigned int i = 0; i < textctrls.size(); i++)
        textctrls[i]->SetSize(GetOffsetToLabelTextCtrl()-xOrigin*xScrollUnits, wxDefaultCoord, wxDefaultCoord, wxDefaultCoord);

}

void DiagramLabelsWindow::OnEraseBackground(wxEraseEvent &WXUNUSED(event) ){}

void DiagramLabelsWindow::ScaleDC(wxDC &dc)
{
    // DoPrepareDC(dc); is wrong..  it will translate both x and y if the
    // window is scrolled, the label windows are active in one
    // direction only.

    // let let base do what ever is needed
//    wxPoint pt = dc.GetDeviceOrigin();
//    wxScrolledWindow::DoPrepareDC(dc);


    int xScrollUnits, xOrigin;
    wxScrolledWindow::GetViewStart(&xOrigin, 0);
    wxScrolledWindow::GetScrollPixelsPerUnit( &xScrollUnits, 0);

    int yScrollUnits, yOrigin;
    m_owner->GetViewStart( 0, &yOrigin );
    m_owner->GetScrollPixelsPerUnit( 0, &yScrollUnits );

    dc.SetDeviceOrigin(-xOrigin*xScrollUnits, -yOrigin * yScrollUnits );

    dc.SetUserScale( m_scaleX, m_scaleY );
}
void DiagramLabelsWindow::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxBufferedPaintDC dc(this);
    PaintBackground(dc);

    ScaleDC(dc);
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
void DiagramLabelsWindow::Draw(wxDC &dc)
{
    if (!m_view) return;
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    if ( !doc ) return;


//    // draw the text to show the signal names
//    wxCoord width = 0;
//    wxCoord w, h;
//    dc.SetPen(wxPen(GetTextColour(), 1));
//    dc.SetFont(m_view->GetFont());
//    for ( unsigned int k = 0 ; k < doc->signals.size() ; ++k )
//    {
//        Signal &sig = doc->signals[k];
//        wxString str = sig.name;
//
//        if ( sig.IsBus )
//            str += _T(" [") + sig.buswidth + _T("]");
//
//        GetTextExtent(str, &w, &h);
//
//        wxCoord additionaloffset = doc->SignalHeight/2 +
//                                   doc->MinimumSignalDistance/2 +
//                                   sig.prespace-
//                                   h/2;
//        dc.DrawText(str, 10, m_view->heightOffsets[k] + additionaloffset);
//        if ( width < w )
//            width = w;
//
//    }
//    width += 30;
//    SetVirtualSize(width, -1);
//    dc.SetPen(wxNullPen);

    // draw the horizontal lines
    wxCoord width = GetVirtualSize().GetX();
    if ( width < GetClientSize().x)
        width = GetClientSize().x;
    dc.SetPen(wxPen(GetLineColour(), 1));
    for ( unsigned int k = 0 ; k < m_view->heightOffsets.size() ; ++k )
        dc.DrawLine(0, m_view->heightOffsets[k], width+m_view->GetScrollPixelsPerUnit(), m_view->heightOffsets[k]);
    dc.SetPen(wxNullPen);

    if (m_drawlet)
        m_drawlet->Draw(dc);
}


void DiagramLabelsWindow::OnMouse(wxMouseEvent &event)
{
    wxPoint pt(event.GetPosition());
    int xScrollUnits, xOrigin;
    wxScrolledWindow::GetViewStart(&xOrigin, 0);
    wxScrolledWindow::GetScrollPixelsPerUnit( &xScrollUnits, 0);

    int yScrollUnits, yOrigin;
    m_owner->GetViewStart( 0, &yOrigin );
    m_owner->GetScrollPixelsPerUnit( 0, &yScrollUnits );

    pt.x -= xOrigin*xScrollUnits;
    pt.y -= yOrigin*yScrollUnits;

    m_view->LabelsMouse(event, pt);

    event.Skip();
};

void DiagramLabelsWindow::OnKeyDown(wxKeyEvent &event)
{
    m_view->LabelsKey(event, true);
}
void DiagramLabelsWindow::OnKeyUp(wxKeyEvent &event)
{
    m_view->LabelsKey(event, false);
}

void DiagramLabelsWindow::OnMouseEnter(wxMouseEvent &event)
{
    wxClientDC dc( this );
    m_owner->PrepareDC( dc );

    if (m_drawlet)
        m_drawlet->Draw(dc);
}
void DiagramLabelsWindow::OnMouseLeave(wxMouseEvent &event)
{
    RemoveDrawlet();
}

void DiagramLabelsWindow::SetDrawlet(HoverDrawlet *drawlet)
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
void DiagramLabelsWindow::RemoveDrawlet()
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
