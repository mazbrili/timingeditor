
// Interface Dependencies ---------------------------------------------
#ifndef HoverDrawlet_h
#define HoverDrawlet_h

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

// END Interface Dependencies -----------------------------------------

class HoverDrawlet
{
  public:
		HoverDrawlet(){}
		virtual ~HoverDrawlet(){}


		virtual bool Draw(wxDC &dc){return false;}
        virtual void UnDraw(wxDC &dc) = 0;

	private:
};

#endif



