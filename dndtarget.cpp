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
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "dndtarget.h"
#include "TimingWindow.h"
//#include "DiagramSplitterWindow.h"

DragnDropTextTarget::DragnDropTextTarget(TimingWindow *owner):
    wxDropTarget(new wxTextDataObject()),
    m_owner(owner)
{
}
wxDragResult DragnDropTextTarget::OnEnter(wxCoord x, wxCoord y, wxDragResult def)
{
    m_owner->OnDragEnter();
    return OnDragOver(x, y, def);
}
void DragnDropTextTarget::OnLeave()
{
    m_owner->OnDragLeave();
}
wxDragResult DragnDropTextTarget::OnData(wxCoord x, wxCoord y, wxDragResult def)
{
    if ( !GetData() )
    {
        wxMessageBox(_T("Failed to get drag and drop data"), _T("Bemerkung") );
        return wxDragNone;
    }

    wxString str = ((wxTextDataObject *)GetDataObject())->GetText();

    if ( m_owner->OnDrop(wxPoint(x,y), str ) )
        return( def );
    else
        return wxDragNone;
}
wxDragResult DragnDropTextTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
    return( m_owner->OnDragOver(wxPoint(x, y), def) );
}



