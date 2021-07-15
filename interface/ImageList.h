#ifndef IMAGELIST_H_INCLUDED
#define IMAGELIST_H_INCLUDED

class CImageList
{
public:
	CImageList();
	~CImageList();

	bool Create(int width, int height, int flags);
	bool Draw(int index, wxDC* pDC, int x, int y, int flags);

#ifdef __WXMSW__
	void Attach(HIMAGELIST hImageList);
#endif

	void Attach(wxImageList* pImageList);
private:
#ifdef __WXMSW__
	HIMAGELIST m_hImageList = NULL;
#endif

	wxImageList* m_pImageList = nullptr;

	bool m_bSetHImageList = false;
};

#endif // IMAGELIST_H_INCLUDED
