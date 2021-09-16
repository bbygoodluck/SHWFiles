#include "ginc.h"
#include "EventHandler.h"
#include "CustomStatusBar.h"
#include "MainFrame.h"

#ifdef __WXMSW__
	#include <Dbt.h>
	static const GUID usb_hid = { 0xA5DCBF10L, 0x6530, 0x11D2, {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED} };
#endif

wxBEGIN_EVENT_TABLE(CMainFrame, wxFrame)
	EVT_SIZE(CMainFrame::OnSize)
	EVT_CLOSE(CMainFrame::OnClose)
wxEND_EVENT_TABLE()

CMainFrame::CMainFrame(const wxString& strTitle)
	: wxFrame(NULL, wxID_ANY, strTitle, wxPoint(theJsonConfig->GetXPos(), theJsonConfig->GetYPos()),
		                                wxSize(theJsonConfig->GetWidth(), theJsonConfig->GetHeight()))
{
	_gMainFrame = this;

	SetIcon(wxIcon("wxwin"));
	wxMenuBar* pMenuBar = wxXmlResource::Get()->LoadMenuBar(this, wxT("SHCMenu"));
	if (pMenuBar == nullptr)
	{
		wxMessageBox(wxT("Failed to load MenuBar resource"), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return;
	}

	SetMenuBar(pMenuBar);
	RegisterEventHandler();

	Initialize();
	CreateToolBar();

	m_pStatusBar = std::make_unique<CCustomStatusBar>(this);
	SetStatusBar(m_pStatusBar.get());

	m_mgr.SetManagedWindow(this);

	theSplitterManager->Init();
	theSplitterManager->GetSplitter()->SetSashGravity(1.0);
	theSplitterManager->DoSplitter();

	m_mgr.AddPane(theSplitterManager->GetSplitter(), wxAuiPaneInfo().CenterPane().PaneBorder(false));
	m_mgr.Update();

	if (theJsonConfig->IsMaximized())
		((wxTopLevelWindow*)wxTheApp->GetTopWindow())->Maximize(true);
	else
		CenterOnScreen();

#ifdef __WXMSW__
	//RegisterDeviceNotification()함수를 이용해서 usb 포트를 감시할 경우 주의해야 할 사항
	//    - DEV_BROADCAST_DEVICEINTERFACE 구조체의 dbcc_devicetype값을 DBT_DEVTYP_DEVICEINTERFACE로 설정할 경우
	//      usb포트에 장치가 연결될때마다 호출이 되므로 usb카드삽입시 새로운 드라이브가 생성되었을때만 이벤트를 발생시킬 경우는
	//      DBT_DEVTYP_VOLUME 값을 셋팅해야 정상적으로 usb카드삽입후 실제로 usb 드라이브가 생성된 이후에 이벤트가 발생한다.
	//      DBT_DEVTYP_DEVICEINTERFACE로 셋팅하게 되면 usb포트에 장치가 삽입되자마자 이벤트가 발생하게 됨
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;

	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_VOLUME;// DBT_DEVTYP_DEVICEINTERFACE;
	NotificationFilter.dbcc_classguid = usb_hid;

	m_hNotifyDev = ::RegisterDeviceNotification(this->GetHWND(), &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
#endif
}

CMainFrame::~CMainFrame()
{

}

void CMainFrame::Initialize()
{
	int iMonitors = wxDisplay::GetCount();

	int nCurrDisp = wxDisplay::GetFromWindow(this);
	if (nCurrDisp < 0)
		nCurrDisp = 0;

	wxDisplay disp(nCurrDisp);

	if (iMonitors == 1)
	{	//모니터가 하나일경우 기존 모니터 인덱스와 다를 경우는 Default로 셋팅
		int iMonitorPos = theJsonConfig->GetMonitorPos();
		if (nCurrDisp != iMonitorPos)
		{
			wxRect rcDefault = theJsonConfig->GetDefaultRect();

			theJsonConfig->SetChangeGlobalVal(wxT("DispMonitor"), 0);
			theJsonConfig->SetChangeGlobalVal(wxT("XPos"), rcDefault.GetLeft());
			theJsonConfig->SetChangeGlobalVal(wxT("YPos"), rcDefault.GetRight());
			theJsonConfig->SetChangeGlobalVal(wxT("Width"), rcDefault.GetWidth());
			theJsonConfig->SetChangeGlobalVal(wxT("Height"), rcDefault.GetHeight());

			if (!theJsonConfig->IsMaximized())
				this->SetSize(rcDefault);
		}
	}

	if (theJsonConfig->IsMaximized())
		this->Maximize();
}

void CMainFrame::OnClose(wxCloseEvent& event)
{
	m_mgr.UnInit();
	if(m_pSHFEventHandler)
		PopEventHandler(true);

	m_pSHFEventHandler = nullptr;
	Destroy();
}

void CMainFrame::OnSize(wxSizeEvent& event)
{
	if(!m_bCreateOK)
		return;

	wxSize szFrame = event.GetSize();

	m_bMaximize = this->IsMaximized() ? true : false;
	int nCurrDisp = wxDisplay::GetFromWindow(this);
	if (nCurrDisp < 0)
		nCurrDisp = 0;

	wxDisplay disp(nCurrDisp);

	theJsonConfig->SetChangeGlobalVal(wxT("Maximized"), m_bMaximize ? 1 : 0);
	if (theJsonConfig->IsCenterOnScreen())
		CenterOnScreen();

	theJsonConfig->SetChangeGlobalVal(wxT("Width"), szFrame.GetWidth());
	theJsonConfig->SetChangeGlobalVal(wxT("Height"), szFrame.GetHeight());

	theSplitterManager->DoSplitter(szFrame);
}

void CMainFrame::RegisterEventHandler()
{
	m_pSHFEventHandler = new CEventHandler(this);
	PushEventHandler(m_pSHFEventHandler);

	m_pSHFEventHandler->LoadBookmark(false);
}

void CMainFrame::CreateToolBar()
{
	wxString strRefreshBMP = theCommonUtil->GetWorkDir() + SLASH + wxT("img") + SLASH + wxT("refresh.bmp");
	wxString strMakeFolderBMP = theCommonUtil->GetWorkDir() + SLASH + wxT("img") + SLASH + wxT("MakeFolder.bmp");
	wxString strEnvBMP = theCommonUtil->GetWorkDir() + SLASH + wxT("img") + SLASH + wxT("environment.bmp");

	m_pToolBar = std::make_unique<wxToolBar>(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxTB_FLAT | wxTB_HORZ_TEXT);//.reset(new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxTB_FLAT | wxTB_HORZ_TEXT));

	m_pToolBar->AddTool(TOOLBAR_ID_REFRESH, theMsgManager->GetMessage(wxT("MSG_DLG_SCD_REFRESH_ALL")), wxBitmap(strRefreshBMP, wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_DLG_SCD_REFRESH_ALL")), wxEmptyString, NULL);
	m_pToolBar->AddSeparator();
	m_pToolBar->AddTool(TOOLBAR_ID_FILE_EDIT, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_EDIT")), wxArtProvider::GetBitmap(wxART_EDIT, wxART_BUTTON), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_EDIT")), wxEmptyString, NULL);
	m_pToolBar->AddTool(TOOLBAR_ID_FIND, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_FIND")), wxArtProvider::GetBitmap(wxART_FIND, wxART_BUTTON), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_FIND")), wxEmptyString, NULL);
	m_pToolBar->AddSeparator();
	m_pToolBar->AddTool(TOOLBAR_ID_DIR_MANAGER, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_MCD")), wxBitmap(strMakeFolderBMP, wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_MCD")), wxEmptyString, NULL);
	m_pToolBar->AddTool(TOOLBAR_ID_BOOKMARK, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_FAVORITE")), wxArtProvider::GetBitmap(wxART_ADD_BOOKMARK, wxART_BUTTON), wxNullBitmap, wxITEM_DROPDOWN, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_BOOKMARK")), wxEmptyString, NULL);
	m_pToolBar->AddSeparator();
	m_pToolBar->AddTool(TOOLBAR_ID_ENV_SETTING, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_ENVIRONMENT")), wxBitmap(strEnvBMP, wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_ENVIRONMENT")), wxEmptyString, NULL);
	m_pToolBar->AddTool(TOOLBAR_ID_ENV_SAVE, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_ENV_SAVE")), wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_BUTTON), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_ENV_SAVE")), wxEmptyString, NULL);
	m_pToolBar->AddStretchableSpace();
	m_pToolBar->AddTool(TOOLBAR_ID_THIS_PROGRAM, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_THISPG")), wxArtProvider::GetBitmap(wxART_HELP_PAGE, wxART_BUTTON), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_THISPG")), wxEmptyString, NULL);
	m_pToolBar->Realize();

	SetToolBar(m_pToolBar.get());

	m_pSHFEventHandler->SetBookmarkDropdown();
	m_pSHFEventHandler->RegisterToolbarEvent();
}

#ifdef __WXMSW__
WXLRESULT CMainFrame::MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
	WXLRESULT rc;
	switch (nMsg)
	{
		case WM_DEVICECHANGE:
			OnDeviceChange(wParam, lParam);
			break;

		default:
			break;
	}

	rc = ::DefWindowProc(this->GetHWND(), nMsg, wParam, lParam);
	return rc;
}

void CMainFrame::OnDeviceChange(WXWPARAM nEventType, WXLPARAM dwData)
{
	if ((nEventType != DBT_DEVICEARRIVAL) && (nEventType != DBT_DEVICEREMOVECOMPLETE))
		return;

	PDEV_BROADCAST_HDR pdbh = reinterpret_cast<DEV_BROADCAST_HDR *>(dwData);

	DWORD dbhSize = pdbh->dbch_size;
	if(dbhSize == 0)
		return;

	if (pdbh->dbch_devicetype != DBT_DEVTYP_VOLUME)
		return;

	switch (nEventType)
	{
		case DBT_DEVICEARRIVAL:
		case DBT_DEVICEREMOVECOMPLETE:
		{
			theDriveInfo->Init();
			theMenuOperation->AddDrive();
		}
			break;

		default:
			break;
	}
}

#endif
