
#include "HoverLine.h"

// END Interface Dependencies -----------------------------------------


HoverLine::HoverLine(wxPoint start, wxPoint stop, wxColour colour)
    :HoverDrawlet(),
    m_a(start),
    m_b(stop),
    m_colour(colour)
{}
HoverLine::~HoverLine(){}


bool HoverLine::Draw(wxDC &dc)
{
    int old_lf = dc.GetLogicalFunction();
    dc.SetLogicalFunction(wxXOR);

    wxPen old_pen = dc.GetPen();
    dc.SetPen(m_colour);

    dc.DrawLine(m_a, m_b);

    dc.SetLogicalFunction(old_lf);
    dc.SetPen(old_pen);

    return true;
}
void HoverLine::UnDraw(wxDC &dc)
{
    this->Draw(dc);
}



