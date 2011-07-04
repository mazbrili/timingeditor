#ifndef VERTICALLINETASK_H
#define VERTICALLINETASK_H

#include "Task.h"
class HoverDrawlet;

class VerticalLineTask : public Task
{
public:
    VerticalLineTask(const Task *task);
    virtual ~VerticalLineTask();
protected:
    void CalcXPos(const wxPoint &pos);
    unsigned int GetYPos(const wxPoint &pos);
    HoverDrawlet *GetDrawlet(const wxPoint &start, const wxPoint &stop)const;
protected:
    wxInt32 yStartPos;
    wxInt32 yEndPos;
    wxInt32 xpos;
    wxUint8 xposoffset;
};

#endif // ADDVERTICALLINETASK_H

