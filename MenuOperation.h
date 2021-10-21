#ifndef MENUOPERATION_H_INCLUDED
#define MENUOPERATION_H_INCLUDED

class CEventHandler;
class CMenuOperation
{
protected:

private:
	explicit CMenuOperation() {};

public:
	~CMenuOperation() {
		m_menuFnPtrMap.clear();
	};
	static CMenuOperation *Get();

	void SetEventHandler(CEventHandler * pHandler);
	void MenuOperationExecution(int iMenuID, _MENU_EVENT_TYPE _mType);
	void Init();
	//파일메뉴
	void FileMenu_Execution();
	void FileMenu_NewTab();
	void FileMenu_Copy();
	void FileMenu_Move();
	void FileMenu_Rename();
	void FileMenu_CreateFolder();
	void FileMenu_Edit();
	void FileMenu_Trash();
	void FileMenu_CompletionDel();
	void FileMenu_ExecCMD();
	void FileMenu_Terminate();
	//편집메뉴
	void EditMenu_AllRelease();
	void EditMenu_AllSelect();
	void EditMenu_Copy_UseClipboard();
	void EditMenu_Move_UseClipboard();
	void EditMenu_Paste_UseClipboard();
	void EditMenu_FileFind();
	void EditMenu_ViewContextMenu();
	//경로메뉴
	void PathMenu_DirManager();
	void PathMenu_Refresh();
	void PathMenu_GotoDirDirectly();

	//압축
	void CompressMenu_DoCompress();
	void CompressMenu_DeCompressCurDir();
	void CompressMenu_DeCompressMkDir();
	void CompressMenu_DeCompressSelDir();
	//보기
	void ViewMenu_FullScreen();
	void ViewMenu_Window_Single();
	void ViewMenu_Window_Vert();
	void ViewMenu_Window_Horz();
	void ViewMenu_Toolbar();
	void ViewMenu_Shortcut();
	void ViewMenu_Column_0();
	void ViewMenu_Column_1();
	void ViewMenu_Column_2();
	void ViewMenu_Column_3();
	void ViewMenu_Column_4();
	void ViewMenu_Column_5();
	void ViewMenu_Column_6();
	void ViewMenu_Column_7();
	void ViewMenu_Column_8();
	void ViewMenu_Column_9();
	void ViewMenu_FileView_Default();
	void ViewMenu_FileView_Hidden();
	void ViewMenu_FileView_All();
	void ViewMenu_SortDefault();
	void ViewMenu_SortTime();
	void ViewMenu_SortSize();
	void ViewMenu_SubDir();
	//즐겨찾기
	void FavoriteMenu_Add();
	void FavoriteMenu_Manager();
	void FavoriteMenu_ItemSelected(int iMenuID);
	//도구
	void ToolMenu_Setting();
	void ToolMenu_SaveSetting();
	//도움말
	void HelpMenu_ThisProgram();

	//ToolbarEvent
	void ExecuteToolbar(int id);

	//2021.09.30 Add Start
	//2021.10.21 Modify
	// 비활성 -> 활성이 되었을 경우 현재 활성탭에 대한 포커스 활성화
	void SetAppActivateOrNot(bool bActivate = true);
	//2021.09.30 Add End
#ifdef __WXMSW__
	//드라이브 추가 삭제
	void AddDrive();
	void UpdateDriveSpace(const wxString& strDrive);
#endif
private:
	wxString GetDirectorySelect();
	void DoFileOperation(FILE_OPERATION fop, bool IsUseClipboard = false);
	void SetFileOperationClipboardData(bool IsCut);
	void DoDeleteTrash(_MENU_EVENT_TYPE mType);
	void ChangeViewStyle(int iViewStyle);
	void ChangeViewColumn(COLUMN_VIEW_OPTION columnOP);
	void DoChangeViewColumn(CTabManager* pTabManager);
	void ChangeFileList(_MENU_EVENT_TYPE mType);
	void DoChangeFileList(CTabManager* pTabManager);
	void SetSort(VIEW_SORT sType);
	void DoSort(CTabManager* pTabManager);
	void DoFavorite(_MENU_EVENT_TYPE mType);
	void DoDecompress(_MENU_EVENT_TYPE _menuType);
private:
	static std::unique_ptr<CMenuOperation> m_pInstance;
	std::unordered_map< int, std::function<void()> > m_menuFnPtrMap;

	CEventHandler* m_pEventHandler;
};

#endif // MENUOPERATION_H_INCLUDED
