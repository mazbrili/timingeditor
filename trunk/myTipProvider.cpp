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

#include "myTipProvider.h"

myTipProvider::myTipProvider(size_t currentTip)
    :wxTipProvider(currentTip)
{
    //m_tips.Add(_T(""));
    m_tips.Add(_T("You can click on the signal to change its state."));
    m_tips.Add(_T("You have unlimited undo/redo capabilities."));
    m_tips.Add(_T("You can click on the bullet on the left to select a signal."));
    m_tips.Add(_T("You can move the bullet on the left to move the trace horizontally."));
    m_tips.Add(_T("You can right-click on the bullets on the left to delete a trace."));
    m_tips.Add(_T("You can add an clock/signal/bus through menu: Edit->Add... ."));
    m_tips.Add(_T("You can add/remove time compressors with the triangle-tool by clicking on the bottom axis."));
    m_tips.Add(_T("You can change the distance at the top/bottom of a signal when it is active."));
    m_tips.Add(_T("You can activate a signal by clicking on the bullet on the left."));
    m_tips.Add(_T("You can add vertical lines with the ruler-tool."));
    m_tips.Add(_T("You can edit text with the I-beam-tool."));
    m_tips.Add(_T("You can \"File->Export to SVG\" to export a vector graphic of the diagram for your documentation."));
}
wxString myTipProvider::GetTip()
{
    wxString res = m_tips[m_currentTip];

    m_currentTip++;

    if ( m_currentTip >= m_tips.GetCount() )    m_currentTip = 0;

    return res;
}

//size_t myTipProvider::GetCurrentTip() const
//{
//    return m_currentTip;
//}
