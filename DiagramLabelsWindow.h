#ifndef DIAGRAMLABELSWINDOW_H
#define DIAGRAMLABELSWINDOW_H

#include <vector>

#include <wx/scrolwin.h>

class TimingView;
//class TimingTextCtrl;
//class wxStaticLine;
class HoverDrawlet;
class LabelText;

class DiagramLabelsWindow : public wxScrolledWindow
{
public:
    DiagramLabelsWindow(TimingView *v, wxWindow* parent, wxScrolledWindow *scrollowner, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxHSCROLL | wxVSCROLL);
    virtual ~DiagramLabelsWindow();
private:
    DiagramLabelsWindow();
    void OnPaint( wxPaintEvent &event );
    void OnEraseBackground(wxEraseEvent &WXUNUSED(event) );
    //void OnSize( wxSizeEvent &WXUNUSED(event) );

private:
    //void OnChildFocus(wxChildFocusEvent &event);

    void OnMouseLeave(wxMouseEvent &event);
    void OnMouseEnter(wxMouseEvent &event);
    void OnMouse(wxMouseEvent &event);
    //void OnMouseWheel(wxMouseEvent &event);

    void OnKeyDown(wxKeyEvent &event);
    void OnKeyUp(wxKeyEvent &event);

    void Draw(wxDC &dc);

    void PaintBackground(wxDC &dc);

private:
    virtual wxColour GetBackgroundColour() const;
    wxColour GetLineColour() const;
    wxColour GetTextColour() const;
    const unsigned int GetOffsetToLabelTextCtrl()const;

public:
    void Update();
    void SetDrawlet(HoverDrawlet *drawlet);
    void RemoveDrawlet();
//    void UpdateSizeToMatchLabels();
//    void UpdateSizeOfLines();

public:
    virtual void ScrollWindow(int dx, int dy, const wxRect* rect = NULL);

private:
    std::vector<LabelText*> textctrls;

private:
    wxScrolledWindow   *m_owner;
    TimingView         *m_view;

    HoverDrawlet           *m_drawlet;

private:
    void ScaleDC(wxDC &dc);
//    std::vector<TimingTextCtrl *> SignalLabels;
//    std::vector<wxStaticLine *> Divisors;

    DECLARE_DYNAMIC_CLASS(DiagramLabelsWindow)
    DECLARE_EVENT_TABLE()
};

#endif // DIAGRAMLABELSWINDOW_H
