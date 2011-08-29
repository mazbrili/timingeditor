#include "SplitterDrawlet.h"

//#include <wx/bitmap.h>

SplitterDrawlet::SplitterDrawlet(unsigned int dist,unsigned int gap):
m_bitmap(NULL),
m_dist(dist),
m_gap(gap)
{
    //ctor
}

SplitterDrawlet::~SplitterDrawlet()
{
    if ( m_bitmap )
        delete m_bitmap;
}
bool SplitterDrawlet::Draw(wxDC &dc)
{
    wxSize sz = dc.GetSize();
    if ( ! m_bitmap )
        delete m_bitmap;
    m_bitmap = new wxBitmap( sz.x, sz.y );

    // Create a memory DC
    wxMemoryDC temp_dc(&dc);
    temp_dc.SelectObject( *m_bitmap );

    temp_dc.Blit( 0, 0, sz.x, sz.y, &dc, 0, 0 );

    dc.Blit( m_dist+m_gap, 0,
            sz.x-m_dist, sz.y,
            &temp_dc,
            m_dist, 0);



    return true;
}
void SplitterDrawlet::UnDraw(wxDC &dc)
{
    if (!m_bitmap)
        return;

    wxSize sz = dc.GetSize();

    // Create a memory DC
    wxMemoryDC temp_dc(&dc);
    temp_dc.SelectObject( *m_bitmap );

    dc.Blit( 0, 0, sz.x, sz.y, &temp_dc, 0, 0 );
}
