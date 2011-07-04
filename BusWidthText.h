#ifndef BUSWIDTHTEXT_H
#define BUSWIDTHTEXT_H

#include "TimingTextCtrl.h"

class BusWidthText : public TimingTextCtrl
{
public:
    BusWidthText(wxPanel *parent, TimingView *view, const wxString &value, const wxPoint& pos, const wxSize& size, unsigned int signalNumber);
    virtual ~BusWidthText();

    virtual wxCommand *GetEnterCommand();
    virtual wxCommand *GetChangedCommand();
protected:
private:
    BusWidthText(){}
    unsigned int m_signalNumber;
private:
    //void OnEnterCommand(wxCommandEvent &event);
    void OnSetFocus(wxFocusEvent &event);
    void OnKillFocus(wxFocusEvent &event);

    //virtual void RestoreText();

private:

    DECLARE_DYNAMIC_CLASS(BusWidthText)
    DECLARE_EVENT_TABLE()
};

#endif // BUSWIDTHTEXT_H

