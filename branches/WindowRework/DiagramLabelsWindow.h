#ifndef DIAGRAMLABELSWINDOW_H
#define DIAGRAMLABELSWINDOW_H

#include <vector>

#include <wx/scrolwin.h>

class TimingView;
class TimingTextCtrl;
class wxStaticLine;

class DiagramLabelsWindow : public wxScrolledWindow
{
public:
    DiagramLabelsWindow(TimingView *v, wxWindow* parent, wxScrolledWindow *scrollowner, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxHSCROLL | wxVSCROLL);
    virtual ~DiagramLabelsWindow();
private:
    DiagramLabelsWindow();
    void OnPaint( wxPaintEvent &event );
    void OnEraseBackground(wxEraseEvent &WXUNUSED(event) );
    void OnSize( wxSizeEvent &WXUNUSED(event) );
//    void OnMousewheel(wxMouseEvent &event);

    void Draw(wxDC &dc);

    void PaintBackground(wxDC &dc);

private:
    virtual wxColour GetBackgroundColour() const;
    wxColour GetLineColour() const;
    wxColour GetTextColour() const;
public:

    void Update();
//    void UpdateSizeToMatchLabels();
//    void UpdateSizeOfLines();
private:

private:
    wxScrolledWindow   *m_owner;
    TimingView         *m_view;


//    std::vector<TimingTextCtrl *> SignalLabels;
//    std::vector<wxStaticLine *> Divisors;

    DECLARE_DYNAMIC_CLASS(DiagramLabelsWindow)
    DECLARE_EVENT_TABLE()
};

#endif // DIAGRAMLABELSWINDOW_H
