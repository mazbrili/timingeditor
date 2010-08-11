#ifndef DIAGRAMSPLITTERWINDOW_H
#define DIAGRAMSPLITTERWINDOW_H

#include <wx/splitter.h>


class ClockSettingsPanel;
class TransitionSettingsPanel;
class AxisSettingsPanel;
class TimeCompressorSettingsPanel;

class TimingView;
class DiagramLabelsWindow;
class DiagramRightWindow;

class DiagramSplitterWindow : public wxSplitterWindow
{
friend class TimingView;
public:
    DiagramSplitterWindow(TimingView *v, wxWindow* parent, wxWindowID id,
        //ClockSettingsPanel *clkpanel, TransitionSettingsPanel *trnpanel,  AxisSettingsPanel *axspanel, TimeCompressorSettingsPanel* tcpanel,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style=wxSP_3D);
    virtual ~DiagramSplitterWindow(){}
private:
    DiagramSplitterWindow(){};




//    ClockSettingsPanel *ClkSetPanel;
//    TransitionSettingsPanel *TranSetPanel;
//    AxisSettingsPanel *AxisSetPanel;
//    TimeCompressorSettingsPanel *TmeCmprssrPanel;


    TimingView *view;
    DiagramLabelsWindow *m_labels;
    DiagramRightWindow  *m_right;

public:
    void Update();

private:
    DECLARE_DYNAMIC_CLASS(DiagramSplitterWindow)
    DECLARE_EVENT_TABLE()

};

#endif // DIAGRAMSPLITTERWINDOW_H
