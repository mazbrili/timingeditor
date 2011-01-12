#ifndef ADDDISCONTINUITYTASK_H
#define ADDDISCONTINUITYTASK_H

#include "Task.h"

class AddDiscontinuityTask : public Task
{
public:
    AddDiscontinuityTask(TimingView *view, DiagramLabelsWindow *labelsWin, DiagramAxisWindow *axisWin, DiagramWavesWindow *waveWin);
    virtual ~AddDiscontinuityTask();

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
    //virtual void TextHasFocus(TimingTextCtrl *ctrl);
    virtual void InitTask();

private:
    void EndTask();

protected:
private:

    void HandleKey(const wxKeyEvent &event, bool down);

    void DoCheckWhenMouseDown(const wxPoint &pos);
    //void NonInvolvedWindowMouse(const wxMouseEvent &event);
    //void DoCheckMoving(const wxPoint &pos);
    //void DoCheckWhenMouseUp(const wxPoint &pos);
    //void SetDrawlet();
private:

};

#endif // ADDDISCONTINUITYTASK_H
