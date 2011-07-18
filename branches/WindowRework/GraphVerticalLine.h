#ifndef GRAPHVERTICALLINE_H
#define GRAPHVERTICALLINE_H

#include "GraphObject.h"

class HoverDrawlet;
class GraphVerticalLine : public GraphObject
{
public:
    GraphVerticalLine(const wxPoint &start,const wxPoint &stop, const bool visible = true);
    virtual ~GraphVerticalLine();

    virtual void Draw(wxDC &dc)const;


    bool HasPoint(const wxPoint &pos, const unsigned char tolerance)const;
    bool IsStartPoint(const wxPoint &pos, const unsigned char tolerance)const;
    bool IsStopPoint(const wxPoint &pos, const unsigned char tolerance)const;

    HoverDrawlet *GetActiveDrawlet(const wxColour &colour)const;
    HoverDrawlet *GetLineDrawlet(const wxColour &colour, int style)const;
    const wxPoint &GetStartPoint()const;

protected:
private:
    bool m_visible;
    wxPoint m_start, m_stop;
};

#endif // GRAPHVERTICALLINE_H
