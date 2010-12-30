#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __DATAOBJECT_H__
#define __DATAOBJECT_H__

#include <wx/dataobj.h>

#include "TimingDoc.h"


static const wxChar *TimingEditorSignalFormatId = _T("TimingEditorSignalDataFormat");

class TimingEditorSignalDataObject : public wxDataObjectSimple
{
public:
    TimingEditorSignalDataObject( Signal *sig );
    virtual ~TimingEditorSignalDataObject();

    virtual size_t GetDataSize() const;
    virtual bool GetDataHere(void *buf) const;
    virtual bool SetData(size_t len, const void *buf);
private:
    Signal *m_sig;
};

#endif

