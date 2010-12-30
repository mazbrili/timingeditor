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

    void LabelsMouse(const wxMouseEvent &event, const wxPoint &pos);
    void WavesMouse(const wxMouseEvent &event, const wxPoint &pos);
    void AxisMouse(const wxMouseEvent &event, const wxPoint &pos);

    wxInt32 GetSelectedSignalNumber();
    wxInt32 GetSelectedDiscontinuity();
    void Copy();
    void Cut();
    void Paste();
    void Delete();
    void SelectAll();
    bool CanPaste();
    bool CanDelete();
    bool HasActiveSelection();
private:
    void AddSignal(Signal *sig);

protected:
private:
    TimingView          *m_view;
    DiagramLabelsWindow *m_labelsWin;
    DiagramAxisWindow   *m_axisWin;
    DiagramWavesWindow   *m_waveWin;
};

#endif // BASETASK_H
