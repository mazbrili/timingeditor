
// Interface Dependencies ---------------------------------------------
#ifndef HoverCheckbos_h
#define HoverCheckbos_h

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "HoverDrawlet.h"

// END Interface Dependencies -----------------------------------------

class HoverCheckbox : public HoverDrawlet
{
public:
    HoverCheckbox(wxPoint pos, bool add = true);
    virtual ~HoverCheckbox();


    virtual bool Draw(wxDC &dc);
    virtual void UnDraw(wxDC &dc);

private:
    wxPoint m_pos;
    bool m_add;
};

#endif






