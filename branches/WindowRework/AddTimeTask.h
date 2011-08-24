#ifndef ADDTIMETASK_H
#define ADDTIMETASK_H

#include "Task.h"


class AddRemoveTimeTask : public Task
{

public:
    AddRemoveTimeTask(const Task *task, int tick, bool add = true);
    virtual ~AddRemoveTimeTask();
    virtual void LabelsMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void WavesMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void AxisMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void LabelsKey(const wxKeyEvent &event, bool down);
    virtual void WavesKey(const wxKeyEvent &event, bool down);
    virtual void AxisKey(const wxKeyEvent &event, bool down);
//    virtual void UpdateClockSettingsPanel(bool attach = true);

//    virtual bool CanDelete()const;
//    virtual bool CanCopy()const;
//    virtual void Copy();
//    virtual void Cut();
//    virtual void Delete();
//    virtual void Paste();
//    virtual bool CanPaste()const;
//    virtual void Update();

protected:

private:
    //void SetDrawlets(int targetpos = -1);
    //virtual void EndTask();
    void OnMouse(const wxMouseEvent &event);
    void OnKey(const wxKeyEvent &event, bool down);
private:
    int m_tick;
    bool m_add;
};
#endif // ADDTIMETASK_H
