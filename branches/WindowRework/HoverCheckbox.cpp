#include "HoverCheckbox.h"


HoverCheckbox::HoverCheckbox(wxPoint pos, bool add):
HoverDrawlet(),
m_pos(pos),
m_add(add)
{
}
HoverCheckbox::~HoverCheckbox()
{

}

bool HoverCheckbox::Draw(wxDC &dc)
{
    int old_lf = dc.GetLogicalFunction();
    dc.SetLogicalFunction(wxXOR);

    wxPen old_pen = dc.GetPen();
    dc.SetPen(wxPen(*wxLIGHT_GREY));



    dc.DrawLine(m_pos             , m_pos+wxPoint(8,0));
    dc.DrawLine(m_pos+wxPoint(8,0), m_pos+wxPoint(8,8));
    dc.DrawLine(m_pos+wxPoint(8,8), m_pos+wxPoint(0,8));
    dc.DrawLine(m_pos+wxPoint(0,8), m_pos             );

    dc.DrawLine(m_pos+wxPoint(2,4), m_pos+wxPoint(7,4));
    if ( m_add )
        dc.DrawLine(m_pos+wxPoint(4,2), m_pos+wxPoint(4,7));


    dc.SetLogicalFunction(old_lf);
    dc.SetPen(old_pen);

    return true;
}

void HoverCheckbox::UnDraw(wxDC &dc)
{
    this->Draw(dc);
}

