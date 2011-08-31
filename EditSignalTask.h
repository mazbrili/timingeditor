#ifndef EDITSIGNALTASK_H
#define EDITSIGNALTASK_H

#include "Task.h"


class EditSignalTask : public Task
{
public:
    EditSignalTask(const Task *task, unsigned int sig, wxInt32 tick, bool leftDown = true);
    virtual ~EditSignalTask();

    virtual void LabelsMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void WavesMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void AxisMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void LabelsKey(const wxKeyEvent &event, bool down);
    virtual void WavesKey(const wxKeyEvent &event, bool down);
    virtual void AxisKey(const wxKeyEvent &event, bool down);

protected:
private:
    void SetDrawlets();
    void OnMouse(const wxMouseEvent &event);
    void OnKey(const wxKeyEvent &event, bool down);
private:
    unsigned int m_sig;
    bool m_leftDown;
    wxInt32 m_startTick;
    bool m_onEdge;
};

#endif // EDITSIGNALTASK_H
