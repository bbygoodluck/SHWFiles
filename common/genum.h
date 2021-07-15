#ifndef __GLOBAL_ENUMERATION_H__
#define __GLOBAL_ENUMERATION_H__

#ifdef __WXMSW__
enum WIN_OS_VER
{
	WIN_OS_NONE = -1,
	WIN_OS_95 = 1,
	WIN_OS_98,
	WIN_OS_ME,
	WIN_OS_NT,
	WIN_OS_2000,
	WIN_OS_XP,
	WIN_OS_2003,
	WIN_OS_VISTA_OR_2008,
	WIN_OS_7_OR_2008R2,
	WIN_OS_7_OVER,
};
#endif

enum _MENU_EVENT_TYPE {
	_MENU_EVENT_NONE = -1,
	_MENU_FILE_EXECUTION,
	_MENU_FILE_NEW_TAB,
	_MENU_FILE_COPY,
	_MENU_FILE_MOVE,
	_MENU_FILE_RENAME,
	_MENU_FILE_ADD_DIR,
	_MENU_FILE_EDIT,
	_MENU_FILE_DEL_TRASH,
	_MENU_FILE_DEL_COMP,
	_MENU_FILE_CMD_EXEC,
	_MENU_FILE_PROGRAM_TERMINATE,
	_MENU_EDIT_ALL_RELEASE,
	_MENU_EDIT_ALL_SELECT,
	_MENU_EDIT_COPY_CLIPBOARD,
	_MENU_EDIT_MOVE_CLIPBOARD,
	_MENU_EDIT_PASTE_CLIPBOARD,
	_MENU_EDIT_FIND,
	_MENU_EDIT_CONTEXT_MENU,
	_MENU_PATH_DIR_MANAGER,
	_MENU_PATH_REFRESH,
	_MENU_PATH_GOTO_DIR_DIRECTLY,
	_MENU_COMPRESS_FILE_VIEW,
	_MENU_COMPRESS_DO_COMPRESS,
	_MENU_DECOMPRESS_CURRENT_DIR,
	_MENU_DECOMPRESS_MK_FOLDER,
	_MENU_DECOMPRESS_SEL_DIR,
	_MENU_VIEW_FULLSCREEN,
	_MENU_VIEW_WINDOW_SINGLE,
	_MENU_VIEW_WINDOW_VERT,
	_MENU_VIEW_WINDOW_HORZ,
	_MENU_VIEW_TOOLBAR,
	_MENU_VIEW_SHORTCUT,
	_MENU_VIEW_COLUMN_0,
	_MENU_VIEW_COLUMN_1,
	_MENU_VIEW_COLUMN_2,
	_MENU_VIEW_COLUMN_3,
	_MENU_VIEW_COLUMN_4,
	_MENU_VIEW_COLUMN_5,
	_MENU_VIEW_COLUMN_6,
	_MENU_VIEW_COLUMN_7,
	_MENU_VIEW_COLUMN_8,
	_MENU_VIEW_COLUMN_9,
	_MENU_VIEW_FILE_DEFAULT,
	_MENU_VIEW_FILE_HIDDEN,
	_MENU_VIEW_FILE_ALL,
	_MENU_VIEW_SORT_DEFAULT,
	_MENU_VIEW_SORT_TIME,
	_MENU_VIEW_SORT_SIZE,
	_MENU_VIEW_SUBDIR,
	_MENU_FAVORITE_ITEM_ADD,
	_MENU_FAVORITE_MANAGER,
	_MENU_FAVORITE_ITEM_SELECTED,
	_MENU_TOOL_ENVIRONMENT,
	_MENU_TOOL_SAVECONFIG,
	_MENU_HELP_THIS_PROGRAM,
	_MENU_CHANGE_ENV_UPDATE_WINDOW,
#ifdef __WXMSW__
	_MENU_ETC_ADD_DRIVE,
	_MENU_DISK_SPACE_UPDATE,
#endif
};

enum SPLIT_WINDOW_STYLE {
	WINDOW_SPLIT_NONE = 0,
	WINDOW_SPLIT_VERT,
	WINDOW_SPLIT_HORZ
};

enum FILE_OPERATION {
	FILE_OP_NONE = -1,
	FILE_OP_COPY = 0,
	FILE_OP_CUT,
	FILE_OP_DELETE,
	FILE_OP_PASTE,
	FILE_OP_OVERWRITE,
	FILE_OP_SKIP,
	FILE_OP_RENAME,
	FILE_OP_RENAME_CANCEL,
	FILE_OP_AUTO,
	FILE_OP_CANCEL,
	FILE_OP_COPY_CLIPBOARD,
	FILE_OP_CUT_CLIPBOARD,
	FILE_OP_PASTE_CLIPBOARD
};

enum FOCUS_POSITION {
	FOCUS_POS_NONE = 0,
	FOCUS_POS_LEFT,
	FOCUS_POS_RIGHT,
	FOCUS_POS_MAX
};

typedef struct _tagCommandItemList {
	wxString m_strCmdName = wxT("");
	wxString m_strDispCmd = wxT("");
	wxString m_strCmd = wxT("");
	wxString m_strParam = wxT("");
	bool m_bUsePreSet = false;
} EXT_COMMAND_ITEM_LIST;

typedef struct _tagCommandList {
	wxString m_strExt = wxT("");
	wxString m_strTitle = wxT("");
} COMMAND_LIST;

enum EXT_COMMAND {
	CMD_DEL = 0,
	CMD_EDIT,
	CMD_COPY,
	CMD_MOVE,
	CMD_COMPRESS,
	CMD_MAX
};

enum VIEW_UPDATE {
	SEL_ITEM_CLEAR_LISTVIEW = 0,
	SEL_INFO_CLEAR_LISTVIEW,
};

enum FILE_COPY_MOVE_OPTION {
	FILE_COPY_MOVE_SAME_DIR = 0,
	FILE_COPY_MOVE_ANOTHER_DIR
};

enum _FS_WACHER_CODE
{
	E_FILESYSMON_SUCCESS = 0,
	E_FILESYSMON_ERRORUNKNOWN,
	E_FILESYSMON_ERRORNOTINIT,
	E_FILESYSMON_ERROROUTOFMEM,
	E_FILESYSMON_ERROROPENFILE,
	E_FILESYSMON_ERRORADDTOIOCP,
	E_FILESYSMON_ERRORREADDIR,
	E_FILESYSMON_NOCHANGE,
	E_FILESYSMON_ERRORDEQUE
};

enum _FS_WATCHER_TYPE
{
	FS_WATCHER_CREATE = 0,
	FS_WATCHER_MODIFY,
	FS_WATCHER_DELETE,
	FS_WATCHER_RENAME,
	FS_WATCHER_RENAME_OLD,
	FS_WATCHER_RENAME_NEW,
	FS_WATCHER_NONE = 99
};

WX_DECLARE_STRING_HASH_MAP(wxVector<EXT_COMMAND_ITEM_LIST>, CommandItemEntries);

enum
{
	SCD_DIRECTORY_SEARCH_THREAD_EVENT = wxID_HIGHEST + 1,
	SCD_MAKE_TREE_INFO_THREAD_EVENT,
	FILE_OPERATION_THREAD,
	FILE_OPERATION_DELETE_THREAD,
	COMPRESS_THREAD_START_EVENT,
	COMPRESS_THREAD_END_EVENT
};

enum TAB_POSITION {
	TAB_LEFT = 0,
	TAB_RIGHT
};

enum TRANSFER_PATH_TYPE {
	TRANSFER_LISTVIEW_TO_PATHVIEW = 1,
	TRANSFER_PATH_VIEW_TO_LISTVIEW,
	TRANSFER_LISTVIEW_DIRINFO_TO_DIRINFOVIEW,
	TRANSFER_LISTVIEW_DETAILINFO_TO_DETAILVIEW,
	TRANSFER_LISTVIEW_DISK_SIZE_TO_DETAILVIEW_SET,
	TRANSFER_LISTVIEW_DISK_SIZE_TO_DETAILVIEW_UPDATE
};

enum COMPRESS_TYPE {
	COMPTYPE_ZIP = 1,
	COMPTYPE_UNZIP,
	COMPTYPE_UNZIP_SKIP,
	COMPTYPE_UNZIP_OVERWRITE,
	COMPTYPE_UNZIP_RENAME,
	COMPTYPE_GZ,
//	COMPTYPE_RAR,
//	COMPTYPE_7Z,
//	COMPTYPE_ARJ,
//	COMPTYPE_TAR,
	COMPTYPE_NONE
};

enum FILE_VIEW_OPTION {
	FILE_VIEW_OP_DEFAULT = 0,
	FILE_VIEW_OP_HIDDEN,
	FILE_VIEW_OP_ALL,
};

enum COLUMN_VIEW_OPTION {
	VIEW_COLUMN_AUTO = 0,
	VIEW_COLUMN_ONE,
	VIEW_COLUMN_TWO,
	VIEW_COLUMN_THREE,
	VIEW_COLUMN_FOUR,
	VIEW_COLUMN_FIVE,
	VIEW_COLUMN_SIX,
	VIEW_COLUMN_SEVEN,
	VIEW_COLUMN_EIGHT,
	VIEW_COLUMN_NINE,
	VIEW_COLUMN_MAX,
};

enum VIEW_SORT {
	VIEW_SORT_DEFAULT = 0,
	VIEW_SORT_TIME,
	VIEW_SORT_SIZE,
};
#endif
