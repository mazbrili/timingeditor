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

#ifndef TIMINGAPP_H
#define TIMINGAPP_H

#include <wx/app.h>

#include "wx/mdi.h"
#include "wx/docview.h"
#include "wx/docmdi.h"

class wxDocManager;
class wxConfig;
class TimingMainFrame;

class TimingApp : public wxApp
{
    public:
        TimingApp(void);
        virtual bool OnInit();

        TimingMainFrame *GetMainFrame(void){ return mainframe;}
        wxConfig *GetConfig(void){return m_config;}
        wxDocManager *GetDocManager(void){return m_docManager;}
        wxInt32 OnExit(void);

        wxDocMDIChildFrame *CreateChildFrame(wxDocument *doc, wxView *view);


    protected:
        TimingMainFrame *mainframe;
    private:
        wxDocManager *m_docManager;
        wxConfig *m_config;
};

DECLARE_APP(TimingApp)
#endif // TIMINGAPP_H
