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

#ifndef __DOC_H__
#define __DOC_H__

#include <wx/docview.h>
#include <wx/cmdproc.h>
#include <wx/mstream.h>



#include <vector>
#include <set>
#include <map>

//#include "cmdproc.h"

// #include visitor
class wxDataOutputStream;
class wxDataInputStream;

enum vals
{
    zero,
    one,
    hz,
    dc1,
    dc2,
    u
};
class Signal
{
public:
    std::vector<vals> values;
    bool IsClock;
    bool IsBus;
    bool GenerateBackground;
    bool ShowPeriodCount;
    // infos about the clock (if it is a clock)
    wxInt32 ticks; // of a half clock period
    wxInt32 delay; // of the clock in ticks


    // graphical information
    wxInt32 space;
    wxInt32 prespace;

    wxString name;
    wxString buswidth;

    std::map<int, wxString> TextValues;

    bool serialize(wxOutputStream &store);
    bool deserialize(wxInputStream &load);
};
//class VLine;
class HArrow
{
public:
    wxInt32 fromVLine;
    wxInt32 toVLine;
    wxInt32 pos;
    wxInt32 signalnmbr;
    wxString text;
    wxPoint  textoffset;
    wxInt32  textgridoffset;

    bool serialize(wxDataOutputStream &store);
    bool deserialize(wxDataInputStream &load);
};
class VLine
{
public:
    wxInt32 StartPos; // above signal number
    wxInt32 EndPos; // below signal number
    wxInt32 vpos;
    wxUint8 vposoffset;
    //std::vector<HArrow *> referencedByHArrows;
    bool serialize(wxDataOutputStream &store);
    bool deserialize(wxDataInputStream &load);
};
class SplArrow
{
public:
    wxPoint StartPoint; // not in pixel coords!!!!
    wxPoint EndPos;
    wxString text;
    wxPoint textoffset;
    bool serialize(wxDataOutputStream &store);
    bool deserialize(wxDataInputStream &load);
};

class TimeCompressor
{
    public:
        wxInt32 pos;
        wxInt32 length;
        wxInt32 enabled;
};

class TimingDocument: public wxDocument
{
    public:
        TimingDocument(void);
        ~TimingDocument(void);

    public:
        bool IsReadOnly(void){return m_readOnly;}

        void SetText(wxInt32 number, wxString text);
        wxString GetText(wxInt32 number);

    private:
        bool m_readOnly;
    public:
        //std::vector<SplArrow> splarrows;
        wxInt32 length;
        std::vector<Signal> signals;
        std::vector<VLine> vertlines;
        std::vector<HArrow> harrows;
        std::vector<TimeCompressor> compressors;

        wxInt32 SignalHeight;
        wxInt32 MinimumSignalDistance;

        wxUint8 TransitWidth; // width in % of the width of a tick
        bool    en50;
        bool    en90;

        wxInt8  TickLengthUnit;// 1: ks  // 0: s    // -1: ms    // -2: us    // -3: ns    // -4: ps    // -5: fs
        wxInt32 TickLength; // in TickLengthUnit
        wxInt32 TackLength; // in ticks
        wxInt32 timeOffset; // in ticks

    protected:
        bool DoSaveDocument(const wxString& file);
	    bool DoOpenDocument(const wxString& file);
        virtual bool OnNewDocument();

    protected:
        DECLARE_DYNAMIC_CLASS(TimingDocument)
};

#endif















