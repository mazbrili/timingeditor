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

#ifndef _DNDTARGET_H_
#define _DNDTARGET_H_

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wx/dnd.h"

class TimingWindow;
//class DiagramSplitterWindow;
/*class DnDText : public wxTextDropTarget
{
public:
    DnDText(MyCanvas *owner) { m_owner = owner; }

    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& text);

private:
    MyCanvas *m_owner;
};*/


class DragnDropTextTarget : public wxDropTarget
{
public:
    DragnDropTextTarget(TimingWindow *owner);
    ~DragnDropTextTarget(){}
    virtual wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult def);
    virtual void         OnLeave();
    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);
    virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
private:
    TimingWindow *m_owner;
};

#endif

