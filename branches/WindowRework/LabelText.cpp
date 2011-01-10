#include "LabelText.h"

LabelText::LabelText(wxWindow *parent, const wxString &value, const wxPoint& pos, const wxSize& size, unsigned int signalNumber):
wxTextCtrl(parent, wxID_ANY, value, pos, size, wxBORDER_NONE ), //wxBORDER_NONE| wxTE_DONTWRAP | wxBORDER_SIMPLE
m_signalNumber(signalNumber)
{
    //ctor
}

LabelText::~LabelText()
{
    //dtor
}
