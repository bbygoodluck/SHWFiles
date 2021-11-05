#ifndef DNDDATAOBJECT_H_INCLUDED
#define DNDDATAOBJECT_H_INCLUDED

class CDnDDataObject : IDataObject
{
public:
    // Constructor / Destructor
    CDnDDataObject(FORMATETC *fmtetc, STGMEDIUM *stgmed, int count);
    ~CDnDDataObject();

public:
    // IUnknown members
    HRESULT __stdcall QueryInterface (REFIID iid, void ** ppvObject);
    ULONG __stdcall AddRef (void);
    ULONG __stdcall Release (void);

    // IDataObject members
    HRESULT __stdcall GetData (FORMATETC *pFormatEtc, STGMEDIUM *pmedium);
    HRESULT __stdcall GetDataHere (FORMATETC *pFormatEtc, STGMEDIUM *pmedium);
    HRESULT __stdcall QueryGetData (FORMATETC *pFormatEtc);
    HRESULT __stdcall GetCanonicalFormatEtc (FORMATETC *pFormatEct, FORMATETC *pFormatEtcOut);
    HRESULT __stdcall SetData (FORMATETC *pFormatEtc, STGMEDIUM *pMedium, BOOL fRelease);
    HRESULT __stdcall EnumFormatEtc (DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc);
    HRESULT __stdcall DAdvise (FORMATETC *pFormatEtc, DWORD advf, IAdviseSink *, DWORD *);
    HRESULT __stdcall DUnadvise (DWORD dwConnection);
    HRESULT __stdcall EnumDAdvise (IEnumSTATDATA **ppEnumAdvise);

private:
    // any private members and functions
    LONG m_lRefCount;
    int LookupFormatEtc(FORMATETC *pFormatEtc);
};

#endif // DNDDATAOBJECT_H_INCLUDED
