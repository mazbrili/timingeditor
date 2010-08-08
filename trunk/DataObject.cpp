#ifdef __GNUG__
// #pragma interface
#endif

#include <wx/mstream.h>
#include <wx/txtstrm.h>

#include "DataObject.h"

#include "enumers.h"


TimingEditorSignaDataObject::TimingEditorSignaDataObject(Signal *sig):
wxDataObjectSimple(wxDataFormat(TimingEditorSignalFormatId)),
m_sig(sig)
{}
TimingEditorSignaDataObject::~TimingEditorSignaDataObject()
{
    delete m_sig;
}



//wxDataObjectSimple::GetDataSize
size_t TimingEditorSignaDataObject::GetDataSize() const
{
    wxMemoryOutputStream memstream;

    m_sig->serialize( memstream );

    if ( memstream.GetLength() != wxInvalidOffset )
        return( memstream.GetLength() );

    return( 0 );
}


//wxDataObjectSimple::GetDataHere
bool TimingEditorSignaDataObject::GetDataHere(void *buf) const
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
bool TimingEditorSignaDataObject::SetData(size_t len, const void *buf)
{
    wxMemoryInputStream memstream((char *)buf, len);
    m_sig->deserialize(memstream);
    return true;
}
