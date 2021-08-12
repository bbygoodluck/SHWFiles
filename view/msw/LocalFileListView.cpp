#include "../../ginc.h"
#include "../../ViewPanel.h"
#include "LocalFileListView.h"


wxBEGIN_EVENT_TABLE(CLocalFileListView, CListView)
	EVT_PAINT(CLocalFileListView::OnPaint)
	EVT_ERASE_BACKGROUND(CLocalFileListView::OnErase)
	EVT_FSWATCH(wxID_ANY, CLocalFileListView::OnFileSystemWatcher)
#ifdef __WXMSW__
	EVT_MY_CUSTOM_COMMAND(wxEVT_DRIVE_ADD_REMOVE, wxID_ANY, CLocalFileListView::OnDriveAddOrRemove)
	EVT_MY_CUSTOM_COMMAND(wxEVT_DISK_SPACE_UPDATE, wxID_ANY, CLocalFileListView::OnUpdateDriveSpace)
#endif
wxEND_EVENT_TABLE()

CLocalFileListView::CLocalFileListView(wxWindow* parent, const int nID, const wxSize& sz)
	: CListView(parent, nID, sz)
{
	m_pFileSystemWatcher = std::make_unique<CLocalFileSystemWatcher>(this);
}

CLocalFileListView::~CLocalFileListView()
{

}

void CLocalFileListView::OnErase(wxEraseEvent& event)
{

}

void CLocalFileListView::OnPaint(wxPaintEvent& event)
{
	if (!m_pDoubleBuffer->IsOk() || m_pDoubleBuffer == nullptr)
		return;

	wxPaintDC dc(this);
	wxMemoryDC* pMemDC = CreateMemoryDC(&dc, m_viewRect);
	Render(pMemDC);

	dc.Blit(0, 0, m_pDoubleBuffer->GetWidth(), m_pDoubleBuffer->GetHeight(), pMemDC, 0, 0);
}

void CLocalFileListView::Render(wxDC* pDC)
{
	// 컬럼계산
	CalcColumn(pDC);
	// 컬럼그리기
	DrawColumn(pDC);
	//디렉토리 정보표시창
	DrawDirInfoArea(pDC);
	//데이터 표시
	DisplayItems(pDC);

	m_bSizeOrColumnChanged = false;
	m_bDirLoaded   = false;
}

void CLocalFileListView::LoadDir(const wxString& strPath)
{
	wxBusyCursor wait;

	TerminateThread();
//	RunReadImageList();
	ReadyImageListThread();

	Initialize();

	//감시디렉토리 설정
	m_pFileSystemWatcher->AddPath(strPath);

	m_pViewPanel->TransferInfomation(TRANSFER_LISTVIEW_TO_PATHVIEW, strPath);

	m_strCurrentPath = strPath;
	m_iPathDepth = theCommonUtil->GetPathDepth(strPath);

	if (!ReadDirectory())
		return;

	DoSortStart();

	AddDrive();
	m_nTotalItems = m_itemList.size();
	RunReadImageList();

	if (m_nCurrentItemIndex > m_nTotalItems)
		m_nCurrentItemIndex = 0;

	m_bDirLoaded = true;

	m_strVolume = theCommonUtil->GetVolume(wxString(m_strCurrentPath));
	m_strVolume += wxT(":") + SLASH;

	SetDiskSpace();

	_HISTORY::const_iterator fIter = m_hashHistory.find(strPath);
	if(fIter != m_hashHistory.end())
	{
		_HistoryItem historyItem = fIter->second;

		m_nCurrentItemIndex = historyItem.iCurrentIndex;
		m_nStartIndex = historyItem.iStartIndex;
	}

	theCommonUtil->RefreshWindow(this, m_viewRect);
}

bool CLocalFileListView::ReadDirectory()
{
	CLocalFileSystem localFileSys;
	if (!localFileSys.BeginFindFiles(m_strCurrentPath, false))
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_SEARCH_DIR_INIT_ERR")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

	wxString strDesc(wxT(""));
	wxString strName(wxT(""));
	wxString strExt(wxT(""));
	bool isDir = false;
	unsigned long lattr = 0;
	wxLongLong llSize(0);
	wxDateTime dt(0.0);

	while (localFileSys.GetNextFile(strName, &isDir, &lattr, &llSize, &dt))
	{
		if (!theJsonConfig->IsViewAllFile())
		{
			if (!theJsonConfig->IsViewHiddenFile())
			{
				if (lattr & ATTR_HIDDEN)
					continue;
			}
			/*
			else if(theJsonConfig->IsViewHiddenFile() && !theJsonConfig->IsViewSystemFile())
			{
				if (lattr & ATTR_SYSTEM)
					continue;
			}
			else if(!theJsonConfig->IsViewHiddenFile() && theJsonConfig->IsViewSystemFile())
			{
				if (lattr & ATTR_HIDDEN)
				{
					if(!(lattr & ATTR_SYSTEM))
						continue;
				}
			}
			*/
		}

		CDirData dirItem;
		dirItem.SetName(strName);

		if (isDir)
		{
			dirItem.SetType(CDirData::item_type::dir);
			m_iDirCount++;
			strDesc = theMsgManager->GetMessage(wxT("MSG_DIR_DESCRIPTION"));
		}
		else
		{
			dirItem.SetType(CDirData::item_type::file);

			m_iFileCount++;
			strExt = theCommonUtil->GetExt(strName);
			strDesc = theExtInfo->GetExtInfo(strExt, m_strCurrentPath + (m_iPathDepth != 1 ? SLASH + strName : strName));

			m_dblFileSizeInDir += llSize.ToDouble();
		}

		dirItem.SetAttribute(lattr);
		dirItem.SetSize(llSize);
		dirItem.SetDateTime(dt);
		dirItem.SetPath(m_strCurrentPath);
		dirItem.SetIconIndex(0);
		dirItem.SetMatch(false);
		dirItem.SetExt(strExt);
		dirItem.SetTypeName(strDesc);

		m_itemList.push_back(dirItem);

		m_strMaxName = FindMaxData(strName, m_strMaxName);
		m_strMaxTypeName = FindMaxData(strDesc, m_strMaxTypeName);

		strName = wxT("");
		strDesc = wxT("");
		strExt = wxT("");

		isDir = false;
		lattr = 0;
		llSize = 0;
		dt = 0;
	}

	return true;
}

void CLocalFileListView::AddDrive()
{
	wxString strDetailInfo(wxT(""));
	//드라이브 정보 적재
	int iDriveCount = theDriveInfo->GetDriveCount();
	wxString strName(wxT(""));
	wxString strDrivespaceInfo(wxT(""));

	m_strMaxDriveName = theDriveInfo->GetMaxDriveName();

	for (int iIndex = 0; iIndex < iDriveCount; iIndex++)
	{
		CDirData dirItem;
		CDriveItem* drvItem = theDriveInfo->GetDriveItem(iIndex);

		wxString strDriveName(drvItem->GetDriveName());
		int iDriveType = drvItem->GetDriveType();

		strName = drvItem->GetDisplayName();
		strDrivespaceInfo = drvItem->GetSpace();
//		if ((iDriveType != wxFS_VOL_CDROM) &&
//			(iDriveType != wxFS_VOL_DVDROM))
//			strName += wxT(" - ") + drvItem->GetSpace();

		dirItem.SetName(strName);
		dirItem.SetDriveName(drvItem->GetDriveName());
		dirItem.SetDriveSpaceInfo(strDrivespaceInfo);
		dirItem.SetSize(0);
		dirItem.SetType(CDirData::item_type::drive);
		dirItem.m_iIconIndex = drvItem->GetIconIndex();
		dirItem.m_iOvelayIndex = drvItem->GetOverlayIconIndex();
		dirItem.m_bIconImageSet = true;

		m_itemList.push_back(dirItem);
	}
}

void CLocalFileListView::OnFileSystemWatcher(wxCustomFileSystemWatcherEvent& event)
{
	int iWatchType = event.GetWatchType();
	wxString strOldName = event.GetOldName();
	wxString strNewName = event.GetNewName();

	switch(iWatchType)
	{
		case FS_WATCHER_CREATE:
			DoCreate(strNewName);
			break;

		case FS_WATCHER_DELETE:
			DoDelete(strNewName);
			break;

		case FS_WATCHER_MODIFY:
			DoModify(strNewName);
			break;

		case FS_WATCHER_RENAME:
			DoRename(strOldName, strNewName);
			break;

		default:
			break;
	}
}

void CLocalFileListView::DoCreate(const wxString& strName)
{
	wxString strFullPathName = MakeFullPathName(strName);
	bool isDir = false;
	unsigned long lattr = 0;
	wxLongLong llSize(0);
	wxDateTime dt(0.0);

	if(!CLocalFileSystem::GetAttributeInfo(strFullPathName, isDir, lattr, &llSize, &dt))
		return;

	if(!isDir)
	{
		if(!CLocalFileSystem::IsWritable(strFullPathName, FILE_SHARE_READ, true))
			return;
	}

	if (!theJsonConfig->IsViewAllFile())
	{
		if (!theJsonConfig->IsViewHiddenFile())
		{
			if (lattr & ATTR_HIDDEN)
				return;
		}
	}

	RemoveDrive();

	wxString strExt(wxT(""));
	wxString strDesc(wxT(""));

	CDirData dirItem;
	dirItem.SetName(strName);

	strDesc = wxT("");
	strExt = wxT("");

	if (isDir)
	{
		m_iDirCount++;
		dirItem.SetType(CDirData::item_type::dir);
		strDesc = theMsgManager->GetMessage(wxT("MSG_DIR_DESCRIPTION"));
	}
	else
	{
		m_iFileCount++;
		dirItem.SetType(CDirData::item_type::file);

		strExt = theCommonUtil->GetExt(strName);
		strDesc = theExtInfo->GetExtInfo(strExt, strFullPathName);

		m_dblFileSizeInDir += llSize.ToDouble();
	}

	dirItem.SetAttribute(lattr);
	dirItem.SetSize(llSize);

	dirItem.SetDateTime(dt);
	dirItem.SetPath(m_strCurrentPath);
	dirItem.SetExt(strExt);
	dirItem.SetTypeName(strDesc);

	m_itemList.push_back(dirItem);
	m_nTotalItems = wx_static_cast(int, m_itemList.size());

	m_strMaxName = FindMaxData(strName, m_strMaxName);
	m_strMaxTypeName = FindMaxData(strDesc, m_strMaxTypeName);

	SetItemImage(m_nTotalItems - 1);

	DoSortStart();
	AddDrive();

	m_nTotalItems = wx_static_cast(int, m_itemList.size());

	UpdateLastAccessTime(wxDateTime::Now());
	m_bSizeOrColumnChanged = true;

	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CLocalFileListView::DoModify(const wxString& strName)
{
	wxString strFullPathName = MakeFullPathName(strName);
	bool isDir = false;
	unsigned long lattr = 0;
	wxLongLong llSize(0);
	wxDateTime dt(0.0);

	if(!CLocalFileSystem::GetAttributeInfo(strFullPathName, isDir, lattr, &llSize, &dt))
		return;

	if(!isDir)
	{
		if(!CLocalFileSystem::IsWritable(strFullPathName, FILE_SHARE_READ | FILE_SHARE_WRITE, true))
			return;
	}

	//아이템이 존재하는지 체크
	bool bExist = false;
	wxVector<CDirData>::iterator iter = GetItemExist(strName, bExist);
	if(iter == m_itemList.end())
		return;

	int iSortType = theJsonConfig->GetSortType();
	if(iSortType == VIEW_SORT_SIZE)
		RemoveDrive();

	if(iter->IsFile())
	{
		m_dblFileSizeInDir -= iter->GetSize().ToDouble();
		m_dblFileSizeInDir += llSize.ToDouble();
	}

	iter->SetAttribute(lattr);
	iter->SetSize(llSize);
	iter->SetDateTime(dt);

	if(iSortType == VIEW_SORT_SIZE)
	{
		DoSortStart();
		AddDrive();
	}

	UpdateLastAccessTime(wxDateTime::Now());
	theCommonUtil->RefreshWindow(this, m_viewRect);

	theMenuOperation->UpdateDriveSpace(m_strVolume);
}

void CLocalFileListView::DoDelete(const wxString& strName)
{
	bool bExist = false;
	wxVector<CDirData>::iterator iter = GetItemExist(strName, bExist);

	if(iter == m_itemList.end())
		return;

	wxString strDesc = iter->GetTypeName();

	m_strMaxName = FindMaxData(strName, m_strMaxName);
	m_strMaxTypeName = FindMaxData(strDesc, m_strMaxTypeName);

	if (iter->IsDir())
		m_iDirCount--;
	else
	{
		m_dblFileSizeInDir -= iter->GetSize().ToDouble();
		m_iFileCount--;
	}

	m_itemList.erase(iter);

	m_nTotalItems = wx_static_cast(int, m_itemList.size());
	if(m_nCurrentItemIndex > m_nTotalItems)
	{
		m_nStartIndex = 0;
		m_nCurrentItemIndex = 0;
	}

	UpdateLastAccessTime(wxDateTime::Now());

	m_bSizeOrColumnChanged = true;
	theCommonUtil->RefreshWindow(this, m_viewRect);

	theMenuOperation->UpdateDriveSpace(m_strVolume);
}

void CLocalFileListView::DoRename(const wxString& strOldName, const wxString& strNewName)
{
	bool isDir = false;
	unsigned long lattr = 0;
	wxLongLong llSize(0);
	wxDateTime dt(0.0);

	wxString strFullPathName = MakeFullPathName(strNewName);
	if(!CLocalFileSystem::GetAttributeInfo(strFullPathName, isDir, lattr, &llSize, &dt))
		return;

	if(!isDir)
	{
		if(!CLocalFileSystem::IsWritable(strFullPathName, FILE_SHARE_READ, true))
			return;
	}

	bool bOldExist = false;


	wxVector<CDirData>::iterator iterOld = GetItemExist(strOldName, bOldExist);

	if(iterOld == m_itemList.end())
		return;

	m_itemList.erase(iterOld);

	bool bNewExist = false;
	wxVector<CDirData>::iterator iterNew = GetItemExist(strNewName, bNewExist);

	if(iterNew != m_itemList.end())
		m_itemList.erase(iterNew);

	RemoveDrive();

	CDirData dirItem;
	dirItem.SetName(strNewName);
	wxString strDesc(wxT(""));

	if(!isDir)
	{
		wxString strExt = theCommonUtil->GetExt(strNewName);
		strDesc = theExtInfo->GetExtInfo(strExt, strFullPathName);

		dirItem.SetType(CDirData::item_type::file);
		dirItem.SetExt(strExt);
	}
	else
	{
		dirItem.SetType(CDirData::item_type::dir);
		strDesc = theMsgManager->GetMessage(wxT("MSG_DIR_DESCRIPTION"));

		m_dblFileSizeInDir += llSize.ToDouble();
	}

	dirItem.SetAttribute(lattr);
	dirItem.SetSize(llSize);
	dirItem.SetDateTime(dt);
	dirItem.SetPath(m_strCurrentPath);

	dirItem.SetTypeName(strDesc);

	m_itemList.push_back(dirItem);
	m_nTotalItems = wx_static_cast(int, m_itemList.size());

	SetItemImage(m_nTotalItems - 1);

	DoSortStart();
	AddDrive();

	m_nTotalItems = wx_static_cast(int, m_itemList.size());

	m_strMaxName = FindMaxData(strNewName, m_strMaxName);
	m_strMaxTypeName = FindMaxData(strDesc, m_strMaxTypeName);

	UpdateLastAccessTime(wxDateTime::Now());

	m_bSizeOrColumnChanged = true;
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CLocalFileListView::ShowFavoriteMenu()
{
	if (theBookmark->GetBookmarkSize() == 0)
	{
		wxMessageBox(wxT("Registered bookmark item is nothing"), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
		return;
	}

	int iItemPosition = m_nCurrentItemIndex - m_nStartIndex;
	CPositionInfo posInfo = m_posList.at(iItemPosition);

	wxRect rcPos(posInfo.m_nameRect);
	rcPos.SetLeft(posInfo.m_nameRect.GetLeft() + 10);
	rcPos.SetBottom(posInfo.m_nameRect.GetBottom() + 1);

	int iFavoriteRight = posInfo.m_nameRect.GetRight() + 3;
	int iViewRight = m_viewRect.GetRight();

	if (iFavoriteRight >= iViewRight)
		rcPos.SetLeft(rcPos.GetLeft() - 20);

	wxPoint pt(rcPos.GetLeft() , rcPos.GetTop() + rcPos.GetHeight());
	wxMenu menuFavorite;
	theBookmark->CreateBookmarkMenu(&menuFavorite);

	this->PopupMenu(&menuFavorite, pt);
}

void CLocalFileListView::OnDriveAddOrRemove(wxCommandEvent& event)
{
	RemoveDrive();
	AddDrive();

	m_nTotalItems = m_itemList.size();

	ReCalcPage();

	m_bSizeOrColumnChanged = true;
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CLocalFileListView::RemoveDrive()
{
	wxVector<CDirData>::iterator iter = m_itemList.end() - 1;
	for (iter; iter != m_itemList.begin(); --iter)
	{
		if (!iter->IsDrive())
			break;

		m_itemList.pop_back();
	}
}

void CLocalFileListView::OnUpdateDriveSpace(wxCommandEvent& event)
{
	wxString strDriveName(wxT(""));
	wxVector<CDirData>::iterator iter = m_itemList.end() - 1;

	for (iter; iter != m_itemList.begin(); --iter)
	{
		strDriveName = iter->GetDriveName();

		if (m_strVolume.CmpNoCase(strDriveName) == 0)
		{
			CDriveItem* drvItem;
			int iDriveType;

			drvItem = theDriveInfo->GetDriveItem(m_strVolume);
			iDriveType = drvItem->GetDriveType();

			if ((iDriveType != wxFS_VOL_CDROM) &&
				(iDriveType != wxFS_VOL_DVDROM))
			{
				wxString strSpace = drvItem->GetSpace();
				iter->SetDriveSpaceInfo(strSpace);
			}

			break;
		}
	}
}
