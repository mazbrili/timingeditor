#include "TimingSubFrame.h"

#include <wx/cmdproc.h>
#include <wx/icon.h>
#include <wx/menu.h>


#include "TimingApp.h"
#include "enumers.h"

TimingSubFrame::TimingSubFrame(wxDocument *doc, wxView* view, wxMDIParentFrame *parent)
:wxDocMDIChildFrame(doc, view, parent, wxID_ANY, "Child Frame")
{
    //ctor

    #ifdef __WXMSW__
        SetIcon(wxIcon(_T("wxwinicon")));
    #else
        SetIcon(wxIcon(_T("wxwinicon.xpm")));
    #endif

    /// Make a menubar
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(wxID_NEW, _("&New...\tCtrl-N"), _("Create a new file"));
    file_menu->Append(wxID_OPEN, _("&Open...\tCtrl-O"), _("Open a existing file"));
    file_menu->Append(wxID_CLOSE, _("&Close\tCtrl-W"), _("Close the active file"));
    file_menu->Append(wxID_SAVE, _("&Save\tCtrl-S"), _("Save the active file"));
    file_menu->Append(wxID_SAVEAS, _("Save &As..."), _("Save the active file under a different name"));
    file_menu->AppendSeparator();
    //file_menu->Append(wxID_PRINT, _("&Print...\tCtrl-P"), _("Print the active file"));
    //file_menu->Append(wxID_PRINT_SETUP, _("Print &Setup..."), _("Prepare the printer"));
    //file_menu->Append(wxID_PREVIEW, _("Print Pre&view"), _("Preview the print of the active file"));
    //file_menu->AppendSeparator();
    file_menu->Append(TIMING_ID_EXPORT_BITMAP, _("Export to PNG"), _("export diagram to a bitmap in PNG format"));
    file_menu->Append(TIMING_ID_EXPORT_SVG, _("Export to SVG"), _("export diagram to a scalable vector graphic"));
    file_menu->Append(TIMING_ID_EXPORT_PS, _("Export to PS"), _("export diagram to postscript format"));
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, _("&Quit\tAlt-F4"), _("Quit the application"));
    //file history
    wxGetApp().GetDocManager()->FileHistoryUseMenu(file_menu);
    wxGetApp().GetDocManager()->FileHistoryAddFilesToMenu(file_menu);
    ///  don't forget to remove the FileHistoryMeni from the file menu while closing the view: FileHistoryRemoveMenu(fileMenu);



    wxMenu *edit_menu = new wxMenu;
    edit_menu->Append(wxID_UNDO, _("&Undo\tCtrl-Z"), _("Undo the last operation"));
    edit_menu->Append(wxID_REDO, _("&Redo\tCtrl-Shift-Z"), _("Redo the last operation"));
    edit_menu->AppendSeparator();
    edit_menu->Append(wxID_CUT, _("Cu&t\tCtrl-X"), _("Cut"));
    edit_menu->Append(wxID_COPY, _("&Copy\tCtrl-C"), _("Copy"));
    edit_menu->Append(wxID_PASTE, _("&Paste\tCtrl-V"), _("Paste"));
    edit_menu->Append(TIMING_ID_DELETE, _("&Delete"), _("Delete"));
    edit_menu->AppendSeparator();
    edit_menu->Append(TIMING_ID_ADD_CLOCK, _("&Add Clock"), _("&Add Clock") );
    edit_menu->Append(TIMING_ID_ADD_SIGNAL, _("Add &Signal"), _("Add &Signal"));
    edit_menu->Append(TIMING_ID_ADD_BUS, _("Add &Bus"), _("Add &Bus"));
    //edit_menu->AppendSeparator();
    doc->GetCommandProcessor()->SetEditMenu(edit_menu);

    wxMenu *panel_menu = new wxMenu;
    panel_menu->Append(TIMING_ID_MENUITEM_CLOCK_TYPE, _("Clock-type"), wxEmptyString, wxITEM_CHECK);
    panel_menu->Append(TIMING_ID_MENUITEM_TIME_COMPRESSOR, _("Time compressor"), wxEmptyString, wxITEM_CHECK);
    panel_menu->Append(TIMING_ID_MENUITEM_AXIS_TIME, _("Axis/Time"), wxEmptyString, wxITEM_CHECK);
    panel_menu->Append(TIMING_ID_MENUITEM_TRANSITION, _("Transition"), wxEmptyString, wxITEM_CHECK);
    panel_menu->AppendSeparator();
    panel_menu->Append(TIMING_ID_MENUITEM_DEFAULT,_("Default"));

    wxMenu *viewMenu = new wxMenu;
    viewMenu->Append(TIMING_ID_GLASS_P, _("Zoom in\tF7"), _("Zoom in the Document"));
    viewMenu->Append(TIMING_ID_GLASS_N, _("Zoom out\tF6"), _("Zoom out the Document"));
    viewMenu->AppendSeparator();
    viewMenu->Append(wxID_ANY, _("Panel"),panel_menu, _("Show or Hide Panel"));

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, _("&About\tF1"), _("Show info about this application"));
    help_menu->Append(TIMING_ID_TIP, _("Tip"), _("Tips on using TimingEditor") );
    help_menu->Append(TIMING_ID_HELP, _("Help"), _("Open help pages"));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _("&File"));
    menu_bar->Append(edit_menu, _("&Edit"));
    menu_bar->Append(viewMenu, _("&View"));
    menu_bar->Append(help_menu, _("&Help"));

    /// Associate the menu bar with the frame
    SetMenuBar(menu_bar);

}

