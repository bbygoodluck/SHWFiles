#include "ginc.h"
#include "MainFrame.h"
#include "TabManager.h"
#include "ViewPanel.h"
#include "./view/ListView.h"
#include "EventHandler.h"
#include "MenuOperation.h"

#include "./dialog/DlgaddDir.h"
#include "./dialog/GenericDirDialog.h"
#include "./dialog/DlgFileOperation.h"
#include "./dialog/DlgFind.h"
#include "./dialog/DirectoryManagerDlg.h"
#include "./dialog/DlgCompress.h"
#include "./dialog/DlgDeCompress.h"
#include "./dialog/DlgFavoriteManager.h"
#include "./dialog/DlgEnv.h"
#include "./dialog/wxMozillaLikeAboutBoxDialog.h"
#include "./dialog/header_png.h"

#ifdef __WXMSW__
static int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM dwData)
{
	switch (uMsg)
	{
		case BFFM_INITIALIZED:
		{
			//캡션에서 Help 삭제
			DWORD dwStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
			::SetWindowLong(hWnd, GWL_EXSTYLE, dwStyle & ~WS_EX_CONTEXTHELP);

			HWND hStatic = GetDlgItem(hWnd, 0x00003742);//14146); // 0x00003742(Static Control)
			if(hStatic)
			{
				::EnableWindow(hStatic, FALSE);
				::ShowWindow(hStatic, SW_HIDE);
			}

			//지정된 폴더가 선택되도록 한다.
			::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, dwData);

			//대화상자가 중앙에 오도록 한다.
			RECT rc;
			GetClientRect(hWnd, &rc);
			::SetWindowPos(hWnd
				    	 , NULL
					     , (GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2
					     , (GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2
					     , 0
					     , 0
					     , SWP_NOZORDER | SWP_NOSIZE);

			//Status text에 3D 테두리 첨가
			HWND hwndLabel = ::GetDlgItem(hWnd, 0x3743);
			if (IsWindow(hwndLabel))
			{
				TCHAR szClass[MAX_PATH] = {0};
				::GetClassName(hwndLabel, szClass, MAX_PATH);
				if (lstrcmpi(szClass, __TEXT("static")))
					break;
			}
			else
				break;

			dwStyle = ::GetWindowLong(hwndLabel, GWL_EXSTYLE);
			::SetWindowLong(hwndLabel, GWL_EXSTYLE, dwStyle | WS_EX_STATICEDGE);
			::SetWindowPos(hwndLabel, NULL, 0, 0, 0, 0,SWP_NOSIZE | SWP_NOMOVE | SWP_DRAWFRAME);
		}
			break;

		case BFFM_SELCHANGED:
		{
			TCHAR szText[MAX_PATH] = {0};
			::SHGetPathFromIDList(reinterpret_cast<LPITEMIDLIST>(lParam), szText);
			::SendMessage(hWnd, BFFM_SETSTATUSTEXT, 0, reinterpret_cast<LPARAM>(szText));
		}
			break;

		case BFFM_VALIDATEFAILED:
			break;
	}

	return 0;
}
#endif


std::unique_ptr<CMenuOperation> CMenuOperation::m_pInstance(nullptr);
CMenuOperation* CMenuOperation::Get()
{
	if (m_pInstance == nullptr)
		m_pInstance.reset(new CMenuOperation());

	m_pInstance->Init();
	return m_pInstance.get();
}

void CMenuOperation::Init()
{
	typedef struct stMenuList {
		wxString strMenuID;
		std::function<void()> funcPtr;
	} MENU_FNPTRLIST;

	MENU_FNPTRLIST stMenufnList[] = {
		{ wxT("m_fMenu_Execute"),	                std::bind(&CMenuOperation::FileMenu_Execution            , this) },
		{ wxT("m_fMenu_NewTab"),	                std::bind(&CMenuOperation::FileMenu_NewTab               , this) },
		{ wxT("m_fMenu_Copy"),	                    std::bind(&CMenuOperation::FileMenu_Copy	             , this) },
		{ wxT("m_fMenu_Move"),	                    std::bind(&CMenuOperation::FileMenu_Move                 , this) },
		{ wxT("m_fMenu_Rename"),	                std::bind(&CMenuOperation::FileMenu_Rename               , this) },
		{ wxT("m_fMenu_MakeFolder"),	            std::bind(&CMenuOperation::FileMenu_CreateFolder         , this) },
		{ wxT("m_fMenu_FileEdit"),	                std::bind(&CMenuOperation::FileMenu_Edit                 , this) },
		{ wxT("m_fMenu_goTrash"),	                std::bind(&CMenuOperation::FileMenu_Trash                , this) },
		{ wxT("m_fMenu_goDel"),	                    std::bind(&CMenuOperation::FileMenu_CompletionDel        , this) },
		{ wxT("m_fMenu_CMD"),	                    std::bind(&CMenuOperation::FileMenu_ExecCMD              , this) },
		{ wxT("m_fMenu_Exit"),	                    std::bind(&CMenuOperation::FileMenu_Terminate            , this) },
		{ wxT("m_eMenu_Release"),	                std::bind(&CMenuOperation::EditMenu_AllRelease	         , this) },
		{ wxT("m_eMenu_SelAll"),	                std::bind(&CMenuOperation::EditMenu_AllSelect            , this ) },
		{ wxT("m_eMenu_Copy"),	                    std::bind(&CMenuOperation::EditMenu_Copy_UseClipboard    , this ) },
		{ wxT("m_eMenu_Cut"),	                    std::bind(&CMenuOperation::EditMenu_Move_UseClipboard    , this ) },
		{ wxT("m_eMenu_Paste"),        	            std::bind(&CMenuOperation::EditMenu_Paste_UseClipboard   , this ) },
		{ wxT("m_eMenu_Search"),	                std::bind(&CMenuOperation::EditMenu_FileFind             , this ) },
		{ wxT("m_eMenu_ContextMenu"),	            std::bind(&CMenuOperation::EditMenu_ViewContextMenu      , this ) },
		{ wxT("m_pMenu_PathMng"),	                std::bind(&CMenuOperation::PathMenu_DirManager           , this ) },
		{ wxT("m_pMenu_Refresh"),	                std::bind(&CMenuOperation::PathMenu_Refresh              , this ) },
		{ wxT("m_pMenu_GotoDirDirectly"), 	        std::bind(&CMenuOperation::PathMenu_GotoDirDirectly      , this ) },
//		{ wxT("m_menuComp_View"),	                _MENU_COMPRESS_FILE_VIEW },
		{ wxT("m_menuComp_Progress"),	            std::bind(&CMenuOperation::CompressMenu_DoCompress       , this ) },
		{ wxT("m_menuComp_CurrentDirCompRelease"),	std::bind(&CMenuOperation::CompressMenu_DeCompressCurDir , this ) },
		{ wxT("m_menuComp_MakeDirCompRelease"),	    std::bind(&CMenuOperation::CompressMenu_DeCompressMkDir  , this ) },
		{ wxT("m_menuComp_SelDirCompRelease"),	    std::bind(&CMenuOperation::CompressMenu_DeCompressSelDir , this ) },
		{ wxT("m_viewMenu_FullScr"),	            std::bind(&CMenuOperation::ViewMenu_FullScreen           , this ) },
		{ wxT("m_viewMenu_Window_0"),	            std::bind(&CMenuOperation::ViewMenu_Window_Single        , this ) },
		{ wxT("m_viewMenu_Window_1"),	            std::bind(&CMenuOperation::ViewMenu_Window_Vert          , this ) },
		{ wxT("m_viewMenu_Window_2"),	            std::bind(&CMenuOperation::ViewMenu_Window_Horz          , this ) },
		{ wxT("m_viewMenu_ToolBar"),	            std::bind(&CMenuOperation::ViewMenu_Toolbar              , this ) },
		{ wxT("m_viewMenu_ShortCut"),	            std::bind(&CMenuOperation::ViewMenu_Shortcut             , this ) },
		{ wxT("m_viewMenu_Column_0"),	            std::bind(&CMenuOperation::ViewMenu_Column_0             , this ) },
		{ wxT("m_viewMenu_Column_1"),	            std::bind(&CMenuOperation::ViewMenu_Column_1             , this ) },
		{ wxT("m_viewMenu_Column_2"),	            std::bind(&CMenuOperation::ViewMenu_Column_2             , this ) },
		{ wxT("m_viewMenu_Column_3"),	            std::bind(&CMenuOperation::ViewMenu_Column_3             , this ) },
		{ wxT("m_viewMenu_Column_4"),	            std::bind(&CMenuOperation::ViewMenu_Column_4             , this ) },
		{ wxT("m_viewMenu_Column_5"),	            std::bind(&CMenuOperation::ViewMenu_Column_5             , this ) },
		{ wxT("m_viewMenu_Column_6"),	            std::bind(&CMenuOperation::ViewMenu_Column_6             , this ) },
		{ wxT("m_viewMenu_Column_7"),	            std::bind(&CMenuOperation::ViewMenu_Column_7             , this ) },
		{ wxT("m_viewMenu_Column_8"),	            std::bind(&CMenuOperation::ViewMenu_Column_8             , this ) },
		{ wxT("m_viewMenu_Column_9"),	            std::bind(&CMenuOperation::ViewMenu_Column_9             , this ) },
		{ wxT("m_viewMenu_DefaultFile"),	        std::bind(&CMenuOperation::ViewMenu_FileView_Default     , this ) },
		{ wxT("m_viewMenu_HiddenFile"),	            std::bind(&CMenuOperation::ViewMenu_FileView_Hidden      , this ) },
		{ wxT("m_viewMenu_AllFile"),	            std::bind(&CMenuOperation::ViewMenu_FileView_All         , this ) },
		{ wxT("m_viewMenu_SortName"),	            std::bind(&CMenuOperation::ViewMenu_SortDefault          , this ) },
		{ wxT("m_viewMenu_SortTime"),	            std::bind(&CMenuOperation::ViewMenu_SortTime             , this ) },
		{ wxT("m_viewMenu_SortSize"),	            std::bind(&CMenuOperation::ViewMenu_SortSize             , this ) },
		{ wxT("m_viewMenu_SubDir"),	                std::bind(&CMenuOperation::ViewMenu_SubDir               , this ) },
		{ wxT("m_favoriteMenu_Add"),	            std::bind(&CMenuOperation::FavoriteMenu_Add              , this ) },
		{ wxT("m_favoriteMenu_Manager"),	        std::bind(&CMenuOperation::FavoriteMenu_Manager          , this ) },
		{ wxT("m_toolMenu_ENV"),	                std::bind(&CMenuOperation::ToolMenu_Setting              , this ) },
		{ wxT("m_toolMenu_CurSave"),	            std::bind(&CMenuOperation::ToolMenu_SaveSetting          , this ) },
		{ wxT("m_helpMenu_thisProgram"),	        std::bind(&CMenuOperation::HelpMenu_ThisProgram          , this ) },
	};

	for(unsigned int i = 0; i < WXSIZEOF(stMenufnList); i++)
	{
		int iMenuID = XRCID(stMenufnList[i].strMenuID);

		std::unordered_map< int, std::function<void()> >::value_type val(iMenuID, stMenufnList[i].funcPtr);
		m_menuFnPtrMap.insert(val);
	}
}

void CMenuOperation::SetEventHandler(CEventHandler * pHandler)
{
	m_pEventHandler = pHandler;
}

void CMenuOperation::MenuOperationExecution(int iMenuID, _MENU_EVENT_TYPE _mType)
{
	std::unordered_map< int, std::function<void()> >::const_iterator fIter = m_menuFnPtrMap.find(iMenuID);
	if(fIter == m_menuFnPtrMap.end())
		return;

	auto menuFnPtr = fIter->second;
	(menuFnPtr)();
}

//파일메뉴
void CMenuOperation::FileMenu_Execution()
{
	wxMessageBox(theMsgManager->GetMessage(wxT("MSG_FILE_MENU_EXEC")), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
}

void CMenuOperation::FileMenu_NewTab()
{
	theSplitterManager->AddTabPage();
}

void CMenuOperation::FileMenu_Copy()
{
	DoFileOperation(FILE_OP_COPY);
}

void CMenuOperation::FileMenu_Move()
{
	DoFileOperation(FILE_OP_CUT);
}

void CMenuOperation::DoFileOperation(FILE_OPERATION fop, bool IsUseClipboard)
{
	wxString strTargetPath = IsUseClipboard ? theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCurrentViewPath() :
								              GetDirectorySelect();


	std::list<wxString> lstSrc;
	if(!IsUseClipboard)
		theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetFileOperationSrc(lstSrc);
	else
	{
		fop = theClipboard->GetFileOperation() == _MENU_EDIT_COPY_CLIPBOARD ? FILE_OP_COPY : FILE_OP_CUT;
		theClipboard->GetData(lstSrc);
	}

	FileOperationDlg dlgFileOP(_gMainFrame);
	dlgFileOP.SetOperationItems(lstSrc, strTargetPath, fop);
	dlgFileOP.ShowModal();
	dlgFileOP.Destroy();

	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->ClearSelectedInfoOfView();
}

wxString CMenuOperation::GetDirectorySelect()
{
	wxString strTargetPath(wxT(""));

	if(theJsonConfig->GetSplitStyle() == WINDOW_SPLIT_NONE)
	{
		wxString strPath = theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCurrentViewPath();
		bool bWindowSelectDir = false;
	#ifdef __WXMSW__
		if(theJsonConfig->GetUseSHBrowseForFolder())
		{
			bWindowSelectDir = true;
			LPITEMIDLIST pidlBrowse;
			TCHAR szDir[MAX_PATH] = {0, };

			BROWSEINFO brInfo;
			wxZeroMemory(brInfo);

			brInfo.hwndOwner = _gMainFrame->GetHWND();
			brInfo.pidlRoot = NULL;
			brInfo.pszDisplayName = szDir;
			brInfo.lpszTitle = _T("Select copy or move target directory");
			brInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_VALIDATE;
			brInfo.lpfn = BrowseCallbackProc;
			brInfo.lParam = (LPARAM)strPath.wc_str();

			pidlBrowse = ::SHBrowseForFolder(&brInfo);
			if(pidlBrowse != NULL)
			{
				::SHGetPathFromIDList(pidlBrowse, szDir);
				strTargetPath = wxString::Format(wxT("%s"), szDir);
			}
		}
	#endif

		if(!bWindowSelectDir)
		{
			wxGenericDirDialog dlgTargetPath(_gMainFrame, wxT("Select target directory"), strPath);
			dlgTargetPath.SetSize(wxSize(800, 600));
			dlgTargetPath.CenterOnScreen();

			if(dlgTargetPath.ShowModal() == wxID_OK)
				strTargetPath = dlgTargetPath.GetPath();

			dlgTargetPath.Destroy();
		}
	}
	else
		strTargetPath = theSplitterManager->GetAnotherTab()->GetActiveViewPanel()->GetCurrentViewPath();

	return strTargetPath;
}

void CMenuOperation::FileMenu_Rename()
{
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->SendEventToListView(_MENU_FILE_RENAME);
}

void CMenuOperation::FileMenu_CreateFolder()
{
	wxString strPath(theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCurrentViewPath());

	DlgAddDir dlgDir(_gMainFrame);
	dlgDir.SetCurrentPath(strPath);
	dlgDir.ShowModal();
	dlgDir.Destroy();
}

void CMenuOperation::FileMenu_Edit()
{
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->SendEventToListView(_MENU_FILE_EDIT);
}

void CMenuOperation::FileMenu_Trash()
{
//	wxString strRecycleBin = theSPFolder->GetSpecialFolder(CSIDL_BITBUCKET);

//	wxString strSrc = wxT("C:\\MyData\\Programming\\MyProject\\C++\\CodeBlocks\\wxWidgets\\MyWorks\\aaa\\MenuOperation.cpp");
//	wxCopyFile(strSrc, strRecycleBin);
	DoDeleteTrash(_MENU_FILE_DEL_TRASH);
}

void CMenuOperation::FileMenu_CompletionDel()
{
	DoDeleteTrash(_MENU_FILE_DEL_COMP);
}

void CMenuOperation::DoDeleteTrash(_MENU_EVENT_TYPE mType)
{
	std::list<wxString> lstDatas;
	bool bTrash = (mType == _MENU_FILE_DEL_TRASH) ? true : false;

	if(!theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetDeleteItems(lstDatas, bTrash))
		return;

	int iRet = 0;
	if(!CLocalFileSystem::RecursiveDelete(lstDatas, _gMainFrame, bTrash, iRet))
	{
		wxMessageBox(wxT("Failed to file delete"), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		return;
	}

	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->ClearSelectedInfoOfView(true);
}

void CMenuOperation::FileMenu_ExecCMD()
{
#ifdef __WXMSW__
	wxString strCurrentPath(theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCurrentViewPath());
	wxString strArgs(wxT(" /k cd "));

	strArgs += strCurrentPath;

	ShellExecute(0, wxT("open"), wxT("C:\\Windows\\System32\\CMD.exe"), strArgs, 0, SW_SHOW);
#else

#endif
}

void CMenuOperation::FileMenu_Terminate()
{
	wxTheApp->GetTopWindow()->Close();
}

//편집메뉴
void CMenuOperation::EditMenu_AllRelease()
{
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->SendEventToListView(_MENU_EDIT_ALL_RELEASE);
}

void CMenuOperation::EditMenu_AllSelect()
{
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->SendEventToListView(_MENU_EDIT_ALL_SELECT);
}

void CMenuOperation::EditMenu_Copy_UseClipboard()
{
	SetFileOperationClipboardData(false);
}

void CMenuOperation::EditMenu_Move_UseClipboard()
{
	SetFileOperationClipboardData(true);
}

void CMenuOperation::SetFileOperationClipboardData(bool IsCut)
{
	std::list<wxString> lstSrc;
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetFileOperationSrc(lstSrc, IsCut);

	if(lstSrc.size() > 0)
	{
		theClipboard->Clear();
		theClipboard->SetFileOperation(IsCut ? _MENU_EDIT_MOVE_CLIPBOARD : _MENU_EDIT_COPY_CLIPBOARD);
		theClipboard->AddData(lstSrc);
		theClipboard->CopyToClipboard();
	}
}

void CMenuOperation::EditMenu_Paste_UseClipboard()
{
	DoFileOperation(FILE_OP_NONE, true);
}

void CMenuOperation::EditMenu_FileFind()
{
	wxString strCurrentPath(theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCurrentViewPath());

	DlgFind findDlg(_gMainFrame);
	findDlg.setSearchDir(strCurrentPath);
	findDlg.ShowModal();
	findDlg.Destroy();
}

void CMenuOperation::EditMenu_ViewContextMenu()
{
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->SendEventToListView(_MENU_EDIT_CONTEXT_MENU);
}

//경로메뉴
void CMenuOperation::PathMenu_DirManager()
{
	wxString strCurrPath(theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCurrentViewPath());
	wxString strDriveName = theCommonUtil->GetVolume(strCurrPath);

	wxString strTreeDataPath = theCommonUtil->GetWorkDir() + SLASH + wxT("treedata");
	if (!wxDirExists(strTreeDataPath))
		wxMkDir(strTreeDataPath);

	wxString strSHCDFilePath = strTreeDataPath + SLASH + strCurrPath.Left(1) + wxT(".SCD");
	DirectoryManagerDlg dirMng(_gMainFrame);
	dirMng.SetSCDFileInfo(strSHCDFilePath, strCurrPath);
	dirMng.ShowModal();
	dirMng.Destroy();
}

void CMenuOperation::PathMenu_Refresh()
{
	wxString strCurrPath(theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCurrentViewPath());
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->ReloadPathOfView(strCurrPath);
}

void CMenuOperation::PathMenu_GotoDirDirectly()
{
	wxString strCurrPath = theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCurrentViewPath();

	wxGenericDirDialog dlgTargetPath(_gMainFrame, wxT("Select target directory"), strCurrPath);
	dlgTargetPath.SetSize(wxSize(600, 450));
	dlgTargetPath.CenterOnScreen();

	if(dlgTargetPath.ShowModal() == wxID_OK)
	{
		wxString strTargetDir(dlgTargetPath.GetPath());
		theSplitterManager->GetActiveTab()->GetActiveViewPanel()->ReloadPathOfView(strTargetDir);
	}

	dlgTargetPath.Destroy();
}

//압축
void CMenuOperation::CompressMenu_DoCompress()
{
	std::vector<wxString> vecItems;
	wxString strCompressedFile(wxT(""));

	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCompressItems(vecItems, strCompressedFile);

	if(vecItems.size() == 0 || strCompressedFile.IsEmpty())
	{
		wxMessageBox(wxT("The compress item size 0 or target compress file is not setting"));
		return;
	}
	//선택파일 해제
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->ClearSelectedInfoOfView(false);

	DlgCompress dlg(_gMainFrame);
	dlg.SetCompressInfo(vecItems, strCompressedFile, wxT("zip"));
	dlg.ShowModal();
	dlg.Destroy();
}

void CMenuOperation::CompressMenu_DeCompressCurDir()
{
	DoDecompress(_MENU_DECOMPRESS_CURRENT_DIR);
}

void CMenuOperation::CompressMenu_DeCompressMkDir()
{
	DoDecompress(_MENU_DECOMPRESS_MK_FOLDER);
}

void CMenuOperation::CompressMenu_DeCompressSelDir()
{
	DoDecompress(_MENU_DECOMPRESS_SEL_DIR);
}

void CMenuOperation::DoDecompress(_MENU_EVENT_TYPE _menuType)
{
	wxString strSelItem = theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCompressedFileForDecompress();

	wxString strExt = theCommonUtil->GetExt(strSelItem);
	if(!theCompress->IsCompressedFile(strExt))
	{
		wxMessageBox(wxT("The selected file is not compressed file"), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
		return;
	}

	wxString strCurrentPath = theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCurrentViewPath();

	if(_menuType == _MENU_DECOMPRESS_SEL_DIR)
	{
		wxGenericDirDialog dlgTargetPath(_gMainFrame, wxT("Select target directory"), strCurrentPath);
		dlgTargetPath.SetSize(wxSize(600, 450));
		dlgTargetPath.CenterOnScreen();

		if(dlgTargetPath.ShowModal() != wxID_OK)
			return;

		strCurrentPath = dlgTargetPath.GetPath();
		dlgTargetPath.Destroy();
	}

	DlgDeCompress dlg(_gMainFrame);
	dlg.SetDecompressInfo(strSelItem, strCurrentPath, _menuType);
	dlg.ShowModal();
	dlg.Destroy();
}
//보기
void CMenuOperation::ViewMenu_FullScreen()
{
	_gMainFrame->Maximize();
}

void CMenuOperation::ViewMenu_Window_Single()
{
	ChangeViewStyle(WINDOW_SPLIT_NONE);
}

void CMenuOperation::ViewMenu_Window_Vert()
{
	ChangeViewStyle(WINDOW_SPLIT_VERT);
}

void CMenuOperation::ViewMenu_Window_Horz()
{
	ChangeViewStyle(WINDOW_SPLIT_HORZ);
}

void CMenuOperation::ChangeViewStyle(int iViewStyle)
{
	theJsonConfig->SetChangeGlobalVal(wxT("SplitWindowStyle"), iViewStyle);
	theSplitterManager->DoSplitter();
}

void CMenuOperation::ViewMenu_Toolbar()
{

}

void CMenuOperation::ViewMenu_Shortcut()
{

}

void CMenuOperation::ViewMenu_Column_0()
{
	ChangeViewColumn(VIEW_COLUMN_AUTO);
}

void CMenuOperation::ViewMenu_Column_1()
{
	ChangeViewColumn(VIEW_COLUMN_ONE);
}

void CMenuOperation::ViewMenu_Column_2()
{
	ChangeViewColumn(VIEW_COLUMN_TWO);
}

void CMenuOperation::ViewMenu_Column_3()
{
	ChangeViewColumn(VIEW_COLUMN_THREE);
}

void CMenuOperation::ViewMenu_Column_4()
{
	ChangeViewColumn(VIEW_COLUMN_FOUR);
}

void CMenuOperation::ViewMenu_Column_5()
{
	ChangeViewColumn(VIEW_COLUMN_FIVE);
}

void CMenuOperation::ViewMenu_Column_6()
{
	ChangeViewColumn(VIEW_COLUMN_SIX);
}

void CMenuOperation::ViewMenu_Column_7()
{
	ChangeViewColumn(VIEW_COLUMN_SEVEN);
}

void CMenuOperation::ViewMenu_Column_8()
{
	ChangeViewColumn(VIEW_COLUMN_EIGHT);
}

void CMenuOperation::ViewMenu_Column_9()
{
	ChangeViewColumn(VIEW_COLUMN_NINE);
}

void CMenuOperation::ChangeViewColumn(COLUMN_VIEW_OPTION columnOP)
{
	theJsonConfig->SetChangeGlobalVal(wxT("DispColumn"), columnOP);
	CTabManager* pTabManager = theSplitterManager->GetActiveTab();

	DoChangeViewColumn(pTabManager);
	if(theJsonConfig->GetSplitStyle() != WINDOW_SPLIT_NONE)
	{
		CTabManager* pAnotherTabManager = theSplitterManager->GetAnotherTab();
		DoChangeViewColumn(pAnotherTabManager);
	}
}

void CMenuOperation::DoChangeViewColumn(CTabManager* pTabManager)
{
	if(pTabManager)
	{
		int iPageCountInTab = pTabManager->GetPageCount();
		int iPageIndex = 0;

		while(iPageIndex < iPageCountInTab)
		{
			((CViewPanel *)pTabManager->GetPage(iPageIndex))->ChangeViewColumn();
			iPageIndex++;
		}
	}
}

void CMenuOperation::ViewMenu_FileView_Default()
{
	ChangeFileList(_MENU_VIEW_FILE_DEFAULT);
}

void CMenuOperation::ViewMenu_FileView_Hidden()
{
	ChangeFileList(_MENU_VIEW_FILE_HIDDEN);
}

void CMenuOperation::ViewMenu_FileView_All()
{
	ChangeFileList(_MENU_VIEW_FILE_ALL);
}

void CMenuOperation::ChangeFileList(_MENU_EVENT_TYPE mType)
{
	int iFileList = mType - _MENU_VIEW_FILE_DEFAULT;
	FILE_VIEW_OPTION enumfvo = _gFileViewOPEnum[iFileList];

	theJsonConfig->SetViewFileKind(enumfvo);

	CTabManager* pTabManager = theSplitterManager->GetActiveTab();
	DoChangeFileList(pTabManager);

	if(theJsonConfig->GetSplitStyle() != WINDOW_SPLIT_NONE)
	{
		CTabManager* pAnotherTabManager = theSplitterManager->GetAnotherTab();
		DoChangeFileList(pAnotherTabManager);
	}
}

void CMenuOperation::DoChangeFileList(CTabManager* pTabManager)
{
	if(pTabManager)
	{
		int iPageCountInTab = pTabManager->GetPageCount();
		int iPageIndex = 0;

		while(iPageIndex < iPageCountInTab)
		{
			((CViewPanel *)pTabManager->GetPage(iPageIndex))->ReloadPathOfView();
			iPageIndex++;
		}
	}
}

void CMenuOperation::ViewMenu_SortDefault()
{
	SetSort(VIEW_SORT_DEFAULT);
}

void CMenuOperation::ViewMenu_SortTime()
{
	SetSort(VIEW_SORT_TIME);
}

void CMenuOperation::ViewMenu_SortSize()
{
	SetSort(VIEW_SORT_SIZE);
}

void CMenuOperation::SetSort(VIEW_SORT sType)
{
	theJsonConfig->SetChangeGlobalVal(wxT("SortType"), sType);

	CTabManager* pTabManager = theSplitterManager->GetActiveTab();
	DoSort(pTabManager);

	if(theJsonConfig->GetSplitStyle() != WINDOW_SPLIT_NONE)
	{
		CTabManager* pAnotherTabManager = theSplitterManager->GetAnotherTab();
		DoSort(pAnotherTabManager);
	}
}

void CMenuOperation::DoSort(CTabManager* pTabManager)
{
	if(pTabManager)
	{
		int iPageCountInTab = pTabManager->GetPageCount();
		int iPageIndex = 0;

		while(iPageIndex < iPageCountInTab)
		{
			((CViewPanel *)pTabManager->GetPage(iPageIndex))->ChangeSort();
			iPageIndex++;
		}
	}
}

void CMenuOperation::ViewMenu_SubDir()
{
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->SendEventToListView(_MENU_VIEW_SUBDIR);
}
//즐겨찾기
void CMenuOperation::FavoriteMenu_Add()
{
	DoFavorite(_MENU_FAVORITE_ITEM_ADD);
}

void CMenuOperation::FavoriteMenu_Manager()
{
	DoFavorite(_MENU_FAVORITE_MANAGER);
}

void CMenuOperation::DoFavorite(_MENU_EVENT_TYPE mType)
{
	DlgFavoriteManager dlg(_gMainFrame);
	if (mType == _MENU_FAVORITE_ITEM_ADD)
	{
		wxString strCurrPath(theSplitterManager->GetActiveTab()->GetActiveViewPanel()->GetCurrentViewPath());
		dlg.SetAddPath(strCurrPath, true);
	}

	dlg.ShowModal();
	dlg.Destroy();

	m_pEventHandler->LoadBookmark(true);
}

void CMenuOperation::FavoriteMenu_ItemSelected(int iMenuID)
{
	wxString strBookmarkPath = theBookmark->GetBookmarkPath(iMenuID);
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->ReloadPathOfView(strBookmarkPath);
}

//도구
void CMenuOperation::ToolMenu_Setting()
{
	DlgEnv dlg(_gMainFrame);
	dlg.ShowModal();

	dlg.Destroy();
}

void CMenuOperation::ToolMenu_SaveSetting()
{
	int xPos = 0;
	int yPos = 0;

	int nCurrDisp = wxDisplay::GetFromWindow(_gMainFrame);

	wxDisplay disp(nCurrDisp);
	wxRect rcMonitor = disp.GetGeometry();

	wxTopLevelWindow* pTopWindow = (wxTopLevelWindow *)wxTheApp->GetTopWindow();
	pTopWindow->GetScreenPosition(&xPos, &yPos);
	wxRect rcScreen = pTopWindow->GetScreenRect();

	int iMaximized = _gMainFrame->IsMaximized() ? 1 : 0;

	if (rcMonitor.GetLeft() != 0)
		xPos = rcMonitor.GetLeft();

	int iWidth = rcScreen.GetWidth();
	int iHeight = rcScreen.GetHeight();

	theJsonConfig->SetChangeGlobalVal(wxT("Maximized"), iMaximized);
	theJsonConfig->SetChangeGlobalVal(wxT("DispMonitor"), nCurrDisp);
	theJsonConfig->SetChangeGlobalVal(wxT("XPos"), xPos);
	theJsonConfig->SetChangeGlobalVal(wxT("YPos"), yPos);
	theJsonConfig->SetChangeGlobalVal(wxT("Width"), iWidth);
	theJsonConfig->SetChangeGlobalVal(wxT("Height"), iHeight);

	if (theJsonConfig->IsSaveLastSession())
	{
		CTabManager* pCurrentTabManager = theSplitterManager->GetActiveTab();
		CTabManager* pAnotherTabManager = nullptr;

		if(theJsonConfig->GetSplitStyle() != WINDOW_SPLIT_NONE)
			pAnotherTabManager = theSplitterManager->GetAnotherTab();

		wxString strActiveKey(wxT(""));
		wxString strAnotherKey(wxT(""));

		bool bLeftTab = pCurrentTabManager == theSplitterManager->GetLeftTab() ? true : false;
		if(bLeftTab)
		{
			strActiveKey = wxT("LeftPath");
			strAnotherKey = wxT("RightPath");
		}
		else
		{
			strActiveKey = wxT("RightPath");
			strAnotherKey = wxT("LeftPath");
		}

		wxString strCurrentPath = pCurrentTabManager->GetActiveViewPanel()->GetCurrentViewPath();
		theJsonConfig->SetChangeGlobalVal(strActiveKey, strCurrentPath);

		if(pAnotherTabManager)
		{
			strCurrentPath = pAnotherTabManager->GetActiveViewPanel()->GetCurrentViewPath();
			theJsonConfig->SetChangeGlobalVal(strAnotherKey, strCurrentPath);
		}
	}

	theJsonConfig->SaveConfig();
	wxMessageBox(wxT("The Current settings have been saved."), PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
}

//도움말
void CMenuOperation::HelpMenu_ThisProgram()
{
	// Create About box
	wxMozillaLikeAboutBoxDialog * dlg = new wxMozillaLikeAboutBoxDialog(_gMainFrame);
	// Set application name
	dlg->SetAppName(PROGRAM_NAME);
	// Set application version
	dlg->SetVersion(PROGRAM_MAJOR_VER + wxT(".") + PROGRAM_MINOR_HI_VER + wxT(".") + PROGRAM_MINOR_LO_VER);
	// Set copyright message
	wxDateTime dt = wxDateTime::Now();
	int iYear = dt.GetYear();
	wxString strCopyRight = wxString::Format(theMsgManager->GetMessage(wxT("MSG_PROGRAMM_COPYRIGHT")), iYear);
	dlg->SetCopyright(strCopyRight);
	// Set header bitmap
	//	dlg->SetHeaderBitmap(wxGetBitmapFromMemory(header_png, sizeof(header_png)));

	wxString strBuildDisp(wxT(""));
	wxString strBuildInfo(wxMozillaLikeAboutBoxDialog::GetBuildInfo(wxMozillaLikeAboutBoxDialog::wxBUILDINFO_LONG).GetData());
	wxString strDeveloper(wxT("Developed by "));
	strDeveloper += theMsgManager->GetMessage(wxT("MSG_PROGRAMM_DEVELOPER"));

	strBuildInfo = wxString::Format(wxT("%s. \n%s"), strBuildInfo, strDeveloper);
	// Set build info message. This is optional step. If you don't specify build info message then
	// default one will be used
	dlg->SetCustomBuildInfo(wxT("Advanced File Manager ") + strBuildInfo);
	// Apply changes
	dlg->ApplyInfo();
	// Show dialog
	dlg->ShowModal();
	// Destroy dialog
	dlg->Destroy();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
void CMenuOperation::ExecuteToolbar(int id)
{
	switch(id)
	{
		case TOOLBAR_ID_REFRESH:
			PathMenu_Refresh();
			break;

		case TOOLBAR_ID_FILE_EDIT:
			FileMenu_Edit();
			break;

		case TOOLBAR_ID_FIND:
			EditMenu_FileFind();
			break;

		case TOOLBAR_ID_DIR_MANAGER:
			PathMenu_DirManager();
			break;

		case TOOLBAR_ID_BOOKMARK:
		{
			wxToolBar* pToolBar = _gMainFrame->GetToolBar();
			wxSize szToolBar = pToolBar->GetSize();

			int iPos = pToolBar->GetToolPos(TOOLBAR_ID_BOOKMARK);
			wxPoint pt = pToolBar->GetPosition();
			pt.x = (pt.x + 60) * iPos;
			pt.y = szToolBar.GetHeight();

			wxMenu menuFavorite;
			theBookmark->CreateBookmarkMenu(&menuFavorite);

			pToolBar->PopupMenu(&menuFavorite, pt);
		}
			break;

		case TOOLBAR_ID_ENV_SETTING:
			ToolMenu_Setting();
			break;

		case TOOLBAR_ID_ENV_SAVE:
			ToolMenu_SaveSetting();
			break;

		case TOOLBAR_ID_THIS_PROGRAM:
			HelpMenu_ThisProgram();
			break;

		default:
			break;
	}
}

#ifdef __WXMSW__
void CMenuOperation::AddDrive()
{
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->SendEventToListView(_MENU_ETC_ADD_DRIVE);
	if(theJsonConfig->GetSplitStyle() != WINDOW_SPLIT_NONE)
	{
		CTabManager* pAnotherTabManager = theSplitterManager->GetAnotherTab();
		pAnotherTabManager->GetActiveViewPanel()->SendEventToListView(_MENU_ETC_ADD_DRIVE);
	}
}

void CMenuOperation::UpdateDriveSpace(const wxString& strDrive)
{
	theDriveInfo->UpdateSpace(strDrive);
	CTabManager* pCurrentTabManager = theSplitterManager->GetActiveTab();
	pCurrentTabManager->GetActiveViewPanel()->SendEventToListView(_MENU_DISK_SPACE_UPDATE);

	if(theJsonConfig->GetSplitStyle() != WINDOW_SPLIT_NONE)
	{
		CTabManager* pAnotherTabManager = theSplitterManager->GetAnotherTab();
		pAnotherTabManager->GetActiveViewPanel()->SendEventToListView(_MENU_DISK_SPACE_UPDATE);
	}
}
#endif

void CMenuOperation::SetAppActivateOrNot(bool bActivate)
{
	if(!bActivate)
	{
		theSplitterManager->SetTabManagerWhenLostFocus();
		return;
	}

	CTabManager* pActiveTab = theSplitterManager->SetActivateTabWhenActivateApp();
	pActiveTab->GetActiveViewPanel()->SetActivateView();
}
