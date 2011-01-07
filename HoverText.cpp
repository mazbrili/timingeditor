
#include "HoverText.h"


HoverText::HoverText(wxString txt, wxPoint pos, wxColour colour)
:HoverDrawlet(),
m_txt(txt),
m_pos(pos),
m_colour(colour),
m_pBitmap(NULL)
{}
HoverText::~HoverText()
{
    if (m_pBitmap)
        delete m_pBitmap;
}

bool HoverText::Draw(wxDC &dc)
{
    //save state of dc
    wxPen old_pen = dc.GetPen();
    dc.SetPen(m_colour);

    //create memory dc with appropriate size
    wxSize size = dc.GetTextExtent(m_txt);
    if (m_pBitmap)delete m_pBitmap;
    m_pBitmap = new wxBitmap(size.GetWidth(), size.GetHeight());
    wxMemoryDC memdc(*m_pBitmap);
    if ( !memdc.IsOk() )
    {
        dc.SetPen(old_pen);
        delete m_pBitmap;
        m_pBitmap = NULL;
        return false;
    }

    memdc.Clear();

    memdc.Blit(wxPoint(0,0),size, &dc, m_pos);

    dc.DrawText(m_txt, m_pos);

    dc.SetPen(old_pen);
    return true;
}
void HoverText::UnDraw(wxDC &dc)
{
    wxMemoryDC memdc(*m_pBitmap);
    dc.Blit(m_pos, wxSize(m_pBitmap->GetWidth(),m_pBitmap->GetHeight()), &memdc, wxPoint(0, 0) );

    delete m_pBitmap;
    m_pBitmap = NULL;
}

