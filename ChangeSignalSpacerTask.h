#ifndef CHANGESIGNALSPACERTASK_H
#define CHANGESIGNALSPACERTASK_H

#include "Task.h"


class ChangeSignalSpacerTask : public Task
{
public:
    ChangeSignalSpacerTask(const Task *task, int posToChange);
    virtual ~ChangeSignalSpacerTask();
    virtual void LabelsMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void WavesMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void AxisMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void LabelsKey(const wxKeyEvent &event, bool down);
    virtual void WavesKey(const wxKeyEvent &event, bool down);
    virtual void AxisKey(const wxKeyEvent &event, bool down);

//    virtual bool CanDelete();
//    virtual bool CanCopy();
//    virtual void Copy();
//    virtual void Cut();
//    virtual void Delete();
//    virtual void Paste();
//    virtual bool CanPaste();
    virtual void Update();

protected:
private:
    void SetDrawlets(int targetpos = -1);
    void OnMouse(const wxMouseEvent &event);
    void OnKey(const wxKeyEvent &event, bool down);
private:
    int m_sig;
    int m_space;
    int m_newSpace;
    int m_startOfSignalPos;
    int m_endOfSignalPos;
    bool m_above;
private:
};

#endif // CHANGESIGNALSPACERTASK_H

