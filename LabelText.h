#ifndef LABELTEXT_H
#define LABELTEXT_H

#include <wx/textctrl.h>


class LabelText : public wxTextCtrl
{
public:
    LabelText(wxWindow *parent, const wxString &value, const wxPoint& pos, const wxSize& size, unsigned int signalNumber);
    virtual ~LabelText();
protected:
private:
    unsigned int m_signalNumber;
};

#endif // LABELTEXT_H
