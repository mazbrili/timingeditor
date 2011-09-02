#include "EditSignalDrawlet.h"

#include "GraphBusSignal.h"
#include "GraphNormalSignal.h"
#include "TimingDoc.h"
#include "TimingView.h"

EditSignalDrawlet::EditSignalDrawlet(Signal *sig, TimingView *view,  wxCoord yoffset):
m_sig(sig),
m_view(view),
m_yoffset(yoffset)
{
    //ctor
    if ( sig->IsBus )
        m_gsig = new GraphBusSignal(m_view, m_sig, m_yoffset);
    else
        m_gsig = new GraphNormalSignal(m_view, m_sig, m_yoffset);
}

EditSignalDrawlet::~EditSignalDrawlet()
{
    //dtor
    delete m_gsig;
    delete m_sig;
    if ( m_bitmap )
        delete m_bitmap;
}

bool EditSignalDrawlet::Draw(wxDC &dc)
{
    wxSize sz = dc.GetSize();
    if ( ! m_bitmap )
        delete m_bitmap;
    m_bitmap = new wxBitmap( sz.x, sz.y );

    // Create a memory DC
    wxMemoryDC temp_dc(&dc);
    temp_dc.SelectObject( *m_bitmap );

    temp_dc.Blit( 0, 0, sz.x, sz.y, &dc, 0, 0 );


    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    unsigned int height = m_sig->prespace + m_sig->space + doc->SignalHeight+doc->MinimumSignalDistance;
    dc.SetPen(*wxWHITE_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(0, m_yoffset, sz.x, height);

    m_gsig->Draw(dc);


    return true;
}
void EditSignalDrawlet::UnDraw(wxDC &dc)
{
    if (!m_bitmap)
        return;

    wxSize sz = dc.GetSize();

    // Create a memory DC
    wxMemoryDC temp_dc(&dc);
    temp_dc.SelectObject( *m_bitmap );

    dc.Blit( 0, 0, sz.x, sz.y, &temp_dc, 0, 0 );
}
