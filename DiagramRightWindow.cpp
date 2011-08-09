#include "DiagramRightWindow.h"

#include "DiagramAxisWindow.h"
#include "DiagramWavesWindow.h"
#include "TimingView.h"

IMPLEMENT_DYNAMIC_CLASS(DiagramRightWindow, wxScrolledWindow)

BEGIN_EVENT_TABLE(DiagramRightWindow, wxScrolledWindow)
  //EVT_PAINT(      DiagramRightWindow::OnPaint)
  EVT_SIZE(       DiagramRightWindow::OnSize)
END_EVENT_TABLE()

bool DiagramRightWindow::Create(TimingView *view, wxWindow *parent, DiagramLabelsWindow *labels, wxWindowID id, const wxPoint &pos, const wxSize &size )
{
    m_view = view;
    wxScrolledWindow::Create(parent, id, pos, size, wxSUNKEN_BORDER);
    m_axis = new DiagramAxisWindow(view, this, this, wxID_ANY, wxDefaultPosition, wxSize(wxDefaultCoord,60) );

    m_waves = new DiagramWavesWindow(view, this, this, m_axis, labels, wxID_ANY, wxDefaultPosition, wxDefaultSize );

    SetTargetWindow( m_waves );

    SetBackgroundColour( wxT("WHITE") );

    wxBoxSizer *mainsizer = new wxBoxSizer(wxVERTICAL);
    mainsizer->Add(m_axis, 0, wxEXPAND);
    mainsizer->Add(m_waves, 1, wxEXPAND);

    SetAutoLayout( true );
    SetSizer( mainsizer );

    //SetScrollbars( 10, 10, 500, 500 );
    //SetVirtualSize(1000, 1000);
    SetScrollRate(m_view->GetScrollPixelsPerUnit(), m_view->GetScrollPixelsPerUnit());

    return true;
}


void DiagramRightWindow::OnSize( wxSizeEvent &WXUNUSED(event) )
{
    // We need to override OnSize so that our scrolled
    // window a) does call Layout() to use sizers for
    // positioning the controls but b) does not query
    // the sizer for their size and use that for setting
    // the scrollable area as set that ourselves by
    // calling SetScrollbar() further down.

    Layout();

    AdjustScrollbars();
}

void DiagramRightWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );
/*
    wxSize size( GetClientSize() );

    long w,h;
    dc.GetTextExtent( wxT("Headline"), &w, &h );

    dc.DrawText( wxT("Headline"), long (size.x / 2 - w / 2), 10 );
*/
}

void DiagramRightWindow::DoUpdate()
{
    m_axis->DoUpdate();
    m_waves->DoUpdate();
}

DiagramAxisWindow *DiagramRightWindow::GetAxisWindow()
{
    return m_axis;
}
DiagramWavesWindow *DiagramRightWindow::GetWavesWindow()
{
    return m_waves;
}
