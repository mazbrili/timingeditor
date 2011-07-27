#ifndef GRAPHHORIZONTALARROW_H
#define GRAPHHORIZONTALARROW_H

#include "GraphObject.h"


class HoverDrawlet;
class GraphHorizontalArrow : public GraphObject
{
public:
    GraphHorizontalArrow(const wxPoint &start, const wxPoint &stop, const wxString str, const wxPoint stroffset, const bool visible = true);
    virtual ~GraphHorizontalArrow();

    void Draw(wxDC &dc)const;

    bool HasPoint(const wxPoint &pos, const unsigned char tolerance)const;
    bool IsStartPoint(const wxPoint &pos, const unsigned char tolerance)const;
    bool IsStopPoint(const wxPoint &pos, const unsigned char tolerance)const;
    bool IsTextPoint(const wxPoint &pos, const unsigned char tolerance)const;
    //HoverDrawlet *GetLineDrawlet(const wxColour &colour, int style)const;
    const wxPoint &GetStartPoint()const{return m_start;}
    const wxPoint &GetStopPoint()const{return m_stop;}
    const wxPoint &GetTextPoint()const{return m_textoffset;}
protected:
private:
    bool m_visible;
    wxString m_text;
    wxPoint m_start, m_stop;

    wxPoint m_textoffset;
};

#endif // GRAPHHORIZONTALARROW_H
