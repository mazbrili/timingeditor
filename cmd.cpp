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
// TODO (daniel#5#): removed harrow by removed vline by removing signal dont gets  restored with undo

//ChangeText(, wxInt32 n, wxString newText):
ChangeText::ChangeText(TimingDocument *doc, wxString *target, wxString newText)
    :wxCommand(true, _T("changing text")),
    m_doc(doc),
    m_target(target),
    m_newText(newText)
{
}
ChangeText::~ChangeText(){}
bool ChangeText::Do(void)
{
    wxString tmp = *m_target;
    *m_target = m_newText;
    m_newText = tmp;
    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool ChangeText::Undo(void)
{
    return Do();
}


ChangeLength::ChangeLength(TimingDocument *doc, wxInt32 newLength)
    :wxCommand(true, _T("changing length of time axis")),
    m_doc(doc),
    m_newLength(newLength)
{
}
ChangeLength::~ChangeLength()
{
    DeleteVLineCommand *dvlc;
    for (wxUint32 k = 0 ; k < delVlineCom.size() ; ++k )
    {
        dvlc = delVlineCom[k];
        if ( dvlc ) delete dvlc;
    }
}
bool ChangeLength::Do(void)
{

    signals.clear();
    signals = m_doc->signals;

    discont.clear();
    disclen.clear();
    std::set<wxInt32>::iterator it;
    std::set<wxInt32>::iterator itlow;
    itlow = m_doc->discontinuities.lower_bound(m_newLength) ;
    for(it = itlow;
        it != m_doc->discontinuities.end() ;
        it++)
    {
        discont.insert(*it);
        disclen[*it] = m_doc->discontlength[*it];
        m_doc->discontlength.erase(*it);
    }
    m_doc->discontinuities.erase(itlow, m_doc->discontinuities.end() );

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

    delVlineCom.clear();
    if ( m_newLength < m_doc->length)
    {
        //std::vector<VLine>::iterator it;
        wxUint32 k;
        //it = m_doc->vertlines.begin();
        for (  k = 0 ; k < m_doc->vertlines.size(); ++k )
        {
            if ( m_doc->vertlines[k].vpos > m_newLength )
            {
                //std::vector<DeleteVLineCommand*> delVlineCom;
                DeleteVLineCommand *cmd = new DeleteVLineCommand(m_doc, k);
                delVlineCom.push_back(cmd);
                cmd->Do();
                k=-1;
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

    for (
        std::set<wxInt32>::iterator it = discont.begin() ;
        it != discont.end() ;
        it++
    )
    {
        m_doc->discontinuities.insert(*it);
        m_doc->discontlength[*it] = disclen[*it];
    }


    m_doc->signals.clear();
    m_doc->signals = signals;


    while ( delVlineCom.size() )
    {
        DeleteVLineCommand *cmd = delVlineCom.back();
        cmd->Undo();
        delete cmd;
        delVlineCom.pop_back();

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
    DeleteVLineCommand *dvlc;
    for (wxUint32 k = 0 ; k < delVlineCom.size() ; ++k )
    {
        dvlc = delVlineCom[k];
        if ( dvlc ) delete dvlc;
    }
}
bool DeleteSignalCommand::Do(void)
{
    // delete vertical lines which are only connected to the deleted signal
    delVlineCom.clear();
    for (wxUint32  k = 0 ; k < m_doc->vertlines.size(); ++k )
    {
        if ( m_doc->vertlines[k].EndPos == m_deletedSigNr &&
            m_doc->vertlines[k].StartPos == m_deletedSigNr )
        {
            //std::vector<DeleteVLineCommand*> delVlineCom;
            DeleteVLineCommand *cmd = new DeleteVLineCommand(m_doc, k);
            delVlineCom.push_back(cmd);
            cmd->Do();
            k = -1;
        }
    }

    // delete the signal
    m_sig = m_doc->signals[m_deletedSigNr];
    std::vector<Signal>::iterator it = m_doc->signals.begin();
    for ( wxInt32 n = 0 ; n < m_deletedSigNr ; ++n)
        ++it;
    m_doc->signals.erase(it);


    // change the vertical lines connected to this signal
    vlines = m_doc->vertlines;
    for ( wxUint32 n = 0 ; n < m_doc->vertlines.size() ; ++n )
    {
        if ( m_doc->vertlines[n].StartPos > m_deletedSigNr)
            m_doc->vertlines[n].StartPos = m_doc->vertlines[n].StartPos-1;
        if ( m_doc->vertlines[n].EndPos >= m_deletedSigNr)
            m_doc->vertlines[n].EndPos = m_doc->vertlines[n].EndPos-1;

//        else if (m_deletedSigNr == m_doc->signals.size() -1 &&
//         m_doc->vertlines[n].EndPos == m_deletedSigNr + 1 )
//            m_doc->vertlines[n].EndPos --;
    }

    harrows = m_doc->harrows;
    for ( std::vector<HArrow>::iterator it = m_doc->harrows.begin();
        it != m_doc->harrows.end();)
    {
        //HArrow &ha = *it;
        if ( it->signalnmbr == m_deletedSigNr )
        {
            m_doc->harrows.erase(it);
            it = m_doc->harrows.begin();
            continue;
        }
        it++;
    }
    for ( std::vector<HArrow>::iterator it = m_doc->harrows.begin();
        it != m_doc->harrows.end();
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

    m_doc->vertlines = vlines;

    while ( delVlineCom.size() )
    {
        DeleteVLineCommand *cmd = delVlineCom.back();
        cmd->Undo();
        delete cmd;
        delVlineCom.pop_back();

    }

    m_doc->harrows = harrows;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}



ChangeClockParamCommand::ChangeClockParamCommand(TimingDocument *doc, wxInt32 changingSigNr, wxInt32 per, wxInt32 delay)
    :wxCommand(true, _T("change clock")),
    m_doc(doc),
    m_changingSigNr(changingSigNr),
    m_newPer(per),
    m_newDelay(delay)
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
    vlines.clear();
    vlines = m_doc->vertlines;
    harrows = m_doc->harrows;

    if ( m_selectedSigNr == -1 )
        m_doc->signals.push_back(m_sig);
    else
    {
        std::vector<Signal>::iterator it = m_doc->signals.begin();
        for ( wxInt32 n = 0 ; n < m_selectedSigNr ; ++n)
            it++;
        m_doc->signals.insert(it, 1, m_sig);

        for ( wxUint32 n = 0 ; n < m_doc->vertlines.size() ; ++n )
        {
            if ( m_doc->vertlines[n].StartPos >= m_selectedSigNr )
                 m_doc->vertlines[n].StartPos++;
            if ( m_doc->vertlines[n].EndPos >= m_selectedSigNr )
                m_doc->vertlines[n].EndPos++;
        }

        for ( wxUint32 n = 0 ; n < m_doc->harrows.size() ; ++n )
        {
            if ( m_doc->harrows[n].signalnmbr >= m_selectedSigNr)
                m_doc->harrows[n].signalnmbr++;
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


    m_doc->vertlines = vlines;
    m_doc->harrows = harrows;


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
    if ( m_doc->discontinuities.find(m_discont) == m_doc->discontinuities.end() )
    {
        m_doc->discontinuities.insert(m_discont);
        m_doc->discontlength[m_discont] = 2;

        m_doc->Modify(true);
        m_doc->UpdateAllViews();
        return true;
    }
    else
        return false;
}
bool AddDiscontCommand::Undo(void)
{
    m_doc->discontinuities.erase(m_discont);
    m_doc->discontlength.erase(m_discont);

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}


RemoveDiscontCommand::RemoveDiscontCommand(TimingDocument *doc, wxInt32 discont)
    :wxCommand(true, _T("removing discont")),
    m_doc(doc),
    m_discont(discont)
{}
RemoveDiscontCommand::~RemoveDiscontCommand(){}
bool RemoveDiscontCommand::Do(void)
{
    if ( m_doc->discontinuities.find(m_discont) != m_doc->discontinuities.end() )
    {
        m_len = m_doc->discontlength[m_discont];
        m_doc->discontinuities.erase(m_discont);
        m_doc->discontlength.erase(m_discont);

        m_doc->Modify(true);
        m_doc->UpdateAllViews();
        return true;
    }
    else
        return false;
}
bool RemoveDiscontCommand::Undo(void)
{
    m_doc->discontinuities.insert(m_discont);
    m_doc->discontlength[m_discont] = m_len;
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


    vlines.clear();
    vlines = m_doc->vertlines;
    harrows = m_doc->harrows;

    /// prepare vertlines
    for ( wxUint32 n = 0 ; n < m_doc->vertlines.size() ; ++n )
    {
        if ( m_doc->vertlines[n].StartPos == m_selectedSigNr)
        {
            if (m_selectedSigNr > m_targetPos )
                m_doc->vertlines[n].StartPos = m_targetPos;
            else
                m_doc->vertlines[n].StartPos = m_targetPos-1;
        }
        else
        {
            if (m_doc->vertlines[n].StartPos < m_selectedSigNr &&
                m_doc->vertlines[n].StartPos >= m_targetPos )
            {
                m_doc->vertlines[n].StartPos++;
            }
            else
            {
                if (m_doc->vertlines[n].StartPos > m_selectedSigNr &&
                    m_doc->vertlines[n].StartPos < m_targetPos )
                {
                    m_doc->vertlines[n].StartPos--;
                }
            }
        }



        if ( m_doc->vertlines[n].EndPos == m_selectedSigNr )
        {
            if (m_selectedSigNr < m_targetPos )
                m_doc->vertlines[n].EndPos = m_targetPos-1;
            else
                m_doc->vertlines[n].EndPos = m_targetPos;

        }
        else
        {
            if (m_doc->vertlines[n].EndPos > m_selectedSigNr &&
                m_doc->vertlines[n].EndPos < m_targetPos)
            {
                m_doc->vertlines[n].EndPos--;
            }
            else
            {
                if (m_doc->vertlines[n].EndPos < m_selectedSigNr &&
                    m_doc->vertlines[n].EndPos >= m_targetPos)
                {
                    m_doc->vertlines[n].EndPos++;
                }
            }
        }

        if ( m_doc->vertlines[n].EndPos < m_doc->vertlines[n].StartPos )
        {
            wxInt32 t = m_doc->vertlines[n].EndPos;
            m_doc->vertlines[n].EndPos = m_doc->vertlines[n].StartPos;
            m_doc->vertlines[n].StartPos = t;
        }
    }

    /// prepare harrows;
    for ( wxUint32 n = 0 ; n < m_doc->harrows.size() ; ++n )
    {
        if ( m_doc->harrows[n].signalnmbr == m_selectedSigNr )
        {
            if ( m_selectedSigNr > m_targetPos )
                m_doc->harrows[n].signalnmbr = m_targetPos;
            else
                m_doc->harrows[n].signalnmbr = m_targetPos-1;
        }
        else
        {
            if ( m_doc->harrows[n].signalnmbr < m_selectedSigNr &&
                m_doc->harrows[n].signalnmbr >= m_targetPos )
            {
                m_doc->harrows[n].signalnmbr++;
            }
            else
            {
                if ( m_doc->harrows[n].signalnmbr > m_selectedSigNr &&
                    m_doc->harrows[n].signalnmbr < m_targetPos )
                {
                    m_doc->harrows[n].signalnmbr--;
                }
            }
        }
        /// check that arrow is not pointing alongside a vertical line (on the left end)
        if ( m_doc->vertlines[m_doc->harrows[n].fromVLine].StartPos > m_doc->harrows[n].signalnmbr ||
            m_doc->vertlines[m_doc->harrows[n].fromVLine].EndPos < m_doc->harrows[n].signalnmbr )
        {
            //add a vline for the arrow
            VLine vl;
            vl.EndPos = m_doc->harrows[n].signalnmbr;
            vl.StartPos = m_doc->harrows[n].signalnmbr;
            vl.vpos = m_doc->vertlines[m_doc->harrows[n].fromVLine].vpos;
            m_doc->vertlines.push_back(vl);
            m_doc->harrows[n].fromVLine = m_doc->vertlines.size()-1;
        }
        /// check that arrow is not pointing alongside a vertical line (on the right end)
        if ( m_doc->vertlines[m_doc->harrows[n].toVLine].StartPos > m_doc->harrows[n].signalnmbr ||
            m_doc->vertlines[m_doc->harrows[n].toVLine].EndPos < m_doc->harrows[n].signalnmbr )
        {
            //add a vline for the arrow
            VLine vl;
            vl.EndPos = m_doc->harrows[n].signalnmbr;
            vl.StartPos = m_doc->harrows[n].signalnmbr;
            vl.vpos = m_doc->vertlines[m_doc->harrows[n].toVLine].vpos;
            m_doc->vertlines.push_back(vl);
            m_doc->harrows[n].toVLine = m_doc->vertlines.size()-1;
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
    m_doc->vertlines = vlines;
    m_doc->harrows = harrows;

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
    harrows.clear();
    harrows = m_doc->harrows;

    for ( wxUint32 n = 0 ; n < m_doc->harrows.size() ; ++n )
    {
        if ( m_doc->harrows[n].signalnmbr == m_selectedSignal )
        {
            if ( m_upper )
            {
                //if ( m_doc->signals[m_selectedSignal].prespace > m_newLength )
                //    m_doc->harrows[n].pos -= (m_doc->signals[m_selectedSignal].prespace - m_newLength);
                if ( m_doc->signals[m_selectedSignal].prespace < m_newLength )
                    m_doc->harrows[n].pos += (-m_doc->signals[m_selectedSignal].prespace + m_newLength);
                else
                {
                    m_doc->harrows[n].pos -= (m_doc->signals[m_selectedSignal].prespace - m_newLength);
                    if ( m_doc->harrows[n].pos < 0 ) m_doc->harrows[n].pos = 0;
                }

            }
            else
            {
                //if ( m_doc->signals[m_selectedSignal].space > m_newLength )
                //    m_doc->harrows[n].pos -= (m_doc->signals[m_selectedSignal].space - m_newLength);
                wxInt32 p = m_doc->signals[m_selectedSignal].prespace +
                            m_doc->SignalHeight +
                            m_doc->MinimumSignalDistance +
                            m_newLength;
                if ( m_doc->harrows[n].pos > p )
                    m_doc->harrows[n].pos = p;

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

    m_doc->harrows = harrows;

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
    DeleteVLineCommand *dvlc;
    for (wxUint32 k = 0 ; k < delVlineCom.size() ; ++k )
    {
        dvlc = delVlineCom[k];
        if ( dvlc ) delete dvlc;
    }
}

bool ChangeLengthLeft::Do(void)
{
    signals.clear();
    signals = m_doc->signals;

    std::map<int, wxString> BusValues;

    discont.clear();
    discont = m_doc->discontinuities;
    disclen = m_doc->discontlength;
    m_doc->discontinuities.clear(); /// add the corrected values later
    m_doc->discontlength.clear();

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
    for (
        std::set<wxInt32>::iterator it = discont.begin();
        it != discont.end();
        it++
    )
    {
        wxInt32 n = *it - (m_doc->length - m_newLength);
        if ( n >= 0 )
        {
            m_doc->discontinuities.insert( n );
            m_doc->discontlength[n] = disclen[*it];
        }
    }

    /// change positoin of vertical lines or remove them
    wxInt32 diff = m_newLength - m_doc->length;
    delVlineCom.clear();
    for ( wxUint32 k = 0 ; k < m_doc->vertlines.size() ; ++k )
    {
        if ( m_doc->vertlines[k].vpos + diff < 0 )
        {
            DeleteVLineCommand *cmd = new DeleteVLineCommand(m_doc, k);
            delVlineCom.push_back(cmd);
            cmd->Do();
            k=-1;
        }
    }
    for ( wxUint32 k = 0 ; k < m_doc->vertlines.size() ; ++k )
        m_doc->vertlines[k].vpos += diff;

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
    m_doc->discontinuities.clear();
    m_doc->discontinuities = discont;
    m_doc->discontlength.clear();
    m_doc->discontlength = disclen;

    m_doc->signals.clear();
    m_doc->signals = signals;

    wxInt32 diff = m_newLength - m_doc->length;
    for ( wxUint32 k = 0 ; k < m_doc->vertlines.size(); ++k )
        m_doc->vertlines[k].vpos += diff;
    while ( delVlineCom.size() )
    {
        DeleteVLineCommand *cmd = delVlineCom.back();
        cmd->Undo();
        delete cmd;
        delVlineCom.pop_back();
    }

    m_doc->timeOffset -= diff;

    wxInt32 tmp = m_doc->length;
    m_doc->length = m_newLength;
    m_newLength = tmp;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}

AddVLineCommand::AddVLineCommand(TimingDocument *doc, VLine newline)
    : wxCommand(true, _T("added a vertical line")),
    m_doc(doc),
    m_newline(newline)
{}
    AddVLineCommand::~AddVLineCommand(){}
bool AddVLineCommand::Do(void)
{
    m_doc->vertlines.push_back(m_newline);

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool AddVLineCommand::Undo(void)
{
    m_doc->vertlines.pop_back();

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}

ChangeVLineCommand::ChangeVLineCommand(TimingDocument *doc, wxInt32 nmbr, wxInt32 newVpos, wxInt32 newUpper, wxInt32 newLower, wxInt32 newVposoffset)
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
ChangeVLineCommand::~ChangeVLineCommand(){}
bool ChangeVLineCommand::Do(void)
{
    wxInt32 tmp;

    // swap the startpos and the new startpos
    tmp = m_doc->vertlines[m_nmbr].StartPos;
//    if ( tmp != m_newUpper) // check varrow if they need a new position
//    {
//        for ( k = k ; k < m_doc->vertlines
//    }
    m_doc->vertlines[m_nmbr].StartPos = m_newUpper;
    m_newUpper = tmp;


    // swap the endpos and the new endpos
    tmp = m_doc->vertlines[m_nmbr].EndPos;
    m_doc->vertlines[m_nmbr].EndPos = m_newLower;
    m_newLower = tmp;


    // swap the vertical position with the new vpos
    tmp = m_doc->vertlines[m_nmbr].vpos;
    m_doc->vertlines[m_nmbr].vpos = m_newVpos;
    m_newVpos = tmp;



    tmp = m_doc->vertlines[m_nmbr].vposoffset;
    m_doc->vertlines[m_nmbr].vposoffset = m_newVposoff;
    m_newVposoff = tmp;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool ChangeVLineCommand::Undo(void)
{
    return Do();
}


ChangeHArrowCommand::ChangeHArrowCommand(TimingDocument *doc, wxInt32 nmbr, wxInt32 pos, wxInt32 sigindex, wxInt32 newLeft, wxInt32 newRight)
    : wxCommand(true, _T("change horizontal Arrow")),
    m_doc(doc),
    m_nmbr(nmbr),
    m_newPos(pos),
    m_newLeft(newLeft),
    m_newRight(newRight),
    m_newPosIndex(sigindex)
{}
ChangeHArrowCommand::~ChangeHArrowCommand(){}
bool ChangeHArrowCommand::Do(void)
{
    wxInt32 tmp;
    // swap the index to fromvline
    tmp = m_doc->harrows[m_nmbr].fromVLine;
    m_doc->harrows[m_nmbr].fromVLine = m_newLeft;
    m_newLeft = tmp;

    // swap the inde to toVLine
    tmp = m_doc->harrows[m_nmbr].toVLine;
    m_doc->harrows[m_nmbr].toVLine = m_newRight;
    m_newRight = tmp;

    // swap the position
    tmp = m_doc->harrows[m_nmbr].pos;
    m_doc->harrows[m_nmbr].pos = m_newPos;
    m_newPos = tmp;

    tmp = m_doc->harrows[m_nmbr].signalnmbr;
    m_doc->harrows[m_nmbr].signalnmbr = m_newPosIndex;
    m_newPosIndex = tmp;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool ChangeHArrowCommand::Undo(void)
{
    return Do();
}

DeleteHArrowCommand::DeleteHArrowCommand(TimingDocument *doc, wxInt32 nmbr)
    : wxCommand(true, _T("delete a horizontal arrow")),
    m_doc(doc),
    m_nmbr(nmbr)
{}
DeleteHArrowCommand::~DeleteHArrowCommand(){}
bool DeleteHArrowCommand::Do(void)
{
    m_harrow = m_doc->harrows[m_nmbr];
    std::vector<HArrow>::iterator it = m_doc->harrows.begin();
    wxInt32 k = 0;
    for ( ; it != m_doc->harrows.end() && k < m_nmbr ;
        it++, ++k)
    {}
    m_doc->harrows.erase(it);

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool DeleteHArrowCommand::Undo(void)
{
    std::vector<HArrow> harrows;
    for ( wxInt32 k = 0 ; k < m_nmbr ; ++k )
        harrows.push_back(m_doc->harrows[k]);
    harrows.push_back(m_harrow);
    for ( wxUint32 k = m_nmbr ; k < m_doc->harrows.size() ; ++k )
        harrows.push_back(m_doc->harrows[k]);
    m_doc->harrows = harrows;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}

DeleteVLineCommand::DeleteVLineCommand(TimingDocument *doc, wxInt32 nmbr)
    : wxCommand(true, _T("delete a vertical line")),
    m_doc(doc),
    m_nmbr(nmbr),
    first(true)
{}

DeleteVLineCommand::~DeleteVLineCommand(){}

bool DeleteVLineCommand::Do(void)
{
    // clear the vertical line
    {
        m_vline = m_doc->vertlines[m_nmbr];

        std::vector<VLine>::iterator it = m_doc->vertlines.begin();
        wxInt32 k = 0;
        for ( ;
            it != m_doc->vertlines.end() && k < m_nmbr ;
            it++, k++)
        {}
        m_doc->vertlines.erase(it);
    }

    // clear all connected horizontal arrows
    m_harrows.clear();
    std::vector<HArrow>::iterator it;
    it = m_doc->harrows.begin();
    while ( it != m_doc->harrows.end() )
    {
        HArrow &ha = *it;
        if ( ha.fromVLine == m_nmbr ||
             ha.toVLine == m_nmbr )
        {
            m_harrows.push_back(ha);
            m_doc->harrows.erase(it);
            it = m_doc->harrows.begin();
            continue;
        }
        it++;
    }
    for ( it = m_doc->harrows.begin() ; it != m_doc->harrows.end() ; it++ )
    {
        HArrow &ha = *it;
        if ( ha.fromVLine > m_nmbr )
            ha.fromVLine = ha.fromVLine-1;
        if ( ha.toVLine > m_nmbr )
            ha.toVLine = ha.toVLine - 1;
    }

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}

bool DeleteVLineCommand::Undo(void)
{
    //m_doc->vertlines.push_back(m_vline);
    std::vector<VLine> vlines;
    for ( wxInt32 k = 0 ; k < m_nmbr ; ++k )
        vlines.push_back(m_doc->vertlines[k]);
    vlines.push_back(m_vline);
    for ( wxUint32 k = m_nmbr ; k < m_doc->vertlines.size() ; ++k )
        vlines.push_back(m_doc->vertlines[k]);
    m_doc->vertlines = vlines;

    std::vector<HArrow>::iterator it;
    for ( it = m_doc->harrows.begin() ; it != m_doc->harrows.end() ; it++ )
    {
        HArrow &ha = *it;
        if ( ha.fromVLine >= m_nmbr )
            ha.fromVLine = ha.fromVLine+1;
        if ( ha.toVLine >= m_nmbr )
            ha.toVLine = ha.toVLine + 1;
    }


    for ( wxUint32 k = 0 ; k < m_harrows.size() ; ++k)
        m_doc->harrows.push_back(m_harrows[k]);



    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}




AddHArrowCommand::AddHArrowCommand(TimingDocument *doc, HArrow newha)
    : wxCommand(true, _T("added a horizontal  arrow")),
    m_doc(doc),
    m_newha(newha)
{}
AddHArrowCommand::~AddHArrowCommand(){}
bool AddHArrowCommand::Do(void)
{
    m_doc->harrows.push_back(m_newha);

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool AddHArrowCommand::Undo(void)
{
    m_doc->harrows.pop_back();

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}


ChangeHArrowTextPosCommand::ChangeHArrowTextPosCommand(TimingDocument *doc, wxInt32 editingNumber, wxInt32 xoff, wxInt32 yoff, wxInt32 gridoff)
    : wxCommand(true, _("change position of text")),
    m_doc(doc),
    m_nmbr(editingNumber),
    m_off(xoff, yoff),
    m_gridoff(gridoff)
{}
ChangeHArrowTextPosCommand::~ChangeHArrowTextPosCommand(){}
bool ChangeHArrowTextPosCommand::Do(void)
{
    wxPoint tmp = m_doc->harrows[m_nmbr].textoffset;
    m_doc->harrows[m_nmbr].textoffset = m_off;
    m_off = tmp;

    wxInt32 tempc = m_doc->harrows[m_nmbr].textgridoffset;
    m_doc->harrows[m_nmbr].textgridoffset = m_gridoff;
    m_gridoff = tempc;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool ChangeHArrowTextPosCommand::Undo(void)
{
    return Do();
}
ChangeTextCommand::ChangeTextCommand(TimingDocument *doc, wxInt32 number, wxString text)
    : wxCommand(true, _("change Text")),
    m_doc(doc),
    m_nmbr(number),
    m_newText(text)
{}
ChangeTextCommand::~ChangeTextCommand()
{}
bool ChangeTextCommand::Do(void)
{
    wxString tmp;
    wxString str;
    wxInt32 n = 0;
    bool found = false;
    /*for ( ; n < m_doc->signals.size() && !found ; ++n )
    {
        if ( n == m_nmbr )
        {
            tmp = m_doc->signals[n].name;
            m_doc->signals[n].name = m_newText;
            m_newText = tmp;
            found = true;
        }
    }*/
    for ( wxUint32 k = 0 ; k < m_doc->signals.size() && !found ; ++k)
    {
        if ( n == m_nmbr )
        {
            tmp = m_doc->signals[k].name;
            m_doc->signals[k].name = m_newText;
            m_newText = tmp;
            found = true;
        }
        ++n;

        if ( m_doc->signals[k].IsBus )
        {
            if ( n == m_nmbr )
            {
                tmp = m_doc->signals[k].buswidth;
                m_doc->signals[k].buswidth = m_newText;
                m_newText = tmp;
                found = true;
            }
            ++n;
        }
    }
    for ( wxUint32 k = 0 ; k < m_doc->signals.size() && !found ; ++k)
    {
        if ( !m_doc->signals[k].IsClock && m_doc->signals[k].IsBus )
        {
            for ( wxUint32 i = 0 ; i < (wxUint32)m_doc->length && !found ; ++i )
            {
                if ( m_doc->signals[k].values[i] == one ||
                     m_doc->signals[k].values[i] == zero )
                {
                    if ( i == 0 || m_doc->signals[k].values[i] != m_doc->signals[k].values[i-1] )
                    {
                        if ( n == m_nmbr )
                        {
                            tmp = m_doc->signals[k].TextValues[i];
                            m_doc->signals[k].TextValues[i] = m_newText;
                            m_newText = tmp;
                            found = true;
                        }
                        n++;
                    }
                }
            }
        }
    }
    for ( wxUint32 k = 0 ; k < m_doc->harrows.size() && !found ; ++k, n++ )
    {
        if ( n == m_nmbr )
        {
            tmp = m_doc->harrows[k].text;
            m_doc->harrows[k].text = m_newText;
            m_newText = tmp;
            found = true;
        }
    }


    if ( found )
    {
        m_doc->Modify(true);
        m_doc->UpdateAllViews();
        return true;
    }
    else
        return false;

}
bool ChangeTextCommand::Undo(void)
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

ChangeAxisSettings::ChangeAxisSettings(TimingDocument *doc, wxInt8 unit, wxInt32 ticklenth, wxInt32 tacklength, wxInt32 offset):
    wxCommand(true, _("Change time-axis settings") ),
    m_doc(doc),
    m_unit(unit),
    m_ticklength(ticklenth),
    m_tacklength(tacklength),
    m_offset(offset)
{}
ChangeAxisSettings::~ChangeAxisSettings(){}
bool ChangeAxisSettings::Do(void)
{
    wxInt8 tmp8 = m_doc->TickLengthUnit;
    m_doc->TickLengthUnit = m_unit;
    m_unit = tmp8;

    wxInt32 tmp = m_doc->TickLength;
    m_doc->TickLength = m_ticklength;
    m_ticklength = tmp;

    tmp = m_doc->TackLength;
    m_doc->TackLength = m_tacklength;
    m_tacklength = tmp;

    tmp = m_doc->timeOffset;
    m_doc->timeOffset = m_offset;
    m_offset = tmp;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool ChangeAxisSettings::Undo(void)
{
    return Do();
}

ChangeTimeCompressor::ChangeTimeCompressor(TimingDocument *doc, wxInt32 index, wxInt32 time)
    :wxCommand(true, _("Change time compressor length") ),
    m_doc(doc),
    m_time(time),
    m_index(index)
{}
bool ChangeTimeCompressor::Do(void)
{
    wxInt32 tmp = m_doc->discontlength[m_index];
    m_doc->discontlength[m_index] = m_time;
    m_time = tmp;

    m_doc->Modify(true);
    m_doc->UpdateAllViews();
    return true;
}
bool ChangeTimeCompressor::Undo(void)
{
    return Do();
}
ChangeTimeCompressor::~ChangeTimeCompressor(){}
