#ifndef DIAGRAMAXISWINDOW_H
#define DIAGRAMAXISWINDOW_H

#include <wx/panel.h>

class TimingView;
class HoverDrawlet;

class DiagramAxisWindow : public wxPanel
{
public:
    DiagramAxisWindow(TimingView *v, wxWindow* parent, wxScrolledWindow *scrollowner, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
    virtual ~DiagramAxisWindow();

public:
    void DoUpdate();

    void SetDrawlet(HoverDrawlet *drawlet);
    void RemoveDrawlet();

private:
    DiagramAxisWindow();
    void OnPaint( wxPaintEvent &event );
    void OnEraseBackground(wxEraseEvent &WXUNUSED(event) );
    void PaintBackground(wxDC &dc);
    void Draw(wxDC &dc);

    void ShiftDC(wxDC &dc);

private:
    void OnMouse(wxMouseEvent &event);
    void OnMouseLeave(wxMouseEvent &event);
    void OnMouseEnter(wxMouseEvent &event);

    void OnKeyDown(wxKeyEvent &event);
    void OnKeyUp(wxKeyEvent &event);

private:
    wxColour GetBackgroundColour()const;
    wxColour GetLineColour()const;
private:
    wxScrolledWindow       *m_owner;
    TimingView             *m_view;

    HoverDrawlet           *m_drawlet;

    DECLARE_DYNAMIC_CLASS(DiagramAxisWindow)
    DECLARE_EVENT_TABLE()
};

#endif // DIAGRAMAXISWINDOW_H
