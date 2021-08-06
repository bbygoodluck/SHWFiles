#ifndef LOCALFILELISTVIEW_H_INCLUDED
#define LOCALFILELISTVIEW_H_INCLUDED

#include "../ListView.h"
#include "../../engine/msw/LocalFileSystemWatcher.h"

class CLocalFileListView : public CListView
{
public:
	explicit CLocalFileListView(wxWindow* parent, const int nID, const wxSize& sz);
	virtual ~CLocalFileListView();

public:
	void LoadDir(const wxString& strPath) override;

protected:
	void Render(wxDC* pDC) override;
	//FileSystemWatcher 함수들..
	void DoCreate(const wxString& strName) override;
	void DoModify(const wxString& strName) override;
	void DoDelete(const wxString& strName) override;
	void DoRename(const wxString& strOldName, const wxString& strNewName) override;

	void ShowFavoriteMenu();

private:
	bool ReadDirectory();
	void AddDrive();
	void RemoveDrive();

private:
	std::unique_ptr<CLocalFileSystemWatcher> m_pFileSystemWatcher;

private:
	void OnPaint(wxPaintEvent& event);
	void OnErase(wxEraseEvent& event);
	void OnFileSystemWatcher(wxCustomFileSystemWatcherEvent& event);
	//CustomEvent
#ifdef __WXMSW__
	void OnDriveAddOrRemove(wxCommandEvent& event);
	void OnUpdateDriveSpace(wxCommandEvent& event);
#endif
	wxDECLARE_EVENT_TABLE();
};

#endif // LOCALFILELISTVIEW_H_INCLUDED
