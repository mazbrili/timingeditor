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
    //virtual bool CanEditText(){return false;}

private:
    void EndTask();

    typedef enum{
        waitingFirstPoint,
        waitingSecondPoint
    }states;
protected:
private:
    states state;

    void NonInvolvedWindowMouse(const wxMouseEvent &event);
    void HandleKey(const wxKeyEvent &event, bool down);

    void DoCheckWhenMouseDown(const wxPoint &pos);
    void DoCheckMoving(const wxPoint &pos);
    void DoCheckWhenMouseUp(const wxPoint &pos);
    void SetDrawlet();
private:
    wxInt32 yStartPos; // above signal number
    wxInt32 yEndPos; // below signal number
    wxInt32 xpos;
    wxUint8 xposoffset;
};

#endif // ADDVERTICALLINETASK_H
