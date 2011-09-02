#ifndef EDITSIGNALTASK_H
#define EDITSIGNALTASK_H

#include "Task.h"

class TimingDocument;
class EditSignalTask : public Task
{
public:
    EditSignalTask(const Task *task, unsigned int sig, wxInt32 tick, const wxPoint &pos, bool leftDown = true);
    virtual ~EditSignalTask();

    virtual void LabelsMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void WavesMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void AxisMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void LabelsKey(const wxKeyEvent &event, bool down);
    virtual void WavesKey(const wxKeyEvent &event, bool down);
    virtual void AxisKey(const wxKeyEvent &event, bool down);
    virtual void Update();

protected:
private:
    void SetDrawlets();
    void OnMouse(const wxMouseEvent &event);
    void OnKey(const wxKeyEvent &event, bool down);

    void MouseDownOnEdge(const wxPoint &pos);
    void MouseDownOnState(const wxPoint &pos);
    void MouseDragEdge(const wxPoint &pos);
    void MouseDragState(const wxPoint &pos);
    void MouseUpEdge(const wxPoint &pos);
    void MouseUpState(const wxPoint &pos);

private:
    unsigned int m_sig;
    const bool m_leftDown;
    wxInt32 m_startTick;
    bool m_startedOnEdge;

    bool m_validMove;
    wxInt32 m_endTick;
     TimingDocument *m_doc;
};

#endif // EDITSIGNALTASK_H
