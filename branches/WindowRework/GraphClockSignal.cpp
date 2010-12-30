#include "GraphClockSignal.h"
#include "TimingView.h"
#include "TimingDoc.h"

GraphClockSignal::GraphClockSignal(TimingView *view, Signal *sig, wxCoord yoffset):
GraphSignal(view, sig, yoffset)
{
    //ctor
}

GraphClockSignal::~GraphClockSignal()
{
    //dtor
}


void GraphClockSignal::DrawBackground(wxDC &dc)const
{
    /// drawing background of clock signal
    dc.SetFont(m_view->GetFont());
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(m_view->GetShadowColour());
    std::vector<wxUint32> positions;
    std::vector<wxString> texts;

    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();

    wxPoint offset;
    offset.x = m_view->GetWavesLeftSpace();
    if ( m_sig->GenerateBackground ) // is clock with background generation on?
    {
        if ( m_sig->ShowPeriodCount )
        {
            positions.clear();
            texts.clear();
        }
        // drawing the background if needed
        wxInt32 n = 0;
        for (wxInt32 k = -((4*m_sig->ticks)- (m_sig->delay % (4*m_sig->ticks))) ; k < 0 ; ++k )
            if ( ++n == 4*m_sig->ticks )
                n = 0;
        for (wxUint32 k = 0; k < m_view->VisibleTicks.size()-1 ; )
        {
            bool hascompressor = false;
            wxInt32 pos = m_view->VisibleTicks[k];
            if ( doc->compressors.find(pos) != doc->compressors.end() && doc->compressors[pos].enabled )
            {
                if ( n < 2*m_sig->ticks )// drawing a bar in the first half of the tick
                    dc.DrawRectangle( offset.x, 0,
                        (m_view->GridStepWidth)/2, m_view->heightOffsets[m_view->heightOffsets.size()-1]
                    );

                wxInt32 len = (doc->compressors[pos].length) % (4*m_sig->ticks);
                n += len - 2;
                if ( n < 0 ) n += (4*m_sig->ticks);
                n %= (4*m_sig->ticks);
                if ( n < 2*m_sig->ticks )// drawing a bar over the second half of the tick
                    dc.DrawRectangle(
                        offset.x + (m_view->GridStepWidth)/2, 0,
                        (m_view->GridStepWidth)/2+1  , m_view->heightOffsets[m_view->heightOffsets.size()-1]
                    );
                ++n;
                hascompressor = true;
            }
            if ( !hascompressor );
            {
                if ( n < 2*m_sig->ticks ){// drawing a bar over the whole tick
                    dc.DrawRectangle( offset.x, 0,
                        (m_view->GridStepWidth)+1, m_view->heightOffsets[m_view->heightOffsets.size()-1]
                    );
                }
                if ( m_sig->ShowPeriodCount &&
                    (n == m_sig->ticks || n == 3*m_sig->ticks))
                {
                    wxUint32 per = m_view->VisibleTicks[k]/2/m_sig->ticks+1;
                    positions.push_back(offset.x);
                    texts.push_back( wxString::Format(_T("%d"),per) );
                    //dc.DrawText( , , heightOffsets[0]);
                }
                ++n;
            }
            n %= (4*m_sig->ticks);
            offset.x += (m_view->GridStepWidth);
            ++k;
        }
    }
    for ( wxUint32 n = 0; n < positions.size() ; ++n )
        dc.DrawText( texts[n] , positions[n], 0);
    dc.SetPen(wxNullPen);
    dc.SetBrush(wxNullBrush);
}
void GraphClockSignal::Draw(wxDC &dc)const
{
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();

    wxPoint offset;
    offset.x = m_view->GetWavesLeftSpace();
    offset.y = m_yoffset + doc->MinimumSignalDistance/2+ m_sig->prespace;

    wxPen boldPen(m_view->GetLineColour(), 2);
    dc.SetPen(boldPen);

    wxInt32 n = 0;
    for (wxInt32 k = -((2*m_sig->ticks)- (m_sig->delay % (2*m_sig->ticks)))
     ; k < 0 ; ++k ) if ( ++n == 2*m_sig->ticks ) n = 0;
    if ( n == 0 || n == m_sig->ticks ) // draw an edge at the left of the diagram
        dc.DrawLine(
            offset.x, offset.y + doc->SignalHeight,
            offset.x, offset.y
        );
    for (wxUint32 k = 0; k < m_view->VisibleTicks.size()-1 ; )
    {
        bool hascompressor = false;
        wxInt32 pos = m_view->VisibleTicks[k];
        if( doc->compressors.find(pos) != doc->compressors.end() && doc->compressors[pos].enabled)
        {
            if ( n < m_sig->ticks )// drawing a one
                dc.DrawLine(
                    offset.x                  , offset.y,
                    offset.x + (m_view->GridStepWidth)/2, offset.y
                );
            else// drawing a zero
                dc.DrawLine(
                    offset.x                  , offset.y + doc->SignalHeight,
                    offset.x + (m_view->GridStepWidth)/2, offset.y + doc->SignalHeight
                );

            wxInt32 len = (doc->compressors[pos].length) % (2*m_sig->ticks);
            n += len -1;
            if ( n < 0 ) n += (2*m_sig->ticks);
            n %= (2*m_sig->ticks);
            if ( n < m_sig->ticks )// drawing a one
                dc.DrawLine(
                    offset.x + (m_view->GridStepWidth)/2, offset.y,
                    offset.x + (m_view->GridStepWidth)  , offset.y
                );
            else// drawing a zero
                dc.DrawLine(
                    offset.x + (m_view->GridStepWidth)/2, offset.y + doc->SignalHeight,
                    offset.x + (m_view->GridStepWidth)  , offset.y + doc->SignalHeight
                );
            ++n;
            hascompressor = true;
        }
        if ( !hascompressor )
        {
            if ( n < m_sig->ticks )// drawing a one
                dc.DrawLine(
                    offset.x                , offset.y,
                    offset.x + (m_view->GridStepWidth), offset.y
                );
            else// drawing a zero
                dc.DrawLine(
                    offset.x                , offset.y + doc->SignalHeight,
                    offset.x + (m_view->GridStepWidth), offset.y + doc->SignalHeight
                );
            ++n;
        }

        if( n == m_sig->ticks || n == 2*m_sig->ticks ) // draw the edge
            dc.DrawLine(
                offset.x + (m_view->GridStepWidth), offset.y + doc->SignalHeight,
                offset.x + (m_view->GridStepWidth), offset.y
            );
        n %= (2*m_sig->ticks);
        offset.x += (m_view->GridStepWidth);
        ++k;
    }
    dc.SetPen(wxNullPen);
}
