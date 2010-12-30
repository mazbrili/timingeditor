#include "GraphBusSignal.h"
#include "TimingView.h"
#include "TimingDoc.h"

GraphBusSignal::GraphBusSignal(TimingView *view, Signal *sig, wxCoord yoffset):
GraphSignal(view, sig, yoffset)
{
    //ctor
}

GraphBusSignal::~GraphBusSignal()
{
    //dtor
}

void GraphBusSignal::Draw(wxDC &dc)const
{
    vals oldval = m_sig->values[0];// first tick can not start with a transition
    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();

    wxPoint offset;
    offset.x = m_view->GetWavesLeftSpace();
    offset.y = m_yoffset + doc->MinimumSignalDistance/2+ m_sig->prespace;

    wxPen boldPen(m_view->GetLineColour(), 2);
    dc.SetPen(boldPen);

    //{
    wxCoord wo = (m_view->GridStepWidth)/(100.0/doc->TransitWidth);
    wxCoord wt = (m_view->GridStepWidth)/2;
    wxCoord ho[6] = {0,0,doc->SignalHeight/2,doc->SignalHeight/2,doc->SignalHeight,doc->SignalHeight};
    wxCoord ht[6];
    ht[4] = ho[5]*50/doc->TransitWidth;
    ht[1] = ho[5]-ht[4];
    ht[2] = ht[4]/2;
    ht[3] = ho[5]-ht[2];
    ht[5] = ht[2]+ho[5]/2;
    ht[0] = ho[5]-ht[5];//}

    for (wxUint32 k = 0; k < m_view->VisibleTicks.size()-1 ; ++k)
    {
        wxInt32 tick = m_view->VisibleTicks[k];
        wxInt32 len = 1;
        vals val = m_sig->values[tick];
        if ( doc->compressors.find(tick) != doc->compressors.end() && doc->compressors[tick].enabled )
            len = doc->compressors[tick].length;

        /// drawing the transitions:
        wxCoord w1 = wo;
        wxCoord h[6] = {ho[0], ho[1], ho[2], ho[3], ho[4], ho[5]};
        wxCoord h0 = ho[5];
        if ( len > 1 && doc->TransitWidth >= 50 ) // time compressor and transition "breiter" than the half tick
        {
            w1 = wt;
            for ( int i = 0 ; i < 6 ; i++) h[i] = ht[i];
        }
        switch(val)
        {
            case zero:
            case one:
                switch ( oldval )
                {
                    case zero:
                    case one:
                        if ( oldval != val )
                        {
                            dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h[1]);
                            dc.DrawLine(offset.x   , offset.y,
                                    offset.x+w1, offset.y+h[4]);
                        }
                        else
                        {
                            dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h0);
                            dc.DrawLine(offset.x   , offset.y,
                                    offset.x+w1, offset.y);
                        }
                        break;
                    case hz:
                        dc.DrawLine(offset.x   , offset.y+h0/2,
                                    offset.x+w1, offset.y+h[5]);
                        dc.DrawLine(offset.x   , offset.y+h0/2,
                                    offset.x+w1, offset.y+h[0]);
                        break;
                    default:
                    case u:
                        {
                            dc.SetBrush(wxBrush(m_view->GetUndefinedSignalColour()));
                            dc.SetPen(wxPen(m_view->GetUndefinedSignalColour(),1));
                            wxPoint rec[3];
                            rec[0] = wxPoint(offset.x     , offset.y);
                            rec[1] = wxPoint(offset.x+wo/2, offset.y + h0/2);
                            rec[2] = wxPoint(offset.x     , offset.y + h0);
                            dc.DrawPolygon(3, rec);
                            dc.SetBrush(wxNullBrush);
                            dc.SetPen(boldPen);
                        }
                        dc.DrawLine(offset.x   , offset.y,
                                    offset.x+w1, offset.y+h[4]);
                        dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h[1]);
                        break;
                }
                break;
            case hz:
                switch ( oldval )
                {
                    case hz:
                        dc.DrawLine(offset.x   , offset.y+h0/2,
                                    offset.x+w1, offset.y+h0/2);
                        break;
                    default:
                    case u:
                        {
                            dc.SetBrush(wxBrush(m_view->GetUndefinedSignalColour()));
                            dc.SetPen(wxPen(m_view->GetUndefinedSignalColour(),1));
                            wxPoint rec[4];
                            rec[0] = wxPoint(offset.x   , offset.y);
                            rec[1] = wxPoint(offset.x+w1, offset.y + h[2]);
                            rec[2] = wxPoint(offset.x+w1, offset.y + h[3]);
                            rec[3] = wxPoint(offset.x   , offset.y + h0);
                            dc.DrawPolygon(4, rec);
                            dc.SetBrush(wxNullBrush);
                            dc.SetPen(boldPen);
                        }
                        // no break here
                    case zero:
                    case one:
                        dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h[3]);
                        dc.DrawLine(offset.x   , offset.y,
                                    offset.x+w1, offset.y+h[2]);
                        break;
                }
                break;
            default:
            case u:
                switch ( oldval )
                {
                    case hz:
                        {
                            dc.SetBrush(wxBrush(m_view->GetUndefinedSignalColour()));
                            dc.SetPen(wxPen(m_view->GetUndefinedSignalColour(),1));
                            wxPoint rec[3];
                            rec[0] = wxPoint(offset.x+w1, offset.y +h[0]);
                            rec[1] = wxPoint(offset.x+w1, offset.y +h[5]);
                            rec[2] = wxPoint(offset.x   , offset.y +h0/2);
                            dc.DrawPolygon(3,rec);
                            dc.SetBrush(wxNullBrush);
                            dc.SetPen(boldPen);
                        }
                        dc.DrawLine(offset.x   , offset.y +h0/2,
                                    offset.x+w1, offset.y +h[0]);
                        dc.DrawLine(offset.x   , offset.y +h0/2,
                                    offset.x+w1, offset.y +h[5]);
                        break;
                    default:
                    case u:
                        {
                            dc.SetBrush(wxBrush(m_view->GetUndefinedSignalColour()));
                            dc.SetPen(wxPen(m_view->GetUndefinedSignalColour(),1));
                            wxPoint rec[4];
                            rec[0] = wxPoint(offset.x   , offset.y);
                            rec[1] = wxPoint(offset.x+w1, offset.y);
                            rec[2] = wxPoint(offset.x+w1, offset.y + h0);
                            rec[3] = wxPoint(offset.x   , offset.y + h0);
                            dc.DrawPolygon(4, rec);
                            dc.SetBrush(wxNullBrush);
                            dc.SetPen(boldPen);
                        }
                        dc.DrawLine(offset.x, offset.y, offset.x+w1, offset.y);
                        dc.DrawLine(offset.x, offset.y+h0, offset.x+w1, offset.y+h0);
                        break;
                    case zero:
                    case one:
                        {
                            dc.SetBrush(wxBrush(m_view->GetUndefinedSignalColour()));
                            dc.SetPen(wxPen(m_view->GetUndefinedSignalColour(),1));
                            wxPoint rec[3];
                            rec[0] = wxPoint(offset.x+w1  , offset.y+h[1]);
                            rec[1] = wxPoint(offset.x+wo/2, offset.y+h0/2);
                            rec[2] = wxPoint(offset.x+w1  , offset.y+h[4]);
                            dc.DrawPolygon(3, rec);
                            dc.SetBrush(wxNullBrush);
                            dc.SetPen(boldPen);
                        }
                        dc.DrawLine(offset.x, offset.y   , offset.x+w1, offset.y+h[4]);
                        dc.DrawLine(offset.x, offset.y+h0, offset.x+w1, offset.y+h[1]);
                        break;
                }
                break;
        }

        /// drawing the state
        wxCoord w2 = (m_view->GridStepWidth);
        if ( len > 1 )
            w2 /= 2;
        if ( len == 1 || doc->TransitWidth < 50 ) switch ( val )
        {
            case hz:
                dc.DrawLine(offset.x+w1, offset.y+h0/2,
                            offset.x+w2, offset.y+h0/2);
                break;
            default:
            case u:
                {
                    dc.SetBrush(wxBrush(m_view->GetUndefinedSignalColour()));
                    dc.SetPen(wxPen(m_view->GetUndefinedSignalColour(),1));
                    wxPoint rec[4];
                    rec[0] = wxPoint(offset.x+w1, offset.y);
                    rec[1] = wxPoint(offset.x+w2, offset.y);
                    rec[2] = wxPoint(offset.x+w2, offset.y+h0);
                    rec[3] = wxPoint(offset.x+w1, offset.y+h0);
                    dc.DrawPolygon(4, rec);
                    dc.SetBrush(wxNullBrush);
                    dc.SetPen(boldPen);
                }
            case one:
            case zero:
                dc.DrawLine(offset.x+w1, offset.y,
                            offset.x+w2, offset.y);
                dc.DrawLine(offset.x+w1, offset.y+h0,
                            offset.x+w2, offset.y+h0);
                break;
        }
        if ( len > 1 )
        {
            w1 = (m_view->GridStepWidth)/2;
            w2 = (m_view->GridStepWidth);
            val = m_sig->values[m_view->VisibleTicks[k+1]-1];
            switch ( val )
            {
                case hz:
                    dc.DrawLine(offset.x+w1, offset.y+h0/2,
                                offset.x+w2, offset.y+h0/2);
                    break;
                default:
                case u:
                    {
                        dc.SetBrush(wxBrush(m_view->GetUndefinedSignalColour()));
                        dc.SetPen(wxPen(m_view->GetUndefinedSignalColour(),1));
                        wxPoint rec[4];
                        rec[0] = wxPoint(offset.x+w1, offset.y);
                        rec[1] = wxPoint(offset.x+w2, offset.y);
                        rec[2] = wxPoint(offset.x+w2, offset.y+h0);
                        rec[3] = wxPoint(offset.x+w1, offset.y+h0);
                        dc.DrawPolygon(4, rec);
                        dc.SetBrush(wxNullBrush);
                        dc.SetPen(boldPen);
                    }
                case one:
                case zero:
                    dc.DrawLine(offset.x+w1, offset.y+h0,
                                offset.x+w2, offset.y+h0);
                    dc.DrawLine(offset.x+w1, offset.y,
                                offset.x+w2, offset.y);
                    break;

            }
        }
        oldval = val;
        offset.x += (m_view->GridStepWidth);
    }

    //drawing the texts
    offset.x = m_view->GetWavesLeftSpace();
    oldval = m_sig->values[0];
    for ( wxUint32 k = 0; k < m_view->VisibleTicks.size()-1 ; ++k )
    {
        wxInt32 tick = m_view->VisibleTicks[k];
        vals val = m_sig->values[tick];
        wxInt32 invislen = 0;

        bool hascompressor = false;
        for ( wxUint32 indx = 0 ; indx < doc->compressors.size() ; ++indx)
        if ( doc->compressors.find(tick) != doc->compressors.end() && doc->compressors[tick].enabled )
        {
            invislen = doc->compressors[tick].length;
            for ( wxInt32 i = 0 ; i < invislen ; ++i )
            {
                wxInt32 ti = tick + i;
                val = m_sig->values[ti];
                if ( ( ti == 0 || (m_sig->values[ti-1] != val)) && (val == one || val == zero ))
                    dc.DrawText(m_sig->TextValues[ti], offset);
            }
            hascompressor = true;
            break;
        }
        if ( !hascompressor )
        {
            if ( (tick == 0 || oldval != val) && (val == one || val == zero ))
            {
                wxPoint textoff(
                    wo + offset.x + k*(m_view->GridStepWidth),
                    offset.y + doc->SignalHeight/2 -
                    dc.GetCharHeight()/2
                );
                //bool viz = true;
                //if ( textoff.y + dc.GetCharHeight()/2 < 0) viz = false;
                //if ( textoff.x                        < m_view->GetWavesLeftSpace() - 5 ) viz = false;
                dc.DrawText(m_sig->TextValues[tick], textoff);//DrawEditableText(dc, sig.TextValues[tick], textoff, viz);
            }
        }
        oldval = val;
    }

    dc.SetPen(wxNullPen);
}

