#ifndef GRAPHCLOCKSIGNAL_H
#define GRAPHCLOCKSIGNAL_H

#include "GraphSignal.h"


class GraphClockSignal : public GraphSignal
{
    public:
        GraphClockSignal(TimingView *view, Signal *sig, wxCoord yoffset);
        virtual ~GraphClockSignal();

        virtual void Draw(wxDC &dc)const;
        virtual void DrawBackground(wxDC &dc)const;

    protected:
    private:
};

#endif // GRAPHCLOCKSIGNAL_H
