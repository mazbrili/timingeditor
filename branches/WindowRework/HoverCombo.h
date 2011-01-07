#ifndef HOVERCOMBO_H
#define HOVERCOMBO_H

#include "HoverDrawlet.h"


class HoverCombo : public HoverDrawlet
{
public:
    HoverCombo(HoverDrawlet *drwlta, HoverDrawlet *drwltb);
    virtual ~HoverCombo();
    virtual bool Draw(wxDC &dc);
    virtual void UnDraw(wxDC &dc);
private:
    HoverDrawlet *m_drwlta, *m_drwltb;
};

#endif // HOVERCOMBO_H
