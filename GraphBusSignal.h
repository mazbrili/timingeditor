#ifndef GRAPHBUSSIGNAL_H
#define GRAPHBUSSIGNAL_H

#include "GraphSignal.h"


class GraphBusSignal : public GraphSignal
{
    public:
        GraphBusSignal(TimingView *view, Signal *sig, wxCoord yoffset);
        virtual ~GraphBusSignal();

        virtual void Draw(wxDC &dc)const;
        virtual void DrawBackground(wxDC &dc)const{}
    protected:
    private:
};

#endif // GRAPHBUSSIGNAL_H
