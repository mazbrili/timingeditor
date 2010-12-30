#include "GraphSignal.h"

GraphSignal::GraphSignal(TimingView *view, Signal *sig, wxCoord yoffset):
m_view(view),
m_sig(sig),
m_yoffset(yoffset)
{
    //ctor
}
GraphSignal::~GraphSignal()
{
    //dtor
}
void GraphSignal::DrawHighToLow(wxDC &dc)
{
}
void GraphSignal::DrawHighToTris(wxDC &dc)
{
}
void GraphSignal::DrawHighToDefined(wxDC &dc)
{
}
void GraphSignal::DrawHighToUndefined(wxDC &dc)
{
}
void GraphSignal::DrawLowToHigh(wxDC &dc)
{
}
void GraphSignal::DrawLowToTris(wxDC &dc)
{
}
void GraphSignal::DrawLowToDefined(wxDC &dc)
{
}
void GraphSignal::DrawLowToUndefined(wxDC &dc)
{
}
void GraphSignal::DrawTrisToHigh(wxDC &dc)
{
}
void GraphSignal::DrawTrisToLow(wxDC &dc)
{
}
void GraphSignal::DrawTrisToDefined(wxDC &dc)
{
}
void GraphSignal::DrawTrisToUndefined(wxDC &dc)
{
}
void GraphSignal::DrawUndefinedToHigh(wxDC &dc)
{
}
void GraphSignal::DrawUndefinedToLow(wxDC &dc)
{
}
void GraphSignal::DrawUndefinedToTris(wxDC &dc)
{
}
void GraphSignal::DrawUndefinedToDefined(wxDC &dc)
{
}
void GraphSignal::DrawDefinedToHigh(wxDC &dc)
{
}
void GraphSignal::DrawDefinedToLow(wxDC &dc)
{
}
void GraphSignal::DrawDefinedToTris(wxDC &dc)
{
}
void GraphSignal::DrawDefinedToUndefined(wxDC &dc)
{
}
void GraphSignal::DrawDefinedToDefined(wxDC &dc)
{
}
void GraphSignal::DrawHigh(wxDC &dc, bool fulllength)
{
}
void GraphSignal::DrawLow(wxDC &dc, bool fulllength)
{
}
void GraphSignal::DrawTris(wxDC &dc, bool fulllength)
{
}
void GraphSignal::DrawDefined(wxDC &dc, bool fulllength)
{
}
void GraphSignal::DrawUndefined(wxDC &dc, bool fulllength)
{
}
