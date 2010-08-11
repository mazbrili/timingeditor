#include "DiagramRightWindow.h"

#include "DiagramAxisWindow.h"
#include "DiagramWaveWindow.h"

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

    m_waves = new DiagramWaveWindow(view, this, this, m_axis, labels, wxID_ANY, wxDefaultPosition, wxDefaultSize );

    SetTargetWindow( m_waves );

    SetBackgroundColour( wxT("WHITE") );
    //SetCursor( wxCursor( wxCURSOR_HAND ) );

    wxBoxSizer *mainsizer = new wxBoxSizer(wxVERTICAL);
    mainsizer->Add(m_axis, 0, wxEXPAND);
    mainsizer->Add(m_waves, 1, wxEXPAND);

    SetAutoLayout( true );
    SetSizer( mainsizer );

    //SetScrollbars( 10, 10, 500, 500 );
    //SetVirtualSize(1000, 1000);
    SetScrollRate(10, 10);

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
}

void DiagramRightWindow::Update()
{
    m_axis->Update();
    m_waves->Update();
}
