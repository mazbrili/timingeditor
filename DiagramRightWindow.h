#ifndef DIAGRAMRIGHTWINDOW_H
#define DIAGRAMRIGHTWINDOW_H

#include <wx/scrolwin.h>

class DiagramLabelsWindow;
class DiagramAxisWindow;
class DiagramWavesWindow;
class TimingView;

class DiagramRightWindow : public wxScrolledWindow
{
public:
    DiagramRightWindow(){}
    bool Create(TimingView *v, wxWindow *parent, DiagramLabelsWindow *labels, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize/*, long style = wxHSCROLL | wxVSCROLl*/);
    virtual ~DiagramRightWindow(){}


public:
    void Update();
    DiagramAxisWindow *GetAxisWindow();
    DiagramWavesWindow *GetWavesWindow();

private:
    void OnPaint( wxPaintEvent &event );
    void OnSize( wxSizeEvent &event );

    TimingView           *m_view;
    DiagramAxisWindow    *m_axis;
    DiagramWavesWindow   *m_waves;
private:
    DECLARE_DYNAMIC_CLASS(DiagramRightWindow)
    DECLARE_EVENT_TABLE()
};

#endif // DIAGRAMRIGHTWINDOW_H
