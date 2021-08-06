#include "ginc.h"
#include "ViewPanel.h"

#ifdef __WXMSW__
	#include "./view/msw/LocalFileListView.h"
#endif

wxBEGIN_EVENT_TABLE(CViewPanel, wxPanel)
	EVT_ERASE_BACKGROUND(CViewPanel::OnErase)
wxEND_EVENT_TABLE()

CViewPanel::CViewPanel(wxWindow* parent, int iID, const wxString& strPath)
	: wxPanel(parent, iID, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxCLIP_CHILDREN | wxCLIP_SIBLINGS | wxNO_FULL_REPAINT_ON_RESIZE)
	, m_Mainsizer(nullptr)
	, m_strPath(strPath)
{
	Init(m_strPath);
}

CViewPanel::~CViewPanel()
{

}

void CViewPanel::Init(const wxString& strPath, bool bRemote)
{
	wxString _strPath(strPath);
	m_Mainsizer = new wxBoxSizer(wxVERTICAL);

	//패스뷰 등록
	m_pPathView = std::make_unique<CPathView>(this, wxPATHVIEW_ID++, wxDefaultPosition, wxSize(wxDefaultSize.GetWidth(), 20));

	//파일 리스트뷰 생성
	m_pFileListView = std::make_unique<CLocalFileListView>(this, ++wxFILFELIST_VIEWID, wxDefaultSize);
	if (!wxDirExists(_strPath))
		_strPath = wxT("C:\\");

	//생성한 뷰 Sizer등록
	m_Mainsizer->Add(m_pPathView.get(), 0, wxEXPAND, 0);
	m_Mainsizer->Add(m_pFileListView.get(), wxSizerFlags(1).Expand().Border(wxALL, 0));

	SetSizer(m_Mainsizer);
	m_Mainsizer->SetSizeHints(this);

	m_pFileListView->LoadDir(_strPath);
}

void CViewPanel::OnErase(wxEraseEvent& WXUNUSED(event))
{

}

void CViewPanel::SetActivateView()
{
	m_pFileListView->SetFocus();
}

void CViewPanel::TransferInfomation(TRANSFER_PATH_TYPE _transferType, const wxString& strInfo)
{
	switch(_transferType)
	{
		case TRANSFER_LISTVIEW_TO_PATHVIEW:
			m_pPathView->AddPath(strInfo);
			break;

		case TRANSFER_PATH_VIEW_TO_LISTVIEW:
			m_pFileListView->LoadDir(strInfo);
			SetActivateView();
			break;

		default:
			break;
	}
}

void CViewPanel::SendEventToListView(_MENU_EVENT_TYPE _mType)
{
	switch(_mType)
	{
		case _MENU_FILE_RENAME:
		{
			wxCommandEvent evt(wxEVT_ITEM_RENAME);
			wxPostEvent(m_pFileListView.get(), evt);
		}
			break;

		case _MENU_FILE_EDIT:
		{
			wxCommandEvent evt(wxEVT_FILE_EDIT);
			wxPostEvent(m_pFileListView.get(), evt);
		}
			break;

		case _MENU_EDIT_ALL_RELEASE:
		case _MENU_EDIT_ALL_SELECT:
		{
			wxCommandEvent evt(wxEVT_ITEM_ALL_RELEASE_OR_SELECT);
			evt.SetInt(_mType);
			wxPostEvent(m_pFileListView.get(), evt);
		}
			break;

		case _MENU_EDIT_CONTEXT_MENU:
		{
			wxCommandEvent evt(wxEVT_EDIT_VIEW_CONTEXT_MENU);
			evt.SetInt(_mType);
			wxPostEvent(m_pFileListView.get(), evt);
		}
			break;

		case _MENU_VIEW_SUBDIR:
		{
			wxCommandEvent evt(wxEVT_VIEW_SUBDIR);
			wxPostEvent(m_pFileListView.get(), evt);
		}
			break;

		case _MENU_ETC_ADD_DRIVE:
		{
			wxCommandEvent evt(wxEVT_DRIVE_ADD_REMOVE);
			wxPostEvent(m_pFileListView.get(), evt);
		}
			break;

		case _MENU_DISK_SPACE_UPDATE:
		{
			wxCommandEvent evt(wxEVT_DISK_SPACE_UPDATE);
			wxPostEvent(m_pFileListView.get(), evt);
		}
			break;

		default:
			break;
	}
}

void CViewPanel::GetFileOperationSrc(std::list<wxString>& lstSrc, bool IsCut)
{
	m_pFileListView->GetFileOperationSource(lstSrc, IsCut);
}

void CViewPanel::ReloadPathOfView(const wxString& strInfo)
{
	wxString strPath(strInfo);
	if(strInfo.IsEmpty())
		strPath = m_pFileListView->m_strCurrentPath;

	theSplitterManager->ChangeTabPagePathName(strPath);
	m_pFileListView->LoadDir(strPath);
}

void CViewPanel::ClearSelectedInfoOfView(bool bDelete)
{
	m_pFileListView->DoSelectedItemsClear(bDelete);
}

void CViewPanel::ChangeViewColumn()
{
	m_pFileListView->m_bSizeOrColumnChanged = true;
	theCommonUtil->RefreshWindow(m_pFileListView.get(), m_pFileListView->m_viewRect);
}

void CViewPanel::ChangeSort()
{
	m_pFileListView->DoSortStart(true);
	theCommonUtil->RefreshWindow(m_pFileListView.get(), m_pFileListView->m_viewRect);
}

bool CViewPanel::GetDeleteItems(std::list<wxString>& lstDatas, bool bTrash)
{
	if(!m_pFileListView->GetTrashOrDeleteData(lstDatas, bTrash))
		return false;

	if(lstDatas.size() == 0)
		return false;

	return true;
}

void CViewPanel::GetCompressItems(std::vector<wxString>& vecItems, wxString& strCompressedFile)
{
	m_pFileListView->GetCompressedItems(vecItems, strCompressedFile);
}

wxString CViewPanel::GetCompressedFileForDecompress()
{
	return m_pFileListView->GetCompressedFileForDecompress();
}
