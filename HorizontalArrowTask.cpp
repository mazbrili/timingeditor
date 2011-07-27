#include "HorizontalArrowTask.h"

#include "HoverLine.h"
#include "HoverCombo.h"
#include "HoverGraphCaret.h"

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

HoverDrawlet *HorizontalArrowTask::GetInsertingDrawlet(wxPoint start, wxPoint stop, int style)const
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
HoverDrawlet *HorizontalArrowTask::GetActiveDrawlet(const wxPoint &start, const wxPoint &stop, int style, const wxColour &colour)const
{
    return new HoverCombo(
        new HoverLine(start, stop, *wxLIGHT_GREY, 1, style),
        new HoverCombo(
            new HoverGraphCaret(start - wxPoint(3,3),wxSize(7,7), colour),
            new HoverGraphCaret(stop - wxPoint(3,3),wxSize(7,7), colour)
        )
    );
}
HoverDrawlet *HorizontalArrowTask::GetActiveDrawlet(const wxPoint &start, const wxPoint &stop, int style, const wxColour &colour, const wxPoint &textpos, int textposlinestyle )const
{
    //::wxLogMessage(_T("HorizontalArrowTask start.x=%d, stop.x=%d, text.x=%d, text.y=%d"), start.x, stop.x, textpos.x, textpos.y);
    return new HoverCombo(
        new HoverCombo(
            new HoverLine(start, stop, *wxLIGHT_GREY, 1, style),
            new HoverCombo(
                new HoverGraphCaret(start - wxPoint(3,3),wxSize(7,7), colour),
                new HoverGraphCaret(stop - wxPoint(3,3),wxSize(7,7), colour)
            )
        ),
        new HoverCombo(
            new HoverGraphCaret(textpos - wxPoint(3,3),wxSize(7,7), colour),
            new HoverLine(wxPoint(start.x/2 +stop.x/2, start.y), textpos, *wxLIGHT_GREY, 1, textposlinestyle )
        )
    );
}
