//    Copyright 2008 Daniel Anselmi
//
//    This file is part of TimingEditor.
//
//    TimingEditor is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    TimingEditor is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with TimingEditor.  If not, see <http://www.gnu.org/licenses/>.
//
//    Contact e-mail: daniel anselmi <danselmi@gmx.ch>
//    Program URL   : http://sourceforge.net/projects/timingeditor/
//
//
#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif


#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/wfstream.h>
//#include <wx/txtstrm.h>
#include <wx/datstrm.h>

#include "TimingDoc.h"
//#include "TimingApp.h" /// wxGetApp()
#include "enumers.h"


IMPLEMENT_DYNAMIC_CLASS(TimingDocument, wxDocument)
TimingDocument::TimingDocument(void)
    :m_readOnly( false )
{
    length = 24;
    SignalHeight = 20;
    MinimumSignalDistance = 10;

    VLine line;

    line.StartPos = 0;
    line.EndPos= 1;
    line.vpos = 1;
    vertlines.push_back(line);

    line.StartPos = 0;
    line.EndPos = 1;
    line.vpos = 15;
    vertlines.push_back(line);


    HArrow ar;
    ar.fromVLine = 0;
    ar.toVLine = 1;
    ar.text = _T("t_1");
    ar.textoffset = wxPoint(0, -10);
    ar.signalnmbr = 0;
    ar.pos = 10;
    harrows.push_back(ar);

    /*std::vector<HArrow> horiarrows;*/

    /*SplArrow sarr;
    sarr.StartPoint = wxPoint(3,0); // not in pixel coords!!!!
    sarr.EndPos = wxPoint(8,1);
    sarr.text = _T("it belongs to the arrow");
    sarr.textoffset = wxPoint(20,20);

    splarrows.push_back(sarr);*/

    Signal sig1;
    {
    sig1.IsClock = true;
    sig1.IsBus = false;
    sig1.ticks = 3;
    sig1.delay = 0;
    sig1.space = 0;
    sig1.prespace = 10;
    sig1.name = _T("Clk");
    signals.push_back(sig1);
    }

    discontinuities.insert(3);

    Signal sig2;
    {
    sig2.IsClock = false;
    sig2.IsBus = false;
    sig2.name = _T("Rst");
    for ( wxInt32 n = 0 ; n < 4 ; ++n )
        sig2.values.push_back(one);
    for ( wxInt32 n = 0 ; n < 20 ; ++n )
    sig2.values.push_back(zero);

    sig2.space = 40;
    sig2.prespace = 0;
    signals.push_back(sig2);
    }

    /*Signal sig3;
    {
    sig3.IsClock = false;
    sig3.IsBus = true;
    sig3.name = _T("TickCount_n");
    sig3.values.push_back(zero);
    sig3.values.push_back(zero);
    sig3.values.push_back(zero);
    sig3.values.push_back(zero);
    sig3.values.push_back(hz);
    sig3.values.push_back(hz);
    sig3.values.push_back(hz);
    sig3.values.push_back(hz);
    sig3.values.push_back(one);
    sig3.values.push_back(one);
    sig3.values.push_back(one);
    sig3.values.push_back(one);
    sig3.values.push_back(zero);
    sig3.values.push_back(zero);
    sig3.values.push_back(zero);
    sig3.values.push_back(zero);
    sig3.values.push_back(u);
    sig3.values.push_back(u);

    sig3.values.push_back(u);
    sig3.values.push_back(u);
    sig3.values.push_back(u);
    sig3.values.push_back(one);
    sig3.values.push_back(one);
    sig3.values.push_back(one);
    sig3.TextValues[0]  = _("HH");
    sig3.TextValues[8]  = _("HA");
    sig3.TextValues[12] = _("HG");
    sig3.TextValues[21] = _("HQ");
//    sig3.TextValues.push_back(_("HH"));
//    sig3.TextValues.push_back(_("HA"));
//    sig3.TextValues.push_back(_("HG"));
//    sig3.TextValues.push_back(_("HQ"));
    sig3.space = 0;
    sig3.prespace = 0;

    signals.push_back(sig3);
    }*/
}
TimingDocument::~TimingDocument(void){}


//wxOutputStream& TimingDocument::SaveObject(wxOutputStream& stream)
//{
//    wxDocument::SaveObject(stream);
//    /// put your code to save here
//    return stream;
//}
//
//wxInputStream& TimingDocument::LoadObject(wxInputStream& stream)
//{
//    wxDocument::LoadObject(stream);
//
//    return stream;
//}
bool TimingDocument::DoSaveDocument(const wxString& file)
{
    wxFileOutputStream outp( file );
    wxDataOutputStream store( outp );

    wxInt32 i;
    wxUint32 ui;

    /// store file format version first
    i = 2;
    store << i;

    ///
    store << length;

    ui = signals.size();
    store << ui;
    for ( wxUint32 n = 0 ; n < ui ; ++n )
    {
        if ( !(signals[n].serialize(store)) )
            return false;
    }

    ui = vertlines.size();
    store << ui;
    for ( wxUint32 n = 0 ; n < ui ; ++n )
    {
        if ( !(vertlines[n].serialize(store)) )
            return false;
    }

    ui = harrows.size();
    store << ui;
    for ( wxUint32 n = 0 ; n < ui ; ++n )
    {
        if ( !(harrows[n].serialize(store)) )
            return false;
    }

    ui = discontinuities.size();
    store << ui;
    std::set<wxInt32>::iterator it;
    for ( it = discontinuities.begin() ; it != discontinuities.end() ; it++)
    {
        i = *it;
        store << i;
    }

    // new with version 2:
    store << SignalHeight;
    store << MinimumSignalDistance;


    return outp.Close();
}

bool TimingDocument::DoOpenDocument(const wxString& file)
{
    wxFileInputStream inp( file );
    wxDataInputStream load( inp );

    wxInt32 i;
    wxUint32 ui;

    /// get file format version first
    load >> i;
    if ( i == 1 || i == 2 )
    {
        load >> length;

        signals.clear();
        load >> ui;
        for ( wxUint32 n = 0 ; n < ui ; ++n )
        {
            Signal sig;
            if ( !(sig.deserialize( load )) )
                return false;
            signals.push_back(sig);
        }

        vertlines.clear();
        load >> ui;
        for ( wxUint32 n = 0 ; n < ui ; ++n )
        {
            VLine vln;
            if ( !(vln.deserialize( load )) )
                return false;
            vertlines.push_back(vln);
        }


        harrows.clear();
        load >> ui;
        for ( wxUint32 n = 0 ; n < ui ; ++n )
        {
            HArrow har;
            if ( !(har.deserialize( load )) )
                return false;
            harrows.push_back(har);
        }

        discontinuities.clear();
        load >> ui;
        for ( wxUint32 n = 0 ; n < ui ; ++n )
        {
            wxInt32 dis;
            load >> dis;
            discontinuities.insert(dis);
        }
        SignalHeight = 20;
        MinimumSignalDistance = 10;
    }

    if ( i == 2)
    {
        wxInt32 height;
        load >> height;
        SignalHeight = height;

        wxInt32 dist;
        load >> dist;
        MinimumSignalDistance = dist;
    }

    if ( i > 2)
    {
        // unknown format version
        return false;
    }



    return true;
}

//class TimingCommandProcessor;
//TimingCommandProcessor* TimingDocument::OnCreateCommandProcessor()
//{
//    return new TimingCommandProcessor();
//}

/*bool TimingDocument::OnOpenDocument(const wxString& filename)
{
    return true;
}*/
bool Signal::serialize(wxDataOutputStream &store)
{
    wxInt32 i;

    /// version
    i = 2;
    store << i;
    store << name;
    store << prespace;
    store << space;

    wxUint8 ui8;
    if ( IsClock )
        ui8 = 0;
    else if ( !IsBus )
        ui8 = 1;
    else
        ui8 = 2;

    store << ui8;

    if ( ui8 == 0 )
    {
        i = ticks;
        store << i;

        i = delay;
        store << i;
    }
    else
    {
        wxUint32 k = values.size();
        store << k;
        wxUint8 v;
        for ( wxUint32 n = 0 ; n < k ; ++n )
        {
            v = values[n];
            switch ( v )
            {
                case zero: v = 0; break;
                case one: v = 1; break;
                case hz: v = 2; break;
                default:
                case u: v = 3; break;
            }
            store << v;
        }

        if ( ui8 == 2 )
        {
            //TextValues;
            wxUint32 nmbr = 0;
            for ( wxUint32 n = 0 ; n < k ; ++n )
            {
                wxUint8 v = values[n];
                if ( v == zero || v == one )
                    if ( n == 0 || values[n] != values[n-1] )
                        ++nmbr;
            }
            store << nmbr;
            for ( wxUint32 n = 0 ; n < k ; ++n )
            {
                wxUint8 v = values[n];
                if ( v == zero || v == one )
                    if ( n == 0 || values[n] != values[n-1] )
                    {
                        store << n;
                        store << TextValues[n];
                    }
            }
            store << buswidth;
        }
    }


    return true;
}
bool Signal::deserialize(wxDataInputStream &load)
{
    wxInt32 i, ver;

    load >> ver;
    if ( ver <= 2 )
    {
        values.clear();
        TextValues.clear();

        load >> name;
        load >> prespace;
        load >> space;

        wxUint8 ui8;
        load >> ui8;
        if ( ui8 == 0 ) // clock
        {
            IsClock = true;
            IsBus = false;
            load >> i;
            ticks = i;

            load >> i;
            delay = i;
        }
        else
        {
            IsClock = false;
            IsBus = false;
            wxUint32 k;
            load >> k;

            wxUint8 v;
            for ( wxUint32 n = 0 ; n < k ; ++n )
            {
                load >> v;
                switch ( v )
                {
                    case 0: values.push_back(zero); break;
                    case 1: values.push_back(one); break;
                    case 2: values.push_back(hz); break;
                    default:
                    case 3: values.push_back(u); break;
                }
            }
            if ( ui8 == 2 )
            {
                IsBus = true;
                wxUint32 nmbr;
                load >> nmbr;
                for ( wxUint32 n = 0 ; n < nmbr ; ++n )
                {
                    wxUint32 t;
                    wxString str;
                    load >> t;
                    load >> str;
                    TextValues[t] = str;
                }
                if (ver >= 2 )
                {
                    load >> buswidth;
                }
            }
        }
        return true;
    }
    else
        return false; // wrong format version
}
bool VLine::serialize(wxDataOutputStream &store)
{
    wxInt32 i;

    /// version
    i = 1;
    store << i;

    store << EndPos;
    store << StartPos;
    store << vpos;
    return true;
}
bool VLine::deserialize(wxDataInputStream &load)
{
    wxInt32 ver;

    load >> ver;
    if ( ver == 1 )
    {
        load >> EndPos;
        load >> StartPos;
        load >> vpos;
        return true;
    }
    else
        return false; // wrong format version
}
bool HArrow::serialize(wxDataOutputStream &store)
{
    wxInt32 i;

    /// version
    i = 2;
    store << i;

    store << fromVLine;
    store << toVLine;
    store << pos;
    store << text;

    i = textoffset.x;
    store << i;
    i = textoffset.y;
    store << i;

    store << signalnmbr;

    return true;
}
bool HArrow::deserialize(wxDataInputStream &load)
{
    wxInt32 ver;

    load >> ver;
    if ( ver == 1 || ver == 2 )
    {
        wxInt32 k;
        load >> fromVLine;
        load >> toVLine;
        load >> pos;
        load >> text;
        load >> k;
            textoffset.x = k;
        load >> k;
            textoffset.y = k;
        signalnmbr = 0;
    }
    if ( ver == 2 )
    {
        load >> signalnmbr;
    }

    if ( ver > 2 ) return false; // wrong format version

    return true;
}
