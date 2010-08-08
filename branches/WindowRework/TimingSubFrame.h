#ifndef TIMINGSUBFRAME_H
#define TIMINGSUBFRAME_H

#include <wx/docmdi.h>

class TimingSubFrame : public wxDocMDIChildFrame
{
    public:
        TimingSubFrame(wxDocument *doc, wxView *view, wxMDIParentFrame *parent);
        virtual ~TimingSubFrame(){}
    protected:
    private:
};

#endif // TIMINGSUBFRAME_H
