
#include "HoverCross.h"


// END Interface Dependencies -----------------------------------------


HoverCross::HoverCross(wxPoint center)
    :HoverDrawlet(),
    m_center(center)
{}
HoverCross::~HoverCross(){}


bool HoverCross::Draw(wxDC &dc)
{
    int old_lf = dc.GetLogicalFunction();
    dc.SetLogicalFunction(wxXOR);

    wxPen old_pen = dc.GetPen();
    dc.SetPen(*wxGREY_PEN);

    dc.CrossHair(m_center);

    dc.SetLogicalFunction(old_lf);
    dc.SetPen(old_pen);

    return true;
}
void HoverCross::UnDraw(wxDC &dc)
{
    this->Draw(dc);
}



