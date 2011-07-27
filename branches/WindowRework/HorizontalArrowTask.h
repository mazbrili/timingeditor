#ifndef HORIZONTALARROWTASK_H
#define HORIZONTALARROWTASK_H

#include "Task.h"

#include "TimingView.h"

class HorizontalArrowTask : public Task
{
public:
    HorizontalArrowTask(const Task *task);
    virtual ~HorizontalArrowTask();
    virtual void Update()=0;

protected:
    void CalcYPos(const wxPoint &pos);

    HoverDrawlet *GetInsertingDrawlet(wxPoint start, wxPoint stop, int style = wxSOLID)const;
    HoverDrawlet *GetActiveDrawlet(const wxPoint &start, const wxPoint &stop, int style, const wxColour &colour, const wxPoint &textpos, int textlinestyle )const;
    HoverDrawlet *GetActiveDrawlet(const wxPoint &start, const wxPoint &stop, int style, const wxColour &colour)const;
    int m_verticalLine;
    int m_ypos, m_yposoffset;
};

#endif // HORIZONTALARROWTASK_H
