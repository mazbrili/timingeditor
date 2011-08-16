#include "VerticalLineTask.h"

#include "TimingView.h"
#include "TimingDoc.h"

#include "HoverCombo.h"
#include "HoverGraphCaret.h"
#include "HoverLine.h"

#include "DiagramWavesWindow.h"

VerticalLineTask::VerticalLineTask(const Task *task):
Task(task)
{
    m_waveWin->ActivateText(false);
}

VerticalLineTask::~VerticalLineTask()
{}

void VerticalLineTask::CalcXPos(const wxPoint &pos)
{
    xpos = GetTickFromPosition(pos);
    xposoffset = 0;

    TimingDocument *doc = (TimingDocument *)m_view->GetDocument();
    if ( !doc || !doc->signals.size() )return;

    wxCoord p = pos.x - m_view->GetWavesLeftSpace() - xpos*m_view->GridStepWidth;
    if ( doc->en50 && doc->en90 )
    {
        if      ( p < m_view->GridStepWidth*(doc->TransitWidth/4.0)/100.0 )
            xposoffset = 0;
        else if ( p < m_view->GridStepWidth*((3.0*doc->TransitWidth/4.0)/100.0) )
            xposoffset = 1;
        else if ( p < m_view->GridStepWidth*(50.0+doc->TransitWidth/2.0)/100.0 )
            xposoffset = 2;
        else
        {
            xpos++;
            xposoffset = 0;
        }
    }
    else if ( doc->en50 )
    {
        if ( p < m_view->GridStepWidth*(doc->TransitWidth/4.0)/100.0 )
            xposoffset = 0;
        else if ( p < m_view->GridStepWidth*((50.0+doc->TransitWidth/4.0)/100.0) )
            xposoffset = 1;
        else
        {
            xpos++;
            xposoffset = 0;
        }
    }
    else if ( doc->en90 )
    {
        if ( p < m_view->GridStepWidth*(doc->TransitWidth/2.0)/100.0 )
            xposoffset = 0;
        else if ( p < m_view->GridStepWidth * (50.0+doc->TransitWidth/2.0) / 100.0 )
            xposoffset = 2;
        else
        {
            xpos++;
            xposoffset = 0;
        }
    }
    else
    {
        if ( p > m_view->GridStepWidth/2 )
            xpos++;
        xposoffset = 0;
    }

}
unsigned int VerticalLineTask::GetYPos(const wxPoint &pos)
{
    unsigned int k;
    for ( k = 0 ; k < m_view->heightOffsets.size()-1 ; ++k )
        if ( pos.y < (m_view->heightOffsets[k] + m_view->heightOffsets[k+1])/2 )
            break;
    return k;
}

HoverDrawlet *VerticalLineTask::GetDrawlet(const wxPoint &start, const wxPoint &stop)const
{
    return
        new HoverCombo(
            new HoverCombo(
                new HoverGraphCaret(start - wxPoint(3,3),wxSize(7,7), m_view->GetActiveGraphCaretColour()),
                new HoverGraphCaret(stop - wxPoint(3,3),wxSize(7,7), m_view->GetActiveGraphCaretColour())
            ),
            new HoverLine(start, stop, *wxLIGHT_GREY)
        );
}

