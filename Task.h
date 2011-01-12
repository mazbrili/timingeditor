#ifndef BASETASK_H
#define BASETASK_H

#include <wx/event.h>

class TimingView;
class DiagramLabelsWindow;
class DiagramAxisWindow;
class DiagramWavesWindow;
class TimingTextCtrl;
class Signal;

class Task
{
public:
    Task(TimingView *view, DiagramLabelsWindow *labelsWin, DiagramAxisWindow *axisWin, DiagramWavesWindow *waveWin);
    virtual ~Task();

    // interface:
    virtual void InitTask() = 0;

    virtual void LabelsMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void WavesMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void AxisMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void LabelsKey(const wxKeyEvent &event, bool down);
    virtual void WavesKey(const wxKeyEvent &event, bool down);
    virtual void AxisKey(const wxKeyEvent &event, bool down);

    virtual wxInt32 GetSelectedSignalNumber();
    virtual wxInt32 GetSelectedDiscontinuity();
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();
    virtual void Delete();
    virtual void SelectAll();
    virtual bool CanPaste();
    virtual bool CanDelete();
    virtual bool HasActiveSelection();
    virtual void TextHasFocus(TimingTextCtrl *ctrl);

public:
    // common methods
    wxInt32 GetTickFromPosition(const wxPoint &pos);
    bool IsOverWaves(const wxPoint &pos);

private:
    void Init();
protected:
    TimingView          *m_view;
    DiagramLabelsWindow *m_labelsWin;
    DiagramAxisWindow   *m_axisWin;
    DiagramWavesWindow  *m_waveWin;
};

#endif // BASETASK_H