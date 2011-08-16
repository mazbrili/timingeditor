#ifndef BUSVALUETEXT_H
#define BUSVALUETEXT_H

#include "TimingTextCtrl.h"
#include "GraphObject.h"

class BusValueText : public GraphObject, public TimingTextCtrl
{
public:
    BusValueText(wxPanel *parent, TimingView *view, const wxString &value, const wxPoint& pos, const wxPoint& unscrolledPos, unsigned int signalNumber, unsigned int tick);
    virtual ~BusValueText();

    virtual wxCommand *GetEnterCommand();
    virtual wxCommand *GetChangedCommand();

public:
    //virtual void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
    virtual void Draw(wxDC &dc)const;
    void SetActive(const bool act = true);
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
    static bool m_active;
    wxPoint m_unscrolledPos;

    DECLARE_DYNAMIC_CLASS(BusValueText)
    DECLARE_EVENT_TABLE()
};

#endif // BUSVALUETEXT_H

