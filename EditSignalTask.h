#ifndef EDITSIGNALTASK_H
#define EDITSIGNALTASK_H

#include "Task.h"


class EditSignalTask : public Task
{
public:
    EditSignalTask(const Task *task, unsigned int sig, wxInt32 tick, bool leftDown = true);
    virtual ~EditSignalTask();
protected:
private:
    unsigned int m_sig;
    bool m_leftDown;
    wxInt32 m_startTick;
    bool m_onEdge;
};

#endif // EDITSIGNALTASK_H
