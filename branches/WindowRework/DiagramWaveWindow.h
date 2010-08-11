#ifndef DIAGRAMWAVEWINDOW_H
#define DIAGRAMWAVEWINDOW_H

#include <wx/panel.h>


class wxScrolledWindow;
class DiagramAxisWindow;
class DiagramLabelsWindow;
class TimingView;

class DiagramWaveWindow : public wxPanel
{
public:
    DiagramWaveWindow(TimingView *v, wxWindow* parent, wxScrolledWindow *scrollowner, DiagramAxisWindow *axis, DiagramLabelsWindow *labels, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
    virtual ~DiagramWaveWindow();
public:
    void Update();
private:
    DiagramWaveWindow();
    void OnPaint( wxPaintEvent &event );
    void OnEraseBackground(wxEraseEvent &WXUNUSED(event) );
    void PaintBackground(wxDC &dc);
    void Draw(wxDC &dc);
    void DrawVerticalLines(wxDC &dc);
    void DrawHorizontalArrows(wxDC &dc);
    void ScrollWindow( int dx, int dy, const wxRect *rect );
private:
    virtual wxColour GetBackgroundColour() const;
    wxColour GetLineColour() const;
    wxColour GetTextColour() const;
    wxColour GetShadowColour() const;
    wxColour GetUndefinedSignalColour() const;
private:
    wxScrolledWindow        *m_owner;
    DiagramAxisWindow       *m_axis;
    DiagramLabelsWindow     *m_labels;
    TimingView              *m_view;

    DECLARE_DYNAMIC_CLASS(DiagramWaveWindow)
    DECLARE_EVENT_TABLE()
};

#endif // DIAGRAMWAVEWINDOW_H
