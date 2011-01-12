#ifndef MAINTASK_H
#define MAINTASK_H

#include "Task.h"
#include <wx/event.h>

class TimingView;
class DiagramLabelsWindow;
class DiagramAxisWindow;
class DiagramWavesWindow;
class TimingTextCtrl;
class Signal;

class MainTask : public Task
{
public:
    MainTask(TimingView *view, DiagramLabelsWindow *labelsWin, DiagramAxisWindow *axisWin, DiagramWavesWindow *waveWin);
    virtual ~MainTask();

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


    virtual void InitTask();
private:
    void AddSignal(Signal *sig);

};

#endif // MAINTASK_H

