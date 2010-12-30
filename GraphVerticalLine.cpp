#include "GraphVerticalLine.h"

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
