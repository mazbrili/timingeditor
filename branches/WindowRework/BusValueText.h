#ifndef BUSVALUETEXT_H
#define BUSVALUETEXT_H

#include "TimingTextCtrl.h"

class BusValueText : public TimingTextCtrl
{
public:
    BusValueText(wxPanel *parent, TimingView *view, const wxString &value, const wxPoint& pos, unsigned int signalNumber, unsigned int tick);
    virtual ~BusValueText();

    virtual wxCommand *GetEnterCommand();
    virtual wxCommand *GetChangedCommand();

public:
    virtual void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
protected:
private:
    BusValueText(){}
    unsigned int m_signalNumber;
    unsigned int m_tick;
private:
    //void OnEnterCommand(wxCommandEvent &event);
    void OnSetFocus(wxFocusEvent &event);
    void OnKillFocus(wxFocusEvent &event);
private:

    virtual void OnDrop(const wxString& data);

private:

    DECLARE_DYNAMIC_CLASS(BusValueText)
    DECLARE_EVENT_TABLE()
};

#endif // BUSVALUETEXT_H

