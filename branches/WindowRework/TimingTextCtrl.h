#ifndef TIMINGTEXTCTRL_H
#define TIMINGTEXTCTRL_H

#include <wx/textctrl.h>

class DiagramLabelsWindow;

class TimingTextCtrl : public wxTextCtrl
{
public:
    TimingTextCtrl(wxWindow* parent, wxWindowID id, const wxString& value = _T(""), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
    TimingTextCtrl(DiagramLabelsWindow* parent, wxWindowID id, const wxString& value = _T(""), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
    virtual ~TimingTextCtrl(){}
private:
    TimingTextCtrl(){}
    void OnUndoRedo(wxCommandEvent &event);
    void OnText(wxCommandEvent &event);
    void OnTextEnter(wxCommandEvent &event);
    void OnSetFocus(wxFocusEvent &event);
    void OnKillFocus(wxFocusEvent &event);

    void SetOptimalSize();

    DiagramLabelsWindow *labelparent;
private:

    DECLARE_DYNAMIC_CLASS(TimingTextCtrl)
    DECLARE_EVENT_TABLE()
};

#endif // TIMINGTEXTCTRL_H
