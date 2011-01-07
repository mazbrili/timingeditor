#ifndef BASETASK_H
#define BASETASK_H

#include <wx/clipbrd.h>
#include <wx/event.h>

#include "TimingView.h"
#include "DataObject.h"

#include "cmd.h"
#include "DiagramAxisWindow.h"
#include "DiagramLabelsWindow.h"
#include "DiagramWavesWindow.h"

class DiagramLabelsWindow;
class DiagramAxisWindow;
class DiagramWavesWindow;

class Signal;

class Task
{
public:
    Task(TimingView *view, DiagramLabelsWindow *labelsWin, DiagramAxisWindow *axisWin, DiagramWavesWindow *waveWin);
    virtual ~Task();

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
private:
    void AddSignal(Signal *sig);

private:
protected:
    TimingView          *m_view;
    DiagramLabelsWindow *m_labelsWin;
    DiagramAxisWindow   *m_axisWin;
    DiagramWavesWindow  *m_waveWin;
};

#endif // BASETASK_H
