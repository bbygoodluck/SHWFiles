#ifndef VIEWPANEL_H_INCLUDED
#define VIEWPANEL_H_INCLUDED

#include "./view/ListView.h"
#include "./view/PathView.h"

class CViewPanel : public wxPanel
{
public:
	explicit CViewPanel(wxWindow* parent, int iID, const wxString& strPath);
	virtual ~CViewPanel();

public:
	void Init(const wxString& strPath, bool bRemote = false);
	void SetActivateView();

	void TransferInfomation(TRANSFER_PATH_TYPE _transferType, const wxString& strInfo = wxT(""));
	void SendEventToListView(_MENU_EVENT_TYPE _mType);
	void ClearSelectedInfoOfView(bool bDelete = false);

	wxString GetCurrentViewPath() { return m_pFileListView->m_strCurrentPath; }

	void GetFileOperationSrc(std::list<wxString>& lstSrc , bool IsCut = false);
	void ReloadPathOfView(const wxString& strInfo = wxT(""));
	void ChangeViewColumn();
	void ChangeSort();
	bool GetDeleteItems(std::list<wxString>& lstDatas, bool bTrash);
	void GetCompressItems(std::vector<wxString>& vecItems, wxString& strCompressedFile);
	wxString GetCompressedFileForDecompress();

private:
	wxBoxSizer* m_Mainsizer;
	wxString	m_strPath;
	wxSize		m_size;

	//패스정보
	std::unique_ptr<CPathView>    m_pPathView = nullptr;
	// 폴더/파일 리스트뷰
	std::unique_ptr<CListView> m_pFileListView = nullptr;
private:
	void OnErase(wxEraseEvent& event);
	wxDECLARE_EVENT_TABLE();
};

#endif // VIEWPANEL_H_INCLUDED
