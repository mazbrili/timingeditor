#ifndef GRAPHNORMALSIGNAL_H
#define GRAPHNORMALSIGNAL_H

#include "GraphSignal.h"


class GraphNormalSignal : public GraphSignal
{
    public:
        GraphNormalSignal(TimingView *view, Signal *sig, wxCoord yoffset);
        virtual ~GraphNormalSignal();

        virtual void Draw(wxDC &dc)const;
        virtual void DrawBackground(wxDC &dc)const{}
    protected:
    private:
};

#endif // GRAPHNORMALSIGNAL_H
