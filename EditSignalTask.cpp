#include "EditSignalTask.h"

EditSignalTask::EditSignalTask(const Task *task, unsigned int sig, wxInt32 tick, bool leftDown):
Task(task),
m_sig(sig),
m_leftDown(leftDown),
m_startTick(tick)
{
    //ctor
    m_onEdge = false; // not implemented yet
}

EditSignalTask::~EditSignalTask()
{
    //dtor
}
