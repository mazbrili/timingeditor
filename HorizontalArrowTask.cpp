#include "HorizontalArrowTask.h"

#include "HoverLine.h"
#include "HoverCombo.h"
HorizontalArrowTask::HorizontalArrowTask(const Task *task):
Task(task)
{
    //ctor
}

HorizontalArrowTask::~HorizontalArrowTask()
{
    //dtor
}
void HorizontalArrowTask::CalcYPos(const wxPoint &pos)
{
    unsigned int k;
    for ( k = 0 ; k < m_view->heightOffsets.size()-1 ; ++k )
        if ( pos.y > m_view->heightOffsets[k] )
            break;

     m_ypos = k;
     m_yposoffset = pos.y - m_view->heightOffsets[k];
}

HoverDrawlet *HorizontalArrowTask::GetDrawlet(wxPoint start, wxPoint stop, int style)
{
    if (stop.x < start.x)
    {
        int tmp = start.x;
        start.x = stop.x;
        stop.x = tmp;
    }
    return new HoverCombo(
                new HoverLine(start, stop, *wxLIGHT_GREY, 1,style),
                new HoverCombo(
                    new HoverCombo(new HoverLine(start, start+wxPoint(+5,-3), *wxLIGHT_GREY),new HoverLine(start, start+wxPoint(+5,+3), *wxLIGHT_GREY)),
                    new HoverCombo(new HoverLine(stop,  stop +wxPoint(-5,-3), *wxLIGHT_GREY),new HoverLine(stop , stop +wxPoint(-5,+3), *wxLIGHT_GREY))
                ));
}
