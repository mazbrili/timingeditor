#ifndef HORIZONTALARROWTASK_H
#define HORIZONTALARROWTASK_H

#include "Task.h"


class HorizontalArrowTask : public Task
{
public:
    HorizontalArrowTask(const Task *task);
    virtual ~HorizontalArrowTask();
    virtual void Update()=0;
protected:
protected:
};

#endif // HORIZONTALARROWTASK_H
