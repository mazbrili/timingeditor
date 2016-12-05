
#include "HoverCross.h"


// END Interface Dependencies -----------------------------------------


HoverCross::HoverCross(wxPoint center, wxColour colour)
    :HoverDrawlet(),
    m_center(center),
    m_colour(colour)
{}
HoverCross::~HoverCross(){}


bool HoverCross::Draw(wxDC &dc)
{
    wxRasterOperationMode old_lf = dc.GetLogicalFunction();
    dc.SetLogicalFunction(wxXOR);

    wxPen old_pen = dc.GetPen();
    dc.SetPen(m_colour);

    dc.CrossHair(m_center);

    dc.SetLogicalFunction(old_lf);
    dc.SetPen(old_pen);

    return true;
}
void HoverCross::UnDraw(wxDC &dc)
{
    this->Draw(dc);
}



