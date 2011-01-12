#ifndef TIMINGTEXTCTRL_H
#define TIMINGTEXTCTRL_H

#include <wx/textctrl.h>

class TimingView;
class wxCommand;
class EditTextTask;

class TimingTextCtrl : public wxTextCtrl
{
public:
    TimingTextCtrl(wxPanel *parent, TimingView *view, const wxString &value, const wxPoint& pos, const wxSize& size, long style = 0);
    virtual ~TimingTextCtrl();

    virtual void RestoreText();
    virtual wxCommand *GetCommand()=0;

    void SetFocusToParent();
protected:
    TimingTextCtrl(){}

protected:
    TimingView *m_view;

protected:
    wxString unchanged;

    friend class EditTextTask;
    EditTextTask *edittask;
private:
    void OnEnterCommand(wxCommandEvent &event);

private:
    DECLARE_EVENT_TABLE()
};

#endif //

