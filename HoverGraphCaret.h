#ifndef HOVERGRAPHCARET_H
#define HOVERGRAPHCARET_H

#include "HoverDrawlet.h"


class HoverGraphCaret : public HoverDrawlet
{
public:
    HoverGraphCaret(wxPoint pos, wxSize size, wxColour colour);
    virtual ~HoverGraphCaret();

    virtual bool Draw(wxDC &dc);
    virtual void UnDraw(wxDC &dc);
protected:
private:
    wxPoint  m_pos;
    wxSize   m_size;
    wxColour m_colour;
    wxBitmap *m_pBitmap;
};

#endif // HOVERGRAPHCARET_H
