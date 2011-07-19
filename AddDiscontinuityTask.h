#ifndef ADDDISCONTINUITYTASK_H
#define ADDDISCONTINUITYTASK_H

#include "Task.h"

class AddDiscontinuityTask : public Task
{
public:
    AddDiscontinuityTask(const Task *task);
    virtual ~AddDiscontinuityTask();

    virtual void LabelsMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void WavesMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void AxisMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void LabelsKey(const wxKeyEvent &event, bool down);
    virtual void WavesKey(const wxKeyEvent &event, bool down);
    virtual void AxisKey(const wxKeyEvent &event, bool down);

private:

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

