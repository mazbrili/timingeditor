#ifndef EDITSIGNALDRAWLET_H
#define EDITSIGNALDRAWLET_H

#include "HoverDrawlet.h"

class Signal;
class GraphSignal;
class TimingView;
class wxBitmap;

class EditSignalDrawlet : public HoverDrawlet
{
public:
    EditSignalDrawlet(Signal *sig, TimingView *view,  wxCoord yoffset);
    virtual ~EditSignalDrawlet();
    virtual bool Draw(wxDC &dc);
    virtual void UnDraw(wxDC &dc);
protected:
private:
    Signal *m_sig;
    GraphSignal *m_gsig;
    TimingView *m_view;
    wxCoord m_yoffset;
    wxBitmap *m_bitmap;
};

#endif // EDITSIGNALDRAWLET_H
