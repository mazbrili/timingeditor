#ifndef LABELTEXT_H
#define LABELTEXT_H

#include "TimingTextCtrl.h"

class LabelText : public TimingTextCtrl
{
public:
    LabelText(wxPanel *parent, TimingView *view, const wxString &value, const wxPoint& pos, const wxSize& size, unsigned int signalNumber);
    virtual ~LabelText();

    virtual wxCommand *GetEnterCommand();
    virtual wxCommand *GetChangedCommand();

public:
    virtual void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
    void SetBuswidthField(TimingTextCtrl * widthctrl);
protected:
private:
    LabelText(){}
    unsigned int m_signalNumber;
private:
    //void OnEnterCommand(wxCommandEvent &event);
    void OnSetFocus(wxFocusEvent &event);
    void OnKillFocus(wxFocusEvent &event);
private:
    TimingTextCtrl *buswidth;

    virtual void OnDrop(const wxString& data);

private:

    DECLARE_DYNAMIC_CLASS(LabelText)
    DECLARE_EVENT_TABLE()
};

#endif // LABELTEXT_H
