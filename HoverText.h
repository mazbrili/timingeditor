
// Interface Dependencies ---------------------------------------------
#ifndef HoverText_h
#define HoverText_h

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "HoverDrawlet.h"

// END Interface Dependencies -----------------------------------------

class HoverText : public HoverDrawlet
{
public:
    HoverText(wxString txt, wxPoint pos);
    virtual ~HoverText();


    virtual bool Draw(wxDC &dc);
    virtual void UnDraw(wxDC &dc);

private:
    wxString m_txt;
    wxPoint m_pos;
};

#endif





