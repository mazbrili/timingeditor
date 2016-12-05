#include "DiagramLabelsWindow.h"

#include <wx/dcbuffer.h>

#include "TimingDoc.h"
#include "TimingView.h"

#include "HoverDrawlet.h"
#include "LabelText.h"
#include "BusWidthText.h"

IMPLEMENT_DYNAMIC_CLASS(DiagramLabelsWindow,wxScrolledWindow)

BEGIN_EVENT_TABLE(DiagramLabelsWindow, wxScrolledWindow)
  EVT_PAINT(               DiagramLabelsWindow::OnPaint)
  EVT_ERASE_BACKGROUND(    DiagramLabelsWindow::OnEraseBackground)
  ////EVT_SIZE(                DiagramLabelsWindow::OnSize)
  ////EVT_MOUSEWHEEL(          DiagramLabelsWindow::OnMousewheel)
  EVT_MOUSE_EVENTS(        DiagramLabelsWindow::OnMouse)
  EVT_KEY_DOWN(            DiagramLabelsWindow::OnKeyDown)
  EVT_KEY_UP(              DiagramLabelsWindow::OnKeyUp)

  //EVT_SET_FOCUS(           DiagramLabelsWindow::OnSetFocus)

END_EVENT_TABLE()

//void DiagramLabelsWindow::OnSetFocus(wxFocusEvent &event)
//{
//}
DiagramLabelsWindow::DiagramLabelsWindow(TimingView *view, wxWindow* parent, wxScrolledWindow *scrollowner, wxWindowID id , const wxPoint& pos, const wxSize& size, long style)
: wxScrolledWindow(  parent, id, pos, size, style ),
m_owner(scrollowner),
m_view(view),
m_drawlet(NULL)
{
    //ctor
    SetVirtualSize(300, -1);

    SetScrollRate( m_view->GetScrollPixelsPerUnit(), 0);

}
DiagramLabelsWindow::~DiagramLabelsWindow(){}
DiagramLabelsWindow::DiagramLabelsWindow(){}


void DiagramLabelsWindow::DoUpdate()
{
    if (!m_view) return;
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    if ( !doc ) return;

    for( unsigned int i = 0; i < textctrls.size(); i++)
        textctrls[i]->Destroy();
    textctrls.clear();

    for ( unsigned int i = 0 ; i < bwidthctrls.size(); i++)
        bwidthctrls[i]->Destroy();
    bwidthctrls.clear();


    wxCoord width = 0;
    // draw the text to show the signal names
    //dc.SetPen(wxPen(GetTextColour(), 1));
    //dc.SetFont(m_view->GetFont());
    for ( unsigned int k = 0 ; k < doc->signals.size() ; ++k )
    {
        wxCoord w, h;
        Signal &sig = doc->signals[k];
        wxString str = sig.name;

        GetTextExtent(str + "[", &w, &h);

        wxCoord additionaloffset = //doc->SignalHeight/2 +
                                   doc->MinimumSignalDistance/2 +
                                   sig.prespace//-h/2
                                   ;

        //dc.DrawText(str, 10, m_view->heightOffsets[k] + additionaloffset);
        wxPoint pos(GetOffsetToLabelTextCtrl(), m_view->heightOffsets[k] + additionaloffset);
        int yScrollUnits, yOrigin;
        m_owner->GetViewStart( 0, &yOrigin );
        m_owner->GetScrollPixelsPerUnit( 0, &yScrollUnits );
        pos.y -= yOrigin*yScrollUnits;

        LabelText *label = new LabelText(this, m_view, str,
                                         pos,
                                         wxSize(w+5,doc->SignalHeight-4), k);
        label->SetBackgroundColour(this->GetBackgroundColour());
        textctrls.push_back( label );


        if ( sig.IsBus )
        {
            wxCoord wadd;
            str = sig.buswidth;
            GetTextExtent(str + "[", &wadd, &h);

            wxPoint pos(GetOffsetToLabelTextCtrl()+ w+15, m_view->heightOffsets[k] + additionaloffset );
            pos.y -= yOrigin*yScrollUnits;
            BusWidthText *bwidth = new BusWidthText(this, m_view, str,
                                                    pos,
                                                    wxSize(wadd+5,doc->SignalHeight-4), k);
            bwidth->SetBackgroundColour(this->GetBackgroundColour());
            label->SetBuswidthField(bwidth);
            bwidthctrls.push_back(bwidth);
            w += wadd + 10;
        }



        if ( width < w )
            width = w;

    }
    width += 30;
    SetVirtualSize(width, -1);


    RemoveDrawlet();
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
void DiagramLabelsWindow::ScrollWindow(int dx, int dy, const wxRect* rect)
{
    wxScrolledWindow::ScrollWindow(dx, dy, rect);

//    int xScrollUnits, xOrigin;
//    wxScrolledWindow::GetViewStart(&xOrigin, 0);
//    wxScrolledWindow::GetScrollPixelsPerUnit( &xScrollUnits, 0);
//
//    for( unsigned int i = 0; i < textctrls.size(); i++)
//        textctrls[i]->SetSize(GetOffsetToLabelTextCtrl()-xOrigin*xScrollUnits, wxDefaultCoord, wxDefaultCoord, wxDefaultCoord);

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
void DiagramLabelsWindow::Draw(wxDC &dc, bool exporting)
{
    if (!m_view) return;
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    if ( !doc ) return;

    // draw the horizontal lines
    if (!exporting)
    {
        wxCoord width = GetVirtualSize().GetX();
        if ( width < GetClientSize().x)
            width = GetClientSize().x;
        dc.SetPen(wxPen(GetLineColour(), 1));
        for ( unsigned int k = 0 ; k < m_view->heightOffsets.size() ; ++k )
            dc.DrawLine(0, m_view->heightOffsets[k], width+m_view->GetScrollPixelsPerUnit()+20, m_view->heightOffsets[k]);
        dc.SetPen(wxNullPen);
    }

    unsigned int k = 0;
    for (unsigned int n = 0; n < doc->signals.size() ; n++)
    {
        LabelText *label = textctrls[n];
        if(exporting)
            label->Draw(dc);

        Signal sig = doc->signals[n];
        if ( sig.IsBus )
        {

            BusWidthText *bwidth = bwidthctrls[k++];

            wxPoint pos = label->GetPosition();
            pos.y = m_view->heightOffsets[n] + doc->MinimumSignalDistance/2 + sig.prespace;

            pos.x += label->GetSize().x;
            dc.DrawText("[", pos.x, pos.y);
            pos.x += bwidth->GetSize().x+10;
            dc.DrawText("]", pos.x, pos.y);
            if(exporting)
                bwidth->Draw(dc);

        }
    }

    if (m_drawlet && !exporting)
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
    pt.y += yOrigin*yScrollUnits;

    m_view->LabelsMouse(event, pt);

    if ( event.ButtonDown(/*wxMOUSE_BTN_LEFT*/) )
        SetFocusIgnoringChildren();

    //event.Skip();
}

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
