
// Interface Dependencies ---------------------------------------------
#ifndef HoverLine_h
#define HoverLine_h

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "HoverDrawlet.h"

// END Interface Dependencies -----------------------------------------

class HoverLine : public HoverDrawlet
{
public:
    HoverLine(wxPoint start, wxPoint stop, wxColour colour, int width = 1, int style = wxSOLID);
    HoverLine(int x, int y, wxColour colour, int width = 1, int style = wxSOLID);
    virtual ~HoverLine();


    virtual bool Draw(wxDC &dc);
    virtual void UnDraw(wxDC &dc);

private:
    wxPoint m_a, m_b;
    wxColour m_colour;
    int m_width;
    int m_style;
    enum expandDirection{
        expand_x,
        expand_y,
        expand_none
    };
    expandDirection m_dir;
};

#endif






