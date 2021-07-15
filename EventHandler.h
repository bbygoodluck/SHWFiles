#ifndef EVENTHANDLER_H_INCLUDED
#define EVENTHANDLER_H_INCLUDED

class CMainFrame;
class CEventHandler : public wxEvtHandler
{
public:
	explicit CEventHandler(CMainFrame* _pMainFrame);
	~CEventHandler();

	void OnBookmarkSelected(wxCommandEvent& event);

public:
	void LoadBookmark(bool bFromFavoriteManager);
	void SetBookmarkDropdown();
	void RegisterToolbarEvent();

private:
	void DoMenuOperation(int iMenuID);
	void Init();

private:
	CMainFrame* m_pMainFrame;
	std::unordered_map<int, _MENU_EVENT_TYPE> m_menuMap;

private:
	void OnMenuEvent(wxCommandEvent& event);
	void OnMenuEventUpdate(wxUpdateUIEvent& event);
	void OnToolbarEvent(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

#endif // EVENTHANDLER_H_INCLUDED
