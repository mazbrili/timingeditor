#include "HoverGraphCaret.h"

HoverGraphCaret::HoverGraphCaret(wxPoint pos, wxSize size, wxColour colour):
m_pos(pos),
m_size(size),
m_colour(colour)
{
    //ctor
}

HoverGraphCaret::~HoverGraphCaret()
{
    //dtor
}

bool HoverGraphCaret::Draw(wxDC &dc)
{
    //save state of dc
    wxPen old_pen = dc.GetPen();
    dc.SetPen(m_colour);
    wxBrush old_brush = dc.GetBrush();
    dc.SetBrush(m_colour);

    //create memory dc with appropriate size
    m_pBitmap = new wxBitmap(m_size.GetWidth(), m_size.GetHeight());
    wxMemoryDC memdc(*m_pBitmap);
    if ( !memdc.IsOk() )
    {
        dc.SetPen(old_pen);
        dc.SetBrush(old_brush);
        delete m_pBitmap;
        return false;
    }

    memdc.Clear();
    memdc.Blit(wxPoint(0,0),m_size, &dc, m_pos);

    dc.DrawRectangle(m_pos, m_size);

    dc.SetPen(old_pen);
    dc.SetBrush(old_brush);
    return true;
}
void HoverGraphCaret::UnDraw(wxDC &dc)
{
    wxMemoryDC memdc(*m_pBitmap);
    dc.Blit(m_pos, m_size, &memdc, wxPoint(0, 0) );

    delete m_pBitmap;
}
