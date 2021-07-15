#include "../ginc.h"
#include "ImageList.h"

CImageList::CImageList()
{

}

CImageList::~CImageList()
{

}

bool CImageList::Create(int width, int height, int flags)
{
#ifdef __WXMSW__
	ImageList_Create(width, height, flags, 1, 1);
#else
#endif // __WXMSW__

	return true;
}

#ifdef __WXMSW__
void CImageList::Attach(HIMAGELIST hImageList)
{
	m_bSetHImageList = true;
	m_hImageList = hImageList;
}
#endif

void CImageList::Attach(wxImageList* pImageList)
{
	m_bSetHImageList = false;
	m_pImageList = pImageList;
}

bool CImageList::Draw(int index, wxDC* pDC, int x, int y, int flags)
{
	if(!m_bSetHImageList)
	{
		if(!m_pImageList)
		{
			wxMessageBox(wxT("wxImageList is null"), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
			return false;
		}

		return m_pImageList->Draw(index, *pDC, x, y, flags);
	}

#ifdef __WXMSW__
	if(!m_hImageList)
	{
		wxMessageBox(wxT("HIMAGELIST is null"), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

	return ImageList_Draw(m_hImageList, index, pDC->GetHDC(), x, y, flags);
#endif // __WXMSW__
}
