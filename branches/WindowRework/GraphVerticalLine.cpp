#include "GraphVerticalLine.h"

#include "HoverCombo.h"
#include "HoverGraphCaret.h"
#include "HoverLine.h"


GraphVerticalLine::GraphVerticalLine(const wxPoint &start,const wxPoint &stop, const bool visible):
m_visible(visible),
m_start(start),
m_stop(stop)
{
    //ctor
}

GraphVerticalLine::~GraphVerticalLine()
{
    //dtor
}

void GraphVerticalLine::Draw(wxDC &dc)const
{
    if (! m_visible)
        return;

    dc.DrawLine( m_start.x, m_start.y, m_stop.x, m_stop.y );

}
const wxPoint &GraphVerticalLine::GetStartPoint()const
{
    return m_start;
}
//void GraphVerticalLine::Erase(wxDC &dc)const
//{
//    if (!m_visible)
//        return;
//
//    int old_lf = dc.GetLogicalFunction();
//    dc.SetLogicalFunction(wxXOR);
//    wxPen old_pen = dc.GetPen();
//    dc.SetPen(m_colour);
//
//    Draw(dc);
//
//    dc.SetLogicalFunction(old_lf);
//    dc.SetPen(old_pen);
//
//}
bool GraphVerticalLine::HasPoint(const wxPoint &pos, const unsigned char tolerance)const
{
    wxCoord y_top, y_bot;
    if (m_start.y > m_stop.y)
    {
        y_top = m_stop.y;
        y_bot = m_start.y;
    }
    else
    {
        y_top = m_start.y;
        y_bot = m_stop.y;
    }
    y_top -= tolerance;
    y_bot += tolerance;

    if ( pos.y > y_top && pos.y < y_bot &&
        pos.x >= m_start.x-tolerance && pos.x <= m_start.x+tolerance )
    {
        return true;
    }
    return false;
}
bool GraphVerticalLine::IsStartPoint(const wxPoint &pos, const unsigned char tolerance)const
{
    if ( pos.x >= m_start.x-tolerance && pos.x <= m_start.x+tolerance &&
         pos.y >= m_start.y-tolerance && pos.y <= m_start.y+tolerance )
    {
        return true;
    }

    return false;
}
bool GraphVerticalLine::IsStopPoint(const wxPoint &pos, const unsigned char tolerance)const
{
    if ( pos.x >= m_stop.x-tolerance && pos.x <= m_stop.x+tolerance &&
         pos.y >= m_stop.y-tolerance && pos.y <= m_stop.y+tolerance )
    {
        return true;
    }

    return false;
}
HoverDrawlet *GraphVerticalLine::GetActiveDrawlet(const wxColour &colour)const
{
    return new HoverCombo(
                new HoverGraphCaret(m_start - wxPoint(3,3),wxSize(7,7), colour),
                new HoverGraphCaret(m_stop - wxPoint(3,3),wxSize(7,7), colour)
            )
            //,new HoverLine(start, stop, *wxLIGHT_GREY)
            ;
}
HoverDrawlet *GraphVerticalLine::GetLineDrawlet(const wxColour &colour, int style)const
{
    return new HoverLine(m_start, m_stop, colour, 1, style);
}
