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
#include <wx/txtstrm.h>
#include <wx/datstrm.h>

#include "TimingDoc.h"
//#include "TimingApp.h" /// wxGetApp()
#include "enumers.h"


IMPLEMENT_DYNAMIC_CLASS(TimingDocument, wxDocument)
TimingDocument::TimingDocument(void)
    :m_readOnly( false )
{
    length = 25;
    SignalHeight = 20;
    TransitWidth = 30; // %
    MinimumSignalDistance = 10;
    en50 = true;
    en90 = true;

    TickLengthUnit = -3;
    TickLength = 10; // in TickLengthUnit
    TackLength = 10; // in ticks
    timeOffset = -2; // in ticks

    Signal sig1;
    {
    sig1.IsClock = true;
    sig1.GenerateBackground = true;
    sig1.IsBus = false;
    sig1.ticks = 4;
    sig1.delay = 6;
    sig1.space = 10;
    sig1.prespace = 10;
    sig1.name = _T("Clk");
    signals.push_back(sig1);
    }
}
TimingDocument::~TimingDocument(void){}

bool TimingDocument::DoSaveDocument(const wxString& file)
{
    wxFileOutputStream outp( file );
    wxDataOutputStream store( outp );

    wxInt32 i;
    wxUint32 ui;

    /// store file format version first
    i = 5;
    store << i;

    ///
    store << length;

    ui = signals.size();
    store << ui;
    for ( wxUint32 n = 0 ; n < ui ; ++n )
    {
        if ( !(signals[n].serialize(outp)) )
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

    ui = compressors.size(); //ui = discontinuities.size();
    store << ui;
    //std::set<wxInt32>::iterator it;
    //for ( it = discontinuities.begin() ; it != discontinuities.end() ; it++)
    for ( wxUint32 n = 0 ; n < ui ; ++n )
    {
        wxInt32 t;

        //i = *it;
        //store << i;
        t = compressors[n].pos;
        store << t;

        //store << discontlength[i];
        t = compressors[n].length;
        store << t;

        wxUint8 en = 0;
        //if ( discontEn[i] ) en = 1;
        if ( compressors[n].enabled ) en = 1;
        store << en;
    }

    // new with version 2:
    store << SignalHeight;
    store << MinimumSignalDistance;

    // new with version 3:
    store << TransitWidth;

    // new with version 4:
    store << TickLengthUnit;
    store << TickLength; // in TickLengthUnit
    store << TackLength; // in ticks
    store << timeOffset; // in ticks

    // version 5
    ui = 0;
    if ( en50 ) ui |= 0x01;
    if ( en90 ) ui |= 0x02;
        store << ui;

    Modify(false);
    UpdateAllViews();

    return outp.Close();
}

bool TimingDocument::DoOpenDocument(const wxString& file)
{
    wxFileInputStream inp( file );
    wxDataInputStream load( inp );

    //wxInt32 i;
    wxInt32  version;
    wxUint32 ui;

    SetFilename(file, true);

    /// get file format version first
    load >> version;
    if ( version >= 1 )
    {
        load >> length;

        signals.clear();
        load >> ui;
        for ( wxUint32 n = 0 ; n < ui ; ++n )
        {
            Signal sig;
            if ( !(sig.deserialize( inp )) )
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

        //discontinuities.clear();
        //discontlength.clear();
        //discontEn.clear();
        compressors.clear();
        load >> ui;
        for ( wxUint32 n = 0 ; n < ui ; ++n )
        {
            TimeCompressor cmprssr;

            //wxInt32 dis;
            //load >> dis;
            //discontinuities.insert(dis);
            wxInt32 pos;
            load >> pos;
            cmprssr.pos = pos;

            if ( version >= 4 )
            {
                wxInt32 l;
                load >> l;
                //discontlength[dis] = l;
                cmprssr.length = l;
            }
            else
                //discontlength[dis] = 2;
                cmprssr.length = 2;
            if ( version >= 5 )
            {
                wxUint8 en;
                load >> en;
                if (en == 0)
                    //discontEn[dis] = false;
                    cmprssr.enabled = false;
                else
                    //discontEn[dis] = true;
                    cmprssr.enabled = true;
            }
            else
                //discontEn[dis] = true;
                cmprssr.enabled = true;

            compressors.push_back(cmprssr);
        }
    }

    if ( version >= 2)
    {
        wxInt32 height;
        load >> height;
        SignalHeight = height;

        wxInt32 dist;
        load >> dist;
        MinimumSignalDistance = dist;
    }
    else
    {
        SignalHeight = 20;
        MinimumSignalDistance = 10;
    }

    if ( version >= 3)
    {
        load >> TransitWidth;
    }
    else
    {
        TransitWidth = 20;
    }

    if ( version >= 4 )
    {
        load >> TickLengthUnit;
        load >> TickLength;
        load >> TackLength;
        load >> timeOffset;
    }
    else
    {
        TickLengthUnit = -3;
        TickLength = 10; // in TickLengthUnit
        TackLength = 10; // in ticks
        timeOffset = 5; // in ticks
    }

    if ( version >= 5 )
    {
        wxUint32 ui;
        load >> ui;
        en50 = false;
        en90 = false;
        if ( ui & 0x01 ) en50 = true;
        if ( ui & 0x02 ) en90 = true;
    }
    else
    {
        en50 = true;
        en90 = true;
    }

    if ( version > 5)
    {
        // unknown format version
        wxMessageDialog dlg(NULL,
            _T("File format is unknown.\nUpdate to a newer version of TimingEditor please."),
            _T("Format unknown"),
            wxOK);
            dlg.ShowModal();

        return false;
    }
    return true;
}

bool Signal::serialize(wxOutputStream &outp)
{
    wxDataOutputStream store( outp );

    wxInt32 i;

    /// version
    i = 4;
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
                case dc1 : v = 4; break;
                case dc2 : v = 5; break;
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

    ui8 = 0;
    if ( GenerateBackground )
        ui8 = 1;
    store << ui8;


    return true;
}

bool Signal::deserialize(wxInputStream &inp)
{
    wxDataInputStream load( inp );
    wxInt32 i, ver;
    GenerateBackground = false;

    load >> ver;
    if ( ver >= 1 )
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
                    case 4: values.push_back(dc1); break;
                    case 5: values.push_back(dc2); break;
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
    }
    if ( ver >= 3 )
    {
        wxUint8 ui8;
        load >> ui8;
        if ( ui8 != 0 )
            GenerateBackground = true;
    }
    if (ver >= 4 )
    {} //-- signals with dc1 and dc 2 state
    if ( ver > 4)
        return false; // wrong format version

    return true;
}
bool VLine::serialize(wxDataOutputStream &store)
{
    wxInt32 i;

    /// version
    i = 2;
    store << i;

    store << EndPos;
    store << StartPos;
    store << vpos;
    store << vposoffset;
    return true;
}
bool VLine::deserialize(wxDataInputStream &load)
{
    wxInt32 ver;

    load >> ver;
    if ( ver >= 1 )
    {
        load >> EndPos;
        load >> StartPos;
        load >> vpos;
    }
    else
    {
        vposoffset = 0;
    }
    if ( ver >= 2 )
    {
        load >> vposoffset;
    }


    if ( ver > 2 ) return false; // wrong format version

    return true;
}
bool HArrow::serialize(wxDataOutputStream &store)
{
    wxInt32 i;

    /// version
    i = 3;
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

    i = textgridoffset;
    store << i;

    return true;
}
bool HArrow::deserialize(wxDataInputStream &load)
{
    wxInt32 ver;

    load >> ver;
    if ( ver >= 1 )
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
    }

    if ( ver >= 2 )
        load >> signalnmbr;
    else
        signalnmbr = 0;

    if ( ver >= 3 )
        load >> textgridoffset;
    else
        textgridoffset = 0;

    if ( ver > 3 ) return false; // wrong format version

    return true;
}

bool TimingDocument::OnNewDocument()
{
    bool res = wxDocument::OnNewDocument();
    // hack to let the view update the title of the child frame
    // filename and title of document are valid after OnNewDocument() has been called
    // is only a problem on wxGTK not wxMSW don't hurt on wxMSW
    UpdateAllViews();
    return res;
}
wxString TimingDocument::GetText(wxInt32 number)
{
    wxInt32 n = 0;
    for ( wxUint32 k = 0 ; k < signals.size() ; ++k )
    {
        if( n == number )
            return signals[k].name;
        ++n;
        if ( signals[k].IsBus )
        {
            if ( n == number )
                return signals[k].buswidth;
            ++n;
        }
    }
    for ( wxUint32 k = 0 ; k < signals.size() ; ++k)
        if ( !signals[k].IsClock && signals[k].IsBus )
            for ( wxUint32 i = 0 ; i < (wxUint32)length ; ++i )
                if ( signals[k].values[i] == one ||
                     signals[k].values[i] == zero )
                    if ( i == 0 || signals[k].values[i] != signals[k].values[i-1] )
                    {
                        if ( n == number )
                            return signals[k].TextValues[i];
                        ++n;
                    }
    for ( wxUint32 k = 0 ; k < harrows.size() ; ++k, n++ )
        if ( n == number )
            return harrows[k].text;

    return wxEmptyString;
}
void TimingDocument::SetText(wxInt32 number, wxString text)
{
    wxInt32 n = 0;
    for ( wxUint32 k = 0 ; k < signals.size() ; ++k )
    {
        if( n == number )
        {
            signals[k].name = text;
            return;
        }
        ++n;
        if ( signals[k].IsBus )
        {
            if ( n == number )
            {
                signals[k].buswidth = text;
                return;
            }
            ++n;
        }
    }
    for ( wxUint32 k = 0 ; k < signals.size() ; ++k)
        if ( !signals[k].IsClock && signals[k].IsBus )
            for ( wxUint32 i = 0 ; i < (wxUint32)length ; ++i )
                if ( signals[k].values[i] == one ||
                     signals[k].values[i] == zero )
                    if ( i == 0 || signals[k].values[i] != signals[k].values[i-1] )
                    {
                        if ( n == number )
                        {
                            signals[k].TextValues[i] = text;
                            return;
                        }
                        ++n;
                    }
    for ( wxUint32 k = 0 ; k < harrows.size() ; ++k, n++ )
        if ( n == number )
        {
            harrows[k].text = text;
            return;
        }
}

