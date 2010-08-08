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

#ifndef __MyTipProvider__
#define __MyTipProvider__

#include <wx/tipdlg.h>

class myTipProvider : public wxTipProvider
{
    public:
        //Constructor.
        //currentTip: The starting tip index.
        myTipProvider(size_t currentTip);


        //Return the text of the current tip and pass to the next one.
        //This function is pure virtual, it should be implemented in
        //the derived classes.
        virtual wxString GetTip();


        //Returns the index of the current tip
        //(i.e. the one which would be returned by GetTip).
        //size_t GetCurrentTip() const;

    private:
        //size_t m_currentTip;
        wxArrayString m_tips;
};

#endif
