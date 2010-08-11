#include "DiagramSplitterWindow.h"

#include "DiagramRightWindow.h"
#include "DiagramLabelsWindow.h"

IMPLEMENT_DYNAMIC_CLASS(DiagramSplitterWindow, wxSplitterWindow)

BEGIN_EVENT_TABLE(DiagramSplitterWindow, wxSplitterWindow)
END_EVENT_TABLE()

//DiagramSplitterWindow::DiagramSplitterWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
DiagramSplitterWindow::DiagramSplitterWindow(TimingView *v, wxWindow* parent, wxWindowID id,
        //ClockSettingsPanel *clkpanel, TransitionSettingsPanel *trnpanel, AxisSettingsPanel *axspanel, TimeCompressorSettingsPanel* tcpanel,
        const wxPoint& pos, const wxSize& size, long style)
: wxSplitterWindow( parent, id, pos, size, style | wxSUNKEN_BORDER | wxSP_3D ),

//    ClkSetPanel(clkpanel),
//    TranSetPanel(trnpanel),
//    AxisSetPanel(axspanel),
//    TmeCmprssrPanel(tcpanel),


    view(v)
{
    //ctor
    SetSplitMode(wxSPLIT_VERTICAL );


    m_right = new DiagramRightWindow();


    wxPanel *left = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
    m_labels = new DiagramLabelsWindow(view, left, m_right , wxID_ANY, wxDefaultPosition, wxSize(60,wxDefaultCoord) );

    wxCoord HeightOfAxis = 60;
    wxBoxSizer *lsizer = new wxBoxSizer(wxVERTICAL);
    lsizer->Add(60,HeightOfAxis);
    lsizer->Add(m_labels, 1, wxEXPAND);
    left->SetAutoLayout(true);
    left->SetSizer(lsizer);


    m_right->Create(view, this, m_labels, wxID_ANY, wxDefaultPosition, wxDefaultSize );


    SplitVertically(left, m_right , 100);
    SetMinimumPaneSize(10);
}

void DiagramSplitterWindow::Update()
{
    //m_labels->Refresh();
    m_labels->Update();
    m_right->Update();
}
