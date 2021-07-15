#include "ginc.h"
#include "MainFrame.h"
#include "EventHandler.h"

wxBEGIN_EVENT_TABLE(CEventHandler, wxEvtHandler)
	EVT_MENU(wxID_ANY, CEventHandler::OnMenuEvent)
	EVT_UPDATE_UI(wxID_ANY, CEventHandler::OnMenuEventUpdate)
wxEND_EVENT_TABLE()

CEventHandler::CEventHandler(CMainFrame* _pMainFrame)
	: m_pMainFrame(_pMainFrame)
{
	Init();
}

CEventHandler::~CEventHandler()
{

}

void CEventHandler::Init()
{
	typedef struct stMenuList {
		wxString strMenuID;
		_MENU_EVENT_TYPE mType = _MENU_EVENT_NONE;
	} MENU_LIST;

	MENU_LIST mList[] = {
		{ wxT("m_fMenu_Execute"),	                _MENU_FILE_EXECUTION },
		{ wxT("m_fMenu_NewTab"),	                _MENU_FILE_NEW_TAB },
		{ wxT("m_fMenu_Copy"),	                    _MENU_FILE_COPY },
		{ wxT("m_fMenu_Move"),	                    _MENU_FILE_MOVE },
		{ wxT("m_fMenu_Rename"),	                _MENU_FILE_RENAME },
		{ wxT("m_fMenu_MakeFolder"),	            _MENU_FILE_ADD_DIR },
		{ wxT("m_fMenu_FileEdit"),	                _MENU_FILE_EDIT },
		{ wxT("m_fMenu_goTrash"),	                _MENU_FILE_DEL_TRASH },
		{ wxT("m_fMenu_goDel"),	                    _MENU_FILE_DEL_COMP },
		{ wxT("m_fMenu_CMD"),	                    _MENU_FILE_CMD_EXEC },
		{ wxT("m_fMenu_Exit"),	                    _MENU_FILE_PROGRAM_TERMINATE },
		{ wxT("m_eMenu_Release"),	                _MENU_EDIT_ALL_RELEASE },
		{ wxT("m_eMenu_SelAll"),	                _MENU_EDIT_ALL_SELECT },
		{ wxT("m_eMenu_Copy"),	                    _MENU_EDIT_COPY_CLIPBOARD },
		{ wxT("m_eMenu_Cut"),	                    _MENU_EDIT_MOVE_CLIPBOARD },
		{ wxT("m_eMenu_Paste"),        	            _MENU_EDIT_PASTE_CLIPBOARD },
		{ wxT("m_eMenu_Search"),	                _MENU_EDIT_FIND },
		{ wxT("m_eMenu_ContextMenu"),	            _MENU_EDIT_CONTEXT_MENU },
		{ wxT("m_pMenu_PathMng"),	                _MENU_PATH_DIR_MANAGER },
		{ wxT("m_pMenu_Refresh"),	                _MENU_PATH_REFRESH },
		{ wxT("m_pMenu_GotoDirDirectly"), 	        _MENU_PATH_GOTO_DIR_DIRECTLY },
		{ wxT("m_menuComp_View"),	                _MENU_COMPRESS_FILE_VIEW },
		{ wxT("m_menuComp_Progress"),	            _MENU_COMPRESS_DO_COMPRESS },
		{ wxT("m_menuComp_CurrentDirCompRelease"),	_MENU_DECOMPRESS_CURRENT_DIR },
		{ wxT("m_menuComp_MakeDirCompRelease"),	    _MENU_DECOMPRESS_MK_FOLDER },
		{ wxT("m_menuComp_SelDirCompRelease"),	    _MENU_DECOMPRESS_SEL_DIR },
		{ wxT("m_viewMenu_FullScr"),	            _MENU_VIEW_FULLSCREEN },
		{ wxT("m_viewMenu_Window_0"),	            _MENU_VIEW_WINDOW_SINGLE },
		{ wxT("m_viewMenu_Window_1"),	            _MENU_VIEW_WINDOW_VERT },
		{ wxT("m_viewMenu_Window_2"),	            _MENU_VIEW_WINDOW_HORZ },
		{ wxT("m_viewMenu_ToolBar"),	            _MENU_VIEW_TOOLBAR },
		{ wxT("m_viewMenu_ShortCut"),	            _MENU_VIEW_SHORTCUT },
		{ wxT("m_viewMenu_Column_0"),	            _MENU_VIEW_COLUMN_0 },
		{ wxT("m_viewMenu_Column_1"),	            _MENU_VIEW_COLUMN_1 },
		{ wxT("m_viewMenu_Column_2"),	            _MENU_VIEW_COLUMN_2 },
		{ wxT("m_viewMenu_Column_3"),	            _MENU_VIEW_COLUMN_3 },
		{ wxT("m_viewMenu_Column_4"),	            _MENU_VIEW_COLUMN_4 },
		{ wxT("m_viewMenu_Column_5"),	            _MENU_VIEW_COLUMN_5 },
		{ wxT("m_viewMenu_Column_6"),	            _MENU_VIEW_COLUMN_6 },
		{ wxT("m_viewMenu_Column_7"),	            _MENU_VIEW_COLUMN_7 },
		{ wxT("m_viewMenu_Column_8"),	            _MENU_VIEW_COLUMN_8 },
		{ wxT("m_viewMenu_Column_9"),	            _MENU_VIEW_COLUMN_9 },
		{ wxT("m_viewMenu_DefaultFile"),	        _MENU_VIEW_FILE_DEFAULT },
		{ wxT("m_viewMenu_HiddenFile"),	            _MENU_VIEW_FILE_HIDDEN },
		{ wxT("m_viewMenu_AllFile"),	            _MENU_VIEW_FILE_ALL },
		{ wxT("m_viewMenu_SortName"),	            _MENU_VIEW_SORT_DEFAULT },
		{ wxT("m_viewMenu_SortTime"),	            _MENU_VIEW_SORT_TIME },
		{ wxT("m_viewMenu_SortSize"),	            _MENU_VIEW_SORT_SIZE },
		{ wxT("m_viewMenu_SubDir"),	                _MENU_VIEW_SUBDIR },
		{ wxT("m_favoriteMenu_Add"),	            _MENU_FAVORITE_ITEM_ADD },
		{ wxT("m_favoriteMenu_Manager"),	        _MENU_FAVORITE_MANAGER },
		{ wxT("m_toolMenu_ENV"),	                _MENU_TOOL_ENVIRONMENT },
		{ wxT("m_toolMenu_CurSave"),	            _MENU_TOOL_SAVECONFIG },
		{ wxT("m_helpMenu_thisProgram"),	        _MENU_HELP_THIS_PROGRAM }
	};

	for(int i = 0; i < WXSIZEOF(mList); i++)
	{
		int iMenuID = XRCID(mList[i].strMenuID);
		_MENU_EVENT_TYPE mType = mList[i].mType;

		std::unordered_map<int, _MENU_EVENT_TYPE>::value_type val(iMenuID, mType);
		m_menuMap.insert(val);
	}

	theMenuOperation->SetEventHandler(this);
}

void CEventHandler::OnMenuEvent(wxCommandEvent& event)
{
	int _iMenuID = event.GetId();
	DoMenuOperation(_iMenuID);
}

void CEventHandler::OnMenuEventUpdate(wxUpdateUIEvent& event)
{
	int _iupdateID = event.GetId();
	//윈도우창
	if ((_iupdateID == XRCID("m_viewMenu_Window_0")) ||	//단일창
		(_iupdateID == XRCID("m_viewMenu_Window_1")) ||	//수직창
		(_iupdateID == XRCID("m_viewMenu_Window_2")) ||	//수평창
		(_iupdateID == XRCID("m_viewMenu_Column_0")) ||	//컬럼자동
		(_iupdateID == XRCID("m_viewMenu_Column_1")) ||	//컬럼1
		(_iupdateID == XRCID("m_viewMenu_Column_2")) ||	//컬럼2
		(_iupdateID == XRCID("m_viewMenu_Column_3")) ||	//컬럼3
		(_iupdateID == XRCID("m_viewMenu_Column_4")) ||	//컬럼4
		(_iupdateID == XRCID("m_viewMenu_Column_5")) ||	//컬럼5
		(_iupdateID == XRCID("m_viewMenu_Column_6")) ||	//컬럼6
		(_iupdateID == XRCID("m_viewMenu_Column_7")) ||	//컬럼7
		(_iupdateID == XRCID("m_viewMenu_Column_8")) ||	//컬럼8
		(_iupdateID == XRCID("m_viewMenu_Column_9")))	//컬럼8
	{
		event.Check(false);
		//윈도우 스타일
		int iViewStyle = theJsonConfig->GetSplitStyle();
		wxString strMenuID = "m_viewMenu_Window_" + wxString::Format("%d", iViewStyle);
		if (_iupdateID == XRCID(strMenuID))
			event.Check(true);

		//컬럼
		int iColumnCnt = theJsonConfig->GetColumnCount();
		strMenuID = "m_viewMenu_Column_" + wxString::Format("%d", iColumnCnt);

		if (_iupdateID == XRCID(strMenuID))
			event.Check(true);
	}
	else if ((_iupdateID == XRCID("m_viewMenu_DefaultFile")) ||
	    	 (_iupdateID == XRCID("m_viewMenu_HiddenFile")) ||
		     (_iupdateID == XRCID("m_viewMenu_SystemFile")) ||
		     (_iupdateID == XRCID("m_viewMenu_AllFile")))

	{
		event.Check(false);
		if (theJsonConfig->IsViewAllFile())
		{
			if (_iupdateID == XRCID("m_viewMenu_AllFile"))
				event.Check(true);
			else
				event.Check(false);
		}
		else
		{
			if (_iupdateID != XRCID("m_viewMenu_AllFile"))
			{
				event.Check(true);

				if (_iupdateID == XRCID("m_viewMenu_HiddenFile"))
				{
					if (!theJsonConfig->IsViewHiddenFile())
						event.Check(false);
				}

				if (_iupdateID == XRCID("m_viewMenu_SystemFile"))
				{
					if (!theJsonConfig->IsViewSystemFile())
						event.Check(false);
				}
			}
		}
	}
	else if ((_iupdateID == XRCID("m_viewMenu_SortName")) ||
	    	 (_iupdateID == XRCID("m_viewMenu_SortTime")) ||
		     (_iupdateID == XRCID("m_viewMenu_SortSize")))
	{
		typedef struct _stSORTTYPE {
			wxString strType;
		} SORTTYPE;

		SORTTYPE _stSortType[] = {
			wxT("Name"),
			wxT("Time"),
			wxT("Size"),
		};

		event.Check(false);
		int iSortType = theJsonConfig->GetSortType();

		wxString strSortMenu = wxT("m_viewMenu_Sort") + _stSortType[iSortType].strType;
		if(_iupdateID == XRCID(strSortMenu))
			event.Check(true);
	}
}

void CEventHandler::DoMenuOperation(int iMenuID)
{
	std::unordered_map<int, _MENU_EVENT_TYPE>::const_iterator fIter = m_menuMap.find(iMenuID);
	if(fIter == m_menuMap.end())
		return;

	theMenuOperation->MenuOperationExecution(iMenuID, fIter->second);
}

void CEventHandler::LoadBookmark(bool bFromFavoriteManager)
{
	wxMenuBar* menuBar = m_pMainFrame->GetMenuBar();
	wxMenu *pFavoriteMenu = menuBar->GetMenu(menuBar->FindMenu(theMsgManager->GetMessage(wxT("MSG_TOOLBAR_FAVORITE"))));

	if (bFromFavoriteManager)
	{
		//기존 추가되었던 메뉴 삭제
		int iMenuCount = pFavoriteMenu->GetMenuItemCount();
		for (int i = 2; i < iMenuCount; i++)
		{
			wxMenuItem *pMenuItem = pFavoriteMenu->FindItemByPosition(2);
			pFavoriteMenu->Destroy(pMenuItem);
		}
	}

	pFavoriteMenu->AppendSeparator();
	theBookmark->CreateBookmarkMenu(pFavoriteMenu);
}

void CEventHandler::SetBookmarkDropdown()
{
	if (theBookmark->GetBookmarkSize() > 0)
	{
		wxToolBar* pToolBar = m_pMainFrame->GetToolBar();

		wxMenu* pFavoriteMenu = new wxMenu();
		theBookmark->CreateBookmarkMenu(pFavoriteMenu);

		pToolBar->SetDropdownMenu(TOOLBAR_ID_BOOKMARK, pFavoriteMenu);
	}
}

void CEventHandler::RegisterToolbarEvent()
{
	this->Connect(TOOLBAR_ID_REFRESH, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CEventHandler::OnToolbarEvent));
	this->Connect(TOOLBAR_ID_FILE_EDIT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CEventHandler::OnToolbarEvent));
	this->Connect(TOOLBAR_ID_FIND, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CEventHandler::OnToolbarEvent));
	this->Connect(TOOLBAR_ID_DIR_MANAGER, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CEventHandler::OnToolbarEvent));
	this->Connect(TOOLBAR_ID_BOOKMARK, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CEventHandler::OnToolbarEvent));
	this->Connect(TOOLBAR_ID_ENV_SETTING, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CEventHandler::OnToolbarEvent));
	this->Connect(TOOLBAR_ID_ENV_SAVE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CEventHandler::OnToolbarEvent));
	this->Connect(TOOLBAR_ID_THIS_PROGRAM, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CEventHandler::OnToolbarEvent));
}

void CEventHandler::OnToolbarEvent(wxCommandEvent& event)
{
	int id = event.GetId();
	theMenuOperation->ExecuteToolbar(id);
}

void CEventHandler::OnBookmarkSelected(wxCommandEvent& event)
{
	int iMenuID = event.GetId();
	theMenuOperation->FavoriteMenu_ItemSelected(iMenuID);
}
