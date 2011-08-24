#include "ActiveHorizontalArrowTask.h"

#include <wx/clipbrd.h>

#include "TimingDoc.h"
#include "TimingView.h"
#include "cmd.h"

//#include "HoverHatch.h"
//#include "HoverCombo.h"
//#include "HoverLine.h"
//#include "OnceDrawlet.h"
//#include "HoverGraphCaret.h"
#include "DiagramWavesWindow.h"
#include "HorizontalArrowText.h"


ActiveHorizontalArrowTask::ActiveHorizontalArrowTask(const Task *task, int horizontalArrowIdx):
HorizontalArrowTask(task),
m_horizontalArrowIdx(horizontalArrowIdx),
state(activeArrow),
m_isValidMove(false),
m_isValidPos(false)
{
    m_doc = (TimingDocument *)m_view->GetDocument();
    m_txtCtrl = m_waveWin->GetHorizontalArrowTextCtrl();
    Init();
}
void ActiveHorizontalArrowTask::Init()
{

    state = activeArrow;
    m_verticalLine = m_doc->horizontalArrows[m_horizontalArrowIdx].fromVerticalLine;
    m_secondVerticalLine = m_doc->horizontalArrows[m_horizontalArrowIdx].toVerticalLine;

    HorizontalArrowText *txtCtrl = m_waveWin->GetHorizontalArrowTextCtrl();
    GraphHorizontalArrow gharrow = m_view->GetGraphHorizontalArrows()[m_horizontalArrowIdx];
    wxPoint p = gharrow.GetTextPoint()-wxPoint(0, txtCtrl->GetSize().y);
    m_waveWin->CalcControlPos(p);
    txtCtrl->SetPosition(p);
    txtCtrl->ChangeValue(m_doc->horizontalArrows[m_horizontalArrowIdx].text);
    txtCtrl->Show();
    txtCtrl->m_horizontalArrow = m_horizontalArrowIdx;
    txtCtrl->m_activeTask = this;
    txtCtrl->SetFocusToParent();

    SetDrawlets();
}

ActiveHorizontalArrowTask::~ActiveHorizontalArrowTask()
{
    HorizontalArrowText *txtCtrl = m_waveWin->GetHorizontalArrowTextCtrl();
    txtCtrl->Hide();
    txtCtrl->m_horizontalArrow = -1;
    txtCtrl->m_activeTask = NULL;
}

void ActiveHorizontalArrowTask::SetDrawlets()
{
    GraphHorizontalArrow gharrow = m_view->GetGraphHorizontalArrows()[m_horizontalArrowIdx];
    if (state == activeArrow)
    {
        m_waveWin->SetDrawlet(GetActiveArrowStateDrawlet());
        return;
    }
    if ( state == movingText )
    {
        HoverDrawlet *drawlet = GetActiveDrawlet(gharrow.GetStartPoint(), gharrow.GetStopPoint(),
                                                 wxTRANSPARENT, m_view->GetActiveGraphCaretColour(), m_pos, wxSHORT_DASH);
        m_waveWin->SetDrawlet(drawlet);
        return;
    }
    if ( state == movingStartPos || state == movingEndPos || state == movingArrow )
    {
        HoverDrawlet *drawlet;
        if (m_isValidMove)
        {
            if ( state == movingStartPos )
                drawlet = GetActiveDrawlet(wxPoint(m_pos.x,gharrow.GetStopPoint().y), gharrow.GetStopPoint(), m_isValidPos ? wxSOLID : wxSHORT_DASH, m_view->GetActiveGraphCaretColour());//, gharrow.GetTextPoint(), wxSOLID);
            else if ( state == movingEndPos )
                drawlet = GetActiveDrawlet(gharrow.GetStartPoint(), wxPoint(m_pos.x, gharrow.GetStartPoint().y ), m_isValidPos ? wxSOLID : wxSHORT_DASH, m_view->GetActiveGraphCaretColour());//, gharrow.GetTextPoint(), wxSOLID);
            else //if ( state == movingArrow )
                drawlet = GetActiveDrawlet(wxPoint(gharrow.GetStartPoint().x, m_pos.y), wxPoint(gharrow.GetStopPoint().x, m_pos.y), wxSHORT_DASH, m_view->GetActiveGraphCaretColour());//, gharrow.GetTextPoint(), wxSOLID);
        }
        else
            drawlet = GetActiveArrowStateDrawlet();
        m_waveWin->SetDrawlet(drawlet);
        return;
    }
}
HoverDrawlet *ActiveHorizontalArrowTask::GetActiveArrowStateDrawlet()
{
    GraphHorizontalArrow gharrow = m_view->GetGraphHorizontalArrows()[m_horizontalArrowIdx];
    return GetActiveDrawlet(gharrow.GetStartPoint(), gharrow.GetStopPoint(),
                     wxTRANSPARENT, m_view->GetActiveGraphCaretColour(), gharrow.GetTextPoint(), wxSOLID);
}

void ActiveHorizontalArrowTask::WavesMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    if ( state == editingText )
    {
        if (event.Button(wxMOUSE_BTN_LEFT))
            EndEditing();
        else if (event.Button(wxMOUSE_BTN_RIGHT))
            Task::EndTask();
        return;
    }
    GraphHorizontalArrow gharrow = m_view->GetGraphHorizontalArrows()[m_horizontalArrowIdx];

    if ( event.ButtonDown(wxMOUSE_BTN_RIGHT) )
    {
        EndTask();
        return;
    }
    if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        const int snapTolerance = GetHorizontalArrowSnapTolerance();

        // check if position of text is clicked
        if( gharrow.IsTextPoint(pos, snapTolerance))
        {
            state = movingText;
            m_pos = gharrow.GetTextPoint();
            CheckMovingText(pos);
            SetDrawlets();
            return;
        }

        // is it a click on an arrow?
        int k = IsOverHorizontalArrow(pos);
        if ( k == -1 )
        {
            EndTask();
            return;
        }
        if ( m_horizontalArrowIdx == k )
        {
            // check if start point is clicked
            if( gharrow.IsStartPoint(pos, snapTolerance) )
            {
                state = movingStartPos;
                m_pos = gharrow.GetStartPoint();
                CheckMovingStartStop(pos);
                SetDrawlets();
                return;
            }
            // check if stop point is clicked
            if ( gharrow.IsStopPoint(pos, snapTolerance) )
            {
                state = movingEndPos;
                m_pos = gharrow.GetStopPoint();
                CheckMovingStartStop(pos);
                SetDrawlets();
                return;
            }
            state = movingArrow;
            m_pos = wxPoint(pos.x, gharrow.GetStartPoint().y);
            CheckMovingArrow(event, pos);
            SetDrawlets();
            return;
        }
        else
        {
            // ???? clicked on anther arrow change selection directly (normal
            // behaviour deselect and select the other -> one additional click
            m_horizontalArrowIdx = k;
            Init();
            return;
        }
    }

    if (event.Dragging() && event.ButtonIsDown(wxMOUSE_BTN_LEFT))
    {
        switch(state)
        {
            case movingText:
                CheckMovingText(pos);
                break;
            case movingArrow:
                CheckMovingArrow(event, pos);
                break;
            case movingStartPos:
            case movingEndPos:
                CheckMovingStartStop(pos);
                break;
            default:
                return;
        }

        SetDrawlets();
        return;
    }

    if (event.ButtonUp(wxMOUSE_BTN_LEFT) )
    {
        if ( m_isValidMove )
        {
            if ( state == movingText )
            {
                int xoff = m_pos.x - (gharrow.GetStartPoint().x/2+gharrow.GetStopPoint().x/2);
                int yoff = m_pos.y - gharrow.GetStartPoint().y;
                m_doc->GetCommandProcessor()->Submit(
                    new ChangeHorizontalArrowTextPosCommand(m_doc, m_horizontalArrowIdx, xoff % (m_view->GridStepWidth), yoff, xoff/(m_view->GridStepWidth) )
                );
                return;
            }
            if (state == movingArrow)
            {
                int sigindex = GetSignalFromPosition(m_pos);
                int xoff = m_pos.y - m_view->heightOffsets[sigindex];

                m_doc->GetCommandProcessor()->Submit(
                    new ChangeHorizontalArrowCommand(m_doc, m_horizontalArrowIdx, xoff, sigindex, m_verticalLine, m_secondVerticalLine )
                );
                return;
            }

            if ( m_isValidPos )
            {
                int xoff = m_doc->horizontalArrows[m_horizontalArrowIdx].pos;
                int sigindex = m_doc->horizontalArrows[m_horizontalArrowIdx].signalnmbr;
                if ( (state == movingStartPos && !gharrow.IsSwapped()) || (state == movingEndPos && gharrow.IsSwapped()) )
                {
                    m_doc->GetCommandProcessor()->Submit(
                        new ChangeHorizontalArrowCommand(m_doc, m_horizontalArrowIdx, xoff, sigindex, m_overVerticalLine, m_secondVerticalLine )
                    );
                    return;
                }
                if ( (state == movingEndPos && !gharrow.IsSwapped()) || (state == movingStartPos && gharrow.IsSwapped()) )
                {
                    m_doc->GetCommandProcessor()->Submit(
                        new ChangeHorizontalArrowCommand(m_doc, m_horizontalArrowIdx, xoff, sigindex, m_verticalLine, m_overVerticalLine )
                    );
                    return;
                }
            }
        }
        Init();
    }
    if (event.Entering() && !event.ButtonIsDown(wxMOUSE_BTN_LEFT) )
    {
        Init();
    }
//    if (event.Leaving() && (state == movingArrow || state == movingStartPos || state == movingEndPos || state == movingText) )
//    {
//        state = activeArrow;
//        SetDrawlets();
//    }

}

void ActiveHorizontalArrowTask::CheckMovingArrow(const wxMouseEvent &event, wxPoint pos)
{
    //"top" has the smaller y position than "bottom"
    int top = m_doc->verticalLines[m_verticalLine].StartPos;
    int top2 = m_doc->verticalLines[m_secondVerticalLine].StartPos;
    if ( top2 > top )
        top = top2; // take the "later" as the top line
    int bot = m_doc->verticalLines[m_verticalLine].EndPos+1;
    int bot2 = m_doc->verticalLines[m_secondVerticalLine].EndPos+1;
    if (bot2 < bot)
        bot = bot2; // take the "earlier" as the bottom line

    top = m_view->heightOffsets[top];
    bot = m_view->heightOffsets[bot];

    if (!event.ShiftDown())
    {
        int ha = IsOverHorizontalArrow(pos);
        if ( ha != -1 && ha != m_horizontalArrowIdx )
            pos.y = m_doc->horizontalArrows[ha].pos + m_view->heightOffsets[m_doc->horizontalArrows[ha].signalnmbr];
    }

    if ( pos.y == m_doc->horizontalArrows[m_horizontalArrowIdx].pos + m_view->heightOffsets[m_doc->horizontalArrows[m_horizontalArrowIdx].signalnmbr])
        m_isValidMove = false;
    else
        m_isValidMove = true;

    if (pos.y > top && pos.y < bot)
        m_pos = pos;
}
void ActiveHorizontalArrowTask::CheckMovingStartStop(const wxPoint &pos)
{
    if (!IsOverWavesRect(pos) )
    {
        m_isValidMove = false;
        m_isValidPos = false;
        return;
    }

    CalcYPos(pos);

    //int old_secondVerticalLine = m_secondVerticalLine;
    m_overVerticalLine = IsOverVerticalLine(wxPoint(pos.x, m_view->heightOffsets[m_ypos] + m_yposoffset ));
    if (m_overVerticalLine == m_secondVerticalLine || m_overVerticalLine == m_verticalLine  )
    {
        m_isValidMove = false;
        m_isValidPos = false;
    }
    else
    {
        m_isValidMove = true;
        m_pos = pos;
        m_isValidPos = m_overVerticalLine != -1;
    }
}
void ActiveHorizontalArrowTask::CheckMovingText(const wxPoint &pos)
{
    m_isValidMove = IsOverWavesRect(pos);
    if (m_isValidMove)
        m_pos = pos;
}

void ActiveHorizontalArrowTask::TextHasFocus(TimingTextCtrl *ctrl)
{
    if (m_txtCtrl != ctrl)
        Task::TextHasFocus(ctrl);
    else
        state = editingText;
}


bool ActiveHorizontalArrowTask::IsTextSelected()const
{
    return !(m_txtCtrl->GetStringSelection().IsEmpty());
}

void ActiveHorizontalArrowTask::Delete()
{
    if (CanDelete())
    {
        if (state == activeArrow)
        {
            state = deleting;
            wxCommandProcessor *cmdproc = m_view->GetDocument()->GetCommandProcessor();
            if (cmdproc)
                cmdproc->Submit(new DeleteHorizontalArrowCommand(m_doc, m_horizontalArrowIdx));
        }
        else // editingText
        {
            long from, to;
            m_txtCtrl->GetSelection(&from, &to);
            if (from != to)
                m_txtCtrl->Remove(from, to);
        }
    }
}
bool ActiveHorizontalArrowTask::CanDelete()const
{
    return (state == activeArrow) || (state == editingText && IsTextSelected() && !IsReadOnly());
}
void ActiveHorizontalArrowTask::Update()
{
    if ( state == deleting )
    {
        EndTask();
        return;
    }
    else//if (state == movingArrow || state == movingEndPos || state == movingStartPos || state == movingText )
    {
        Init();
    }
}

void ActiveHorizontalArrowTask::OnMouse(const wxMouseEvent &event)
{
    if ( state == editingText )
    {
        if (event.Button(wxMOUSE_BTN_LEFT))
            EndEditing();
        if (event.Button(wxMOUSE_BTN_RIGHT))
            EndTask();
        return;
    }
    if (event.ButtonDown(wxMOUSE_BTN_ANY ))
        EndTask();
}
void ActiveHorizontalArrowTask::OnKey(const wxKeyEvent &event, bool down)
{
    if ( down && event.GetKeyCode() == WXK_DELETE)
    {
        if ( CanDelete())
            Delete();
        return;
    }
    if ( down && event.GetKeyCode() == WXK_ESCAPE)
        EndTask();
}
void ActiveHorizontalArrowTask::LabelsMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}
void ActiveHorizontalArrowTask::AxisMouse(const wxMouseEvent &event, const wxPoint &pos)
{
    OnMouse(event);
}
void ActiveHorizontalArrowTask::LabelsKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
void ActiveHorizontalArrowTask::WavesKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}
void ActiveHorizontalArrowTask::AxisKey(const wxKeyEvent &event, bool down)
{
    OnKey(event, down);
}

bool ActiveHorizontalArrowTask::CanCut()const
{
    return state == editingText &&  IsTextSelected() && !IsReadOnly();
}
bool ActiveHorizontalArrowTask::CanCopy()const
{
    return IsTextSelected();
}
bool ActiveHorizontalArrowTask::CanPaste()const
{
    bool ret = false;
    if ( state == editingText && wxTheClipboard->Open())
    {
        ret = wxTheClipboard->IsSupported( wxDF_TEXT );
        wxTheClipboard->Close();
    }
    return ret;
}
void ActiveHorizontalArrowTask::Copy()

{
    if (state == editingText && wxTheClipboard->Open())
    {
        // This data objects are held by the clipboard,
        // so do not delete them in the app.
        wxString str = m_txtCtrl->GetStringSelection();
        if ( !str.IsEmpty() )
            wxTheClipboard->SetData( new wxTextDataObject(str) );
        wxTheClipboard->Close();
    }
}
void ActiveHorizontalArrowTask::Cut()
{
    if ( state == editingText )
        m_txtCtrl->Cut();
}
void ActiveHorizontalArrowTask::Paste()
{
    if ( state == editingText)
        m_txtCtrl->Paste();
}
void ActiveHorizontalArrowTask::SelectAll()
{
    if ( state == editingText)
        m_txtCtrl->SetSelection(-1, -1);
}

void ActiveHorizontalArrowTask::EndEditing()
{
    HorizontalArrowText *txtCtrl = m_waveWin->GetHorizontalArrowTextCtrl();
    state = activeArrow;
    wxCommandProcessor *cmdproc = m_view->GetDocument()->GetCommandProcessor();
    if (cmdproc)
        cmdproc->Submit(txtCtrl->GetChangedCommand());
}
void ActiveHorizontalArrowTask::OnText()
{
    EndEditing();
}
void ActiveHorizontalArrowTask::OnEnterText()
{
    EndEditing();
}
