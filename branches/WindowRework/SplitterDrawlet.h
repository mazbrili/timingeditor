#ifndef SPLOTTERDRAWLET_H
#define SPLOTTERDRAWLET_H

#include "HoverDrawlet.h"

class wxBitmap;

class SplitterDrawlet : public HoverDrawlet
{
public:
    SplitterDrawlet(unsigned int dist, unsigned int gap);
    virtual ~SplitterDrawlet();
    virtual bool Draw(wxDC &dc);
    virtual void UnDraw(wxDC &dc);
    enum {splitX, splitY};
private:
    wxBitmap *m_bitmap;
    unsigned int m_dist;
    unsigned int m_gap;
};

#endif // SPLOTTERDRAWLET_H

