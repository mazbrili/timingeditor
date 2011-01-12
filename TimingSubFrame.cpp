#include "TimingSubFrame.h"

#include <wx/cmdproc.h>

#include "TimingApp.h"
#include "enumers.h"


TimingSubFrame::TimingSubFrame(wxDocument *doc, wxView* view, wxMDIParentFrame *parent)
:wxDocMDIChildFrame(doc, view, parent, wxID_ANY, _T("Child Frame"))
{
    //ctor

    #ifdef __WXMSW__
        SetIcon(wxIcon(_T("wxwinicon")));
    #else
        SetIcon(wxIcon(_T("wxwinicon.xpm")));
    #endif

    /// Make a menubar
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(wxID_NEW, _T("&New...\tCtrl-N"), _T("Create a new file"));
    file_menu->Append(wxID_OPEN, _T("&Open...\tCtrl-O"), _T("Open a existing file"));
    file_menu->Append(wxID_CLOSE, _T("&Close\tCtrl-W"), _T("Close the active file"));
    file_menu->Append(wxID_SAVE, _T("&Save\tCtrl-S"), _T("Save the active file"));
    file_menu->Append(wxID_SAVEAS, _T("Save &As..."), _T("Save the active file under a different name"));
    file_menu->AppendSeparator();
    //file_menu->Append(wxID_PRINT, _T("&Print...\tCtrl-P"), _T("Print the active file"));
    //file_menu->Append(wxID_PRINT_SETUP, _T("Print &Setup..."), _T("Prepare the printer"));
    //file_menu->Append(wxID_PREVIEW, _T("Print Pre&view"), _T("Preview the print of the active file"));
    //file_menu->AppendSeparator();
    file_menu->Append(TIMING_ID_EXPORT_BITMAP, _T("Export to PNG"), _T("export diagram to a bitmap in PNG format"));
    file_menu->Append(TIMING_ID_EXPORT_SVG, _T("Export to SVG"), _T("export diagram to a scalable vector graphic"));
    file_menu->Append(TIMING_ID_EXPORT_PS, _T("Export to PS"), _T("export diagram to postscript format"));
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, _T("&Quit\tAlt-F4"), _T("Quit the application"));
    //file history
    wxGetApp().GetDocManager()->FileHistoryUseMenu(file_menu);
    wxGetApp().GetDocManager()->FileHistoryAddFilesToMenu(file_menu);
    ///  don't forget to remove the FileHistoryMeni from the file menu while closing the view: FileHistoryRemoveMenu(fileMenu);



    wxMenu *edit_menu = new wxMenu;
    edit_menu->Append(wxID_UNDO, _T("&Undo\tCtrl-Z"), _T("Undo the last operation"));
    edit_menu->Append(wxID_REDO, _T("&Redo\tCtrl-Shift-Z"), _T("Redo the last operation"));
    edit_menu->AppendSeparator();
    edit_menu->Append(wxID_CUT, _T("Cu&t\tCtrl-X"), _T("Cut"));
    edit_menu->Append(wxID_COPY, _T("&Copy\tCtrl-C"), _T("Copy"));
    edit_menu->Append(wxID_PASTE, _T("&Paste\tCtrl-V"), _T("Paste"));
    edit_menu->Append(TIMING_ID_DELETE, _T("&Delete"), _T("Delete"));
    edit_menu->AppendSeparator();
    edit_menu->Append(TIMING_ID_ADD_CLOCK, _T("&Add Clock"), _T("&Add Clock") );
    edit_menu->Append(TIMING_ID_ADD_SIGNAL, _T("Add &Signal"), _T("Add &Signal"));
    edit_menu->Append(TIMING_ID_ADD_BUS, _T("Add &Bus"), _T("Add &Bus"));
    //edit_menu->AppendSeparator();
    doc->GetCommandProcessor()->SetEditMenu(edit_menu);

    wxMenu *panel_menu = new wxMenu;
    panel_menu->Append(TIMING_ID_MENUITEM_CLOCK_TYPE, _T("Clock-type"), wxEmptyString, wxITEM_CHECK);
    panel_menu->Append(TIMING_ID_MENUITEM_TIME_COMPRESSOR, _T("Time compressor"), wxEmptyString, wxITEM_CHECK);
    panel_menu->Append(TIMING_ID_MENUITEM_AXIS_TIME, _T("Axis/Time"), wxEmptyString, wxITEM_CHECK);
    panel_menu->Append(TIMING_ID_MENUITEM_TRANSITION, _T("Transition"), wxEmptyString, wxITEM_CHECK);
    panel_menu->AppendSeparator();
    panel_menu->Append(TIMING_ID_MENUITEM_DEFAULT,_T("Default"));

    wxMenu *viewMenu = new wxMenu;
    viewMenu->Append(TIMING_ID_GLASS_P, _T("Zoom in\tF7"), _T("Zoom in the Document"));
    viewMenu->Append(TIMING_ID_GLASS_N, _T("Zoom out\tF6"), _T("Zoom out the Document"));
    viewMenu->AppendSeparator();
    viewMenu->Append(wxID_ANY, _T("Panel"),panel_menu, _T("Show or Hide Panel"));

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, _T("&About\tF1"), _T("Show info about this application"));
    help_menu->Append(TIMING_ID_TIP, _T("Tip"), _T("Tips on using TimingEditor") );
    help_menu->Append(TIMING_ID_HELP, _T("Help"), _T("Open help pages"));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _T("&File"));
    menu_bar->Append(edit_menu, _T("&Edit"));
    menu_bar->Append(viewMenu, _T("&View"));
    menu_bar->Append(help_menu, _T("&Help"));

    /// Associate the menu bar with the frame
    SetMenuBar(menu_bar);

}

