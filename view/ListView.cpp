#include "../ginc.h"
#include "../ViewPanel.h"
#include "../interface/ImageList.h"
#include "MyToolTipView.h"
#include "ListView.h"

#ifdef __WXMSW__
	#include "../interface/msw/MSWContextMenuHandler.h"
#else
#endif

#include <wx/dir.h>

wxBEGIN_EVENT_TABLE(CListView, wxWindow)
    EVT_CHAR_HOOK(CListView::OnCharHook)
    EVT_CHAR(CListView::OnChar)
    EVT_SET_FOCUS(CListView::OnSetFocus)
	EVT_KILL_FOCUS(CListView::OnKillFocus)
	EVT_SIZE(CListView::OnSize)
	EVT_LEFT_DOWN(CListView::OnMouseLBottonDown)
	EVT_LEFT_DCLICK(CListView::OnMouseLButtonDBClick)
	EVT_RIGHT_DOWN(CListView::OnMouseRButtonDown)
	EVT_RIGHT_UP(CListView::OnMouseRButtonUp)
	EVT_MOTION(CListView::OnMouseMove)
	EVT_MOUSEWHEEL(CListView::OnMouseWheel)
	EVT_IDLE(CListView::OnIdle)
	//Custom Event
	EVT_MY_CUSTOM_COMMAND(wxEVT_ITEM_RENAME, wxID_ANY, CListView::OnItemRename)
	EVT_MENU_RANGE(EXTERNAL_PROGRAM_START_ID, EXTERNAL_PROGRAM_END_ID, CListView::OnMenuFileEditProcess)
	EVT_MY_CUSTOM_COMMAND(wxEVT_FILE_EDIT, wxID_ANY, CListView::OnMenuFileEdit)
	EVT_MY_CUSTOM_COMMAND(wxEVT_ITEM_ALL_RELEASE_OR_SELECT, wxID_ANY, CListView::OnMenuEditAllReleaseOrSelect)
	EVT_MY_CUSTOM_COMMAND(wxEVT_EDIT_VIEW_CONTEXT_MENU, wxID_ANY, CListView::OnMenuEditViewContextMenu)
	EVT_MY_CUSTOM_COMMAND(wxEVT_VIEW_SUBDIR, wxID_ANY, CListView::OnMenuViewSubDir)
	EVT_MENU_RANGE(VIEW_SUBDIR_START_ID, VIEW_SUBDIR_START_END, CListView::OnMenuSelectSubDir)
wxEND_EVENT_TABLE()

CListView::CListView(wxWindow* parent, const int nID, const wxSize& sz)
	: wxWindow(parent, nID, wxDefaultPosition, sz, FILELISTVIEW_STYLE)
	, m_pViewPanel((CViewPanel *)parent)
{
	//상위폴더 이동 이미지
	m_icoUpDir = wxArtProvider::GetIcon(wxART_GO_DIR_UP, wxART_OTHER, wxSize(16, 16));

	m_colDefault = theJsonConfig->GetDefaultColor();
	m_colDrive   = theJsonConfig->GetDriveColor();
	m_colDir     = theJsonConfig->GetDirColor();
	m_colSize    = theJsonConfig->GetDefaultColor();
	m_colTime    = theJsonConfig->GetDefaultColor();
	m_colAttr    = theJsonConfig->GetAttributeColor();
	m_colType    = theJsonConfig->GetDescriptionColor();
	m_colMatch   = theJsonConfig->GetMatchColor();

#ifdef __WXMSW__
	m_pImageList = new CImageList();
	m_pImageList->Attach(_gImageList);
#else
#endif // __WXMSW__
	//이름변경 컨트롤
	m_pTxtCtrlForRename = std::make_unique<wxTextCtrl>(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxBORDER_THEME);
	m_pTxtCtrlForRename->SetBackgroundColour(wxColour(220, 220, 220));
	m_pTxtCtrlForRename->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_pTxtCtrlForRename->SetFont(*_gFont);
	m_pTxtCtrlForRename->Show(false);

	m_pTxtCtrlForRename->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(CListView::OnKeyDownTextCtrl), NULL, this);
	m_pTxtCtrlForRename->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(CListView::OnEnterTextCtrl), NULL, this);
	m_pTxtCtrlForRename->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(CListView::OnKillFocusTxtCtrl), NULL, this);

	m_pMyTooltipView = new CMyTooltipView(this);
	m_pMyTooltipView->SetSize(60, 20);
	m_pMyTooltipView->Show(false);

	m_pMyTooltipKeyInput = new CMyTooltipView(this);
	m_pMyTooltipKeyInput->SetSize(60, 20);
	m_pMyTooltipKeyInput->Show(false);

	m_pMyTooltipDetail = new CMyTooltipView(this);
	m_pMyTooltipDetail->SetSize(60, 20);
	m_pMyTooltipDetail->Show(false);

	m_ProcessKeyEvent.push_back(WXK_F4);
	m_ProcessKeyEvent.push_back(WXK_REVERSE_SLASH);
	m_ProcessKeyEvent.push_back(WXK_TAB);
	m_ProcessKeyEvent.push_back(WXK_SPACE);
	m_ProcessKeyEvent.push_back(wxKEY_DOT);
	m_ProcessKeyEvent.push_back(WXK_BACK);
	m_ProcessKeyEvent.push_back(WXK_RETURN);
	m_ProcessKeyEvent.push_back(WXK_LEFT);
	m_ProcessKeyEvent.push_back(WXK_RIGHT);
	m_ProcessKeyEvent.push_back(WXK_UP);
	m_ProcessKeyEvent.push_back(WXK_DOWN);
	m_ProcessKeyEvent.push_back(WXK_PAGEDOWN);
	m_ProcessKeyEvent.push_back(WXK_PAGEUP);
	m_ProcessKeyEvent.push_back(WXK_HOME);
	m_ProcessKeyEvent.push_back(WXK_END);

	SetBackgroundStyle(wxBG_STYLE_PAINT);
}

CListView::~CListView()
{
	TerminateThread();

	AllClear();
	if(m_pImageList)
		delete m_pImageList;

	m_pImageList = nullptr;

	if(m_pMyTooltipKeyInput)
		delete m_pMyTooltipKeyInput;

	m_pMyTooltipKeyInput = nullptr;

	if(m_pMyTooltipView)
		delete m_pMyTooltipView;

	m_pMyTooltipView = nullptr;

	if(m_pMyTooltipDetail)
		delete m_pMyTooltipDetail;

	m_pMyTooltipDetail = nullptr;

	m_pTxtCtrlForRename->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(CListView::OnKeyDownTextCtrl), NULL, this);
	m_pTxtCtrlForRename->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(CListView::OnEnterTextCtrl), NULL, this);
	m_pTxtCtrlForRename->Disconnect(wxEVT_KILL_FOCUS, wxFocusEventHandler(CListView::OnKillFocusTxtCtrl), NULL, this);
}

void CListView::OnCharHook(wxKeyEvent& event)
{
	int iKeyCode = event.GetKeyCode();

	if(m_bRename || (!m_strKeyInput.IsEmpty() && iKeyCode == 46))
	{
		event.Skip();
		return;
	}

	bool bControlDown = event.ControlDown();
	bool bAltDown     = event.AltDown();
	bool bShiftDown   = event.ShiftDown();

	wxString strKeyName(theCommonUtil->GetKeyName(event));

	if (bShiftDown)
	{
		if(bControlDown)
		{
			event.Skip();
			return;
		}

#ifdef __WXMSW__
		if (theJsonConfig->IsShift_IME_KOR_MoveDrive())
			theCommonUtil->SetImeModeToEnglish(this);

		wxString strDriveName = strKeyName + wxT(":") + SLASH;
		if (theDriveInfo->IsExistDrive(strDriveName))
		{
			theSplitterManager->ChangeTabPagePathName(strDriveName);
			LoadDir(strDriveName);
		}
#endif // __WXMSW__
	}
	else if (bControlDown)
	{
		//Something to do
	}
	else if (bAltDown)
	{
		//Something to do
	}
	else if(iKeyCode == WXK_ESCAPE)
	{
		InitKeyInputTooltip();
		DoSelectedItemsClear();
		theCommonUtil->RefreshWindow(this, m_viewRect);
	}
	else
	{
		wxVector<int>::iterator iter = std::find(m_ProcessKeyEvent.begin(), m_ProcessKeyEvent.end(), iKeyCode);
		if(iter != m_ProcessKeyEvent.end())
			ProcessKeyEvent(iKeyCode);
	}

	event.Skip();
}


void CListView::OnChar(wxKeyEvent& event)
{
	int iKeyCode = event.GetKeyCode();
	if (m_strKeyInput.IsEmpty() && theSkipKeyMap->IsExistSkipKey(iKeyCode))
	{
		event.Skip();
		return;
	}

	bool bShift = wxIsShiftDown();
	bool bControl = wxIsCtrlDown();

	if(bControl)
	{
		event.Skip();
		return;
	}

	wxString strKeyName(theCommonUtil->GetKeyName(event));
	if(bShift)
	{
		if (theJsonConfig->IsShift_IME_KOR_MoveDrive())
		{
			wxString strDrive = wxString(strKeyName).MakeUpper();
			strDrive += wxT(":");
			strDrive += SLASH;

			if (theDriveInfo->IsExistDrive(strDrive))
			{
				event.Skip();
				return;
			}
		}
	}

	bool bRefresh = false;
	int iLenKeyInput = 0;
	if(iKeyCode == WXK_BACK)
	{
		if(!m_strKeyInput.IsEmpty())
		{
			iLenKeyInput = m_strKeyInput.Len();
			m_strKeyInput = m_strKeyInput.Left(iLenKeyInput - 1);
		}
	}
	else
	{
		m_strKeyInput += strKeyName;
		iLenKeyInput = m_strKeyInput.Len();
	}

	if(iLenKeyInput > 0)
	{
		DoMatchClear();

		if(!m_strKeyInput.IsEmpty())
		{
			wxClientDC dc(this);
			wxSize sztip = dc.GetTextExtent(m_strKeyInput);
			wxSize szTooltip;

			wxRect rcWindow =this->GetClientRect();
			wxPoint ptTooltip(rcWindow.GetRight() / 2 - sztip.GetWidth(), rcWindow.GetHeight() / 2);

			szTooltip.SetWidth(sztip.GetWidth() + 10);
			szTooltip.SetHeight(sztip.GetHeight() + 5);

			m_pMyTooltipKeyInput->SetTooltipText(m_strKeyInput);
			m_pMyTooltipKeyInput->SetThemeEnabled(true);
			m_pMyTooltipKeyInput->SetPosition(ptTooltip);
			m_pMyTooltipKeyInput->SetSize(szTooltip);
			m_pMyTooltipKeyInput->Show(true);

			FindMatchItems();
			bRefresh = true;
		}
		else
		{
			InitKeyInputTooltip();
			//클리어
			bRefresh = true;
		}
	}
	else
		InitKeyInputTooltip();

	if(bRefresh)
		theCommonUtil->RefreshWindow(this, m_viewRect);

	event.Skip();
}

void CListView::InitKeyInputTooltip()
{
	DoMatchClear();

	m_pMyTooltipKeyInput->SetTooltipText(wxT(""));
	m_pMyTooltipKeyInput->Show(false);
	m_strKeyInput = wxT("");
}

void CListView::FindMatchItems()
{
	m_matchItems.clear();

	wxString strKeyInput(m_strKeyInput);
#ifdef __WXMSW__
	strKeyInput.MakeLower();
#endif

	wxString strRegEx(wxT("(^") + strKeyInput + wxT(")"));
	wxRegEx _regex(strRegEx);

	wxString strName;
	for (int iIndex = 0; iIndex < m_nTotalItems; iIndex++)
	{
		CDirData* pData = (CDirData *)&m_itemList.at(iIndex);
		strName = pData->GetName();

		if ((strName == wxT("..")) || pData->IsDrive())
			continue;

#ifdef __WXMSW__
		strName.MakeLower();
#endif
		if (!strKeyInput.IsEmpty())
		{
			if (_regex.Matches(strName))
			{
				pData->SetMatch(true);
				m_matchItems.push_back(iIndex);
			}
			else
				pData->SetMatch(false);
		}
		else
			pData->SetMatch(false);
	}

	if (m_matchItems.size() > 0)
	{
		m_nCurrentItemIndex = m_matchItems.at(0);
		ReCalcPage();
	}
}

void CListView::DoMatchClear()
{
	wxVector<int>::const_iterator fIter = m_matchItems.begin();
	while(fIter != m_matchItems.end())
	{
		CDirData* pData = (CDirData *)&m_itemList.at(*fIter);
		pData->SetMatch(false);

		fIter++;
	}

	m_matchItems.clear();
}

void CListView::DoSelectedItemsClear(bool bDeleted)
{
	if(!bDeleted)
	{
		std::unordered_map<int, SELITEM_INFO>::iterator iTer = m_hashSelectedItem.begin();
		while(iTer != m_hashSelectedItem.end())
		{
			SELITEM_INFO _Info = iTer->second;
			CDirData* data = (CDirData *)&m_itemList.at(_Info.m_iSelIndex);

			data->SetItemSelected(false);
			iTer++;
		}

		theCommonUtil->RefreshWindow(this, m_viewRect);
	}

	m_hashSelectedItem.clear();
	m_pMyTooltipView->SetTooltipText(wxT(""));
	m_pMyTooltipView->Show(false);

	m_iSelDirCnt = 0;
	m_iSelFileCnt = 0;
}

void CListView::GetFileOperationSource(std::list<wxString>& lstSrc, bool IsCut)
{
	if(m_hashSelectedItem.size() == 0)
	{
		CDirData* pData = (CDirData *)&m_itemList.at(m_nCurrentItemIndex);
		if(pData->IsDrive())
		{
			wxMessageBox(theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_NOT_SUPPORT_DRIVE")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
			return;
		}

		pData->SetCut(false);
		if(IsCut)
			pData->SetCut(true);

		lstSrc.push_back(pData->GetFullPath());
	}
	else
	{
		std::unordered_map<int, SELITEM_INFO>::iterator iTer = m_hashSelectedItem.begin();
		while(iTer != m_hashSelectedItem.end())
		{
			SELITEM_INFO _Info = iTer->second;
			CDirData* pData = (CDirData *)&m_itemList.at(_Info.m_iSelIndex);

			pData->SetCut(false);
			if(IsCut)
				pData->SetCut(true);

			lstSrc.push_back(pData->GetFullPath());
			iTer++;
		}
	}

	if(IsCut)
		theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CListView::ProcessKeyEvent(const int nKeyCode)
{
	switch (nKeyCode)
	{
		case WXK_F4:
			ShowFavoriteMenu();
			break;

		case WXK_REVERSE_SLASH:
			GotoRoot();
			break;

		case WXK_TAB:
			theSplitterManager->ChangeActiveTab();
			break;

		case WXK_SPACE:
			if (!SetSelectedItem(WXK_SPACE))
				return;

			m_nCurrentItemIndex++;
			MoveRightAndDownKey();
			break;

		case wxKEY_DOT:
		case WXK_BACK:
		{
			if(nKeyCode == WXK_BACK && !m_strKeyInput.IsEmpty())
				return;

				//최상위 Path
			if(m_iPathDepth == 1)
				return;

			m_nCurrentItemIndex = 0;
			if (!PressEnterKey())
				return;
		}
			break;

		case WXK_RETURN:
		{
			if (!PressEnterKey())
				return;
		}
			break;

		case WXK_LEFT:
			if (m_nDispColumn > 1)
			{
				m_nCurrentItemIndex -= m_nItemCountInColumn;
				MoveLeftAndUpKey();
			}

			break;

		case WXK_RIGHT:
			if (m_nDispColumn > 1)
			{
				m_nCurrentItemIndex += m_nItemCountInColumn;
				MoveRightAndDownKey();
			}
			break;

		case WXK_UP:
			if (wxGetKeyState(WXK_SHIFT))
			{
				if (!SetSelectedItem(WXK_UP))
					return;
			}
			else
			{
				m_nCurrentItemIndex--;
				MoveLeftAndUpKey();
			}

			break;

		case WXK_DOWN:
			if (wxGetKeyState(WXK_SHIFT))
			{
				if (!SetSelectedItem(WXK_DOWN))
					return;
			}
			else
			{
				m_nCurrentItemIndex++;
				MoveRightAndDownKey();
			}

			break;

		case WXK_PAGEDOWN:
			m_nCurrentItemIndex += m_nDisplayItemInView;

			if ((m_nStartIndex + (m_nItemCountInColumn * m_nDispColumn)) < m_nTotalItems)
				m_nStartIndex += (m_nItemCountInColumn * m_nDispColumn);

			if (m_nCurrentItemIndex >= (m_nTotalItems - 1))
				m_nCurrentItemIndex = m_nTotalItems - 1;

			break;

		case WXK_PAGEUP:
			m_nCurrentItemIndex -= (m_nItemCountInColumn * m_nDispColumn);
			m_nStartIndex -= (m_nItemCountInColumn * m_nDispColumn);

			if (m_nStartIndex <= 0)
				m_nStartIndex = 0;

			if (m_nCurrentItemIndex <= 0)
				m_nCurrentItemIndex = 0;

			break;

		case WXK_HOME:
			m_nCurrentItemIndex = 0;
			m_nStartIndex = 0;
			break;

		case WXK_END:
			if (m_nTotalItems > m_nDisplayItemInView)
			{
				int iMod = m_nTotalItems % m_nDisplayItemInView;
				m_nStartIndex = m_nTotalItems - iMod;

				if (m_nStartIndex <= 0)
					m_nStartIndex = 0;
			}

			m_nCurrentItemIndex = m_nTotalItems - 1;
			break;

		default:
			 return;

	}

	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CListView::MoveLeftAndUpKey()
{
	if (!((m_nCurrentItemIndex >= m_nStartIndex) && (m_nCurrentItemIndex <= (m_nStartIndex + m_nDisplayItemInView))))
		m_nStartIndex -= m_nItemCountInColumn;

	if (m_nCurrentItemIndex <= 0)
	{
		m_nCurrentItemIndex = 0;
		m_nStartIndex = 0;
	}
}

void CListView::MoveRightAndDownKey()
{
	if ((m_nStartIndex + m_nDisplayItemInView) <= (m_nTotalItems - 1))
	{
		if (m_nCurrentItemIndex > ((m_nStartIndex + m_nDisplayItemInView) - 1))
			m_nStartIndex += m_nItemCountInColumn;
	}

	if (m_nCurrentItemIndex >= (m_nTotalItems - 1))
		m_nCurrentItemIndex = m_nTotalItems - 1;
}

bool CListView::PressEnterKey()
{
	CDirData pCurData = m_itemList.at(m_nCurrentItemIndex);
	wxString strName(pCurData.GetName());
	wxString strSearchDir(wxT(""));

	if (pCurData.IsDir())
	{
		if (strName == wxT(".."))
		{
			strSearchDir = theCommonUtil->GetParentPath(pCurData.GetPath());

			if (strSearchDir.IsEmpty())
				return false;
		}
		else
		{
			strSearchDir = m_strCurrentPath + (m_iPathDepth == 1 ? strName : SLASH + strName);
			//디렉토리 접근여부 체크
			if (!CLocalFileSystem::IsCanReadDir(strSearchDir))
				return false;

			//하위 디렉토리 히스토리가 존재하면 삭제한다.
			_HISTORY::iterator fIter = m_hashHistory.find(strSearchDir);
			if(fIter != m_hashHistory.end())
				m_hashHistory.erase(fIter);

			int nHistoryCount = m_hashHistory.size();
			if (nHistoryCount < m_nMaxHistoryCount)
			{
				_HistoryItem historyItem;
				historyItem.iCurrentIndex = m_nCurrentItemIndex;
				historyItem.iStartIndex = m_nStartIndex;

				m_hashHistory[m_strCurrentPath] = historyItem;
			}
		}
	}
	else if (pCurData.IsFile())
	{
		theCommonUtil->LaunchAndExec(pCurData.GetFullPath(), m_strCurrentPath);
		return false;
	}
	else
	{
		strSearchDir = pCurData.GetDriveName();
		if (!CLocalFileSystem::IsCanReadDir(strSearchDir))
			return false;
	}

	if(pCurData.IsDir() || pCurData.IsDrive())
	{
		LoadDir(strSearchDir);
		theSplitterManager->ChangeTabPagePathName(strSearchDir);
	}

	return true;
}

void CListView::GotoRoot()
{
#ifdef __WXMSW__
	if(!CLocalFileSystem::IsCanReadDir(m_strVolume))
	{
		wxString strMsg;
		strMsg = m_strVolume + theMsgManager->GetMessage(wxT("MSG_IS")) + wxT(" ") + theMsgManager->GetMessage(wxT("MSG_DIREDTORY_READ_FAIL"));
		wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
		return;
	}

	//이동 루트드라이브 == 현재디렉토리
	if (m_strVolume.CmpNoCase(m_strCurrentPath) == 0)
		return;

	theSplitterManager->ChangeTabPagePathName(m_strVolume);
	LoadDir(m_strVolume);
#else
#endif
}

void CListView::OnSize(wxSizeEvent& event)
{
	wxSize size = GetClientSize();
	if ((size.x == 0) || (size.y == 0))
		return;

	ChangeViewSize(size);
	m_viewRect = GetClientRect();

	m_viewDirInfo.SetLeft(m_viewRect.GetLeft());
	m_viewDirInfo.SetTop(m_viewRect.GetTop());
	m_viewDirInfo.SetRight(m_viewRect.GetRight());
	m_viewDirInfo.SetBottom(m_viewRect.GetTop() + 20);

	m_viewRectDetail.SetLeft(m_viewRect.GetLeft());
	m_viewRectDetail.SetTop(m_viewRect.GetBottom() - 21);
	m_viewRectDetail.SetRight(m_viewRect.GetRight());
	m_viewRectDetail.SetBottom(m_viewRect.GetBottom());

	m_viewRectDisp.SetLeft(m_viewRect.GetLeft());
	m_viewRectDisp.SetTop(m_viewRect.GetTop() + 20);
	m_viewRectDisp.SetRight(m_viewRect.GetRight());
	m_viewRectDisp.SetBottom(m_viewRect.GetBottom() - 20);

	m_bSizeOrColumnChanged = true;
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CListView::OnSetFocus(wxFocusEvent& event)
{
	m_bSetFocus = true;
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CListView::OnKillFocus(wxFocusEvent& event)
{
	m_bSetFocus = false;
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

wxMemoryDC* CListView::CreateMemoryDC(wxDC* pDC, const wxRect& rc, const wxColour& colPen, const wxColour colBrush)
{
	if(m_pMemDC == nullptr)
		m_pMemDC = std::make_unique<wxMemoryDC>(pDC);

	DetachDC();
	m_pMemDC->SelectObject(*m_pDoubleBuffer);

	wxPen pen(colPen);
	wxBrush brush(colBrush);

	m_pMemDC->SetFont(*_gFont);
	m_pMemDC->SetPen(pen);
	m_pMemDC->SetBrush(brush);
	m_pMemDC->DrawRectangle(0, 0, rc.GetWidth(), rc.GetHeight());

	return m_pMemDC.get();
}

void CListView::ChangeViewSize(const wxSize& sz)
{
	m_szChagned = sz;
	if (m_pDoubleBuffer)
		delete m_pDoubleBuffer;

	m_pDoubleBuffer = new wxBitmap(m_szChagned.x, m_szChagned.y);
}

void CListView::DetachDC()
{
	ClearMemoryDC();
	m_pMemDC->SetPen(wxNullPen);
	m_pMemDC->SetBrush(wxNullBrush);
	m_pMemDC->SetFont(wxNullFont);
	m_pMemDC->SelectObject(wxNullBitmap);
}

void CListView::ClearMemoryDC()
{
	m_pMemDC->Clear();
}

void CListView::ClearMemoryDC(wxMemoryDC* pMemDC)
{
	pMemDC->Clear();
}

void CListView::AllClear()
{
	Clear();
	wxVector<CDirData>().swap(m_itemList);
	m_itemList.reserve(0);

	wxVector<CColumnPoint>().swap(m_ptList);
	m_ptList.reserve(0);

	wxVector<CPositionInfo>().swap(m_posList);
	m_posList.reserve(0);

	wxVector<int>().swap(m_matchItems);
	m_matchItems.reserve(0);
}

void CListView::Clear()
{
	m_itemList.clear();
	m_dispNameInfoMap.clear();
	m_hashSelectedItem.clear();
}

void CListView::Initialize()
{
	m_strKeyInput = wxT("");
	m_pMyTooltipView->SetTooltipText(wxT(""));
	m_pMyTooltipView->Show(false);

	m_pMyTooltipKeyInput->SetTooltipText(wxT(""));
	m_pMyTooltipKeyInput->Show(false);

	m_pMyTooltipDetail->SetTooltipText(wxT(""));
	m_pMyTooltipDetail->Show(false);

	Clear();

	m_iTotalPositionCnt = 0;
	//선택 파일수
	m_iSelFileCnt = 0;
	//선택 디렉토리 수
	m_iSelDirCnt = 0;
	//디렉토리 수
	m_iDirCount = 0;
	//파일수
	m_iFileCount = 0;
	//용량
	m_dblFileSizeInDir = 0.0;
	m_dblTotalSpace = 0.0;
	m_dblFreeSpace = 0.0;
	//아이템 전체수
	m_nTotalItems = 0;
	//화면표시 시작 인덱스
	m_nStartIndex = 0;
	//현재 인덱스
	m_nCurrentItemIndex = 0;
	//현재 디렉토리
	m_strCurrentPath = wxT("");
	//Max Name
	m_strMaxName     = wxT("");
	//Max Type Name
	m_strMaxTypeName = wxT("");
	m_strDispDiskSpace = wxT("");
	m_strTotalSpace = wxT("");
	m_strFreeSpace = wxT("");

	//디렉토리 Load 플래그
	m_bDirLoaded = false;
	//화면 변경 플래그
	m_bSizeOrColumnChanged = false;
}

void CListView::CalcColumn(wxDC* pDC)
{
	if (m_nTotalItems <= 0)
		return;

	if(!m_bSizeOrColumnChanged && !m_bDirLoaded)
		return;

	bool bPosReset = false;
	m_nDispColumn = theJsonConfig->GetColumnCount();
	COLUMN_VIEW_OPTION enumColumnView = _gColumnViewOPEnum[m_nDispColumn];
	if(m_enumColumnViewOption != enumColumnView)
	{
		m_enumColumnViewOption = enumColumnView;
		bPosReset = true;
	}

	if(m_bSizeOrColumnChanged)
		bPosReset = true;

	m_iCharHeight = pDC->GetCharHeight() + 2;

	m_nItemCountInColumn = (m_viewRectDisp.GetHeight() / m_iCharHeight);
	if (m_nItemCountInColumn == 0)
		return;

	if(m_enumColumnViewOption == VIEW_COLUMN_AUTO)
	{
		bPosReset = true;
		//자동컬럼인경우
		if (!CalcAutoColumn(pDC, m_viewRectDisp))
			return;
	}
	//화면안에 표시될 아이템 수
	m_nDisplayItemInView = m_nItemCountInColumn * m_nDispColumn;

	int nWidth = m_viewRectDisp.GetWidth();//.GetRight();
	int nBottom = m_viewRectDisp.GetBottom();
	if(bPosReset)
	{
		m_ptList.clear();
		m_dispNameInfoMap.clear();

		int nDispWidth = static_cast<int>(nWidth / m_nDispColumn);
		if (m_nDispColumn > 1)
		{
			//컬럼구분선 좌표
			wxPoint pt1(0, 0);
			wxPoint pt2(0, 0);
			//컬럼 좌표 설정
			for (int nColumnCount = 0; nColumnCount < (m_nDispColumn - 1); nColumnCount++)
			{
				CColumnPoint colPoint;
				colPoint.pt1.x = nDispWidth * (nColumnCount + 1);
				colPoint.pt1.y = 0;

				colPoint.pt2.x = nDispWidth * (nColumnCount + 1);
				colPoint.pt2.y = nBottom;

				m_ptList.push_back(colPoint);
			}
		}

		m_nDispColumnEndPoint = nDispWidth;
	}

	//아이템 표시좌표 계산
	CalcPosition(pDC);
	//아이콘 읽기
//	if(m_bDirLoaded)
//		RunReadImageList();
}

bool CListView::CalcAutoColumn(wxDC* pDC, const wxRect& viewRect)
{
	//전체건수 / 컬럼당 아이템수 <= 0 이면 컬럼은 1
	m_nDispColumn = m_nTotalItems / m_nItemCountInColumn;
	if (m_nDispColumn <= 0)
		m_nDispColumn = 1;
	else
	{	//컬럼당 표시아이템수 > 전체 아이템수 ==> 컬럼은 1
		if (m_nItemCountInColumn > m_nTotalItems)
			m_nDispColumn = 1;
		else
		{	//표시가능 컬럼수
			int iAvalibleDispItem = 0;

			wxSize szEng = pDC->GetTextExtent(wxString("A"));
			wxSize szHan = pDC->GetTextExtent(wxString("가"));
			//한글과 영문의 평균 Width
			int iAvgCharWidth = (szEng.GetWidth() + szHan.GetWidth()) / 2;
			//최대 표시가능 Width
			int iMaxDispWidth = viewRect.GetWidth();
			//자동컬럼시 표시글자수의 최대값은 16
			int iMaxDispCharWidth = iAvgCharWidth * 16;
			int iDispColumn = 0;

			while (1)
			{	//전체 표시가능 Width > 최대 표시가능 Width
				if (iMaxDispWidth > iMaxDispCharWidth)
				{	//표시컬럼 증가
					iDispColumn++;
					//표시가능 아이템수 증가
					iAvalibleDispItem += m_nItemCountInColumn;
					//전체 표시가능 Width -= 최대표시가능Width
					iMaxDispWidth -= iMaxDispCharWidth;

					if (iAvalibleDispItem >= m_nTotalItems)
						break;
				}
				else
					break;
			}

			//자동 컬럼 선택시 최대 9개의 컬럼
			m_nDispColumn = iDispColumn >= 9 ? 9 : iDispColumn;
		}
	}

	if (m_nDispColumn <= 0)
		m_nDispColumn = 1;

	return true;
}

void CListView::DrawColumn(wxDC* pDC)
{
	pDC->SetPen(wxPen(theJsonConfig->GetColumnColor()));

	for (auto &colItem : m_ptList)
		pDC->DrawLine(colItem.pt1, colItem.pt2);
}

void CListView::CalcPosition(wxDC* pDC)
{
	size_t iposItemCnt = m_posList.size();
	if(iposItemCnt == 0 || iposItemCnt > 200)
		m_posList.reserve(200);

	m_posList.clear();

	m_bDispFlag[0] = false;
	m_bDispFlag[1] = false;
	m_bDispFlag[2] = false;
	m_bDispFlag[3] = false;
	m_bDispFlag[4] = false;

	wxString strAttr(wxT("RHSA"));
	wxString strTime(wxT("0000-00-00 00:00"));
	wxString strSize(wxT("123.45 MB"));
	wxString strFileSize(wxT("123.45 "));
	wxString strFileSizeType(wxT("MB"));

	wxSize sizeMaxName = pDC->GetTextExtent(m_strMaxName);
	wxSize sizeMaxTypeName = pDC->GetTextExtent(m_strMaxTypeName);
	wxSize sizeAttr = pDC->GetTextExtent(strAttr);
	wxSize sizeTime = pDC->GetTextExtent(strTime);
	wxSize sizeSize = pDC->GetTextExtent(strSize);
	wxSize sizeFileSize = pDC->GetTextExtent(strFileSize);
	wxSize szFileSizeType = pDC->GetTextExtent(strFileSizeType);

	int iNameWidth = sizeMaxName.GetWidth();
	int iSizeWidth = sizeSize.GetWidth();
	int iTimeWidth = sizeTime.GetWidth();
	int iAttrWidth = sizeAttr.GetWidth();
	int iTypeWidth = sizeMaxTypeName.GetWidth();

	unsigned int arrWidth[5] = { 0, };
	arrWidth[0] = iNameWidth;
	arrWidth[1] = iSizeWidth;
	arrWidth[2] = iTimeWidth;
	arrWidth[3] = iAttrWidth;
	arrWidth[4] = iTypeWidth;

	int iSizeSum = 0;
	int iAvaliableWidth = m_nDispColumnEndPoint - (ICON_SELECTED_WIDTH_GAP + (GAP_WIDTH * 21));

	for (int i = 0; i < 5; i++)
	{
		iSizeSum += arrWidth[i];
		if (iAvaliableWidth > iSizeSum)
			m_bDispFlag[i] = true;
	}

	m_bDispFlag[0] = true;
	if (!m_bDispFlag[1])
		iAvaliableWidth = m_nDispColumnEndPoint - (ICON_SELECTED_WIDTH_GAP + (GAP_WIDTH * 12));

	int iTempColumn = 0;
	int iDispIndex = 0;
	int iStartX = 1;
	int iStartY = m_viewRectDisp.GetTop();

	int iDisp_x1 = 0;
	int iDisp_y1 = 0;
	int iDisp_x2 = 0;
	int iDisp_y2 = 0;

	//2019.04.17 ADD
	int iGapWidth = GAP_WIDTH * 2;

	for (int nIndex = 0; nIndex < m_nDisplayItemInView; nIndex++)
	{
		if (nIndex > (m_nItemCountInColumn - 1))
		{
			if ((nIndex % m_nItemCountInColumn) == 0)
			{
				iTempColumn++;
				iDispIndex = 0;
			}
		}

		int x1 = iStartX + (iTempColumn * m_nDispColumnEndPoint);
		int y1 = (iStartY + 1) + (m_iCharHeight * iDispIndex);
		int x2 = m_nDispColumnEndPoint - 1;// (iStartX + ((iTempColumn + 1) * m_nDispColumnEndPoint)) - 1;
		int y2 = m_iCharHeight;// iStartY + (iHeight * (iDispIndex + 1));

		wxRect rect(x1, y1, x2, y2);

		CPositionInfo posInfo;
		posInfo.m_mainRect = rect;

		int iIcon_x1 = x1 + ICON_SELECTED_WIDTH_GAP + GAP_WIDTH;
		int iIcon_y1 = y1 + 2;
		int iIcon_x2 = ICON_WIDTH;
		int iIcon_y2 = y2;

		wxRect rcIcon(iIcon_x1, iIcon_y1, iIcon_x2, iIcon_y2);
		posInfo.m_iconRect = rcIcon;

//		// 이름표시
//		if (m_bDispFlag[0])
//		{
//			iDisp_x1 = rcIcon.GetRight() + (GAP_WIDTH * 3);
//			iDisp_y1 = y1 + 1;
//			iDisp_x2 = iNameWidth;
//			iDisp_y2 = y2;
//
//			if (!m_bDispFlag[1])
//				iDisp_x2 = iAvaliableWidth;
//
//			wxRect rcName(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
//			posInfo.m_nameRect = rcName;
//		}

		//설명
		if (m_bDispFlag[4])
		{
			iDisp_x1 = rect.GetRight() - (iTypeWidth + iGapWidth);
			iDisp_y1 = y1;
			iDisp_x2 = iTypeWidth;
			iDisp_y2 = y2;

			wxRect rcType(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_typeNameRect = rcType;
		}

		//속성
		if (m_bDispFlag[3])
		{
		//	iDisp_x1 = rect.GetRight() - (iAttrWidth + iGapWidth);// m_bDispFlag[4] == true ? posInfo.m_typeNameRect.GetLeft() - (iAttrWidth + iGapWidth) : rect.GetRight() - (iAttrWidth + iGapWidth);
			iDisp_x1 = m_bDispFlag[4] == true ? posInfo.m_typeNameRect.GetLeft() - (iAttrWidth + iGapWidth * 2) : rect.GetRight() - (iAttrWidth + iGapWidth);
			iDisp_y1 = y1 + 1;
			iDisp_x2 = iAttrWidth;
			iDisp_y2 = y2;

			wxRect rcAttr(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_attrRect = rcAttr;
		}

		//수정시간
		if (m_bDispFlag[2])
		{
			iDisp_x1 = m_bDispFlag[3] == true ? posInfo.m_attrRect.GetLeft() - (iTimeWidth + iGapWidth * 2) : rect.GetRight() - (iTimeWidth + iGapWidth);
			iDisp_y1 = y1 + 1;
			iDisp_x2 = iTimeWidth;
			iDisp_y2 = y2;

			wxRect rcTime(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_timeRect = rcTime;
		}

		//사이즈 표시
		if (m_bDispFlag[1])
		{
			iDisp_x1 = m_bDispFlag[2] == true ? posInfo.m_timeRect.GetLeft() - (iSizeWidth + iGapWidth * 2) : rect.GetRight() - (iSizeWidth + iGapWidth);
			iDisp_y1 = y1 + 1;
			iDisp_x2 = iSizeWidth;
			iDisp_y2 = y2;

			wxRect rcSize(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_sizeRect = rcSize;

			int iFileSizeWidth = sizeFileSize.GetWidth();

			iDisp_x1 = posInfo.m_sizeRect.GetLeft();
			iDisp_y1 = y1 + 1;
			iDisp_x2 = iFileSizeWidth;
			iDisp_y2 = y2;

			wxRect rcFileSize(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_sizeRectFile = rcFileSize;

			int iSizeTypeWidth = szFileSizeType.GetWidth();

			iDisp_x1 = rcFileSize.GetRight() + iGapWidth;
			iDisp_y1 = y1 + 1;
			iDisp_x2 = iSizeTypeWidth;
			iDisp_y2 = y2;

			wxRect rcFileSizeType(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_sizeRectType = rcFileSizeType;
		}

		// 이름표시
		if (m_bDispFlag[0])
		{
			iDisp_x1 = rcIcon.GetRight() + (GAP_WIDTH * 3);
			iDisp_y1 = y1 + 1;
			iDisp_x2 = m_bDispFlag[1] == true ? posInfo.m_sizeRect.GetLeft() - (iGapWidth * 3) : iAvaliableWidth;
			iDisp_y2 = y2;

			wxRect rcName(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_nameRect = rcName;
		}
		/*
		}
		else
		{
			//사이즈 표시
			if (m_bDispFlag[1])
			{
				iDisp_x1 = posInfo.m_nameRect.GetRight() + (GAP_WIDTH * 2);
				iDisp_y1 = y1 + 1;
				iDisp_x2 = iSizeWidth;
				iDisp_y2 = y2;

				wxRect rcSize(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
				posInfo.m_sizeRect = rcSize;

				int iFileSizeWidth = sizeFileSize.GetWidth();

				iDisp_x1 = posInfo.m_nameRect.GetRight() + (GAP_WIDTH * 2);
				iDisp_y1 = y1 + 1;
				iDisp_x2 = iFileSizeWidth;
				iDisp_y2 = y2;

				wxRect rcFileSize(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
				posInfo.m_sizeRectFile = rcFileSize;

				int iSizeTypeWidth = szFileSizeType.GetWidth();

				iDisp_x1 = rcFileSize.GetRight() + (GAP_WIDTH * 2);
				iDisp_y1 = y1 + 1;
				iDisp_x2 = iSizeTypeWidth;
				iDisp_y2 = y2;

				wxRect rcFileSizeType(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
				posInfo.m_sizeRectType = rcFileSizeType;
			}

			//수정시간
			if (m_bDispFlag[2])
			{
				iDisp_x1 = posInfo.m_sizeRect.GetRight() + (GAP_WIDTH * 4);
				iDisp_y1 = y1 + 1;
				iDisp_x2 = iTimeWidth;
				iDisp_y2 = y2;

				wxRect rcTime(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
				posInfo.m_timeRect = rcTime;
			}

			//속성
			if (m_bDispFlag[3])
			{
				iDisp_x1 = posInfo.m_timeRect.GetRight() + (GAP_WIDTH * 4);
				iDisp_y1 = y1 + 1;
				iDisp_x2 = iAttrWidth;
				iDisp_y2 = y2;

				wxRect rcAttr(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
				posInfo.m_attrRect = rcAttr;
			}

			//설명
			if (m_bDispFlag[4])
			{
				iDisp_x1 = posInfo.m_attrRect.GetRight() + (GAP_WIDTH * 3);
				iDisp_y1 = y1 + 1;
				iDisp_x2 = iTypeWidth;
				iDisp_y2 = y2;

				wxRect rcType(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
				posInfo.m_typeNameRect = rcType;
			}
		}
		*/
		m_posList.push_back(posInfo);
		iDispIndex++;
	}

	m_iTotalPositionCnt = m_posList.size();
}

void CListView::DoSortStart(bool bSetDrive)
{
	int iSortType = theJsonConfig->GetSortType();
	wxVector<CDirData>::const_iterator iter = m_itemList.begin();
	wxString strName = iter->GetName();
	int iSortStartIndex = 0;
//	int iSortEndIndex = 0;

	if(strName.Cmp(wxT("..")) == 0)
		iSortStartIndex = 1;

//#ifdef __WXMSW__
//	if(bSetDrive)
//		iSortEndIndex = theDriveInfo->GetDriveCount();
//#endif // __WXMSW__

	switch(iSortType)
	{
		case VIEW_SORT_DEFAULT:
			std::sort(m_itemList.begin() + iSortStartIndex, m_itemList.end()/* - iSortEndIndex */, CSorting::DirSortOfName);
			break;

		case VIEW_SORT_TIME:
			std::sort(m_itemList.begin() + iSortStartIndex, m_itemList.end()/* - iSortEndIndex */, CSorting::DirSortOfTime);
			break;

		case VIEW_SORT_SIZE:
			std::sort(m_itemList.begin() + iSortStartIndex, m_itemList.end()/* - iSortEndIndex */, CSorting::DirSortOfSize);
			break;

		default:
			break;
	}
}

void CListView::ReCalcPage()
{
	int nStartIndex = 0;
	while (1)
	{
		if ((m_nCurrentItemIndex >= nStartIndex) && (m_nCurrentItemIndex <= (nStartIndex + m_nDisplayItemInView)))
			break;

		nStartIndex += m_nDisplayItemInView;
		if (nStartIndex > m_nTotalItems)
		{
			nStartIndex = m_nTotalItems - m_nDisplayItemInView;
			break;
		}
	}

	m_nStartIndex = nStartIndex;
	if (m_nStartIndex < 0)
		m_nStartIndex = 0;

	if (m_nCurrentItemIndex < 0)
		m_nCurrentItemIndex = 0;
}

wxString CListView::CalcDispStr(wxDC* pDC, const wxString& strSourceSave, const wxString& strSource, bool isDrive)
{
	CPositionInfo posInfo = m_posList.at(0);

	wxString strName(strSource);
	wxString strDispName(wxT(""));
	wxString strSrcSave(strSourceSave);
	wxString strSrcName(strSource);
	wxString strDisp(wxT(""));

	wxSize szNameSize = pDC->GetTextExtent(strName);

	int iDispWidth = isDrive ? posInfo.m_mainRect.GetWidth() - 35 : posInfo.m_nameRect.GetWidth();
	int iNameWidth = szNameSize.GetWidth();

	int iLen = strName.length();

	if (iNameWidth > iDispWidth)
	{
		std::unordered_map<wxString, wxString>::const_iterator fIter = m_dispNameInfoMap.find(strSrcSave);
		if (fIter != m_dispNameInfoMap.end())
			return fIter->second;

		for (int iIndex = 0; iIndex < iLen; iIndex++)
		{
			strDisp = strName.Left(iIndex + 1);
			wxSize sizeText = pDC->GetTextExtent(wxString(strDisp + wxT("...")));
			if ((sizeText.GetWidth()) > iDispWidth)
			{
				//	strDisp = strName.Left(iIndex + (isDrive ? 1 : 3));
				strDisp = strName.Left(iIndex);
				break;
			}
		}

		strDisp += wxT("...");

		std::unordered_map<wxString, wxString>::value_type val(strSrcSave, strDisp);
		m_dispNameInfoMap.insert(val);
	}
	else
		strDisp = strName;

	return strDisp;
}

wxString CListView::FindMaxData(const wxVector<wxString>& vecData)
{
	wxClientDC clientDC(this);

	clientDC.SetFont(*_gFont);
	wxVector<wxString> vecTmp(vecData);

	wxString strItem(wxT(""));
	wxVector<wxString>::iterator iter = std::max_element(vecTmp.begin(), vecTmp.end(), [&clientDC](const wxString& a, const wxString& b)
																						{
																							wxString strA(a);
																							wxString strB(b);

																							wxSize szA = clientDC.GetTextExtent(strA);
																							wxSize szB = clientDC.GetTextExtent(strB);

																							return szA.GetWidth() < szB.GetWidth();
																						}
	);

	strItem = *iter;
	clientDC.SetFont(wxNullFont);

	return strItem;
}

wxString CListView::FindMaxData(const wxString& a, const wxString& b)
{
	wxClientDC clientDC(this);
	clientDC.SetFont(*_gFont);

	wxSize szA = clientDC.GetTextExtent(a);
	wxSize szB = clientDC.GetTextExtent(b);
	wxString strMaxData = szA.GetWidth() < szB.GetWidth() ? b : a;

	clientDC.SetFont(wxNullFont);
	return strMaxData;
}

void CListView::DisplayItems(wxDC* pDC)
{
	if (m_nTotalItems <= 0)
		return;

	int nDisplayItemCount = m_nStartIndex + m_nDisplayItemInView;
	if (nDisplayItemCount >= m_nTotalItems)
		nDisplayItemCount = m_nTotalItems;

	if (nDisplayItemCount <= 0)
		return;

	int nPosIndex = 0;

	wxColour dispColor;

	wxString strName(wxT(""));
	wxString strSrcName(wxT(""));
	wxString strFullPathName(wxT(""));
	wxString strSpace(wxT(""));

	for (int nIndex = m_nStartIndex; nIndex < nDisplayItemCount; nIndex++)
	{
		if (nIndex >= m_nTotalItems)
			nIndex = m_nTotalItems - 1;

		if (nPosIndex >= m_iTotalPositionCnt)
			nPosIndex = m_iTotalPositionCnt - 1;

		wxVector<CDirData>::iterator iter = m_itemList.begin() + nIndex;

	//	CDirData itemData = m_itemList.at(nIndex);

		CPositionInfo posInfo = m_posList.at(nPosIndex);

		strSrcName = iter->GetName();
		strName = iter->GetName();

		bool isDrive = iter->IsDrive() ? true : false;
		bool isDir = iter->IsDir() ? true : false;
		bool isFile = iter->IsFile() ? true : false;
		bool isCut = iter->IsCut() ? true : false;

		bool bSelected = iter->IsItemSelected();
		bool bMatched = iter->IsMatch();

		//표시 색상
		if (isDrive)     dispColor = m_colDrive;
		else if (isDir)  dispColor = m_colDir;
		else
		{
			wxString strExt(theCommonUtil->GetExt(strName).Upper());
			dispColor = theJsonConfig->GetExtColor(strExt);
		}

	//	if ((iMatchCount > 0) && !isDrive)
	//		dispColor = wxColour(90, 90, 90);

		if (bMatched)
			dispColor = m_colMatch;

		if (bSelected)
			dispColor = wxColour(255, 255, 80);// wxColour(172, 89, 255);

		//아이템이 선택되었을경우
		if (m_nCurrentItemIndex == nIndex)
		{
			DisplayItemDetailInfo(pDC, m_nCurrentItemIndex);

			wxRect rcFillRect(posInfo.m_mainRect);
			rcFillRect.SetBottom(rcFillRect.GetBottom() + 1);

			wxPen   pen;//(dispColor);
			wxBrush brush;//(dispColor);

			//포커스가 없는경우
			if (!m_bSetFocus)
			{
				pen = wxPen(wxColour(30, 30, 30), 1);
				brush = wxColour(30, 30, 30);
			}
			else
			{
				pen = dispColor;
				brush = dispColor;
			}

			pDC->SetPen(pen);
			pDC->SetBrush(brush);

//			pDC->DrawRoundedRectangle(rcFillRect, -0.08);
			pDC->DrawRectangle(rcFillRect);
			//포커스가 없는경우
			if (!m_bSetFocus)
				dispColor = wxColour(90, 90, 90);
			else
				dispColor = wxColour(0, 0, 0);
		}
		else
		{
			if (bSelected || bMatched)
			{
				wxRect rcFillRect(posInfo.m_mainRect);
				rcFillRect.SetBottom(rcFillRect.GetBottom() + 1);

				wxPen   pen(wxColour(30, 30, 30));
				wxBrush brush(wxColour(19, 102, 142));

				pDC->SetPen(pen);
				pDC->SetBrush(brush);

				pDC->DrawRectangle(rcFillRect);

				pDC->SetPen(wxNullPen);
				pDC->SetBrush(wxNullBrush);
			}
		}

		wxColour colSelected = (m_nCurrentItemIndex == nIndex) ? dispColor : m_colDefault;
		wxString strDisp(wxT(""));
#ifdef __WXMSW__
		strSrcName.MakeLower();
#endif

		strDisp = CalcDispStr(pDC, strSrcName, strName, isDrive);
		pDC->SetTextForeground(dispColor);
		pDC->DrawLabel(strDisp, posInfo.m_nameRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

		if(!isDrive)
		{
			if (m_bDispFlag[1])
			{
				if (isDir)
					pDC->DrawLabel(theMsgManager->GetMessage(wxT("MSG_DIR_FILESIZE_POS")), posInfo.m_sizeRect, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
				else
				{
					wxString strFileSize(iter->GetSizeString());// (wxT(""));
					wxString strFileSizeType(iter->GetSizeTypeString());// (wxT(""));
					wxColour colorType(iter->GetSizeColor());// (wxColour(192, 192, 192));
					wxColour colDispColor;

					if (iter->GetAttribute() & ATTR_RDONLY)
					{
						colDispColor = *wxYELLOW;
						colorType = *wxYELLOW;
					}
					else
						colDispColor = m_colDefault;

					pDC->SetTextForeground(m_nCurrentItemIndex == nIndex ? dispColor : colDispColor);

					if (strFileSizeType.Cmp(_T("")) == 0)	//사이즈가 KB이하인경우
						pDC->DrawLabel(strFileSize, posInfo.m_sizeRect, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
					else
					{	//사이즈가 KB이상인경우
						pDC->DrawLabel(strFileSize, posInfo.m_sizeRectFile, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
						pDC->SetTextForeground(m_nCurrentItemIndex == nIndex ? dispColor : colorType);
						pDC->DrawLabel(strFileSizeType, posInfo.m_sizeRectType, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
					}
				}
			}

			//시간정보
			if (m_bDispFlag[2])
			{
				wxString strTime = iter->GetDateTimeToString();
				pDC->SetTextForeground(colSelected);
				pDC->DrawLabel(strTime, posInfo.m_timeRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
			}
			//속성정보
			if (m_bDispFlag[3])
			{
				wxString strAttr = iter->GetAttributeToString();
				pDC->SetTextForeground(colSelected);
				pDC->DrawLabel(strAttr, posInfo.m_attrRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
			}

			//type정보
			if (m_bDispFlag[4])
			{
				wxString strDesc = iter->GetTypeName();
				pDC->SetTextForeground(m_nCurrentItemIndex == nIndex ? dispColor : m_colType);
				pDC->DrawLabel(strDesc, posInfo.m_typeNameRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
			}
		}
#ifdef __WXMSW__
		else
		{
			int iNameRCWidth = posInfo.m_nameRect.GetWidth();
			int iMaxDriveNameWidth = pDC->GetTextExtent(m_strMaxDriveName).GetWidth();
			int iDriveRate = iMaxDriveNameWidth * 100 / iNameRCWidth;
			int iDriveWidth = iNameRCWidth * iDriveRate / 100;
			wxString strDriveSpaceInfo = iter->GetDriveSpaceInfo();

			int iDriveSpaceInfoWidth = pDC->GetTextExtent(strDriveSpaceInfo).GetWidth();

			int x1 = posInfo.m_nameRect.GetLeft() + iDriveWidth + ICON_WIDTH;
			int y1 = posInfo.m_nameRect.GetTop();
			int x2 = x1 + iDriveSpaceInfoWidth;
			int y2 = posInfo.m_nameRect.GetBottom();

			if((iNameRCWidth > iMaxDriveNameWidth) && (x2 < posInfo.m_mainRect.GetRight()))
			{
				wxRect rcDriveSpace;

				rcDriveSpace.SetLeft(x1);
				rcDriveSpace.SetTop(y1);
				rcDriveSpace.SetRight(x2);
				rcDriveSpace.SetBottom(y2);

				pDC->DrawLabel(strDriveSpaceInfo, rcDriveSpace, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
			}
		}
#endif
		if (bSelected)
		{	//아이템이 선택되었을때 선택표시를 Polygon으로 처리(▶)
			wxPoint ptSel[3];

			ptSel[0].x = 0;
			ptSel[0].y = 0;

			ptSel[1].x = 0;
			ptSel[1].y = 0;

			ptSel[2].x = 0;
			ptSel[2].y = 0;

			//Polygon 보정
			wxRect rcMain(posInfo.m_mainRect);

			ptSel[0].x = rcMain.GetLeft() + 3;
			ptSel[0].y = rcMain.GetTop() + 3;

			ptSel[1].x = rcMain.GetLeft() + 9;
			ptSel[1].y = rcMain.GetTop() + 9;

			ptSel[2].x = rcMain.GetLeft() + 3;
			ptSel[2].y = rcMain.GetBottom() - 2;

			wxPen penSel(wxColour(225, 113, 0));
			wxBrush brushSel(wxColour(225, 113, 0));

			pDC->SetPen(penSel);
			pDC->SetBrush(brushSel);

			pDC->DrawPolygon(3, ptSel, -1);

			pDC->SetPen(wxNullPen);
			pDC->SetBrush(wxNullBrush);
		}

		if (theCommonUtil->Compare(iter->GetName(), wxT("..")) == 0)
			pDC->DrawIcon(m_icoUpDir, wxPoint(posInfo.m_iconRect.GetLeft(), posInfo.m_iconRect.GetTop()));
		else
		{
			int iImageFlag = ILD_NORMAL | ILC_MASK;

			if (isDir || isFile)
			{
				if (isCut)
					iImageFlag |= ILD_BLEND25;

				if (iter->GetAttribute() & ATTR_HIDDEN)
					iImageFlag |= ILD_BLEND25;
			}

			int iImageX = posInfo.m_iconRect.GetLeft();
			int iImageY = posInfo.m_iconRect.GetTop();

			DrawItemImage(pDC, iImageX, iImageY, iImageFlag, iter->m_iIconIndex, iter->m_iOvelayIndex);
		}

		nPosIndex++;
	}
}

void CListView::DrawDirInfoArea(wxDC* pDC)
{
	wxFont fontNormal;
	wxFont fontBold;

	fontNormal.Create(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("Segoe UI"));
	fontBold.Create(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, _T("Segoe UI"));

	wxPen pen(wxColour(0, 0, 0));
	wxBrush brush(wxColour(240, 240, 240));

	pDC->SetPen(pen);
	pDC->SetBrush(brush);

	pDC->DrawRectangle(m_viewDirInfo);

	wxString strDirCount = wxString::Format(wxT("%d"), m_iDirCount);
	wxString strFileCount = wxString::Format(wxT("%d"), m_iFileCount);
	wxString strDirName(wxT(" ") + theMsgManager->GetMessage(wxT("MSG_DIR")) + wxT(", "));
	wxString strFileName(wxT(" ") + theMsgManager->GetMessage(wxT("MSG_FILE_DESCRIPTION")) + wxT(" "));

	int xPos = m_viewDirInfo.GetLeft() + 5;
	int yPos = m_viewDirInfo.GetTop() + 3;

	pDC->SetFont(fontBold);
	pDC->SetTextForeground(wxColour(0, 0, 255));
	pDC->DrawText(strDirCount, xPos, yPos);

	wxSize sizeInfo = pDC->GetTextExtent(strDirCount);
	xPos += sizeInfo.GetWidth();

	pDC->SetFont(fontNormal);
	pDC->SetTextForeground(wxColour(0, 0, 0));
	pDC->DrawText(strDirName, xPos , yPos);

	sizeInfo = pDC->GetTextExtent(strDirName);
	xPos += sizeInfo.GetWidth();

	pDC->SetFont(fontBold);
	pDC->SetTextForeground(wxColour(0, 0, 255));
	pDC->DrawText(strFileCount, xPos, yPos);

	sizeInfo =pDC->GetTextExtent(strFileCount);
	xPos += sizeInfo.GetWidth();

	pDC->SetFont(fontNormal);
	pDC->SetTextForeground(wxColour(0, 0, 0));
	pDC->DrawText(strFileName, xPos, yPos);

	sizeInfo = pDC->GetTextExtent(strFileName);
	xPos += sizeInfo.GetWidth();

	wxString strFileSize(wxT(""));
	wxString strFileSizeType(wxT(""));

	wxColour colorType(wxColour(192, 192, 192));

	theCommonUtil->GetSizeInfo(m_dblFileSizeInDir, strFileSize, strFileSizeType, colorType);
	if (strFileSizeType.IsEmpty())
		strFileSizeType = wxT("Bytes");

	wxString strSizeInfo = wxString::Format(wxT("(%s %s)"), strFileSize, strFileSizeType);
	pDC->SetFont(fontBold);
	pDC->DrawText(strSizeInfo, xPos, yPos);

	pDC->SetFont(fontNormal);
	sizeInfo = pDC->GetTextExtent(m_strDispDiskSpace);

	xPos = m_viewDirInfo.GetRight() - sizeInfo.GetWidth();
	pDC->DrawText(m_strDispDiskSpace, xPos - 5, yPos);

	wxRect rcDiskSpace;
	rcDiskSpace.SetLeft(xPos - 150);
	rcDiskSpace.SetTop(m_viewDirInfo.GetTop() + 2);
	rcDiskSpace.SetRight(xPos - 10);
	rcDiskSpace.SetBottom(m_viewDirInfo.GetBottom() - 2);

	wxPen penDisk(wxColour(100, 100, 100));
	wxBrush brushDisk(wxColour(192, 192, 192));

	pDC->SetPen(penDisk);
	pDC->SetBrush(brushDisk);
	pDC->DrawRectangle(rcDiskSpace);

	double dblTotalWidth = rcDiskSpace.GetWidth() * 1.0;
	double dblUsed = m_dblTotalSpace - m_dblFreeSpace;

	int iUsedWidth = (int)((dblUsed * dblTotalWidth) / m_dblTotalSpace);

	wxPen penUsed(wxColour(100, 100, 100));
	wxBrush brushUsed(wxColour(44, 184, 65));
	wxRect rcDiskSpaceUsed(rcDiskSpace.GetLeft(), rcDiskSpace.GetTop(), iUsedWidth, rcDiskSpace.GetHeight());

	pDC->SetPen(penUsed);
	pDC->SetBrush(brushUsed);
	pDC->DrawRectangle(rcDiskSpaceUsed);

	wxPen penLine(wxColour(255, 255, 255));
	pDC->SetPen(penLine);

	pDC->DrawLine(rcDiskSpace.GetLeft(), rcDiskSpace.GetBottom(), rcDiskSpace.GetRight() + 1, rcDiskSpace.GetBottom());
	pDC->DrawLine(rcDiskSpace.GetRight(), rcDiskSpace.GetTop(), rcDiskSpace.GetRight(), rcDiskSpace.GetBottom());

	pDC->SetFont(*_gFont);
}

void CListView::SetDiskSpace()
{
	if (wxIsReadable(m_strVolume))
	{
		int iDriveType = theDriveInfo->GetDriveType(m_strVolume);

		if ((iDriveType != wxFS_VOL_CDROM) &&
			(iDriveType != wxFS_VOL_DVDROM))
		{
			wxString strTotalSpace(wxT(""));
			wxString strFreeSpace(wxT(""));

			theDriveInfo->GetDiskSpace(m_strVolume, m_dblTotalSpace, m_dblFreeSpace);

			double dblFreePrecent = m_dblFreeSpace * 100.0 / m_dblTotalSpace;

			m_strTotalSpace = wxString::Format(wxT("%04.2f GB"), (m_dblTotalSpace / GIGABYTE));
			m_strFreeSpace = wxString::Format(wxT("%04.2f GB"), (m_dblFreeSpace / GIGABYTE));

			m_strDispDiskSpace = m_strVolume;
			m_strDispDiskSpace += wxT(" ");
			m_strDispDiskSpace += m_strFreeSpace;
			m_strDispDiskSpace += wxString::Format(wxT("(%.2f%) "), dblFreePrecent);
			m_strDispDiskSpace += theMsgManager->GetMessage(wxT("MSG_DISK_FREE"));
			m_strDispDiskSpace += wxT(" (");
			m_strDispDiskSpace += theMsgManager->GetMessage(wxT("MSG_DISK_TOTAL"));
			m_strDispDiskSpace += wxT(" ");
			m_strDispDiskSpace += m_strTotalSpace;
			m_strDispDiskSpace += wxT(")");
		}
		else
			m_strDispDiskSpace = wxT("");
	}
}

void CListView::DisplayItemDetailInfo(wxDC* pDC, int iIndex)
{
	wxVector<CDirData>::const_iterator iter = m_itemList.begin() + iIndex;

	wxPen pen(wxColour(0, 0, 0));
	wxBrush brush(wxColour(192, 192, 192));

	wxFont font;
	font.Create(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("Segoe UI"));

	pDC->SetFont(font);
	pDC->SetPen(pen);
	pDC->SetBrush(brush);

	pDC->DrawRectangle(m_viewRectDetail);

	int iImageFlag = ILD_NORMAL | ILC_MASK;
	DrawItemImage(pDC, m_viewRectDetail.GetLeft() + 3, m_viewRectDetail.GetTop() + 3, iImageFlag, iter->m_iIconIndex, iter->m_iOvelayIndex);


	wxString strDetailInfo = iter->GetName();
	strDetailInfo.append(wxT("  |  "));
#ifdef __WXMSW__
	if(iter->IsDrive())
	{
		wxString strDriveName = iter->GetDriveName();
		CDriveItem* pDriveItem = theDriveInfo->GetDriveItem(strDriveName);
		strDetailInfo = pDriveItem->GetDisplayName();
		strDetailInfo += wxT(" ");
		strDetailInfo += pDriveItem->GetSpace();
	}
	else
	{
#endif
		if(iter->IsFile())
		{
			strDetailInfo += theCommonUtil->SetComma(iter->GetSize().ToString());
			strDetailInfo += wxT(" Bytes");
			strDetailInfo.append(wxT("  |  "));
		}

		wxString strAttr = iter->GetAttributeToString();

		strDetailInfo.append(strAttr);
		strDetailInfo.append(wxT("  |  "));


		wxString strTime = iter->GetDateTimeToString();
		strDetailInfo.append(strTime);
		strDetailInfo.append(wxT("  |  "));
		strDetailInfo.append(iter->GetTypeName());
#ifdef __WXMSW__
	}
#endif

	pDC->SetTextForeground(wxColour(0, 0, 0));
	pDC->DrawText(strDetailInfo, m_viewRectDetail.GetLeft() + 22, m_viewRectDetail.GetTop() + 3);

	pDC->SetFont(*_gFont);
}

void CListView::DrawItemImage(wxDC* pDC, int x, int y, int flags, int iIconIndex, int iOverlayIndex)
{
	flags |= INDEXTOOVERLAYMASK(iOverlayIndex + 1);
	m_pImageList->Draw(iIconIndex, pDC, x, y, flags);
}

void CListView::SetItemImage(int iItemIndex)
{
	wxVector<CDirData>::iterator iter = m_itemList.begin() + iItemIndex;
	bool isDrive = iter->IsDrive();
	if(isDrive)
		return;

#ifdef __WXMSW__
	wxString strFullPath = iter->GetFullPath();

	SHFILEINFO sfi;
	wxZeroMemory(sfi);

	DWORD dwNum = GetFileAttributes(strFullPath);
	SHGetFileInfo(strFullPath, dwNum, &sfi, sizeof(sfi), IMAGELIST_FLAG);

	int iIconIndex = sfi.iIcon & 0x00FFFFFF;
	int iOverlay = (sfi.iIcon >> 24) - 1;

	iter->m_iIconIndex = iIconIndex;
	iter->m_iOvelayIndex = iOverlay;
	iter->m_bIconImageSet = true;

	DestroyIcon(sfi.hIcon);
#else
#endif
}

void CListView::TerminateThread()
{
	if(GetThread() && GetThread()->IsRunning())
	{
		m_bImageListThreadStop = true;
		GetThread()->Wait();
	}
}

void CListView::ReadyImageListThread()
{
	m_bCreateThreadImageList = false;
	if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
	{
		wxLogError("Could not create the worker thread!");
		return;
	}

	m_bCreateThreadImageList = true;
}

void CListView::RunReadImageList()
{
	if(m_bCreateThreadImageList)
	{
		if (GetThread()->Run() != wxTHREAD_NO_ERROR)
		{
			wxLogError("Could not run the worker thread!");
			return;
		}
	}
}

wxThread::ExitCode CListView::Entry()
{
	int nPosIndex = 0;
	int iStartIndex = m_nStartIndex;

	for(int i = 0; i < m_nTotalItems; i++)
	{
		if(m_bImageListThreadStop)
			break;

		SetItemImage(i);

		//아이콘 표시를 위해 좌표계산이 완료될때까지 기다린다.
		//화면표시 시작위치가 바뀌게 되었을경우 해당 인덱스까지 올때까지 기다린다.
		if(iStartIndex <= i)
		{
			if((m_iTotalPositionCnt > 0) && (nPosIndex < m_iTotalPositionCnt))
			{
				CPositionInfo posInfo = m_posList.at(nPosIndex);
				theCommonUtil->RefreshWindow(this, posInfo.m_iconRect);
				nPosIndex++;
			}
		}

		if(iStartIndex != m_nStartIndex)
		{
			iStartIndex = m_nStartIndex;
			nPosIndex = 0;
		}
	}

	m_bImageListThreadStop = false;
	return (wxThread::ExitCode)0;
}

bool CListView::SetSelectedItem(int iKeyCode)
{
	int iTotalItem = m_itemList.size();
	if(m_nCurrentItemIndex >= iTotalItem - 1)
		return false;

	wxVector<CDirData>::iterator fIter = m_itemList.begin() + m_nCurrentItemIndex;

	if((theCommonUtil->Compare(fIter->GetName(), wxT("..")) != 0) && !fIter->IsDrive())
	{
		//선택이 되어 있는 상태면 선택이 안된상태로 변경
		bool bNewSelected = fIter->IsItemSelected() ? false : true;
		fIter->SetItemSelected(bNewSelected);

		if (bNewSelected) //아이템 선택
		{
			SELITEM_INFO _Info;

			_Info.m_iSelIndex = m_nCurrentItemIndex;
			_Info.m_bFile = fIter->IsFile();
			_Info.m_strFullPath = fIter->GetFullPath();

			std::unordered_map<int, SELITEM_INFO>::value_type valsel(m_nCurrentItemIndex, _Info);
			m_hashSelectedItem.insert(valsel);

			if(fIter->IsFile())
				m_iSelFileCnt++;
			else
				m_iSelDirCnt++;
		}
		else
		{
			std::unordered_map<int, SELITEM_INFO>::const_iterator findKey = m_hashSelectedItem.find(m_nCurrentItemIndex);
			if (findKey != m_hashSelectedItem.end())
			{
				SELITEM_INFO _Info = findKey->second;
				if(_Info.m_bFile)
					m_iSelFileCnt--;
				else
					m_iSelDirCnt--;

				m_hashSelectedItem.erase(m_nCurrentItemIndex);
			}
		}

		if(m_hashSelectedItem.size() > 0)
		{
			wxString strSelString(theMsgManager->GetMessage(wxT("MSG_DETAILINFO_VIEW_SELINFO")));
			wxString strSelItems = strSelString + wxString::Format(theMsgManager->GetMessage(wxT("MSG_DETAILINFO_VIEW_ITEM_SELECT")), m_iSelDirCnt, m_iSelFileCnt);

			DisplaySelectedItemInfo(strSelItems, 0, 0);
		}
		else
		{
			m_pMyTooltipView->Show(false);
			m_iSelDirCnt = 0;
			m_iSelFileCnt = 0;
		}
	}

	return true;
}

void CListView::DisplaySelectedItemInfo(const wxString& strMsg, int xPos, int yPos, bool bDispRenameInfo)
{
	m_pMyTooltipView->Show(false);

	wxClientDC dc(this);
	wxSize sztip = dc.GetTextExtent(strMsg);
	wxSize szTooltip;

	szTooltip.SetWidth(sztip.GetWidth() + 10);
	szTooltip.SetHeight(sztip.GetHeight() + 5);

	if(!bDispRenameInfo)
	{
		xPos = m_viewRectDisp.GetRight() - szTooltip.GetWidth();
		yPos = m_viewRectDisp.GetBottom() - szTooltip.GetHeight();
	}

	m_pMyTooltipView->SetTooltipText(strMsg);
	m_pMyTooltipView->SetThemeEnabled(true);
	m_pMyTooltipView->SetPosition(wxPoint(xPos, yPos));
	m_pMyTooltipView->SetSize(szTooltip);
	m_pMyTooltipView->Show(true);
}

wxString CListView::MakeFullPathName(const wxString& strName)
{
#ifdef __WXMSW__
	return m_iPathDepth == 1 ? m_strCurrentPath + strName : m_strCurrentPath + SLASH + strName;
#else
#endif
}

wxVector<CDirData>::iterator CListView::GetItemExist(const wxString& strName, bool& bExist)
{
	bExist = false;
	auto it = std::find_if(m_itemList.begin(), m_itemList.end(), [&strName](const CDirData& data) {
		wxString strTgt(data.GetName());
		strTgt = strTgt.Trim();

		wxString strSrc(strName);
		strSrc = strSrc.Trim();

#ifdef __WXMSW__
		return (strSrc.CmpNoCase(strTgt) == 0);
#else
		return (strSrc.Cmp(strTgt) == 0);
#endif
	});

	if (it != m_itemList.end())
		bExist = true;

	return it;
}

void CListView::UpdateLastAccessTime(const wxDateTime& dt)
{
	if(m_iPathDepth == 1)
		return;

	CDirData* pData = (CDirData *)&m_itemList.at(0);
	pData->SetDateTime(dt);
}

void CListView::OnKeyDownTextCtrl(wxKeyEvent& event)
{
	int iKeyCode = event.GetKeyCode();
	event.Skip();

	if (iKeyCode == WXK_ESCAPE)
	{
		m_bRename = false;
		m_pTxtCtrlForRename->SetLabelText(wxT(""));
		m_pTxtCtrlForRename->Show(false);
		m_pMyTooltipView->Show(false);
	}
}

void CListView::OnEnterTextCtrl(wxCommandEvent& event)
{
	m_bRename = false;
	wxString strNewRename = m_pTxtCtrlForRename->GetValue();

	wxString strOldPathName = MakeFullPathName(m_strItemToRename);
	wxString strNewPathName = MakeFullPathName(strNewRename);
#if !defined(__WXMSW__)
	bool bExist = false;
	GetItemExist(strNewRename, bExist);
	if (bExist)
	{
		wxString strMsg = strNewPathName + theMsgManager->GetMessage(wxT("MSG_DLG_ADD_DIR_EXIST"));
		wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
		return;
	}
#endif
	if(!theCommonUtil->IsCreatableDirOrFileName(strNewRename))
	{
		DoRenameOn(strNewRename);
		return;
	}

	DoRenameFromMenu(strOldPathName, strNewPathName);

	m_pTxtCtrlForRename->SetValue(wxT(""));
	m_pTxtCtrlForRename->Show(false);
	m_pMyTooltipView->Show(false);

	event.Skip();
}

void CListView::OnKillFocusTxtCtrl(wxFocusEvent& event)
{
	event.Skip();

	m_pTxtCtrlForRename->SetLabelText(wxT(""));
	m_pTxtCtrlForRename->Show(false);
	m_pMyTooltipView->Show(false);

	this->SetFocus();
}

void CListView::DoRenameOn(const wxString& strRename)
{
	int iCurrentPosition = m_nCurrentItemIndex % m_nDisplayItemInView;
	CPositionInfo posInfo = m_posList.at(iCurrentPosition);

	int iPosX1 = posInfo.m_mainRect.GetLeft();
	int iPosY1 = posInfo.m_mainRect.GetTop();
	int iPosX2 = posInfo.m_mainRect.GetWidth();
	int iPosY2 = posInfo.m_mainRect.GetHeight() + 1;

	int iLength = strRename.Len();
	wxString strExt = theCommonUtil->GetExt(strRename);
	int iExtLen = strExt.Len();
	if(iExtLen != 0)
	{
		iExtLen += 1;
		iLength -= iExtLen;
	}

	m_pTxtCtrlForRename->SetSize(wxRect(iPosX1, iPosY1, iPosX2, iPosY2));
	m_pTxtCtrlForRename->SetLabelText(strRename);
	m_pTxtCtrlForRename->SetSelection(0, iLength);
	m_pTxtCtrlForRename->Show(true);
	m_pTxtCtrlForRename->SetFocus();

	wxString strDontUse(theMsgManager->GetMessage(wxT("MSG_INFO_RENAME_DONTUSE")) + theMsgManager->GetMessage(wxT("MSG_INFO_RENAME_DONTUSE_STRING")));
	DisplaySelectedItemInfo(strDontUse, iPosX1 + 30, iPosY1 + iPosY2, true);
}

void CListView::DoRenameFromMenuPrepare()
{
	wxVector<CDirData>::const_iterator fIter = m_itemList.begin() + m_nCurrentItemIndex;
	m_strItemToRename = fIter->GetName();
	if(fIter->IsDrive() || (theCommonUtil->Compare(wxT(".."), m_strItemToRename) == 0))
		return;

	m_bRename = true;
	DoRenameOn(m_strItemToRename);
}

void CListView::DoRenameFromMenu(const wxString& strOldPathName, const wxString& strNewPathName)
{
	wxRename(strOldPathName, strNewPathName);
}

void CListView::OnMouseLBottonDown(wxMouseEvent& event)
{
	DoMouseProcess(event.GetPosition());

	if(!m_bSetFocus) theSplitterManager->ChangeActiveTab();
}

void CListView::OnMouseLButtonDBClick(wxMouseEvent& event)
{
	DoMouseProcess(event.GetPosition(), true);
}

void CListView::OnMouseRButtonDown(wxMouseEvent& event)
{
	DoMouseProcess(event.GetPosition());
}

void CListView::OnMouseRButtonUp(wxMouseEvent& event)
{
	DisplayContextMenu(event.GetPosition());
}

void CListView::OnMouseMove(wxMouseEvent& event)
{
	m_bMouseMoveAndFound = FindItemInMousePoint(event.GetPosition(), true);
}

void CListView::OnMouseWheel(wxMouseEvent& event)
{
	if(!m_bSetFocus)
		return;

	int iWheelRotation = event.GetWheelRotation();
	if (iWheelRotation < 0) //Down
	{
		m_nCurrentItemIndex++;
		if(m_nCurrentItemIndex > (m_nTotalItems - 1))
			m_nCurrentItemIndex = m_nTotalItems - 1;
	}
	else //Up
	{
		m_nCurrentItemIndex--;
		if(m_nCurrentItemIndex < 0)
			m_nCurrentItemIndex = 0;
	}

	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CListView::DoMouseProcess(const wxPoint& pt, bool bDblClick)
{
	m_bMouseClickItemFound = false;
	if (FindItemInMousePoint(pt))
	{
		if (bDblClick)
			PressEnterKey();

		m_bMouseClickItemFound = true;
		theCommonUtil->RefreshWindow(this, m_viewRect);
	}
}

bool CListView::FindItemInMousePoint(const wxPoint& pt, bool IsMouseMove)
{
	bool bFoundOK = false;
	wxVector<CPositionInfo>::const_iterator fIter = m_posList.begin();

	int iClickPosIndex = 0;
	while(fIter != m_posList.end())
	{
		if(fIter->m_mainRect.Contains(pt))
		{	//아이템 시작인덱스 + 클릭인덱스 < 전체 아이템수
			bFoundOK = (m_nStartIndex + iClickPosIndex) < m_nTotalItems ? true : false;
			break;
		}

		iClickPosIndex++;
		fIter++;
	}

	if(bFoundOK)
	{
		if(!IsMouseMove)
			m_nCurrentItemIndex = m_nStartIndex + iClickPosIndex;
		else
			m_iMouseMoveFoundIndx = m_nStartIndex + iClickPosIndex;
	}

	return bFoundOK;
}

//Custom Event
void CListView::OnItemRename(wxCommandEvent &event)
{
	DoRenameFromMenuPrepare();
}

void CListView::OnMenuFileEdit(wxCommandEvent& event)
{
	CheckCanFileEdit();
}

void CListView::OnMenuFileEditProcess(wxCommandEvent& event)
{
	int iID = event.GetId();
	int iIndex = iID - EXTERNAL_PROGRAM_START_ID;

	DoFileEdit(iIndex);
}

void CListView::CheckCanFileEdit()
{
	int iExternalPGCount = _gExternalPGList.size();
	if(iExternalPGCount == 0)
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_FILE_MENU_EDIT_NOT_SET")), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
		return;
	}

	int iSelCount = m_hashSelectedItem.size();
	if(iSelCount == 0)
	{
		CDirData data = m_itemList.at(m_nCurrentItemIndex);
		if (data.IsDir() || data.IsDrive())
		{
			wxString strMsg = data.IsDrive() ? wxT("MSG_FILE_MENU_EDIT_NOT_SUPPORT_DRV") : wxT("MSG_FILE_MENU_EDIT_NOT_SUPPORT_DIR");
			wxMessageBox(theMsgManager->GetMessage(strMsg), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
			return;
		}
	}
	else
	{
		bool bIncludedDir = false;
		std::unordered_map<int, SELITEM_INFO>::const_iterator fIter = m_hashSelectedItem.begin();
		while(fIter != m_hashSelectedItem.end())
		{
			SELITEM_INFO _info = fIter->second;
			if(!_info.m_bFile)
			{
				bIncludedDir = true;
				break;
			}

			fIter++;
		}

		if(bIncludedDir)
		{
			wxMessageBox(wxT("The directory is included in the selection"), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
			return;
		}
	}

	if(iExternalPGCount > 1)
	{
		int iItemPosition = m_nCurrentItemIndex - m_nStartIndex;//% m_nDisplayItemInView;
		CPositionInfo posInfo = m_posList.at(iItemPosition);

		wxPoint pt(posInfo.m_nameRect.GetLeft() + 5, posInfo.m_nameRect.GetTop() + ICON_HEIGHT + 5);
		wxMenu menu;

		for (int i = 0; i < iExternalPGCount; i++)
		{
			EXTERNAL_EDIT_PROGRAM extInfo = _gExternalPGList.at(i);
			wxMenuItem* pMenuItem = menu.Append(EXTERNAL_PROGRAM_START_ID + i, extInfo._strPGName);

			int iIconIndex;
			int iOverlayIndex;
			theCommonUtil->GetIconIndex(extInfo._strPGPath, iIconIndex, iOverlayIndex);
			HICON hIcon = ImageList_GetIcon(_gImageList, iIconIndex, ILD_NORMAL);
			wxIcon ico;
			ico.CreateFromHICON(hIcon);

			if (ico.IsOk())
			{
				wxBitmap bmp;
				bmp.CopyFromIcon(ico);

				if(bmp.IsOk())
					pMenuItem->SetBitmap(bmp);
				else
					pMenuItem->SetBitmap(_gExternalPG);
			}
			else
				pMenuItem->SetBitmap(_gExternalPG);

			DestroyIcon(hIcon);
		}

		this->PopupMenu(&menu, pt);
	}
	else
	{
		//프로그램 실행
		DoFileEdit(0);
	}
}

void CListView::DoFileEdit(int iIndex)
{
	EXTERNAL_EDIT_PROGRAM extInfo = _gExternalPGList.at(iIndex);

	wxString strFilePG = extInfo._strPGPath;
	wxString strArgs = extInfo._strPGArgs;
	if (strFilePG == wxT(""))
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_FILE_MENU_EDIT_NOT_SET")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return;
	}

	if (!wxFileExists(strFilePG))
	{
		wxString strMsg = wxString::Format(theMsgManager->GetMessage(wxT("MSG_FILE_MENU_EDIT_NOT_EXIST_PROGRAM")), strFilePG);
		wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return;
	}

	std::unordered_map<int, SELITEM_INFO>::iterator iTer = m_hashSelectedItem.begin();
	CDirData data;
	if (iTer == m_hashSelectedItem.end())
	{
		data = m_itemList.at(m_nCurrentItemIndex);
		wxString strFullPath = data.GetFullPath();
		wxExecute(strFilePG + wxT(" ") + strArgs + wxT(" ") + wxT("\"") + data.GetFullPath() + wxT("\""));
	}
	else
	{
		while(iTer != m_hashSelectedItem.end())
		{
			SELITEM_INFO _Info = iTer->second;
			data = m_itemList.at(_Info.m_iSelIndex);
			wxExecute(strFilePG + wxT(" ") + strArgs + wxT(" ") + wxT("\"") + data.GetFullPath() + wxT("\""));

			iTer++;
		}

		DoSelectedItemsClear();
		theCommonUtil->RefreshWindow(this, m_viewRect);
	}
}


void CListView::OnMenuEditAllReleaseOrSelect(wxCommandEvent& event)
{
	_MENU_EVENT_TYPE mType = (_MENU_EVENT_TYPE)event.GetInt();
	DoSelectAllOrRelease(mType);
}

void CListView::DoSelectAllOrRelease(_MENU_EVENT_TYPE _mType)
{
	m_hashSelectedItem.clear();
	m_iSelDirCnt = 0;
	m_iSelFileCnt = 0;

	bool bAllSelect = _mType == _MENU_EDIT_ALL_SELECT ? true : false;
	wxVector<CDirData>::iterator fIter = m_itemList.begin();
	int iIndex = 0;
	while(fIter != m_itemList.end())
	{
		if(!fIter->IsDrive() && (fIter->GetName().Cmp(wxT("..")) != 0))
		{
			fIter->SetItemSelected(bAllSelect);

			if(bAllSelect)
			{
				if(fIter->IsDir())
					m_iSelDirCnt++;
				else
					m_iSelFileCnt++;

				SELITEM_INFO _Info;
				_Info.m_iSelIndex = iIndex;
				_Info.m_bFile = fIter->IsFile();
				_Info.m_strFullPath = fIter->GetFullPath();

				std::unordered_map<int, SELITEM_INFO>::value_type valsel(iIndex, _Info);
				m_hashSelectedItem.insert(valsel);
			}
		}

		fIter++;
		iIndex++;
	}

	if(bAllSelect)
	{
		wxString strSelString(theMsgManager->GetMessage(wxT("MSG_DETAILINFO_VIEW_SELINFO")));
		wxString strSelItems = strSelString + wxString::Format(theMsgManager->GetMessage(wxT("MSG_DETAILINFO_VIEW_ITEM_SELECT")), m_iSelDirCnt, m_iSelFileCnt);

		DisplaySelectedItemInfo(strSelItems, 0, 0);
	}
	else
		m_pMyTooltipView->Show(false);

	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CListView::OnMenuEditViewContextMenu(wxCommandEvent& event)
{
	m_bContextMenuFromMenuEvent = true;
	int iItemPosition = m_nCurrentItemIndex % m_nDisplayItemInView;
	CPositionInfo posInfo = m_posList.at(iItemPosition);

	wxPoint pt(posInfo.m_nameRect.GetRight() / 4, posInfo.m_nameRect.GetTop() + ICON_HEIGHT);

	DisplayContextMenu(pt);
	m_bContextMenuFromMenuEvent = false;
}

void CListView::DisplayContextMenu(const wxPoint& pt)
{
	int iSelectedItemIndex = m_nCurrentItemIndex;
	int iItemCount = m_itemList.size();

	if (iItemCount < m_nCurrentItemIndex)
		m_nCurrentItemIndex = iSelectedItemIndex;

	wxArrayString arrString;
	int iSelectedItems = m_hashSelectedItem.size();
	if (iSelectedItems != 0)
	{
		std::unordered_map<int, SELITEM_INFO>::const_iterator iter = m_hashSelectedItem.begin();
		while(iter != m_hashSelectedItem.end())
		{
			SELITEM_INFO _ItemInfo = iter->second;
			arrString.Add(_ItemInfo.m_strFullPath);

			iter++;
		}
	}
	else
	{
		wxString strContextPath = m_strCurrentPath;
		if(m_bMouseClickItemFound || m_bContextMenuFromMenuEvent)
		{
			CDirData item = m_itemList.at(m_nCurrentItemIndex);
			if (item.GetName().Cmp(wxT("..")) == 0)
				return;
#ifdef __WXMSW__
			strContextPath = item.IsDrive() ? item.GetDriveName() : item.GetFullPath();
#else
			strContextPath = item.GetFullPath();
#endif
		}

		arrString.Add(strContextPath);
	}

	CContextMenuHandler cMenu;
	cMenu.SetObject(arrString);
	cMenu.ShowContextMenu(this, pt);
}

bool CListView::GetTrashOrDeleteData(std::list<wxString>& lstDatas, bool bTrash)
{
	bool bDel = false;
	int iSelectedItems = m_hashSelectedItem.size();

	wxString strMsg(wxT(""));
	wxString strGoTrash = theMsgManager->GetMessage(wxT("MSG_DLG_DELETE_TRASH"));
	wxString strDelComplete = theMsgManager->GetMessage(wxT("MSG_DLG_DELETE_NOT_TRASH"));

	bool bGoTrash = bTrash;
	int iRetValue = 0;
	bool bOpenCheck = false;

	if (iSelectedItems == 0)
	{
		CDirData selItem = m_itemList.at(m_nCurrentItemIndex);
		if (theCommonUtil->Compare(selItem.GetName(), wxT("..")) == 0)
			return false;

		if (selItem.IsFile())
			bOpenCheck = CLocalFileSystem::IsCheckedFileOpen(selItem.GetFullPath());

		strMsg = selItem.GetFullPath();

		if (bOpenCheck)
		{
			wxMessageBox(strMsg + wxT(" is opened!. you can't delete operation!"), wxT("Delete...."), wxICON_ERROR, this);
			return false;
		}

		strMsg += wxT("\n");
		strMsg += bGoTrash ? strGoTrash : strDelComplete;

		wxString strDelItem(selItem.GetFullPath());
		lstDatas.push_back(strDelItem);

		iRetValue = wxMessageBox(strMsg, wxT("Delete...."), wxYES_NO | wxICON_EXCLAMATION, this);
	}
	else
	{
		int iDelItemCounts = 0;

		std::unordered_map<int, SELITEM_INFO>::const_iterator iter = m_hashSelectedItem.begin();
		strMsg = wxT("");
		strMsg += wxString::Format(wxT("%d "), iSelectedItems);
		strMsg += theMsgManager->GetMessage(wxT("MSG_DLG_DELETE_SELITEM"));
		strMsg += wxT("\n\n");

		wxString strItem(wxT(""));
		for (iter; iter != m_hashSelectedItem.end(); ++iter)
		{
			bOpenCheck = false;

			SELITEM_INFO _info = iter->second;

			strItem = _info.m_strFullPath;
			bool bFile = _info.m_bFile;

			if (bFile)
			{
				bOpenCheck = CLocalFileSystem::IsCheckedFileOpen(strItem);
				if (bOpenCheck)
				{
					int iRet = wxMessageBox(strItem + wxT(" is opend another program!. this file to be skipped! \n Continue?"), wxT("Delete...."), wxYES_NO | wxICON_ERROR, this);
					if (iRet == wxYES)
					{
						CDirData* data = (CDirData *)&m_itemList.at(_info.m_iSelIndex);
						data->SetItemSelected(false);
						continue;
					}
					else
						return false;
				}
			}

			lstDatas.push_back(strItem);
			iDelItemCounts++;
			strMsg += strItem;
			strMsg += wxT("\n");
		}

		strMsg += wxT("\n");
		strMsg += bGoTrash ? strGoTrash : strDelComplete;
		if(iDelItemCounts > 0)
			iRetValue = wxMessageBox(strMsg, wxT("Delete...."), wxYES_NO | wxICON_EXCLAMATION, this);
	}

	if (iRetValue == wxYES)
	{
		if (lstDatas.size() > 0)
			bDel = true;
	}

	return bDel;
}

void CListView::GetCompressedItems(std::vector<wxString>& vecItems, wxString& strCompressedFile)
{
	int iSelCount = m_hashSelectedItem.size();
	if(iSelCount == 0)
	{
		CDirData data = m_itemList.at(m_nCurrentItemIndex);
		if (data.IsDrive())
		{
			wxMessageBox(wxT("The selected item is drive and You cannnot will be compressison"), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
			return;
		}

		vecItems.emplace_back(data.GetFullPath());
		wxString strTmp = theCommonUtil->GetName(data.GetFullPath());

		strCompressedFile = m_iPathDepth == 1 ? m_strCurrentPath + strTmp + wxT(".") + wxT("zip") : m_strCurrentPath + SLASH + strTmp + wxT(".") + wxT("zip");
	}
	else
	{
		std::unordered_map<int, SELITEM_INFO>::const_iterator fIter = m_hashSelectedItem.begin();
		while(fIter != m_hashSelectedItem.end())
		{
			SELITEM_INFO _info = fIter->second;

			vecItems.emplace_back(_info.m_strFullPath);
			fIter++;
		}

		wxString strTmp = theCommonUtil->GetPathName(m_strCurrentPath);
		strCompressedFile = m_iPathDepth == 1 ? m_strCurrentPath + m_strCurrentPath.Left(1) + wxT(".") + wxT("zip")
		                                      : m_strCurrentPath + SLASH + strTmp + wxT(".") + wxT("zip");
	}
}

wxString CListView::GetCompressedFileForDecompress()
{
	auto item = m_itemList.begin() + m_nCurrentItemIndex;
	return item->GetFullPath();
}

void CListView::OnMenuViewSubDir(wxCommandEvent& event)
{
	auto item = m_itemList.at(m_nCurrentItemIndex);
	if(!item.IsDir())
	{
		wxMessageBox(wxT("The selected item is not directory"), PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
		return;
	}

	m_subdirMap.clear();

	wxMenu menuSub;
	m_iSubDirMenuID = VIEW_SUBDIR_START_ID - 1;

	wxString strSearchDir(m_strCurrentPath + (m_iPathDepth == 1 ? item.GetName() : SLASH + item.GetName()));
	if(item.GetName().Cmp(wxT("..")) == 0)
		strSearchDir = theCommonUtil->GetParentPath(m_strCurrentPath);

	DoViewSubDir(&menuSub, strSearchDir);

	int iItemPosition = m_nCurrentItemIndex - m_nStartIndex;
	CPositionInfo posInfo = m_posList.at(iItemPosition);

	wxPoint pt(posInfo.m_nameRect.GetLeft() + 5, posInfo.m_nameRect.GetTop() + ICON_HEIGHT + 5);
	this->PopupMenu(&menuSub, pt);
}

void CListView::DoViewSubDir(wxMenu* pMenu, const wxString& strPath )
{
#ifdef __WXMSW__
	WIN32_FIND_DATA fd;
	HANDLE hFind;
	bool bfound;
	wxString strFind_path(strPath);

	strFind_path += SLASH + wxT("*.*");

	hFind = FindFirstFileEx(strFind_path, FindExInfoStandard, &fd, FindExSearchNameMatch, 0, 0);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	bfound = true;

	do
	{
		wxString strName(fd.cFileName);
		if (std::find(_gVecIgnore.begin(), _gVecIgnore.end(), strName.ToStdString()) != _gVecIgnore.end())
			continue;

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			m_iSubDirMenuID++;
			wxString strNewPath(strPath + SLASH + strName);

			wxMenuItem* pMenuItem = pMenu->Append(m_iSubDirMenuID, strName, strNewPath);
			pMenuItem->SetBitmap(_gNormalFolderIco);

			this->Bind(wxEVT_SELECT_SUBDIR, &CListView::OnMenuSelectSubDir, this, m_iSubDirMenuID);
			std::unordered_map<int, wxString>::value_type val(m_iSubDirMenuID, strNewPath);
			m_subdirMap.insert(val);
		}
	} while ((bfound = FindNextFile(hFind, &fd) != 0));
#else
#endif
}

void CListView::OnMenuSelectSubDir(wxCommandEvent& event)
{
	int iId = event.GetId();
	std::unordered_map<int, wxString>::const_iterator fIter = m_subdirMap.find(iId);
	if(fIter == m_subdirMap.end())
		return;

	wxString strPath = fIter->second;
	LoadDir(strPath);
}

void CListView::OnIdle(wxIdleEvent& event)
{
	LASTINPUTINFO last_input;

	last_input.cbSize = sizeof(LASTINPUTINFO);
	if(GetLastInputInfo(&last_input))
	{
		DWORD dwidle_time;
		dwidle_time = GetTickCount() - last_input.dwTime;
		if(dwidle_time > 1500)
		{
			CDirData data = m_itemList.at(m_nCurrentItemIndex);
			int iCurrentPosition = m_nCurrentItemIndex - m_nStartIndex;// m_nDisplayItemInView;
			CPositionInfo posInfo = m_posList.at(iCurrentPosition);

			wxString strDetailInfo(data.GetName());
			if(strDetailInfo.Cmp(wxT("..")) == 0)
				return;

			strDetailInfo.append(wxT(" | "));
		#ifdef __WXMSW__
			if(data.IsDrive())
			{
				wxString strDriveName = data.GetDriveName();
				CDriveItem* pDriveItem = theDriveInfo->GetDriveItem(strDriveName);
				strDetailInfo = pDriveItem->GetDisplayName();
				strDetailInfo += wxT(" | ");
				strDetailInfo += pDriveItem->GetSpace();
			}
			else
			{
		#endif
				if(data.IsFile())
				{
					strDetailInfo += theCommonUtil->SetComma(data.GetSize().ToString());
					strDetailInfo += wxT(" Bytes");
					strDetailInfo.append(wxT(" | "));
				}

				wxString strAttr = data.GetAttributeToString();

				strDetailInfo.append(strAttr);
				strDetailInfo.append(wxT(" | "));

				wxString strTime = data.GetDateTimeToString();
				strDetailInfo.append(strTime);
				strDetailInfo.append(wxT(" | "));
				strDetailInfo.append(data.GetTypeName());
		#ifdef __WXMSW__
			}
		#endif

			if(!m_bSetFocus)
				return;

			wxClientDC dc(this);
			wxSize sztip = dc.GetTextExtent(strDetailInfo);
			wxSize szTooltip;

			wxPoint ptTooltip(posInfo.m_nameRect.GetLeft(), posInfo.m_nameRect.GetBottom() + 2);

			szTooltip.SetWidth(sztip.GetWidth() + 10);
			szTooltip.SetHeight(sztip.GetHeight() + 5 );

			int iXPos = ptTooltip.x + szTooltip.GetWidth();
			if(m_viewRect.GetRight() < iXPos)
			{
				ptTooltip.x -= ((szTooltip.GetWidth() - posInfo.m_nameRect.GetWidth()) + 5);

				if(ptTooltip.x <= 0)
					ptTooltip.x = posInfo.m_nameRect.GetWidth();
			}

			m_pMyTooltipDetail->SetTooltipText(strDetailInfo);
			m_pMyTooltipDetail->SetThemeEnabled(true);
			m_pMyTooltipDetail->SetPosition(ptTooltip);
			m_pMyTooltipDetail->SetSize(szTooltip);
			m_pMyTooltipDetail->Show(true);
		}
		else
		{
			m_pMyTooltipDetail->SetTooltipText(wxT(""));
			m_pMyTooltipDetail->Show(false);
		}
	}

	event.RequestMore(false);
}
