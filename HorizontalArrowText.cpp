#include "HorizontalArrowText.h"
#include "ActiveHorizontalArrowTask.h"
#include "TimingView.h"

#include "cmd.h"

IMPLEMENT_DYNAMIC_CLASS(HorizontalArrowText,TimingTextCtrl)

BEGIN_EVENT_TABLE(HorizontalArrowText, TimingTextCtrl)
  EVT_SET_FOCUS(           HorizontalArrowText::OnSetFocus)
  EVT_TEXT_ENTER(wxID_ANY, HorizontalArrowText::OnEnterCommand)
  EVT_TEXT(wxID_ANY,       HorizontalArrowText::OnText)
  //EVT_KILL_FOCUS(          BusWidthText::OnKillFocus)
END_EVENT_TABLE()


HorizontalArrowText::HorizontalArrowText(wxPanel *parent, TimingView *view):
TimingTextCtrl(parent, view, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxBORDER_NONE ), //wxBORDER_NONE| wxTE_DONTWRAP | wxBORDER_SIMPLE
m_horizontalArrow(-1),
m_activeTask(NULL)
{
    //ctor
    //ChangeValue("[" + value + "]");
}

HorizontalArrowText::~HorizontalArrowText()
{
    //dtor
}
void HorizontalArrowText::OnText(wxCommandEvent &event)
{
}
void HorizontalArrowText::OnEnterCommand(wxCommandEvent &event)
{
    if ( m_activeTask )
        m_activeTask->OnEnterText();
}
void HorizontalArrowText::OnSetFocus(wxFocusEvent &event)
{
    m_view->TextHasFocus(this);
}
void HorizontalArrowText::OnKillFocus(wxFocusEvent &event)
{}
wxCommand *HorizontalArrowText::GetEnterCommand()
{
    if ( m_horizontalArrow == -1)
        return NULL;
    return new ChangeHorizontalArrowTextCommand((TimingDocument *)m_view->GetDocument(), m_horizontalArrow, GetValue());
}
wxCommand *HorizontalArrowText::GetChangedCommand()
{
    if ( m_horizontalArrow == -1)
        return NULL;
    return new ChangeHorizontalArrowTextCommand((TimingDocument *)m_view->GetDocument(), m_horizontalArrow, GetValue());
}
//void HorizontalArrowText::RestoreText()
//{
//    ChangeValue("[" + unchanged + "]");
//    ChangeValue( unchanged );
//
//    should not fail, because labels, axis and waves are panels
//    wxPanel *parentpanel = dynamic_cast<wxPanel *>( m_parent );
//    if ( parentpanel )
//        parentpanel->SetFocusIgnoringChildren();
//    else
//        m_parent->SetFocus();
//
//    edittask = NULL;
//}
