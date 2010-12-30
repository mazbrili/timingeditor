#ifndef GRAPHSIGNAL_H
#define GRAPHSIGNAL_H

#include "GraphObject.h"

class TimingView;
class Signal;
class GraphSignal : public GraphObject
{
    public:
        GraphSignal(TimingView *view, Signal *sig, wxCoord yoffset); //offset = m_view->heightOffsets[n]+
        virtual ~GraphSignal();

        virtual void Draw(wxDC &dc)const=0;
        virtual void DrawBackground(wxDC &dc)const=0;
    protected:
        TimingView *m_view;
        Signal *m_sig;
        wxCoord m_yoffset;

    protected:
        void DrawHighToLow(wxDC &dc);
        void DrawHighToTris(wxDC &dc);
        void DrawHighToDefined(wxDC &dc);
        void DrawHighToUndefined(wxDC &dc);

        void DrawLowToHigh(wxDC &dc);
        void DrawLowToTris(wxDC &dc);
        void DrawLowToDefined(wxDC &dc);
        void DrawLowToUndefined(wxDC &dc);

        void DrawTrisToHigh(wxDC &dc);
        void DrawTrisToLow(wxDC &dc);
        void DrawTrisToDefined(wxDC &dc);
        void DrawTrisToUndefined(wxDC &dc);

        void DrawUndefinedToHigh(wxDC &dc);
        void DrawUndefinedToLow(wxDC &dc);
        void DrawUndefinedToTris(wxDC &dc);
        void DrawUndefinedToDefined(wxDC &dc);

        void DrawDefinedToHigh(wxDC &dc);
        void DrawDefinedToLow(wxDC &dc);
        void DrawDefinedToTris(wxDC &dc);
        void DrawDefinedToUndefined(wxDC &dc);
        void DrawDefinedToDefined(wxDC &dc);

        void DrawEdge(wxDC &dc);

        void DrawHigh(wxDC &dc, bool fulllength = false);
        void DrawLow(wxDC &dc, bool fulllength = false);
        void DrawTris(wxDC &dc, bool fulllength = false);
        void DrawDefined(wxDC &dc, bool fulllength = false);
        void DrawUndefined(wxDC &dc, bool fulllength = false);

    protected:
    private:
};

#endif // GRAPHSIGNAL
