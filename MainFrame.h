#ifndef MAINFRAME_H_INCLUDED
#define MAINFRAME_H_INCLUDED

class CEventHandler;
class CCustomStatusBar;
class CMainFrame : public wxFrame
{
public:
	CMainFrame(const wxString& strTitle);
	virtual ~CMainFrame();

	void SetCreateOK(bool bCreateOK = false) {
		m_bCreateOK = bCreateOK;
	}

private:
	void Initialize();
	void RegisterEventHandler();
	void CreateToolBar();

private:
	wxAuiManager	m_mgr;
	bool m_bMaximize = false;
	bool m_bCreateOK = false;
	bool m_bFirstExec = true;

	CEventHandler*	m_pSHFEventHandler;
	std::unique_ptr<CCustomStatusBar> m_pStatusBar;
	std::unique_ptr<wxToolBar> m_pToolBar;

#ifdef __WXMSW__
	HDEVNOTIFY m_hNotifyDev;
#endif
private:
#ifdef __WXMSW__
	virtual WXLRESULT MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
	void OnDeviceChange(WXWPARAM nEventType, WXLPARAM dwData);
#endif

	void OnClose(wxCloseEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnMaximized(wxMaximizeEvent& event);
	void OnIconized(wxIconizeEvent& event);

	wxDECLARE_EVENT_TABLE();
};

#endif // MAINFRAME_H_INCLUDED
