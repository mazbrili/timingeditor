#include "GraphHorizontalArrow.h"


GraphHorizontalArrow::GraphHorizontalArrow(const wxPoint &start, const wxPoint &stop, const wxString str, const wxPoint stroffset, bool visible):
m_visible(visible),
m_text(str),
m_textoffset(stroffset)
{
    //ctor
    m_swapped = start.x > stop.x;
    if ( m_swapped ) // swap
    {
        m_start = stop;
        m_stop = start;
    }
    else
    {
        m_start = start;
        m_stop = stop;
    }
}

GraphHorizontalArrow::~GraphHorizontalArrow()
{
    //dtor
}
bool GraphHorizontalArrow::IsSwapped()const{return m_swapped;}
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

    wxCoord CharHeight = dc.GetCharHeight();
    toff.y -= CharHeight;

    dc.DrawText( m_text, toff );
}

bool GraphHorizontalArrow::HasPoint(const wxPoint &pos, const unsigned char tolerance)const
{
    wxCoord x_left, x_right;
//    if (m_start.x > m_stop.x)
//    {
//        x_left = m_stop.x;
//        x_right = m_start.x;
//    }
//    else
//    {
        x_left = m_start.x;
        x_right = m_stop.x;
//    }
    x_left -= tolerance;
    x_right += tolerance;

    if ( pos.x > x_left && pos.x < x_right &&
        pos.y >= m_start.y-tolerance && pos.y <= m_start.y+tolerance )
        return true;

    return false;
}
bool GraphHorizontalArrow::IsStartPoint(const wxPoint &pos, const unsigned char tolerance)const
{
    if ( pos.x >= m_start.x-tolerance && pos.x <= m_start.x+tolerance &&
         pos.y >= m_start.y-tolerance && pos.y <= m_start.y+tolerance )
        return true;

    return false;
}
bool GraphHorizontalArrow::IsStopPoint(const wxPoint &pos, const unsigned char tolerance)const
{
    if ( pos.x >= m_stop.x-tolerance && pos.x <= m_stop.x+tolerance &&
         pos.y >= m_stop.y-tolerance && pos.y <= m_stop.y+tolerance )
        return true;

    return false;
}
bool GraphHorizontalArrow::IsTextPoint(const wxPoint &pos, const unsigned char tolerance)const
{
    if ( pos.x >= m_textoffset.x-tolerance && pos.x <= m_textoffset.x+tolerance &&
         pos.y >= m_textoffset.y-tolerance && pos.y <= m_textoffset.y+tolerance )
        return true;

    return false;
}
//HoverDrawlet *GraphHorizontalArrow::GetLineDrawlet(const wxColour &colour, int style)const
//{
//    return new HoverLine(m_start, m_stop, colour, 1, style);
//}
