#ifndef DiagramWavesWindow_H
#define DiagramWavesWindow_H

#include <wx/panel.h>

#include <vector>


class wxScrolledWindow;
class DiagramAxisWindow;
class DiagramLabelsWindow;
class TimingView;
class HoverDrawlet;
class HorizontalArrowText;

class DiagramWavesWindow : public wxPanel
{
public:
    DiagramWavesWindow(TimingView *v, wxWindow* parent, wxScrolledWindow *scrollowner, DiagramAxisWindow *axis, DiagramLabelsWindow *labels, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
    virtual ~DiagramWavesWindow();
public:
    void Update();

    void SetDrawlet(HoverDrawlet *drawlet);
    void RemoveDrawlet();
    virtual bool SetCursor(const wxCursor &cursor, bool forchilds = false );
    void CalcUnscrolledPoint(wxPoint &point)const;
private:
    DiagramWavesWindow();
    void OnPaint( wxPaintEvent &event );
    void OnEraseBackground(wxEraseEvent &WXUNUSED(event) );
    void PaintBackground(wxDC &dc);
    void Draw(wxDC &dc);
    void DrawVerticalLines(wxDC &dc);
    void DrawHorizontalArrows(wxDC &dc);
    void DrawDiscontinuities(wxDC &dc);
    void DrawSignals(wxDC &dc);
    void DrawSignalSeparators(wxDC &dc);
    void ScrollWindow( int dx, int dy, const wxRect *rect );
    void OnSize(wxSizeEvent &event);


    void OnMouse(wxMouseEvent &event);
    void OnMouseEnter(wxMouseEvent &event);
    void OnMouseLeave(wxMouseEvent &event);

    void OnKeyDown(wxKeyEvent &event);
    void OnKeyUp(wxKeyEvent &event);
public:
    HorizontalArrowText    *GetHorizontalArrowTextCtrl(){return m_horizontalArrowTextCtrl;}
private:
    wxScrolledWindow        *m_owner;
    DiagramAxisWindow       *m_axis;
    DiagramLabelsWindow     *m_labels;
    TimingView              *m_view;

    HoverDrawlet            *m_drawlet;

    HorizontalArrowText     *m_horizontalArrowTextCtrl;
    std::vector<wxTextCtrl*> textctrls;

    DECLARE_DYNAMIC_CLASS(DiagramWavesWindow)
    DECLARE_EVENT_TABLE()
};

#endif // DiagramWavesWindow_H
