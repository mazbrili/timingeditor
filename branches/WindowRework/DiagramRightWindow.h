#ifndef DIAGRAMRIGHTWINDOW_H
#define DIAGRAMRIGHTWINDOW_H

#include <wx/scrolwin.h>

class DiagramLabelsWindow;
class DiagramAxisWindow;
class DiagramWaveWindow;
class TimingView;

class DiagramRightWindow : public wxScrolledWindow
{
public:
    DiagramRightWindow(){}
    bool Create(TimingView *v, wxWindow *parent, DiagramLabelsWindow *labels, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize/*, long style = wxHSCROLL | wxVSCROLl*/);
    virtual ~DiagramRightWindow(){}


public:
    void Update();

private:
    void OnPaint( wxPaintEvent &event );
    void OnSize( wxSizeEvent &event );

    TimingView           *m_view;
    DiagramAxisWindow    *m_axis;
    DiagramWaveWindow    *m_waves;
private:
    DECLARE_DYNAMIC_CLASS(DiagramRightWindow)
    DECLARE_EVENT_TABLE()
};

#endif // DIAGRAMRIGHTWINDOW_H
