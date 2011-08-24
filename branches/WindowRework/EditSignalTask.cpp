#include "EditSignalTask.h"

EditSignalTask::EditSignalTask(const Task *task, unsigned int sig):
Task(task),
m_sig(sig)
{
    //ctor
}

EditSignalTask::~EditSignalTask()
{
    //dtor
}
