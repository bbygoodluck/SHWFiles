#include "../../ginc.h"
#include "DnDDataObject.h"

CDnDDataObject::CDnDDataObject(FORMATETC *fmtetc, STGMEDIUM *stgmed, int count)
{
    // reference count must ALWAYS start at 1
    m_lRefCount = 1;
    m_nNumFormats = count;

    m_pFormatEtc = new FORMATETC[count];
    m_pStgMedium = new STGMEDIUM[count];

    for(int i = 0; i < count; i++)
    {
        m_pFormatEtc[i] = fmtetc[i];
        m_pStgMedium[i] = stgmed[i];
    }
}

CDnDDataObject::~CDnDDataObject()
{

}

// IUnknown members
HRESULT CDnDDataObject::QueryInterface (REFIID iid, void ** ppvObject)
{

}

ULONG CDnDDataObject::AddRef (void)
{

}

ULONG CDnDDataObject::Release (void)
{

}

// IDataObject members
HRESULT CDnDDataObject::GetData (FORMATETC *pFormatEtc, STGMEDIUM *pmedium)
{

}

HRESULT CDnDDataObject::GetDataHere (FORMATETC *pFormatEtc, STGMEDIUM *pmedium)
{

}

HRESULT CDnDDataObject::QueryGetData (FORMATETC *pFormatEtc)
{
    return (LookupFormatEtc(pFormat) == -1) ? DV_E_FORMATETC : S_OK;
}

HRESULT CDnDDataObject::GetCanonicalFormatEtc (FORMATETC *pFormatEct, FORMATETC *pFormatEtcOut)
{

}

HRESULT CDnDDataObject::SetData (FORMATETC *pFormatEtc, STGMEDIUM *pMedium, BOOL fRelease)
{

}

HRESULT CDnDDataObject::EnumFormatEtc (DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc)
{

}

HRESULT CDnDDataObject::DAdvise (FORMATETC *pFormatEtc, DWORD advf, IAdviseSink *, DWORD *)
{

}

HRESULT CDnDDataObject::DUnadvise (DWORD dwConnection)
{

}

HRESULT CDnDDataObject::EnumDAdvise (IEnumSTATDATA **ppEnumAdvise)
{

}

int CDnDDataObject::LookupFormatEtc(FORMATETC *pFormatEtc)
{
    // check each of our formats in turn to see if one matches
    for(int i = 0; i < m_nNumFormats; i++)
    {
        if( (m_pFormatEtc[i].tymed & pFormatEtc->tymed)      &&
            m_pFormatEtc[i].cfFormat == pFormatEtc->cfFormat &&
            m_pFormatEtc[i].dwAspect == pFormatEtc->dwAspect)
        {
            // return index of stored format
            return i;
        }
    }

    // error, format not found
    return -1;
}
