
// Interface Dependencies ---------------------------------------------
#ifndef HoverCross_h
#define HoverCross_h

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "HoverDrawlet.h"

// END Interface Dependencies -----------------------------------------

class HoverCross : public HoverDrawlet
{
public:
    HoverCross(wxPoint center);
    virtual ~HoverCross();


    virtual bool Draw(wxDC &dc);
    virtual void UnDraw(wxDC &dc);

private:
    wxPoint m_center;
};

#endif





