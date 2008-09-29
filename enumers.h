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

#ifndef ENUMERS_H
#define ENUMERS_H
enum
{
    TIMING_ID_TIP = wxID_HIGHEST+1,
    TIMING_ID_GLASS_N,
    TIMING_ID_GLASS_P,
//    TIMING_ID_CHANGECLOCK,
//    TIMING_ID_DELETE_SIGNAL,
    TIMING_ID_ADD_CLOCK,
    TIMING_ID_ADD_SIGNAL,
    TIMING_ID_ADD_BUS,
    TIMING_ID_NEUTRAL,
    TIMING_ID_DISCONTINUATION,
    TIMING_ID_RULER,
    TIMING_ID_HARROW,
    TIMING_ID_EDITTEXT,
    TIMING_ID_EXPORT_BITMAP,
    TIMING_ID_EXPORT_SVG,
    TIMING_ID_EXPORT_PS,

    TIMING_ID_HELP,

    TIMING_ID_DELETE,


    TIMING_ID_PANEL_CLK_APPLY,
    TIMING_ID_PANEL_CLK_TXTPERIOD,
    TIMING_ID_PANEL_CLK_TXTDELAY,
    TIMING_ID_PANEL_CLK_SHADOW,

    TIMING_ID_PANEL_TRANS_CHECK10,
    TIMING_ID_PANEL_TRANS_APPLY,
    TIMING_ID_PANEL_TRANS_CHECK50,
    TIMING_ID_PANEL_TRANS_CHECK90,
    TIMING_ID_PANEL_TRANS_WIDTH,

    TIMING_ID_PANEL_AXIS_APPLY,
    TIMING_ID_PANEL_AXIS_TICKLENGTH,
    TIMING_ID_PANEL_AXIS_UNITCHOICE,
    TIMING_ID_PANEL_AXIS_TACKLENGTH,
    TIMING_ID_PANEL_AXIS_OFFSET,

    TIMING_ID_PANEL_TC_APPLY,
    TIMING_ID_PANEL_TC_TXTFIELD,
};

#endif

