#ifndef TIMINGTEXTCTRL_H
#define TIMINGTEXTCTRL_H

#include <wx/textctrl.h>
#include <wx/dnd.h>

class TimingView;
class wxCommand;
class EditTextTask;
class TimingTextCtrl;

//class TimingTextDropTarget : public wxTextDropTarget
//{
//public:
//    TimingTextDropTarget(TimingTextCtrl *ctrl):m_owner(ctrl){}
//private:
//    TimingTextCtrl *m_owner;
//    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& data);
//
//};

class TimingTextCtrl : public wxTextCtrl
{
public:
    TimingTextCtrl(wxPanel *parent, TimingView *view, const wxString &value, const wxPoint& pos, const wxSize& size, long style = 0);
    virtual ~TimingTextCtrl();

    virtual void RestoreText();
    virtual wxCommand *GetEnterCommand()=0;
    virtual wxCommand *GetChangedCommand()=0;

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
    void OnText(wxCommandEvent &event);

//    virtual void OnDrop(const wxString& data);
//    friend class TimingTextDropTarget;

private:
    DECLARE_EVENT_TABLE()
};

#endif //

