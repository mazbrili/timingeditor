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

#include "TimingDoc.h"
#include "cmd.h"



ChangeLength::ChangeLength(TimingDocument *doc, wxInt32 newLength)
    :wxCommand(true, _T("changing length of time axis")),
    m_doc(doc),
    m_newLength(newLength)
{
}
ChangeLength::~ChangeLength()
{
    DeleteVerticalLineCommand *dvlc;
    for (wxUint32 k = 0 ; k < deleteVerticalLineCommands.size() ; ++k )
    {
        dvlc = deleteVerticalLineCommands[k];
        if ( dvlc ) delete dvlc;
    }
}
bool ChangeLength::Do(void)
{
    compressors.clear();
    compressors = m_doc->compressors;
    for ( std::map<wxInt32, TimeCompressor>::iterator it = m_doc->compressors.begin() ; it != m_doc->compressors.end() ; it++ )
    {
        if ( it->first > m_newLength )
            m_doc->compressors.erase(it);
    }

    signals.clear();
    signals = m_doc->signals;
    for ( wxUint32 n = 0 ; n < signals.size() ; ++n )
    {
        if ( !signals[n].IsClock )
        {
            m_doc->signals[n].values.clear();
            if ( m_newLength < m_doc->length )
            {
                for ( wxInt32 k = 0 ; k < m_newLength ; ++k )
                {
                    m_doc->signals[n].values.push_back(signals[n].values[k]);
                }
            }
            wxInt32 k = 0;
            for (; k < m_doc->length ; ++k )
            {
                m_doc->signals[n].values.push_back(signals[n].values[k]);
            }
            vals val = signals[n].values[k-1];
            for ( ; k < m_newLength ; ++ k)
            {
                m_doc->signals[n].values.push_back(val);
            }
        }
    }

    deleteVerticalLineCommands.clear();
    if ( m_newLength < m_doc->length)
    {
        //std::vector<VerticalLine>::iterator it;
        wxUint32 k;
        //it = m_doc->verticalLines.begin();
        for (  k = 0 ; k < m_doc->verticalLines.size(); ++k )
        {
            if ( m_doc->verticalLines[k].vpos > m_newLength )
            {
                DeleteVerticalLineCommand *cmd = new DeleteVerticalLineCommand(m_doc, k);
                deleteVerticalLineCommands.push_back(cmd);
                cmd->Do();
                break;
            }
        }
    }


    wxInt32 tmp = m_doc->length;
    m_doc->length = m_newLength;
    m_newLength = tmp;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool ChangeLength::Undo(void)
{

    m_doc->compressors = compressors;

    //m_doc->signals.clear();
    m_doc->signals = signals;

    while ( deleteVerticalLineCommands.size() )
    {
        DeleteVerticalLineCommand *cmd = deleteVerticalLineCommands.back();
        cmd->Undo();
        delete cmd;
        deleteVerticalLineCommands.pop_back();

    }

    wxInt32 tmp = m_doc->length;
    m_doc->length = m_newLength;
    m_newLength = tmp;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}



ChangeSignal::ChangeSignal(TimingDocument *doc, wxInt32 changingSigNr, Signal newSig)
    :wxCommand(true, _T("changing signal values")),
    m_doc(doc),
    m_changingSigNr(changingSigNr),
    m_newSig(newSig)
{
//    if ( !m_newSig.IsClock && m_newSig.IsBus )
//    {
//        wxInt32 k = 0;
//        for ( wxInt32 n = 0 ; n < m_newSig.values.size() ; ++n )
//            if ( m_newSig.values[n] == one || m_newSig.values[n] == zero )
//                if ( n == 0 || m_newSig.values[n-1] != m_newSig.values[n])
//                    k++;
        //while ( k < m_newSig.TextValues.size() )
        //    m_newSig.TextValues.pop_back();
//    }
}

ChangeSignal::~ChangeSignal(){}
bool ChangeSignal::Do(void)
{
    Signal tmp = m_doc->signals[m_changingSigNr];
    m_doc->signals[m_changingSigNr] = m_newSig;
    m_newSig = tmp;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool ChangeSignal::Undo(void)
{
    return Do();
}

DeleteSignalCommand::DeleteSignalCommand(TimingDocument *doc, wxInt32 deletedSigNr)
    :wxCommand(true, _T("deleting signal")),
    m_doc(doc),
    m_deletedSigNr(deletedSigNr)
{
}
DeleteSignalCommand::~DeleteSignalCommand()
{
    DeleteVerticalLineCommand *dvlc;
    for (wxUint32 k = 0 ; k < deleteVerticalLineCommands.size() ; ++k )
    {
        dvlc = deleteVerticalLineCommands[k];
        if ( dvlc ) delete dvlc;
    }
}
bool DeleteSignalCommand::Do(void)
{
    // delete vertical lines which are only connected to the deleted signal
    deleteVerticalLineCommands.clear();
    for (wxUint32  k = 0 ; k < m_doc->verticalLines.size(); ++k )
    {
        if ( m_doc->verticalLines[k].EndPos == m_deletedSigNr &&
            m_doc->verticalLines[k].StartPos == m_deletedSigNr )
        {
            DeleteVerticalLineCommand *cmd = new DeleteVerticalLineCommand(m_doc, k);
            deleteVerticalLineCommands.push_back(cmd);
            cmd->Do();
            break;
        }
    }

    // delete the signal
    m_sig = m_doc->signals[m_deletedSigNr];
    std::vector<Signal>::iterator it = m_doc->signals.begin();
    for ( wxInt32 n = 0 ; n < m_deletedSigNr ; ++n)
        ++it;
    m_doc->signals.erase(it);


    // change the vertical lines connected to this signal
    verticalLines = m_doc->verticalLines;
    for ( wxUint32 n = 0 ; n < m_doc->verticalLines.size() ; ++n )
    {
        if ( m_doc->verticalLines[n].StartPos > m_deletedSigNr)
            m_doc->verticalLines[n].StartPos = m_doc->verticalLines[n].StartPos-1;
        if ( m_doc->verticalLines[n].EndPos >= m_deletedSigNr)
            m_doc->verticalLines[n].EndPos = m_doc->verticalLines[n].EndPos-1;
    }

    horizontalArrows = m_doc->horizontalArrows;
    for ( std::vector<HorizontalArrow>::iterator it = m_doc->horizontalArrows.begin();
        it != m_doc->horizontalArrows.end();)
    {
        if ( it->signalnmbr == m_deletedSigNr )
        {
            m_doc->horizontalArrows.erase(it);
            it = m_doc->horizontalArrows.begin();
            continue;
        }
        it++;
    }
    for ( std::vector<HorizontalArrow>::iterator it = m_doc->horizontalArrows.begin();
        it != m_doc->horizontalArrows.end();
        it++)
    {
        if ( it->signalnmbr > m_deletedSigNr )
        {
            it->signalnmbr--;
        }
    }



    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool DeleteSignalCommand::Undo(void)
{
    std::vector<Signal>::iterator it = m_doc->signals.begin();
    for ( wxInt32 n = 0 ; n < m_deletedSigNr ; ++n)
        it++;
    m_doc->signals.insert(it, 1, m_sig);

    m_doc->verticalLines = verticalLines;

    m_doc->horizontalArrows = horizontalArrows;

    while ( deleteVerticalLineCommands.size() )
    {
        DeleteVerticalLineCommand *cmd = deleteVerticalLineCommands.back();
        cmd->Undo();
        delete cmd;
        deleteVerticalLineCommands.pop_back();

    }

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}



ChangeClockParamCommand::ChangeClockParamCommand(TimingDocument *doc, wxInt32 changingSigNr, wxInt32 per, wxInt32 delay, bool shadow, bool DrawPeriod)
    :wxCommand(true, _T("change clock")),
    m_doc(doc),
    m_changingSigNr(changingSigNr),
    m_newPer(per),
    m_newDelay(delay),
    m_shadow(shadow),
    m_DrawPeriod(DrawPeriod)
{}
ChangeClockParamCommand::~ChangeClockParamCommand(){}
bool ChangeClockParamCommand::Do(void)
{
    wxInt32 tmp;

    tmp = m_doc->signals[m_changingSigNr].ticks;
    m_doc->signals[m_changingSigNr].ticks = m_newPer;
    m_newPer = tmp;

    tmp = m_doc->signals[m_changingSigNr].delay;
    m_doc->signals[m_changingSigNr].delay = m_newDelay;
    m_newDelay = tmp;

    bool btmp;
    btmp = m_doc->signals[m_changingSigNr].GenerateBackground;
    m_doc->signals[m_changingSigNr].GenerateBackground = m_shadow;
    m_shadow = btmp;

    btmp = m_doc->signals[m_changingSigNr].ShowPeriodCount;
    m_doc->signals[m_changingSigNr].ShowPeriodCount = m_DrawPeriod;
    m_DrawPeriod = btmp;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool ChangeClockParamCommand::Undo(void)
{
    return Do();
}

AddSignalCommand::AddSignalCommand(TimingDocument *doc, wxInt32 selectedSigNr, Signal sig)
    :wxCommand(true, _T("Adding Signal") ),
    m_doc(doc),
    m_selectedSigNr(selectedSigNr),
    m_sig(sig)
{}
AddSignalCommand::~AddSignalCommand(){}
bool AddSignalCommand::Do(void)
{
    verticalLines.clear();
    verticalLines = m_doc->verticalLines;
    horizontalArrows = m_doc->horizontalArrows;

    wxInt32 last = m_sig.values.size() - 1;
    for ( wxInt32 k = m_sig.values.size() ; k <= m_doc->length && !(m_sig.IsClock) ; ++k )
        m_sig.values.push_back(m_sig.values[last]);

    if ( m_selectedSigNr == -1 )
        m_doc->signals.push_back(m_sig);
    else
    {
        std::vector<Signal>::iterator it = m_doc->signals.begin();
        for ( wxInt32 n = 0 ; n < m_selectedSigNr ; ++n)
            it++;
        m_doc->signals.insert(it, 1, m_sig);

        for ( wxUint32 n = 0 ; n < m_doc->verticalLines.size() ; ++n )
        {
            if ( m_doc->verticalLines[n].StartPos >= m_selectedSigNr )
                 m_doc->verticalLines[n].StartPos++;
            if ( m_doc->verticalLines[n].EndPos >= m_selectedSigNr )
                m_doc->verticalLines[n].EndPos++;
        }

        for ( wxUint32 n = 0 ; n < m_doc->horizontalArrows.size() ; ++n )
        {
            if ( m_doc->horizontalArrows[n].signalnmbr >= m_selectedSigNr)
                m_doc->horizontalArrows[n].signalnmbr++;
        }
    }

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool AddSignalCommand::Undo(void)
{
    if ( m_selectedSigNr == -1 )
        m_doc->signals.pop_back();
    else
    {
        std::vector<Signal>::iterator it = m_doc->signals.begin();
        for ( wxInt32 n = 0 ; n < m_selectedSigNr ; ++n)
            it++;
        m_doc->signals.erase(it);
    }


    m_doc->verticalLines = verticalLines;
    m_doc->horizontalArrows = horizontalArrows;


    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}

AddDiscontCommand::AddDiscontCommand(TimingDocument *doc, wxInt32 discont)
    : wxCommand(true, _T("adding discont")),
    m_doc(doc),
    m_discont(discont)
{}
AddDiscontCommand::~AddDiscontCommand(){}
bool AddDiscontCommand::Do(void)
{
    TimeCompressor cmprssr;
    cmprssr.length = 2;
    cmprssr.enabled = true;

    m_doc->compressors[m_discont] = cmprssr;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;

}
bool AddDiscontCommand::Undo(void)
{
    m_doc->compressors.erase(m_discont);

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}


RemoveDiscontCommand::RemoveDiscontCommand(TimingDocument *doc, wxInt32 discont)
    :wxCommand(true, _T("removing discont")),
    m_doc(doc),
    m_pos(discont)
{}
RemoveDiscontCommand::~RemoveDiscontCommand(){}
bool RemoveDiscontCommand::Do(void)
{
    if (m_doc->compressors.find(m_pos) != m_doc->compressors.end())
    {
        m_cmprssr = m_doc->compressors[m_pos];
        m_doc->compressors.erase(m_pos);
        m_doc->Modify(true);
        m_doc->UpdateAllViews();
        return true;
    }
    else
        return false;
}
bool RemoveDiscontCommand::Undo(void)
{
    m_doc->compressors[m_pos] = m_cmprssr;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}

MoveSignalPosCommand::MoveSignalPosCommand(TimingDocument *doc, wxInt32 selectedSigNr, wxInt32 targetPos)
    : wxCommand(true, _T("moving signal position")),
    m_doc(doc),
    m_selectedSigNr(selectedSigNr),
    m_targetPos(targetPos)
{}

MoveSignalPosCommand::~MoveSignalPosCommand(){}
bool MoveSignalPosCommand::DoMove(void)
{
    Signal sig = m_doc->signals[m_selectedSigNr];

    if ( m_targetPos == (wxInt32)m_doc->signals.size() ) // moved to end of vector
    {
        m_doc->signals.push_back(sig);
        std::vector<Signal>::iterator it = m_doc->signals.begin();
        for ( wxUint32 n = 0 ; n < (wxUint32)m_selectedSigNr ; ++n)
            it++;
        m_doc->signals.erase(it);

        wxInt32 tmp = m_selectedSigNr;
        m_selectedSigNr = m_doc->signals.size()-1;
        m_targetPos = tmp;
    }
    else
    {
        std::vector<Signal>::iterator it = m_doc->signals.begin();
        for ( wxUint32 n = 0 ; n < (wxUint32)m_targetPos ; ++n)
            it++;
        m_doc->signals.insert(it, sig);


        it = m_doc->signals.begin();
        for ( wxUint32 n = 0 ; n < (wxUint32)m_selectedSigNr ; ++n)
            it++;
        if ( m_targetPos < m_selectedSigNr )
            it++;
        m_doc->signals.erase(it);

        wxInt32 tmp = m_selectedSigNr;


        if ( m_targetPos > m_selectedSigNr )
        {
            m_selectedSigNr = m_targetPos - 1;
            m_targetPos = tmp;
        }
        else
        {
            m_selectedSigNr = m_targetPos ;
            m_targetPos = tmp+1;
        }
    }
    return true;
}
bool MoveSignalPosCommand::Do(void)
{


    verticalLines.clear();
    verticalLines = m_doc->verticalLines;
    horizontalArrows = m_doc->horizontalArrows;

    /// prepare verticalLines
    for ( wxUint32 n = 0 ; n < m_doc->verticalLines.size() ; ++n )
    {
        if ( m_doc->verticalLines[n].StartPos == m_selectedSigNr)
        {
            if (m_selectedSigNr > m_targetPos )
                m_doc->verticalLines[n].StartPos = m_targetPos;
            else
                m_doc->verticalLines[n].StartPos = m_targetPos-1;
        }
        else
        {
            if (m_doc->verticalLines[n].StartPos < m_selectedSigNr &&
                m_doc->verticalLines[n].StartPos >= m_targetPos )
            {
                m_doc->verticalLines[n].StartPos++;
            }
            else
            {
                if (m_doc->verticalLines[n].StartPos > m_selectedSigNr &&
                    m_doc->verticalLines[n].StartPos < m_targetPos )
                {
                    m_doc->verticalLines[n].StartPos--;
                }
            }
        }



        if ( m_doc->verticalLines[n].EndPos == m_selectedSigNr )
        {
            if (m_selectedSigNr < m_targetPos )
                m_doc->verticalLines[n].EndPos = m_targetPos-1;
            else
                m_doc->verticalLines[n].EndPos = m_targetPos;

        }
        else
        {
            if (m_doc->verticalLines[n].EndPos > m_selectedSigNr &&
                m_doc->verticalLines[n].EndPos < m_targetPos)
            {
                m_doc->verticalLines[n].EndPos--;
            }
            else
            {
                if (m_doc->verticalLines[n].EndPos < m_selectedSigNr &&
                    m_doc->verticalLines[n].EndPos >= m_targetPos)
                {
                    m_doc->verticalLines[n].EndPos++;
                }
            }
        }

        if ( m_doc->verticalLines[n].EndPos < m_doc->verticalLines[n].StartPos )
        {
            wxInt32 t = m_doc->verticalLines[n].EndPos;
            m_doc->verticalLines[n].EndPos = m_doc->verticalLines[n].StartPos;
            m_doc->verticalLines[n].StartPos = t;
        }
    }

    /// prepare horizontalArrows;
    for ( wxUint32 n = 0 ; n < m_doc->horizontalArrows.size() ; ++n )
    {
        if ( m_doc->horizontalArrows[n].signalnmbr == m_selectedSigNr )
        {
            if ( m_selectedSigNr > m_targetPos )
                m_doc->horizontalArrows[n].signalnmbr = m_targetPos;
            else
                m_doc->horizontalArrows[n].signalnmbr = m_targetPos-1;
        }
        else
        {
            if ( m_doc->horizontalArrows[n].signalnmbr < m_selectedSigNr &&
                m_doc->horizontalArrows[n].signalnmbr >= m_targetPos )
            {
                m_doc->horizontalArrows[n].signalnmbr++;
            }
            else
            {
                if ( m_doc->horizontalArrows[n].signalnmbr > m_selectedSigNr &&
                    m_doc->horizontalArrows[n].signalnmbr < m_targetPos )
                {
                    m_doc->horizontalArrows[n].signalnmbr--;
                }
            }
        }
        /// check that arrow is not pointing alongside a vertical line (on the left end)
        if ( m_doc->verticalLines[m_doc->horizontalArrows[n].fromVerticalLine].StartPos > m_doc->horizontalArrows[n].signalnmbr ||
            m_doc->verticalLines[m_doc->horizontalArrows[n].fromVerticalLine].EndPos < m_doc->horizontalArrows[n].signalnmbr )
        {
            //add a vertical line for the arrow
            VerticalLine vl;
            vl.EndPos = m_doc->horizontalArrows[n].signalnmbr;
            vl.StartPos = m_doc->horizontalArrows[n].signalnmbr;
            vl.vpos = m_doc->verticalLines[m_doc->horizontalArrows[n].fromVerticalLine].vpos;
            m_doc->verticalLines.push_back(vl);
            m_doc->horizontalArrows[n].fromVerticalLine = m_doc->verticalLines.size()-1;
        }
        /// check that arrow is not pointing alongside a vertical line (on the right end)
        if ( m_doc->verticalLines[m_doc->horizontalArrows[n].toVerticalLine].StartPos > m_doc->horizontalArrows[n].signalnmbr ||
            m_doc->verticalLines[m_doc->horizontalArrows[n].toVerticalLine].EndPos < m_doc->horizontalArrows[n].signalnmbr )
        {
            //add a vertical line for the arrow
            VerticalLine vl;
            vl.EndPos = m_doc->horizontalArrows[n].signalnmbr;
            vl.StartPos = m_doc->horizontalArrows[n].signalnmbr;
            vl.vpos = m_doc->verticalLines[m_doc->horizontalArrows[n].toVerticalLine].vpos;
            m_doc->verticalLines.push_back(vl);
            m_doc->horizontalArrows[n].toVerticalLine = m_doc->verticalLines.size()-1;
        }
    }


    DoMove();


    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool MoveSignalPosCommand::Undo(void)
{
    //return Do();

    DoMove();
    m_doc->verticalLines = verticalLines;
    m_doc->horizontalArrows = horizontalArrows;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}


ChangeSpaceCommand::ChangeSpaceCommand(TimingDocument *doc, wxInt32 selectedSignal, wxInt32 newLength, bool upper)
    : wxCommand(true, _T("changing space around signal") ),
    m_doc(doc),
    m_selectedSignal(selectedSignal),
    m_newLength(newLength),
    m_upper(upper)
{}
ChangeSpaceCommand::~ChangeSpaceCommand(){}
bool ChangeSpaceCommand::Do(void)
{
    horizontalArrows.clear();
    horizontalArrows = m_doc->horizontalArrows;

    for ( wxUint32 n = 0 ; n < m_doc->horizontalArrows.size() ; ++n )
    {
        if ( m_doc->horizontalArrows[n].signalnmbr == m_selectedSignal )
        {
            if ( m_upper )
            {
                if ( m_doc->signals[m_selectedSignal].prespace < m_newLength )
                    m_doc->horizontalArrows[n].pos += (-m_doc->signals[m_selectedSignal].prespace + m_newLength);
                else
                {
                    m_doc->horizontalArrows[n].pos -= (m_doc->signals[m_selectedSignal].prespace - m_newLength);
                    if ( m_doc->horizontalArrows[n].pos < 0 ) m_doc->horizontalArrows[n].pos = 0;
                }

            }
            else
            {
                wxInt32 p = m_doc->signals[m_selectedSignal].prespace +
                            m_doc->SignalHeight +
                            m_doc->MinimumSignalDistance +
                            m_newLength;
                if ( m_doc->horizontalArrows[n].pos > p )
                    m_doc->horizontalArrows[n].pos = p;

            }
        }
    }

    DoChangeSpace();

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}

bool ChangeSpaceCommand::DoChangeSpace(void)
{
    if ( !m_upper )
    {
        wxInt32 tmp = m_doc->signals[m_selectedSignal].space;
        m_doc->signals[m_selectedSignal].space = m_newLength;
        m_newLength = tmp;
    }
    else
    {
        wxInt32 tmp = m_doc->signals[m_selectedSignal].prespace;
        m_doc->signals[m_selectedSignal].prespace = m_newLength;
        m_newLength = tmp;
    }
    return true;
}

bool ChangeSpaceCommand::Undo(void)
{

    DoChangeSpace();

    m_doc->horizontalArrows = horizontalArrows;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}

ChangeLengthLeft::ChangeLengthLeft(TimingDocument *doc, wxInt32 newLength)
    : wxCommand(true, _T("changing length of time axis")),
    m_doc(doc),
    m_newLength(newLength)
{}
ChangeLengthLeft::~ChangeLengthLeft()
{
    DeleteVerticalLineCommand *dvlc;
    for (wxUint32 k = 0 ; k < deleteVerticalLineCommands.size() ; ++k )
    {
        dvlc = deleteVerticalLineCommands[k];
        if ( dvlc ) delete dvlc;
    }
}

bool ChangeLengthLeft::Do(void)
{
    signals.clear();
    signals = m_doc->signals;

    std::map<int, wxString> BusValues;

    compressors = m_doc->compressors;
    m_doc->compressors.clear(); /// add the corrected values later

    /// change the values of the signal
    for ( wxUint32 n = 0 ; n < signals.size() ; ++n )
    {
        if ( !signals[n].IsClock )
        {
            vals val = m_doc->signals[n].values[0];
            m_doc->signals[n].values.clear();
            wxInt32 k = 0;
            if ( m_doc->length < m_newLength )// make it longer
            {
                for ( k = 0 ; k < m_newLength - m_doc->length ; ++k )
                    m_doc->signals[n].values.push_back(val);
                for ( k = 0 ; k < m_doc->length ; ++k )
                    m_doc->signals[n].values.push_back(signals[n].values[k]);

                if ( signals[n].IsBus )
                {
                    m_doc->signals[n].TextValues.clear();
                    wxInt32 len = m_newLength - m_doc->length;
                    for ( std::map<int, wxString>::iterator it =
                          signals[n].TextValues.begin() ;
                          it != signals[n].TextValues.end() ;
                          it++
                        )
                    {
                        if ( it->first == 0 )
                            m_doc->signals[n].TextValues[0] = it->second;
                        else
                            m_doc->signals[n].TextValues[it->first + len] = it->second;
                    }
                }
            }
            else // make it shorter
            {
                for ( k =  m_doc->length - m_newLength ; k < m_doc->length  ; ++k )
                    m_doc->signals[n].values.push_back(signals[n].values[k]);
                if ( signals[n].IsBus )
                {
                    m_doc->signals[n].TextValues.clear();
                    wxInt32 len = m_doc->length - m_newLength;
                    wxInt32 nnn = - len;
                    wxString n_text = _("");
                    for ( std::map<int, wxString>::iterator it = signals[n].TextValues.begin();
                          it != signals[n].TextValues.end() ;
                          it++
                        )
                    {
                        if ( it->first - len >= 0 )
                            m_doc->signals[n].TextValues[it->first - len] = it->second;

                        if ( it->first - len < 0 && it->first >= nnn )
                        {
                            n_text = it->second;
                            nnn = it->first;
                        }

                    }
                    m_doc->signals[n].TextValues[0] = n_text;
                }
            }
        }
        else
        {
            int ticks = m_doc->signals[n].ticks; // of a half clock period
            int delay = m_doc->signals[n].delay;

            m_doc->signals[n].delay =
            ( 2 * ticks + delay + (m_newLength - m_doc->length)) % ( 2 * ticks );

        }
    }

    /// calc new position of dicontinuities (do not add them if outside the graph)
    for ( std::map<wxInt32, TimeCompressor>::iterator it = compressors.begin(); it != compressors.end();it++ )
    {
        wxInt32 n = it->first - (m_doc->length - m_newLength);
        if ( n >= 0 )
        {
            TimeCompressor cmprssr;
            cmprssr.length = it->second.length;
            cmprssr.enabled = it->second.enabled;
            m_doc->compressors[n] = cmprssr;
        }
    }

    /// change positoin of vertical lines or remove them
    wxInt32 diff = m_newLength - m_doc->length;
    deleteVerticalLineCommands.clear();
    for ( wxUint32 k = 0 ; k < m_doc->verticalLines.size() ; ++k )
    {
        if ( m_doc->verticalLines[k].vpos + diff < 0 )
        {
            DeleteVerticalLineCommand *cmd = new DeleteVerticalLineCommand(m_doc, k);
            deleteVerticalLineCommands.push_back(cmd);
            cmd->Do();
            break;
        }
    }
    for ( wxUint32 k = 0 ; k < m_doc->verticalLines.size() ; ++k )
        m_doc->verticalLines[k].vpos += diff;

    m_doc->timeOffset -= diff;

    wxInt32 tmp = m_doc->length;
    m_doc->length = m_newLength;
    m_newLength = tmp;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}

bool ChangeLengthLeft::Undo(void)
{
    m_doc->compressors = compressors;

    m_doc->signals = signals;

    wxInt32 diff = m_newLength - m_doc->length;
    for ( wxUint32 k = 0 ; k < m_doc->verticalLines.size(); ++k )
        m_doc->verticalLines[k].vpos += diff;
    while ( deleteVerticalLineCommands.size() )
    {
        DeleteVerticalLineCommand *cmd = deleteVerticalLineCommands.back();
        cmd->Undo();
        delete cmd;
        deleteVerticalLineCommands.pop_back();
    }

    m_doc->timeOffset -= diff;

    wxInt32 tmp = m_doc->length;
    m_doc->length = m_newLength;
    m_newLength = tmp;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}

AddVerticalLineCommand::AddVerticalLineCommand(TimingDocument *doc, VerticalLine newline)
    : wxCommand(true, _T("added a vertical line")),
    m_doc(doc),
    m_newline(newline)
{}
AddVerticalLineCommand::~AddVerticalLineCommand(){}
bool AddVerticalLineCommand::Do(void)
{
    m_doc->verticalLines.push_back(m_newline);

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool AddVerticalLineCommand::Undo(void)
{
    m_doc->verticalLines.pop_back();

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}

ChangeVerticalLineCommand::ChangeVerticalLineCommand(TimingDocument *doc, wxInt32 nmbr, wxInt32 newVpos, wxInt32 newUpper, wxInt32 newLower, wxInt32 newVposoffset)
    : wxCommand(true, _T("change a vertical line")),
    m_doc(doc),
    m_nmbr(nmbr),
    m_newVpos(newVpos),
    m_newVposoff(newVposoffset)
{
    if ( newUpper <= newLower )
    {
        m_newUpper = newUpper;
        m_newLower = newLower;
    }
    else
    {
        m_newUpper = newLower+1;
        m_newLower = newUpper-1;
    }
}
ChangeVerticalLineCommand::~ChangeVerticalLineCommand(){}
bool ChangeVerticalLineCommand::Do(void)
{
    wxInt32 tmp;

    // swap the startpos and the new startpos
    tmp = m_doc->verticalLines[m_nmbr].StartPos;
//    if ( tmp != m_newUpper) // check varrow if they need a new position
//    {
//        for ( k = k ; k < m_doc->verticalLines
//    }
    m_doc->verticalLines[m_nmbr].StartPos = m_newUpper;
    m_newUpper = tmp;


    // swap the endpos and the new endpos
    tmp = m_doc->verticalLines[m_nmbr].EndPos;
    m_doc->verticalLines[m_nmbr].EndPos = m_newLower;
    m_newLower = tmp;


    // swap the vertical position with the new vpos
    tmp = m_doc->verticalLines[m_nmbr].vpos;
    m_doc->verticalLines[m_nmbr].vpos = m_newVpos;
    m_newVpos = tmp;



    tmp = m_doc->verticalLines[m_nmbr].vposoffset;
    m_doc->verticalLines[m_nmbr].vposoffset = m_newVposoff;
    m_newVposoff = tmp;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool ChangeVerticalLineCommand::Undo(void)
{
    return Do();
}


ChangeHorizontalArrowCommand::ChangeHorizontalArrowCommand(TimingDocument *doc, wxInt32 nmbr, wxInt32 pos, wxInt32 sigindex, wxInt32 newLeft, wxInt32 newRight)
    : wxCommand(true, _T("change horizontal Arrow")),
    m_doc(doc),
    m_nmbr(nmbr),
    m_newPos(pos),
    m_newLeft(newLeft),
    m_newRight(newRight),
    m_newPosIndex(sigindex)
{}
ChangeHorizontalArrowCommand::~ChangeHorizontalArrowCommand(){}
bool ChangeHorizontalArrowCommand::Do(void)
{
    wxInt32 tmp;
    // swap the index to fromVerticalLine
    tmp = m_doc->horizontalArrows[m_nmbr].fromVerticalLine;
    m_doc->horizontalArrows[m_nmbr].fromVerticalLine = m_newLeft;
    m_newLeft = tmp;

    // swap the inde to toVerticalLine
    tmp = m_doc->horizontalArrows[m_nmbr].toVerticalLine;
    m_doc->horizontalArrows[m_nmbr].toVerticalLine = m_newRight;
    m_newRight = tmp;

    // swap the position
    tmp = m_doc->horizontalArrows[m_nmbr].pos;
    m_doc->horizontalArrows[m_nmbr].pos = m_newPos;
    m_newPos = tmp;

    tmp = m_doc->horizontalArrows[m_nmbr].signalnmbr;
    m_doc->horizontalArrows[m_nmbr].signalnmbr = m_newPosIndex;
    m_newPosIndex = tmp;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool ChangeHorizontalArrowCommand::Undo(void)
{
    return Do();
}

DeleteHorizontalArrowCommand::DeleteHorizontalArrowCommand(TimingDocument *doc, wxInt32 nmbr)
    : wxCommand(true, _T("delete a horizontal arrow")),
    m_doc(doc),
    m_nmbr(nmbr)
{}
DeleteHorizontalArrowCommand::~DeleteHorizontalArrowCommand(){}
bool DeleteHorizontalArrowCommand::Do(void)
{
    m_horizontalArrow = m_doc->horizontalArrows[m_nmbr];
    std::vector<HorizontalArrow>::iterator it = m_doc->horizontalArrows.begin();
    wxInt32 k = 0;
    for ( ; it != m_doc->horizontalArrows.end() && k < m_nmbr ;
        it++, ++k)
    {}
    m_doc->horizontalArrows.erase(it);

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool DeleteHorizontalArrowCommand::Undo(void)
{
    std::vector<HorizontalArrow> horizontalArrows;
    for ( wxInt32 k = 0 ; k < m_nmbr ; ++k )
        horizontalArrows.push_back(m_doc->horizontalArrows[k]);
    horizontalArrows.push_back(m_horizontalArrow);
    for ( wxUint32 k = m_nmbr ; k < m_doc->horizontalArrows.size() ; ++k )
        horizontalArrows.push_back(m_doc->horizontalArrows[k]);
    m_doc->horizontalArrows = horizontalArrows;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}

DeleteVerticalLineCommand::DeleteVerticalLineCommand(TimingDocument *doc, wxInt32 nmbr)
    : wxCommand(true, _T("delete a vertical line")),
    m_doc(doc),
    m_nmbr(nmbr),
    first(true)
{}

DeleteVerticalLineCommand::~DeleteVerticalLineCommand(){}

bool DeleteVerticalLineCommand::Do(void)
{
    // clear the vertical line
    {
        m_verticalLine = m_doc->verticalLines[m_nmbr];

        std::vector<VerticalLine>::iterator it = m_doc->verticalLines.begin();
        wxInt32 k = 0;
        for ( ;
            it != m_doc->verticalLines.end() && k < m_nmbr ;
            it++, k++)
        {}
        m_doc->verticalLines.erase(it);
    }

    // clear all connected horizontal arrows
    m_horizontalArrows.clear();
    std::vector<HorizontalArrow>::iterator it;
    it = m_doc->horizontalArrows.begin();
    while ( it != m_doc->horizontalArrows.end() )
    {
        HorizontalArrow &ha = *it;
        if ( ha.fromVerticalLine == m_nmbr ||
             ha.toVerticalLine == m_nmbr )
        {
            m_horizontalArrows.push_back(ha);
            m_doc->horizontalArrows.erase(it);
            it = m_doc->horizontalArrows.begin();
            continue;
        }
        it++;
    }
    for ( it = m_doc->horizontalArrows.begin() ; it != m_doc->horizontalArrows.end() ; it++ )
    {
        HorizontalArrow &ha = *it;
        if ( ha.fromVerticalLine > m_nmbr )
            ha.fromVerticalLine = ha.fromVerticalLine-1;
        if ( ha.toVerticalLine > m_nmbr )
            ha.toVerticalLine = ha.toVerticalLine - 1;
    }

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}

bool DeleteVerticalLineCommand::Undo(void)
{
    std::vector<VerticalLine> verticalLines;
    for ( wxInt32 k = 0 ; k < m_nmbr ; ++k )
        verticalLines.push_back(m_doc->verticalLines[k]);
    verticalLines.push_back(m_verticalLine);
    for ( wxUint32 k = m_nmbr ; k < m_doc->verticalLines.size() ; ++k )
        verticalLines.push_back(m_doc->verticalLines[k]);
    m_doc->verticalLines = verticalLines;

    std::vector<HorizontalArrow>::iterator it;
    for ( it = m_doc->horizontalArrows.begin() ; it != m_doc->horizontalArrows.end() ; it++ )
    {
        HorizontalArrow &ha = *it;
        if ( ha.fromVerticalLine >= m_nmbr )
            ha.fromVerticalLine = ha.fromVerticalLine+1;
        if ( ha.toVerticalLine >= m_nmbr )
            ha.toVerticalLine = ha.toVerticalLine + 1;
    }


    for ( wxUint32 k = 0 ; k < m_horizontalArrows.size() ; ++k)
        m_doc->horizontalArrows.push_back(m_horizontalArrows[k]);



    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}




AddHorizontalArrowCommand::AddHorizontalArrowCommand(TimingDocument *doc, HorizontalArrow newha)
    : wxCommand(true, _T("added a horizontal  arrow")),
    m_doc(doc),
    m_newha(newha)
{}
AddHorizontalArrowCommand::~AddHorizontalArrowCommand(){}
bool AddHorizontalArrowCommand::Do(void)
{
    m_doc->horizontalArrows.push_back(m_newha);

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool AddHorizontalArrowCommand::Undo(void)
{
    m_doc->horizontalArrows.pop_back();

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}


ChangeHorizontalArrowTextPosCommand::ChangeHorizontalArrowTextPosCommand(TimingDocument *doc, wxInt32 editingNumber, wxInt32 xoff, wxInt32 yoff, wxInt32 gridoff)
    : wxCommand(true, _("change position of text")),
    m_doc(doc),
    m_nmbr(editingNumber),
    m_off(xoff, yoff),
    m_gridoff(gridoff)
{}
ChangeHorizontalArrowTextPosCommand::~ChangeHorizontalArrowTextPosCommand(){}
bool ChangeHorizontalArrowTextPosCommand::Do(void)
{
    wxPoint tmp = m_doc->horizontalArrows[m_nmbr].textoffset;
    m_doc->horizontalArrows[m_nmbr].textoffset = m_off;
    m_off = tmp;

    wxInt32 tempc = m_doc->horizontalArrows[m_nmbr].textgridoffset;
    m_doc->horizontalArrows[m_nmbr].textgridoffset = m_gridoff;
    m_gridoff = tempc;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool ChangeHorizontalArrowTextPosCommand::Undo(void)
{
    return Do();
}
ChangeHorizontalArrowTextCommand::ChangeHorizontalArrowTextCommand(TimingDocument *doc, wxInt32 editingNumber, wxString newText):
wxCommand(true, _T("change text of arrow")),
m_doc(doc),
m_nmbr(editingNumber),
m_text(newText)
{}
ChangeHorizontalArrowTextCommand::~ChangeHorizontalArrowTextCommand(){}
bool ChangeHorizontalArrowTextCommand::Do(void)
{
    wxString tmp = m_doc->horizontalArrows[m_nmbr].text;
    m_doc->horizontalArrows[m_nmbr].text = m_text;
    m_text = tmp;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool ChangeHorizontalArrowTextCommand::Undo(void)
{
    return Do();
}


ChangeSignalName::ChangeSignalName(TimingDocument *doc, wxString newName, unsigned int SignalNumber):
wxCommand(true, _T("Change signal name")),
m_doc(doc),
m_signalNumber(SignalNumber),
m_newText(newName)
{}
ChangeSignalName::~ChangeSignalName(){}
bool ChangeSignalName::Do()
{
    bool haschanged;
    wxString tmp(
        m_doc->signals[m_signalNumber].name
    );
    haschanged = tmp != m_newText;
    if ( haschanged )
    {
        m_doc->signals[m_signalNumber].name = m_newText;
        m_newText = tmp;
    }

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return haschanged;
}
bool ChangeSignalName::Undo()
{
    return Do();
}


ChangeSignalBuswidth::ChangeSignalBuswidth(TimingDocument *doc, wxString newName, unsigned int SignalNumber):
wxCommand(true, _T("Change signal bus-width")),
m_doc(doc),
m_signalNumber(SignalNumber),
m_newText(newName)
{}
ChangeSignalBuswidth::~ChangeSignalBuswidth(){}
bool ChangeSignalBuswidth::Do()
{
    bool haschanged;
    wxString tmp(
        m_doc->signals[m_signalNumber].buswidth
    );
    haschanged = tmp != m_newText;
    if ( haschanged )
    {
        m_doc->signals[m_signalNumber].buswidth = m_newText;
        m_newText = tmp;
    }

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return haschanged;
}
bool ChangeSignalBuswidth::Undo()
{
    return Do();
}




ChangeTransitionWidth::ChangeTransitionWidth(TimingDocument *doc, wxInt8 width, bool en50, bool en90)
    :wxCommand(true, _("change transition width") ),
    m_doc(doc),
    m_width(width),
    m_en50(en50),
    m_en90(en90)
{}
ChangeTransitionWidth::~ChangeTransitionWidth(){}
bool ChangeTransitionWidth::Do(void)
{
    wxInt8 tmp = m_doc->TransitWidth;
    m_doc->TransitWidth = m_width;
    m_width = tmp;

    bool t =  m_doc->en50;
    m_doc->en50 = m_en50;
    m_en50 = t;
    t =  m_doc->en90;
    m_doc->en90 = m_en90;
    m_en90 = t;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool ChangeTransitionWidth::Undo(void)
{
    return Do();
}

ChangeAxisSettings::ChangeAxisSettings(TimingDocument *doc, wxInt8 unit, wxInt32 ticklenth, wxInt32 markerlength, wxInt32 offset, wxInt32 totallength):
    wxCommand(true, _("Change time-axis settings") ),
    m_doc(doc),
    m_unit(unit),
    m_ticklength(ticklenth),
    m_markerlength(markerlength),
    m_offset(offset),
    AddRemoveTimeCommand(NULL)
{
    if ( doc->length != totallength )
    {
        if ( doc->length < totallength )
        {
            AddRemoveTimeCommand = new AddTimeCommand(doc, doc->length, totallength - doc->length);
        }
        else
        {
            AddRemoveTimeCommand = new RemoveTimeCommand(doc, totallength, doc->length);
        }
    }
}
ChangeAxisSettings::~ChangeAxisSettings(){}
bool ChangeAxisSettings::Do(void)
{
    Change();

    if( AddRemoveTimeCommand )
        return AddRemoveTimeCommand->Do();
    else
    {
        m_doc->Modify(true);
        m_doc->UpdateAllViews();
        return true;
    }
}
void ChangeAxisSettings::Change()
{
    wxInt8 tmp8 = m_doc->TickLengthUnit;
    m_doc->TickLengthUnit = m_unit;
    m_unit = tmp8;

    wxInt32 tmp = m_doc->TickLength;
    m_doc->TickLength = m_ticklength;
    m_ticklength = tmp;

    tmp = m_doc->MarkerLength;
    m_doc->MarkerLength = m_markerlength;
    m_markerlength = tmp;

    tmp = m_doc->timeOffset;
    m_doc->timeOffset = m_offset;
    m_offset = tmp;
}
bool ChangeAxisSettings::Undo(void)
{
    if (AddRemoveTimeCommand )
    {
        bool ret = AddRemoveTimeCommand->Undo();
        Change();
        return ret;
    }
    else
    {
        Change();
        m_doc->Modify(true);
        m_doc->UpdateAllViews();
        return true;
    }
}


ChangeTimeCompressor::ChangeTimeCompressor(TimingDocument *doc, wxInt32 index, wxInt32 time, bool en):
    wxCommand(true, _("Change time compressor") ),
    m_doc(doc),
    m_time(time),
    m_index(index),
    m_en(en)
{}
ChangeTimeCompressor::ChangeTimeCompressor(TimingDocument *doc, wxInt32 index, wxInt32 time):
    wxCommand(true, _("Change time compressor length") ),
    m_doc(doc),
    m_time(time),
    m_index(index)
{
    if ( m_doc->compressors.find(m_index) != m_doc->compressors.end())
        m_en = m_doc->compressors[m_index].enabled;
}
ChangeTimeCompressor::ChangeTimeCompressor(TimingDocument *doc, wxInt32 index, bool en)
    :wxCommand(true, _("Enable/Disable time compressor") ),
    m_doc(doc),
    m_index(index),
    m_en(en)
{
    if ( m_doc->compressors.find(m_index) != m_doc->compressors.end())
        m_time = m_doc->compressors[m_index].length;
}
bool ChangeTimeCompressor::Do(void)
{
    if ( m_doc->compressors.find(m_index) != m_doc->compressors.end())
    {
        wxInt32 tmp = m_doc->compressors[m_index].length;
        m_doc->compressors[m_index].length = m_time;
        m_time = tmp;

        bool btmp = m_doc->compressors[m_index].enabled;
        m_doc->compressors[m_index].enabled = m_en;
        m_en = btmp;
    }

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool ChangeTimeCompressor::Undo(void)
{
    return Do();
}
ChangeTimeCompressor::~ChangeTimeCompressor(){}

AddTimeCommand::AddTimeCommand(TimingDocument *doc, wxInt32 pos, wxInt32 len)
: wxCommand(true, _("Insert duration in time") ),
    m_doc(doc),
    m_pos(pos),
    m_len(len)
{
}
AddTimeCommand::~AddTimeCommand(){}
bool AddTimeCommand::Do(void)
{
    // move vertical lines
    for ( wxUint32 n = 0 ; n < m_doc->verticalLines.size() ; ++n )
    {
        if ( m_doc->verticalLines[n].vpos > m_pos )
             m_doc->verticalLines[n].vpos += m_len;
    }

    // move time compressors
    std::map<wxInt32, TimeCompressor> cmprs;
    std::map<wxInt32, TimeCompressor>::iterator it;
    for (it = m_doc->compressors.begin(); it != m_doc->compressors.end() ; it++ )
    {
        if ( it->first >= m_pos )
            cmprs[it->first + m_len] = it->second;
        else
            cmprs[it->first] = it->second;
    }
    m_doc->compressors = cmprs;

    // insert signal states
    for ( wxInt32 n = 0 ; n < (wxInt32)m_doc->signals.size() ; ++n)
        if ( !m_doc->signals[n].IsClock )
        {
            Signal sig = m_doc->signals[n];
            vals val = sig.values[m_pos];
            sig.values.clear();
            for (wxInt32 k = 0 ; k < m_pos ; ++k )
                sig.values.push_back(m_doc->signals[n].values[k]);
            for ( wxInt32 k = 0 ; k < m_len ; ++k)
                sig.values.push_back(val);
            for ( wxInt32 k = m_pos ; k < (wxInt32)m_doc->signals[n].values.size() ; ++k )
                sig.values.push_back(m_doc->signals[n].values[k]);
            m_doc->signals[n].values = sig.values;

            // - move texts of bus signals
            for ( wxInt32 k = m_doc->length ; k >= m_pos; k-- )
            {
                if ( m_doc->signals[n].TextValues.find(k) != m_doc->signals[n].TextValues.end() )
                {
                    m_doc->signals[n].TextValues[k+m_len] = m_doc->signals[n].TextValues[k];
                    m_doc->signals[n].TextValues.erase(k);
                }
            }
        }

    //adjust length stored in document
    m_doc->length += m_len;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool AddTimeCommand::Undo(void)
{
    // move vertical lines
    for ( wxUint32 n = 0 ; n < m_doc->verticalLines.size() ; ++n )
    {
        if ( m_doc->verticalLines[n].vpos > m_pos )
             m_doc->verticalLines[n].vpos -= m_len;
    }

    // move time compressors back
    std::map<wxInt32, TimeCompressor> cmprs;
    std::map<wxInt32, TimeCompressor>::iterator it;
    for (it = m_doc->compressors.begin(); it != m_doc->compressors.end() ; it++ )
    {
        if ( it->first > m_pos)
            cmprs[it->first - m_len] = it->second;
        else
            cmprs[it->first] = it->second;
    }

    // remove signal states
    for ( wxInt32 n = 0 ; n < (wxInt32)m_doc->signals.size() ; ++n)
        if ( !m_doc->signals[n].IsClock )
        {

            Signal sig = m_doc->signals[n];
            sig.values.clear();
            for (wxInt32 k = 0 ; k < m_pos ; ++k )
                sig.values.push_back(m_doc->signals[n].values[k]);
            for ( wxInt32 k = m_pos+m_len ; k < (wxInt32)m_doc->signals[n].values.size() ; ++k )
                sig.values.push_back(m_doc->signals[n].values[k]);
            m_doc->signals[n].values = sig.values;


            // - move texts of signals
            for ( wxInt32 k = m_pos + m_len ; k < m_doc->length; ++k )
            {
                if ( m_doc->signals[n].TextValues.find(k) != m_doc->signals[n].TextValues.end() )
                {
                    m_doc->signals[n].TextValues[k-m_len] = m_doc->signals[n].TextValues[k];
                    m_doc->signals[n].TextValues.erase(k);
                }
            }
        }

    //adjust length stored in document
    m_doc->length -= m_len;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}


RemoveTimeCommand::RemoveTimeCommand(TimingDocument *doc, wxInt32 beg, wxInt32 end)
:wxCommand(true, _T("Delete a time slice") ),
    m_doc(doc),
    m_beg(beg),
    m_end(end)
{
    if ( beg > end)
    {
        m_beg = end;
        m_end = beg;
    }
}

RemoveTimeCommand::~RemoveTimeCommand()
{
    DeleteVerticalLineCommand *dvlc;
    for (wxUint32 k = 0 ; k < deleteVerticalLineCommands.size() ; ++k )
    {
        dvlc = deleteVerticalLineCommands[k];
        if ( dvlc ) delete dvlc;
    }
}

bool RemoveTimeCommand::Do(void)
{
    //signals
    m_signals = m_doc->signals;
    for ( wxUint32 n = 0 ; n < m_doc->signals.size() ; ++n )
    {
        if (!m_doc->signals[n].IsClock)
        {
            wxInt32 k = 0;
            std::vector<vals>::iterator st, ed;
            for( std::vector<vals>::iterator it = m_doc->signals[n].values.begin() ; it != m_doc->signals[n].values.end() ; it++ )
            {
                if ( k == m_beg )
                    st = it;
                if ( k == m_end )
                    ed = it;
                ++k;
            }
            m_doc->signals[n].values.erase(st,ed);
        }
    }

    //time compressors
    m_compressors = m_doc->compressors;
    m_doc->compressors.clear();
    for ( std::map<wxInt32, TimeCompressor>::iterator it = m_compressors.begin(); it != m_compressors.end() ; it++ )
    {
        if ( it->first < m_beg )
            m_doc->compressors[it->first] = it->second;
        else if ( it->first >= m_end )
            m_doc->compressors[it->first - m_end + m_beg ] = it->second;
    }

    //vertical lines
    /// change positoin of vertical lines or remove them
    deleteVerticalLineCommands.clear();
    for ( wxUint32 k = 0 ; k < m_doc->verticalLines.size() ; ++k )
    {
        if ( m_doc->verticalLines[k].vpos >= m_beg && m_doc->verticalLines[k].vpos < m_end )
        {
            DeleteVerticalLineCommand *cmd = new DeleteVerticalLineCommand(m_doc, k);
            deleteVerticalLineCommands.push_back(cmd);
            cmd->Do();
            break;
        }
    }
    for ( wxUint32 k = 0 ; k < m_doc->verticalLines.size() ; ++k )
        if ( m_doc->verticalLines[k].vpos >= m_end )
            m_doc->verticalLines[k].vpos -= ( m_end - m_beg );

    //length
    m_doc->length -= (m_end - m_beg);

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}

bool RemoveTimeCommand::Undo(void)
{
    //signals
    m_doc->signals = m_signals;

    //compressors
    m_doc->compressors = m_compressors;

    //vertical lines
    for ( wxUint32 k = 0 ; k < m_doc->verticalLines.size(); ++k )
        if ( m_doc->verticalLines[k].vpos >= m_beg )
            m_doc->verticalLines[k].vpos += (m_end -m_beg);
    while ( deleteVerticalLineCommands.size() )
    {
        DeleteVerticalLineCommand *cmd = deleteVerticalLineCommands.back();
        cmd->Undo();
        delete cmd;
        deleteVerticalLineCommands.pop_back();
    }

    //length
    m_doc->length += (m_end - m_beg);

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}

