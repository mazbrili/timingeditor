#ifndef ACTIVESIGNALTASK_H
#define ACTIVESIGNALTASK_H

#include "Task.h"


class ActiveSignalTask : public Task
{
public:
    ActiveSignalTask(const Task *task, int sig);
    virtual ~ActiveSignalTask();
    virtual void LabelsMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void WavesMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void AxisMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void LabelsKey(const wxKeyEvent &event, bool down);
    virtual void WavesKey(const wxKeyEvent &event, bool down);
    virtual void AxisKey(const wxKeyEvent &event, bool down);
    virtual void UpdateClockSettingsPanel(bool attach = true);

    virtual bool CanDelete();
    virtual bool CanCopy();
    virtual void Copy();
    virtual void Cut();
    virtual void Delete();
    virtual void Paste();
    virtual bool CanPaste();
    virtual void Update();

    int GetSelectedSignalNumber();

protected:
private:
    void SetDrawlets();
    void EndTask();
    void OnMouse(const wxMouseEvent &event);
    void OnKey(const wxKeyEvent &event, bool down);
private:
    int m_sig;
};

#endif // ACTIVESIGNALTASK_H
