#ifndef GRAPHVERTICALLINE_H
#define GRAPHVERTICALLINE_H

#include "GraphObject.h"


class GraphVerticalLine : public GraphObject
{
public:
    GraphVerticalLine(const wxPoint &start,const wxPoint &stop, const bool visible = true);
    virtual ~GraphVerticalLine();

    virtual void Draw(wxDC &dc)const;
protected:
private:
    bool m_visible;
    wxPoint m_start, m_stop;
};

#endif // GRAPHVERTICALLINE_H
