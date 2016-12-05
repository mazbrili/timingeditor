
#include "HoverLine.h"

// END Interface Dependencies -----------------------------------------


HoverLine::HoverLine(wxPoint start, wxPoint stop, wxColour colour, int width, int style)
:HoverDrawlet(),
m_a(start),
m_b(stop),
m_colour(colour),
m_width(width),
m_style(style)
{
    m_dir = expand_none;
}
HoverLine::HoverLine(int x, int y, wxColour colour, int width, int style)
:HoverDrawlet(),
m_colour(colour),
m_width(width),
m_style(style)
{
    if ( x < 0 )
        m_dir = expand_x;
    else
        m_dir = expand_y;

    m_a.x = x;
    m_a.y = y;
}

HoverLine::~HoverLine(){}


bool HoverLine::Draw(wxDC &dc)
{
    wxRasterOperationMode old_lf = dc.GetLogicalFunction();
    dc.SetLogicalFunction(wxXOR);

    wxPen old_pen = dc.GetPen();
    dc.SetPen(wxPen(m_colour, m_width, m_style));

    if ( m_dir == expand_none )
        dc.DrawLine(m_a, m_b);
    else if ( m_dir == expand_x)
    {
        wxPoint a(0, m_a.y), b(dc.GetSize().GetWidth(), m_a.y);
        dc.DrawLine(a, b);
    }
    else
    {
        wxPoint a(m_a.x, 0), b(m_a.x, dc.GetSize().GetHeight());
        dc.DrawLine(a, b);
    }


    dc.SetLogicalFunction(old_lf);
    dc.SetPen(old_pen);

    return true;
}
void HoverLine::UnDraw(wxDC &dc)
{
    this->Draw(dc);
}



