#ifndef EDITSIGNALTASK_H
#define EDITSIGNALTASK_H

#include "Task.h"


class EditSignalTask : public Task
{
public:
    EditSignalTask(const Task *task, unsigned int sig);
    virtual ~EditSignalTask();
protected:
private:
    unsigned int m_sig;
};

#endif // EDITSIGNALTASK_H
