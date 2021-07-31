#ifndef LISTVIEW_H_INCLUDED
#define LISTVIEW_H_INCLUDED

#include "../engine/LocalFileSystem.h"
#include "../interface/PositionInfo.h"
#include "../interface/DirData.h"
#include "../interface/Sorting.h"

struct _HistoryItem
{
	int iCurrentIndex;
	int iStartIndex;
};

WX_DECLARE_STRING_HASH_MAP(_HistoryItem, _HISTORY);

class CMyTooltipView;
class CImageList;
class CViewPanel;
class CLock;

class CListView : public wxWindow, public wxThreadHelper
{
	friend class CViewPanel;
protected:
	const int ICON_WIDTH              = 16;
	const int ICON_HEIGHT             = 16;
	const int ICON_SELECTED_WIDTH_GAP = 10;
	const int GAP_WIDTH               = 2;
	const int GAP_HEIGHT              = 2;

	wxIcon   m_icoUpDir;
	wxColour m_colDefault;
	wxColour m_colDrive;
	wxColour m_colDir;
	wxColour m_colSize;
	wxColour m_colTime;
	wxColour m_colAttr;
	wxColour m_colType;
	wxColour m_colMatch;

protected:
	typedef struct _SELITEM_INFO {
		int m_iSelIndex;
		wxString m_strFullPath;
		bool m_bFile;
	} SELITEM_INFO;

	class CColumnPoint
	{
	public:
		wxPoint pt1;
		wxPoint pt2;
	};

	typedef struct _FILEICON_INFO {
		int iIconIndex = 0;
		int iOverlayIndex = 0;
	} FILE_ICONINFO;

	typedef std::unordered_map<int, FILE_ICONINFO> FILEICON_MAP;

public:
	explicit CListView(wxWindow* parent, const int nID, const wxSize& sz);
	virtual ~CListView();

	virtual void LoadDir(const wxString& strPath) = 0;
	void DoSelectedItemsClear(bool bDeleted = false);

protected:
	//FileSystemWatcher 함수들..
	virtual void DoCreate(const wxString& strName) = 0;
	virtual void DoModify(const wxString& strName) = 0;
	virtual void DoDelete(const wxString& strName) = 0;
	virtual void DoRename(const wxString& strOldName, const wxString& strNewName) = 0;
	wxString MakeFullPathName(const wxString& strName);

	virtual void Render(wxDC* pDC) = 0;
    virtual wxThread::ExitCode Entry();

	void Clear();
	void Initialize();

	wxMemoryDC* CreateMemoryDC(wxDC* pDC, const wxRect& rc, const wxColour& colPen = wxColour(0, 0, 0), const wxColour colBrush = wxColour(0, 0, 0));
	void ClearMemoryDC(wxMemoryDC* pMemDC);
	void ClearMemoryDC();
	void ChangeViewSize(const wxSize& sz);

	void CalcColumn(wxDC* pDC);
	bool CalcAutoColumn(wxDC* pDC, const wxRect& viewRect);
	void DrawColumn(wxDC* pDC);
	void CalcPosition(wxDC* pDC);
	void DrawDirInfoArea(wxDC* pDC);
	void SetDiskSpace();

	void DoSortStart();
	void ReCalcPage();

	wxString CalcDispStr(wxDC* pDC, const wxString& strSourceSave, const wxString& strSource, bool isDrive);
	wxString FindMaxData(const wxVector<wxString>& vecData);
	wxString FindMaxData(const wxString& a, const wxString& b);

	void DisplayItems(wxDC* pDC);
	void DisplayItemDetailInfo(wxDC* pDC, int iIndex);

	void SetItemImage(int iItemIndex);
	void DrawItemImage(wxDC* pDC, int x, int y, int flags, int iIconIndex = 0, int iOverlayIndex = 0);

	virtual void ProcessKeyEvent(const int nKeyCode);
	void MoveLeftAndUpKey();
	void MoveRightAndDownKey();

	bool PressEnterKey();

	void TerminateThread();
	void ReadyImageListThread();
	void RunReadImageList();

	void GotoRoot();
	//선택아이템 처리
	bool SetSelectedItem(int iKeyCode);
	//이아템 매칭처리
	void FindMatchItems();
	void DoMatchClear();
	void InitKeyInputTooltip();
	void UpdateLastAccessTime(const wxDateTime& dt);
	//아이템존재여부 확인
	wxVector<CDirData>::iterator GetItemExist(const wxString& strName, bool& bExist);

	//이름변경(메뉴실행시)
	void DoRenameFromMenu(const wxString& strOldPathName, const wxString& strNewPathName);
	//이름변경
	void DoRenameFromMenuPrepare();
	void DoRenameOn(const wxString& strRename);

	//마우스이벤트 처리
	void DoMouseProcess(const wxPoint& pt, bool bDblClick = false);
	bool FindItemInMousePoint(const wxPoint& pt, bool IsMouseMove = false);
	void GetFileOperationSource(std::list<wxString>& lstSrc, bool IsCut = false);

	//외부프로그램 실행
	void DoFileEdit(int iIndex);
	//콘텍스트 메뉴 보기
	void DisplayContextMenu(const wxPoint& pt);
	virtual void ShowFavoriteMenu() = 0;

	bool GetTrashOrDeleteData(std::list<wxString>& lstDatas, bool bTrash);
	void GetCompressedItems(std::vector<wxString>& vecItems, wxString& strCompressedFile);
	wxString GetCompressedFileForDecompress();

private:
	void DetachDC();
	void DisplaySelectedItemInfo(const wxString& strMsg, int xPos, int yPos, bool bDispRenameInfo = false);
	void AllClear();
	//편집 가능여부 체크
	void CheckCanFileEdit();
	//전체선택/해제
	void DoSelectAllOrRelease(_MENU_EVENT_TYPE _mType);
	void DoViewSubDir(wxMenu* pMenu, const wxString& strPath );
protected:
	// 화면 버퍼(더블버퍼링 이용)
	wxBitmap* m_pDoubleBuffer = nullptr;
	//변경사이즈(OnSize Event)
	wxSize m_szChagned = wxSize(0, 0);
	//화면 영역 크기
	wxRect m_viewRect = wxRect(0, 0, 0, 0);
	//디렉토리 정보 표시영역
	wxRect m_viewDirInfo = wxRect(0, 0, 0, 0);
	//화면 표시 영역
	wxRect m_viewRectDisp = wxRect(0, 0, 0, 0);
	//상세보기 영역
	wxRect m_viewRectDetail = wxRect(0, 0, 0, 0);
	// 컬럼 라인 정보
	wxVector<CColumnPoint> m_ptList;
	// 데이터
	wxVector<CDirData> m_itemList;
	// 포지션정보
	wxVector<CPositionInfo>	m_posList;
	// 화면표시이름
	std::unordered_map<wxString, wxString> m_dispNameInfoMap;
	//이미지 인덱스
	FILEICON_MAP m_IconMap;
	//이미지리스트
	CImageList* m_pImageList = nullptr;

	// 문자 높이
	int m_iCharHeight         = 0;
	// 컬럼당 표시가능한 아이템수
	int m_nItemCountInColumn  = 0;
	// 컬럼수
	int	m_nDispColumn         = 0;
	// 화면에 표시가능한 아이템수
	int	m_nDisplayItemInView  = 0;
	// 컬럼끝 포인트
	int	m_nDispColumnEndPoint = 0;
	// 전체아이템수
	int	m_nTotalItems         = 0;
	// 화면표시 아이템 시작위치
	int m_nStartIndex;
	// 현재커서위치
	int m_nCurrentItemIndex   = 0;
	// 현재포지션위치
	int	m_iCurrentPosition    = 0;
	// 디렉토리수
	int m_iDirCount           = 0;
	// 파일수
	int m_iFileCount          = 0;
	//선택 파일수
	int m_iSelFileCnt         = 0;
	//선택 디렉토리 수
	int m_iSelDirCnt          = 0;
	//Path Depth
	int m_iPathDepth          = 1;
	//전체 포지션 카운터
	int m_iTotalPositionCnt   = 0;
	//마우스 이동 인덱스
	int m_iMouseMoveFoundIndx = 0;
	//히스토리 최대 카운터
	int	m_nMaxHistoryCount    = 300;
	//폴더 사이즈
	double m_dblFileSizeInDir = 0.0;
	//디스크 전체 사이즈
	double m_dblTotalSpace    = 0.0;
	//디스크 가용 사이즈
	double m_dblFreeSpace     = 0.0;
	//디렉토리 Load 플래그
	bool m_bDirLoaded   = false;
	//화면 변경 플래그
	bool m_bSizeOrColumnChanged = false;
	//윈도우 포커스 플래그
	bool m_bSetFocus    = false;
	//컬림 계산플래그
	bool m_bCalcColumn  = false;
	// 화면에 표시가능한 아이템
	bool m_bDispFlag[5] = {false, };
	//쓰레드 중지플래그
	bool m_bImageListThreadStop = false;
	//마우스클릭 아이템 찾기플래그
	bool m_bMouseClickItemFound = false;
	bool m_bContextMenuFromMenuEvent = false;
	//마우스 이동 후 검색된 아이템
	bool m_bMouseMoveAndFound = false;
	//이미지리스트 쓰레드 생성플래그
	bool m_bCreateThreadImageList = false;
	// 현재 디렉토리
	wxString m_strCurrentPath = wxT("");
	// 가장긴 이름
	wxString m_strMaxName = wxT("");
	// 가장긴 파일유형
	wxString m_strMaxTypeName = wxT("");
	//드라이브 명
	wxString m_strVolume = wxT("");
	//이름변경
	wxString m_strItemToRename = wxT("");
	// 키 입력문자
	wxString m_strKeyInput = wxT("");
	//디스크 용량
	wxString m_strTotalSpace = wxT("");
	wxString m_strFreeSpace = wxT("");
	wxString m_strDispDiskSpace = wxT("");

	COLUMN_VIEW_OPTION m_enumColumnViewOption;
	//디렉토리방문이력
	_HISTORY m_hashHistory;

	//선택 아이템
	std::unordered_map<int, SELITEM_INFO> m_hashSelectedItem;

	//tooltip window
	CMyTooltipView* m_pMyTooltipView = nullptr;
	CMyTooltipView* m_pMyTooltipKeyInput = nullptr;
	CMyTooltipView* m_pMyTooltipDetail = nullptr;

	CViewPanel* m_pViewPanel;
	CLock m_lock;
	//Match 아이템 정보
	wxVector<int> m_matchItems;
	// 이름변경 텍스트컨트롤
	std::unique_ptr<wxTextCtrl> m_pTxtCtrlForRename = nullptr;

	int m_iSubDirMenuID = 0;

private:
	std::unique_ptr<wxMemoryDC> m_pMemDC = nullptr;
	std::unordered_map<int, wxString> m_subdirMap;

	wxVector<int> m_ProcessKeyEvent;
	bool m_bRename = false;

private:
	void OnChar(wxKeyEvent& event);
	void OnCharHook(wxKeyEvent& event);
	void OnSetFocus(wxFocusEvent& event);
	void OnKillFocus(wxFocusEvent& event);
	void OnSize(wxSizeEvent& event);

	//이름변경 이벤트
	void OnEnterTextCtrl(wxCommandEvent& event);
	void OnKeyDownTextCtrl(wxKeyEvent& event);
	void OnKillFocusTxtCtrl(wxFocusEvent& event);

	//마우스 이벤트
	void OnMouseLBottonDown(wxMouseEvent& event);
	void OnMouseLButtonDBClick(wxMouseEvent& event);
	void OnMouseRButtonDown(wxMouseEvent& event);
	void OnMouseRButtonUp(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseWheel(wxMouseEvent& event);

	//Idle 이벤트
	void OnIdle(wxIdleEvent& event);
	//Custom Event
	void OnItemRename(wxCommandEvent &event);
	//파일편집(외부편집프로그램이 여러개인경우)
	void OnMenuFileEditProcess(wxCommandEvent& event);
	void OnMenuFileEdit(wxCommandEvent& event);
	//전체선택/해제
	void OnMenuEditAllReleaseOrSelect(wxCommandEvent& event);
	//컨텍스트 메뉴
	void OnMenuEditViewContextMenu(wxCommandEvent& event);
	//서브디렉토리 보기
	void OnMenuViewSubDir(wxCommandEvent& event);
	void OnMenuSelectSubDir(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
    wxDECLARE_NO_COPY_CLASS(CListView);
};

#endif // LISTVIEW_H_INCLUDED
