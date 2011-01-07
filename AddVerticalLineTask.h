#ifndef ADDVERTICALLINETASK_H
#define ADDVERTICALLINETASK_H

#include "Task.h"


class AddVerticalLineTask : public Task
{
public:
    AddVerticalLineTask(TimingView *view, DiagramLabelsWindow *labelsWin, DiagramAxisWindow *axisWin, DiagramWavesWindow *waveWin);
    virtual ~AddVerticalLineTask();

    virtual void LabelsMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void WavesMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void AxisMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void LabelsKey(const wxKeyEvent &event, bool down);
    virtual void WavesKey(const wxKeyEvent &event, bool down);
    virtual void AxisKey(const wxKeyEvent &event, bool down);

    //virtual wxInt32 GetSelectedSignalNumber();
    //virtual wxInt32 GetSelectedDiscontinuity();
    //virtual void Copy();
    //virtual void Cut();
    virtual void Paste();
    //virtual void Delete();
    //virtual void SelectAll();
    virtual bool CanPaste();
    //virtual bool CanDelete();
    //virtual bool HasActiveSelection();
private:
    typedef enum{
        waitingFirstPoint,
        waitingSecondPoint
    }states;
protected:
private:
    states state;

    void EndTask();
    void NonInvolvedWindowMouse(const wxMouseEvent &event);
};

#endif // ADDVERTICALLINETASK_H
