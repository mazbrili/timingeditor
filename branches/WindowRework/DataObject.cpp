#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/mstream.h>
#include <wx/txtstrm.h>

#include "DataObject.h"

#include "enumers.h"


TimingEditorSignalDataObject::TimingEditorSignalDataObject(Signal *sig):
wxDataObjectSimple(wxDataFormat(TimingEditorSignalFormatId)),
m_sig(sig)
{}
TimingEditorSignalDataObject::~TimingEditorSignalDataObject()
{
    delete m_sig;
}



//wxDataObjectSimple::GetDataSize
size_t TimingEditorSignalDataObject::GetDataSize() const
{
    wxMemoryOutputStream memstream;

    m_sig->serialize( memstream );

    if ( memstream.GetLength() != wxInvalidOffset )
        return( memstream.GetLength() );

    return( 0 );
}


//wxDataObjectSimple::GetDataHere
bool TimingEditorSignalDataObject::GetDataHere(void *buf) const
{
    wxMemoryOutputStream memstream;
    m_sig->serialize(memstream);

    ///copy the data
    wxInt32 len =  memstream.CopyTo((char *)buf, memstream.GetLength() );

    ///check that copied data has the same length as the data in the memorystream
    if ( len == memstream.GetLength() )
        return true;
    else
        return false;
}

//wxDataObjectSimple::SetData
bool TimingEditorSignalDataObject::SetData(size_t len, const void *buf)
{
    wxMemoryInputStream memstream((char *)buf, len);
    m_sig->deserialize(memstream);
    return true;
}
