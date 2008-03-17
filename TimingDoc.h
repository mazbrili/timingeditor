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
    u
};
class Signal
{
public:
    std::vector<vals> values;
    bool IsClock;
    bool IsBus;
    // infos about the clock (if it is a clock)
    wxInt32 ticks; // of a half clock period
    wxInt32 delay; // of the clock in ticks


    // graphical information
    wxInt32 space;
    wxInt32 prespace;

    wxString name;
    wxString buswidth;

    std::map<int, wxString> TextValues;

    bool serialize(wxDataOutputStream &store);
    bool deserialize(wxDataInputStream &load);
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
    wxPoint textoffset;

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
class TimingDocument: public wxDocument
{
    public:
        TimingDocument(void);
        ~TimingDocument(void);

//        #if wxUSE_STD_IOSTREAM
//        wxSTD ostream& SaveObject(wxSTD ostream& text_stream);
//        wxSTD istream& LoadObject(wxSTD istream& text_stream);
//        #else
//        wxOutputStream& SaveObject(wxOutputStream& stream);
//        wxInputStream& LoadObject(wxInputStream& stream);
//        #endif
    public:
        bool IsReadOnly(void){return m_readOnly;}

    private:
        bool m_readOnly;

    public:
        //wxArrayString SignalNames;
        wxInt32 length;
        std::vector<Signal> signals;
        std::set<wxInt32> discontinuities;
        std::vector<VLine> vertlines;
        std::vector<HArrow> harrows;
        //std::vector<SplArrow> splarrows;

        wxInt32 SignalHeight;
        wxInt32 MinimumSignalDistance;

        wxUint8 TransitWidth; // width in percent of the width of a tick
        bool    en5090;

    protected:
        bool DoSaveDocument(const wxString& file);
	    bool DoOpenDocument(const wxString& file);

    //private:
    //virtual TimingCommandProcessor* OnCreateCommandProcessor();
    //virtual MyCommandProcessor* OnCreateCommandProcessor();

    protected:
        DECLARE_DYNAMIC_CLASS(TimingDocument)
};

/*class DrawingCommand: public wxCommand
{
 protected:
  DoodleSegment *segment;
  DrawingDocument *doc;
  int cmd;
 public:
  DrawingCommand(const wxString& name, int cmd, DrawingDocument *ddoc, DoodleSegment *seg);
  ~DrawingCommand(void);

  bool Do(void);
  bool Undo(void);
};*/

#endif















