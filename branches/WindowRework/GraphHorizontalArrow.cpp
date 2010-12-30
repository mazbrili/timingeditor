#include "GraphHorizontalArrow.h"

GraphHorizontalArrow::GraphHorizontalArrow(const wxPoint &start, const wxPoint &stop, const wxString str, const wxPoint stroffset, const bool visible):
m_visible(visible),
m_text(str),
m_start(start),
m_stop(stop),
m_textoffset(stroffset)
{
    //ctor
}

GraphHorizontalArrow::~GraphHorizontalArrow()
{
    //dtor
}

void GraphHorizontalArrow::Draw(wxDC &dc)const
{
    if ( ! m_visible )
        return;

    dc.DrawLine(  m_start.x,   m_start.y, m_stop.x,   m_stop.y);
    /// arrowhead:
    dc.DrawLine(m_start.x, m_start.y, m_start.x+5, m_start.y-3);
    dc.DrawLine(m_start.x, m_start.y, m_start.x+5, m_start.y+3);
    dc.DrawLine( m_stop.x, m_stop.y,  m_stop.x-5,  m_stop.y-3);
    dc.DrawLine( m_stop.x, m_stop.y,  m_stop.x-5,  m_stop.y+3);

    wxPoint toff = m_textoffset;


    toff.y -= dc.GetCharHeight();

    dc.DrawText( m_text, toff );
}
