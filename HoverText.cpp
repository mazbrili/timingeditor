
#include "HoverText.h"


HoverText::HoverText(wxString txt, wxPoint pos)
:HoverDrawlet(),
m_txt(txt),
m_pos(pos)
{}
HoverText::~HoverText(){}

bool HoverText::Draw(wxDC &dc)
{
    int old_lf = dc.GetLogicalFunction();
    dc.SetLogicalFunction(wxXOR);

    wxPen old_pen = dc.GetPen();
    dc.SetPen(*wxBLACK_PEN);

    dc.DrawText(m_txt, m_pos);

    dc.SetLogicalFunction(old_lf);
    dc.SetPen(old_pen);

    return true;
}
void HoverText::UnDraw(wxDC &dc)
{
    this->Draw(dc);
}

