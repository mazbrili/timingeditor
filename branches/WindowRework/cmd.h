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

#ifndef __CMD__
#define __CMD__

#include <wx/docview.h>
#include <wx/cmdproc.h>

#include "TimingDoc.h"

class DeleteVLineCommand : public wxCommand
{
public:
    DeleteVLineCommand(TimingDocument *doc, wxInt32 nmbr);
    ~DeleteVLineCommand();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    wxInt32 m_nmbr;
    VLine m_vline;
    std::vector<HArrow> m_harrows;
    bool first;
};
class ChangeLength : public wxCommand
{
public:
    ChangeLength(TimingDocument *doc, wxInt32 newLength);
    ~ChangeLength();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    bool m_shorten;
    wxInt32 m_newLength;
    std::vector< Signal > signals;
    std::map<wxInt32, TimeCompressor > compressors;
    std::vector<DeleteVLineCommand*> delVlineCom;
};
class ChangeLengthLeft : public wxCommand
{
public:
    ChangeLengthLeft(TimingDocument *doc, wxInt32 newLength);
    ~ChangeLengthLeft();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    bool m_shorten;
    wxInt32 m_newLength;
    std::vector< Signal > signals;
    std::map<wxInt32, TimeCompressor > compressors;

    std::vector<DeleteVLineCommand*> delVlineCom;
};
class ChangeSignal : public wxCommand
{
public:
    ChangeSignal(TimingDocument *doc, wxInt32 changingSigNr, Signal newSig);
    ~ChangeSignal();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    wxInt32 m_changingSigNr;
    Signal m_newSig;
};
class DeleteSignalCommand : public wxCommand
{
public:
    DeleteSignalCommand(TimingDocument *doc, wxInt32 deletedSigNr);
    ~DeleteSignalCommand();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    wxInt32 m_deletedSigNr;
    Signal m_sig;
    std::vector<VLine> vlines;
    std::vector<HArrow> harrows;
    std::vector<DeleteVLineCommand*> delVlineCom;
};
class ChangeClockParamCommand : public wxCommand
{
public:
    ChangeClockParamCommand(TimingDocument *doc, wxInt32 changingSigNr, wxInt32 per, wxInt32 delay, bool shadow, bool DrawPeriod);
    ~ChangeClockParamCommand();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    wxInt32 m_changingSigNr;
    wxInt32 m_newPer;
    wxInt32 m_newDelay;
    bool    m_shadow;
    bool    m_DrawPeriod;
};
class MoveSignalPosCommand : public wxCommand
{
public:
    MoveSignalPosCommand(TimingDocument *doc, wxInt32 selectedSigNr, wxInt32 targetPos);
    ~MoveSignalPosCommand();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    wxInt32 m_selectedSigNr;
    wxInt32 m_targetPos;
    std::vector<VLine> vlines;
    std::vector<HArrow> harrows;
    bool DoMove(void);
};
class AddSignalCommand : public wxCommand
{
public:
    AddSignalCommand(TimingDocument *doc, wxInt32 selectedSigNr, Signal sig);
    ~AddSignalCommand();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    wxInt32 m_selectedSigNr;
    Signal m_sig;
    std::vector<VLine> vlines;
    std::vector<HArrow> harrows;
};
class AddDiscontCommand : public wxCommand
{
public:
    AddDiscontCommand(TimingDocument *doc, wxInt32 discont);
    ~AddDiscontCommand();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    wxInt32 m_discont;
};
class RemoveDiscontCommand : public wxCommand
{
public:
    RemoveDiscontCommand(TimingDocument *doc, wxInt32 discont);
    ~RemoveDiscontCommand();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    wxInt32 m_pos;
    TimeCompressor m_cmprssr;
};
class ChangeSpaceCommand : public wxCommand
{
public:
    ChangeSpaceCommand(TimingDocument *doc, wxInt32 selectedSignal, wxInt32 newLength, bool upper);
    ~ChangeSpaceCommand();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    wxInt32 m_selectedSignal;
    wxInt32 m_newLength;
    bool m_upper;
    std::vector<HArrow> harrows;
    bool DoChangeSpace(void);
};
class AddVLineCommand : public wxCommand
{
public:
    AddVLineCommand(TimingDocument *doc, VLine newline);
    ~AddVLineCommand();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    VLine m_newline;
};
class AddHArrowCommand : public wxCommand
{
public:
    AddHArrowCommand(TimingDocument *doc, HArrow newha);
    ~AddHArrowCommand();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    HArrow m_newha;
};
class ChangeVLineCommand : public wxCommand
{
public:
    ChangeVLineCommand(TimingDocument *doc, wxInt32 nmbr, wxInt32 newVpos, wxInt32 newUpper, wxInt32 newLower, wxInt32 newVposoffset);
    ~ChangeVLineCommand();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    wxInt32 m_nmbr;
    wxInt32 m_newVpos;
    wxInt32 m_newUpper;
    wxInt32 m_newLower;
    wxInt32 m_newVposoff;
};
class ChangeHArrowCommand : public wxCommand
{
public:
    ChangeHArrowCommand(TimingDocument *doc, wxInt32 nmbr, wxInt32 pos, wxInt32 sigindex, wxInt32 newLeft, wxInt32 newRight);
    ~ChangeHArrowCommand();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    wxInt32 m_nmbr;
    wxInt32 m_newPos;
    wxInt32 m_newLeft;
    wxInt32 m_newRight;
    wxInt32 m_newPosIndex;
};
class DeleteHArrowCommand : public wxCommand
{
public:
    DeleteHArrowCommand(TimingDocument *doc, wxInt32 nmbr);
    ~DeleteHArrowCommand();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    wxInt32 m_nmbr;
    HArrow m_harrow;
};

class ChangeHArrowTextPosCommand : public wxCommand
{
public:
    ChangeHArrowTextPosCommand(TimingDocument *doc, wxInt32 editingNumber, wxInt32 xoff, wxInt32 yoff, wxInt32 gridoff);
    ~ChangeHArrowTextPosCommand();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    wxInt32 m_nmbr;
    wxPoint m_off;
    wxInt32 m_gridoff;
};

class ChangeSignalName : public wxCommand
{
public:
    ChangeSignalName(TimingDocument *doc, wxString newName, unsigned int SignalNumber);
    ~ChangeSignalName();
    bool Do();
    bool Undo();
protected:
    TimingDocument *m_doc;
    unsigned int m_signalNumber;
    wxString m_newText;
};
class ChangeSignalBuswidth : public wxCommand
{
public:
    ChangeSignalBuswidth(TimingDocument *doc, wxString newName, unsigned int SignalNumber);
    ~ChangeSignalBuswidth();
    bool Do();
    bool Undo();
protected:
    TimingDocument *m_doc;
    unsigned int m_signalNumber;
    wxString m_newText;
};
class ChangeTransitionWidth : public wxCommand
{
public:
    ChangeTransitionWidth(TimingDocument *doc, wxInt8 width, bool en50, bool en90);
    ~ChangeTransitionWidth();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    wxInt8 m_width;
    bool m_en50;
    bool m_en90;
};

class ChangeAxisSettings : public wxCommand
{
public:
    ChangeAxisSettings(TimingDocument *doc, wxInt8 unit, wxInt32 ticklenth, wxInt32 markerlength, wxInt32 offset, wxInt32 totallength);
    ~ChangeAxisSettings();
    bool Do(void);
    bool Undo(void);
private:
    void Change();
protected:
    TimingDocument *m_doc;
    wxInt8 m_unit;
    wxInt32 m_ticklength;
    wxInt32 m_markerlength;
    wxInt32 m_offset;
    wxCommand *AddRemoveTimeCommand;
};
class ChangeTimeCompressor : public wxCommand
{
public:
    ChangeTimeCompressor(TimingDocument *doc, wxInt32 index, wxInt32 time, bool en);
    ChangeTimeCompressor(TimingDocument *doc, wxInt32 index, wxInt32 time);
    ChangeTimeCompressor(TimingDocument *doc, wxInt32 index, bool en);
    ~ChangeTimeCompressor();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    wxInt32 m_time;
    wxInt32 m_index;
    bool    m_en;
};

class AddTimeCommand : public wxCommand
{
public:
    AddTimeCommand(TimingDocument *doc, wxInt32 pos, wxInt32 len);
    ~AddTimeCommand();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    wxInt32 m_pos;
    wxInt32 m_len;
};
class RemoveTimeCommand : public wxCommand
{
public:
    RemoveTimeCommand(TimingDocument *doc, wxInt32 beg, wxInt32 end);
    ~RemoveTimeCommand();
    bool Do(void);
    bool Undo(void);
protected:
    TimingDocument *m_doc;
    wxInt32 m_beg;
    wxInt32 m_end;
    std::vector<Signal> m_signals;
    std::map<wxInt32, TimeCompressor> m_compressors;

    std::vector<DeleteVLineCommand*> delVlineCom;
};
#endif //__CMD__
