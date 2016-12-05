
#include "HoverHatch.h"


// END Interface Dependencies -----------------------------------------


HoverHatch::HoverHatch(wxPoint pos, wxSize size, wxColour colour, expandDirection dir)
:HoverDrawlet(),
m_pos(pos),
m_size(size),
m_colour(colour),
m_dir(dir)
{}
HoverHatch::~HoverHatch(){}


bool HoverHatch::Draw(wxDC &dc)
{
    wxRasterOperationMode old_lf = dc.GetLogicalFunction();
    dc.SetLogicalFunction(wxXOR);

    wxPen old_pen = dc.GetPen();
    dc.SetPen(m_colour);

    wxBrush old_brush = dc.GetBrush();
    dc.SetBrush(wxBrush(m_colour, wxCROSS_HATCH));

    wxSize size = m_size;
    if (m_dir == expand_x)
        size.x = dc.GetSize().GetWidth();
    if (m_dir == expand_y)
        size.y = dc.GetSize().GetHeight();
    dc.DrawRectangle(m_pos, size);



    dc.SetLogicalFunction(old_lf);
    dc.SetPen(old_pen);
    dc.SetBrush(old_brush);

    return true;
}
void HoverHatch::UnDraw(wxDC &dc)
{
    this->Draw(dc);
}



