
// Interface Dependencies ---------------------------------------------
#ifndef HoverCross_h
#define HoverCross_h

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "HoverDrawlet.h"


class HoverHatch : public HoverDrawlet
{
public:
enum expandDirection{
    expand_x,
    expand_y,
    expand_none
};

    HoverHatch(wxPoint pos, wxSize size, wxColour colour, expandDirection);
    virtual ~HoverHatch();


    virtual bool Draw(wxDC &dc);
    virtual void UnDraw(wxDC &dc);

private:
    wxPoint m_pos;
    wxSize m_size;
    wxColour m_colour;
    expandDirection m_dir;
};

#endif





