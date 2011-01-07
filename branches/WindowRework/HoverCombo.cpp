#include "HoverCombo.h"

HoverCombo::HoverCombo(HoverDrawlet *drwlta, HoverDrawlet *drwltb):
m_drwlta(drwlta),
m_drwltb(drwltb)
{
    //ctor
}

HoverCombo::~HoverCombo()
{
    delete m_drwlta;
    delete m_drwltb;
}


bool HoverCombo::Draw(wxDC &dc)
{
    bool ret = m_drwlta->Draw(dc);
    ret = m_drwltb->Draw(dc) && ret;
    return ret;
}
void HoverCombo::UnDraw(wxDC &dc)
{
    m_drwltb->UnDraw(dc);
    m_drwlta->UnDraw(dc);
}
