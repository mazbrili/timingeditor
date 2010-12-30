#ifndef GRAPHHORIZONTALARROW_H
#define GRAPHHORIZONTALARROW_H

#include "GraphObject.h"


class GraphHorizontalArrow : public GraphObject
{
public:
    GraphHorizontalArrow(const wxPoint &start, const wxPoint &stop, const wxString str, const wxPoint stroffset, const bool visible = true);
    virtual ~GraphHorizontalArrow();

    void Draw(wxDC &dc)const;

protected:
private:
    bool m_visible;
    wxString m_text;
    wxPoint m_start, m_stop;

    wxPoint m_textoffset;
};

#endif // GRAPHHORIZONTALARROW_H
