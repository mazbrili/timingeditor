#ifndef HORRIZONTALARROWTEXT_H
#define HORRIZONTALARROWTEXT_H

#include "TimingTextCtrl.h"
class ActiveHorizontalArrowTask;

class HorizontalArrowText : public TimingTextCtrl
{
public:
    HorizontalArrowText(wxPanel *parent, TimingView *view);
    virtual ~HorizontalArrowText();

    virtual wxCommand *GetEnterCommand();
    virtual wxCommand *GetChangedCommand();
protected:
    friend class ActiveHorizontalArrowTask;
private:
    HorizontalArrowText(){}
    int m_horizontalArrow;
    ActiveHorizontalArrowTask *m_activeTask;
private:
    void OnEnterCommand(wxCommandEvent &event);
    void OnText(wxCommandEvent &event);
    void OnSetFocus(wxFocusEvent &event);
    void OnKillFocus(wxFocusEvent &event);

    //virtual void RestoreText();

private:

    DECLARE_DYNAMIC_CLASS(HorizontalArrowText)
    DECLARE_EVENT_TABLE()
};

#endif // HORRIZONTALARROWTEXT_H


