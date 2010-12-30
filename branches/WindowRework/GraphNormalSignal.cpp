#include "GraphNormalSignal.h"
#include "TimingView.h"
#include "TimingDoc.h"

GraphNormalSignal::GraphNormalSignal(TimingView *view, Signal *sig, wxCoord yoffset):
GraphSignal(view, sig, yoffset)
{
    //ctor
}

GraphNormalSignal::~GraphNormalSignal()
{
    //dtor
}

void GraphNormalSignal::Draw(wxDC &dc)const
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
                switch ( oldval )
                {
                    case zero:
                        dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h0);
                        break;
                    case one:
                        dc.DrawLine(offset.x   , offset.y,
                                    offset.x+w1, offset.y+h[4]);
                        break;
                    case hz:
                        dc.DrawLine(offset.x   , offset.y+h0/2,
                                    offset.x+w1, offset.y+h[5]);
                        break;
                    default:
                    case u:
                        {
                            dc.SetBrush(wxBrush(m_view->GetUndefinedSignalColour()));
                            dc.SetPen(wxPen(m_view->GetUndefinedSignalColour(),1));
                            wxPoint rec[4];
                            rec[0] = wxPoint(offset.x   , offset.y);
                            rec[1] = wxPoint(offset.x+w1, offset.y + h[4]);
                            rec[2] = wxPoint(offset.x+w1, offset.y + h0);
                            rec[3] = wxPoint(offset.x   , offset.y + h0);
                            dc.DrawPolygon(4, rec);
                            dc.SetBrush(wxNullBrush);
                            dc.SetPen(boldPen);
                        }
                        dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h0);
                        dc.DrawLine(offset.x   , offset.y,
                                    offset.x+w1, offset.y+h[4]);
                        break;
                    case dc1:
                    case dc2:
                        dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h0);
                        dc.DrawLine(offset.x   , offset.y,
                                    offset.x+w1, offset.y+h[4]);
                        break;
                }
                break;
            case one:
                switch ( oldval )
                {
                    case zero:
                        dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h[1]);
                        break;
                    case one:
                        dc.DrawLine(offset.x   , offset.y,
                                    offset.x+w1, offset.y);
                        break;
                    case hz:
                        dc.DrawLine(offset.x   , offset.y+h0/2,
                                    offset.x+w1, offset.y+h[0]);
                        break;
                    default:
                    case u:{
                            dc.SetBrush(wxBrush(m_view->GetUndefinedSignalColour()));
                            dc.SetPen(wxPen(m_view->GetUndefinedSignalColour(),1));
                            wxPoint rec[4];
                            rec[0] = wxPoint(offset.x   , offset.y+h0);
                            rec[1] = wxPoint(offset.x+w1, offset.y+h[1]);
                            rec[2] = wxPoint(offset.x+w1, offset.y);
                            rec[3] = wxPoint(offset.x   , offset.y);
                            dc.DrawPolygon(4, rec);
                            dc.SetBrush(wxNullBrush);
                            dc.SetPen(boldPen);
                        }
                        dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h[1]);
                        dc.DrawLine(offset.x   , offset.y,
                                    offset.x+w1, offset.y);
                        break;
                    case dc1:
                    case dc2:
                        dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h[1]);
                        dc.DrawLine(offset.x   , offset.y,
                                    offset.x+w1, offset.y);
                        break;
                }
                break;
            case hz:
                switch ( oldval )
                {
                    case zero:
                        dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h[3]);
                        break;
                    case one:
                        dc.DrawLine(offset.x   , offset.y,
                                    offset.x+w1, offset.y+h[2]);
                        break;
                    case hz:
                        dc.DrawLine(offset.x   , offset.y+h0/2,
                                    offset.x+w1, offset.y+h0/2);
                        break;
                    default:
                    case u:
                        {   dc.SetBrush(wxBrush(m_view->GetUndefinedSignalColour()));
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
                        dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h[3]);
                        dc.DrawLine(offset.x   , offset.y,
                                    offset.x+w1, offset.y+h[2]);
                        break;
                    case dc1:
                    case dc2:
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
                    case zero:
                        {
                            dc.SetBrush(wxBrush(m_view->GetUndefinedSignalColour()));
                            dc.SetPen(wxPen(m_view->GetUndefinedSignalColour(),1));
                            wxPoint rec[3];
                            rec[0] = wxPoint(offset.x   , offset.y+h0);
                            rec[1] = wxPoint(offset.x+w1, offset.y+h0);
                            rec[2] = wxPoint(offset.x+w1, offset.y+h[1]);
                            dc.DrawPolygon(3, rec);
                            dc.SetBrush(wxNullBrush);
                            dc.SetPen(boldPen);
                        }
                        dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h0);
                        dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h[1]);
                        break;
                    case one:
                        {
                            dc.SetBrush(wxBrush(m_view->GetUndefinedSignalColour()));
                            dc.SetPen(wxPen(m_view->GetUndefinedSignalColour(),1));
                            wxPoint rec[3];
                            rec[0] = wxPoint(offset.x   , offset.y);
                            rec[1] = wxPoint(offset.x+w1, offset.y);
                            rec[2] = wxPoint(offset.x+w1, offset.y+h[4]);
                            dc.DrawPolygon(3, rec);
                            dc.SetBrush(wxNullBrush);
                            dc.SetPen(boldPen);
                        }
                        dc.DrawLine(offset.x   , offset.y,
                                    offset.x+w1, offset.y+h[4]);
                        dc.DrawLine(offset.x   , offset.y,
                                    offset.x+w1, offset.y);
                        break;
                    case hz:
                        {
                            dc.SetBrush(wxBrush(m_view->GetUndefinedSignalColour()));
                            dc.SetPen(wxPen(m_view->GetUndefinedSignalColour(),1));
                            wxPoint rec[3];
                            rec[0] = wxPoint(offset.x   , offset.y+h0/2);
                            rec[1] = wxPoint(offset.x+w1, offset.y+h[5]);
                            rec[2] = wxPoint(offset.x+w1, offset.y+h[0]);
                            dc.DrawPolygon(3, rec);
                            dc.SetBrush(wxNullBrush);
                            dc.SetPen(boldPen);
                        }
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
                            wxPoint rec[4];
                            rec[0] = wxPoint(offset.x   , offset.y);
                            rec[1] = wxPoint(offset.x+w1, offset.y);
                            rec[2] = wxPoint(offset.x+w1, offset.y + h0);
                            rec[3] = wxPoint(offset.x   , offset.y + h0);
                            dc.DrawPolygon(4, rec);
                            dc.SetBrush(wxNullBrush);
                            dc.SetPen(boldPen);
                        }
                        dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h0);
                        dc.DrawLine(offset.x   , offset.y,
                                    offset.x+w1, offset.y);
                        break;
                    case dc1:
                    case dc2:
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
                        dc.DrawLine(offset.x   , offset.y ,
                                    offset.x+w1, offset.y+h[4]);
                        dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h[1]);

                        break;
                }
                break;
            case dc1:
            case dc2:
                switch( oldval )
                {
                    case one:
                        dc.DrawLine(offset.x   , offset.y,
                                    offset.x+w1, offset.y);
                        dc.DrawLine(offset.x   , offset.y,
                                    offset.x+w1, offset.y+h[4]);
                        break;
                    case zero:
                        dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h0);
                        dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h[1]);
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
                            rec[1] = wxPoint(offset.x+wo/2, offset.y+h0/2);
                            rec[2] = wxPoint(offset.x     , offset.y+h0);
                            dc.DrawPolygon(3, rec);
                            dc.SetBrush(wxNullBrush);
                            dc.SetPen(boldPen);
                        }
                        dc.DrawLine(offset.x   , offset.y,
                                    offset.x+w1, offset.y+h[4]);
                        dc.DrawLine(offset.x   , offset.y+h0,
                                    offset.x+w1, offset.y+h[1]);
                        break;
                    case dc1:
                    case dc2:
                        if ( val != oldval )
                        {
                            dc.DrawLine(offset.x   , offset.y,
                                        offset.x+w1, offset.y+h[4]);
                            dc.DrawLine(offset.x   , offset.y+h0,
                                        offset.x+w1, offset.y+h[1]);
                        }
                        else
                        {
                            dc.DrawLine(offset.x   , offset.y,
                                        offset.x+w1, offset.y);
                            dc.DrawLine(offset.x   , offset.y+h0,
                                        offset.x+w1, offset.y+h0);
                        }
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
            case one:
                dc.DrawLine(offset.x+w1, offset.y,
                            offset.x+w2, offset.y);
                break;
            case zero:
                dc.DrawLine(offset.x+w1, offset.y+h0,
                            offset.x+w2, offset.y+h0);
                break;
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
                dc.DrawLine(offset.x+w1, offset.y,
                            offset.x+w2, offset.y);
                dc.DrawLine(offset.x+w1, offset.y+h0,
                            offset.x+w2, offset.y+h0);
                break;
            case dc1:
            case dc2:
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
                case one:
                    dc.DrawLine(offset.x+w1, offset.y,
                                offset.x+w2, offset.y);
                    break;
                case zero:
                    dc.DrawLine(offset.x+w1, offset.y+h0,
                                offset.x+w2, offset.y+h0);
                    break;
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
                    dc.DrawLine(offset.x+w1, offset.y,
                                offset.x+w2, offset.y);
                    dc.DrawLine(offset.x+w1, offset.y+h0,
                                offset.x+w2, offset.y+h0);
                    break;
                case dc1:
                case dc2:
                    dc.DrawLine(offset.x+w1, offset.y,
                                offset.x+w2, offset.y);
                    dc.DrawLine(offset.x+w1, offset.y+h0,
                                offset.x+w2, offset.y+h0);
                    break;
            }
        }
        oldval = val;
        offset.x+= (m_view->GridStepWidth);
    }


    dc.SetPen(wxNullPen);
}

