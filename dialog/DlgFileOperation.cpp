#include "../ginc.h"
#include "../view/ListView.h"
#include "DlgFileOperation.h"
#include "DlgFileOperationCheck.h"
#include "../engine/LocalFileSystem.h"

wxDEFINE_EVENT(wxEVT_FILE_OPERATION_START, wxCommandEvent);

wxBEGIN_EVENT_TABLE(FileOperationDlg, wxDialog)
	EVT_THREAD(FILE_OPERATION_THREAD, FileOperationDlg::OnFileOperationEnd)
	EVT_MY_CUSTOM_COMMAND(wxEVT_FILE_OPERATION_CHECK, wxID_ANY, FileOperationDlg::OnFileCheckOperation)
wxEND_EVENT_TABLE()

FileOperationDlg::FileOperationDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
	: wxDialog(parent, id, title, pos, size, style)
	, m_iTotalCount(0)
	, m_iProgressTotal(0)
{
	SetIcon(wxIcon("wxwin"));

	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	m_lstSource = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES | wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_SIMPLE);
	m_lstSource->SetBackgroundColour(wxColour(230, 230, 230));
#ifdef __WXMSW__
	ListView_SetImageList(m_lstSource->GetHWND(), _gImageList, LVSIL_SMALL);
#endif // __WXMSW__

	bSizer1->Add(m_lstSource, 0, wxALL | wxEXPAND, 5);

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText3 = new wxStaticText(this, wxID_ANY, wxT("Source :"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText3->Wrap(-1);
	bSizer14->Add(m_staticText3, 0, wxALL | wxEXPAND, 5);

	m_staticSource = new wxStaticText(this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticSource->Wrap(-1);
	bSizer14->Add(m_staticSource, 1, wxALL, 5);


	bSizer2->Add(bSizer14, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText6 = new wxStaticText(this, wxID_ANY, wxT("Target :"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText6->Wrap(-1);
	bSizer15->Add(m_staticText6, 0, wxALL, 5);

	m_staticTarget = new wxStaticText(this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticTarget->Wrap(-1);
	bSizer15->Add(m_staticTarget, 1, wxALL, 5);


	bSizer2->Add(bSizer15, 1, wxEXPAND, 5);


	bSizer1->Add(bSizer2, 0, wxEXPAND, 5);

	m_staticline11 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	bSizer1->Add(m_staticline11, 0, wxEXPAND | wxALL, 5);

	m_staticItem = new wxStaticText(this, wxID_ANY, wxT("Copy : "), wxDefaultPosition, wxDefaultSize, 0);
	m_staticItem->Wrap(-1);
	bSizer1->Add(m_staticItem, 0, wxALL | wxEXPAND, 5);

	m_progressCurr = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(-1, 15), wxGA_HORIZONTAL);
	m_progressCurr->SetValue(0);
	bSizer1->Add(m_progressCurr, 0, wxALL | wxEXPAND, 5);

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer(wxHORIZONTAL);

	m_staticCurrent = new wxStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
	m_staticCurrent->Wrap(-1);
	bSizer5->Add(m_staticCurrent, 1, wxALL, 5);

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	m_staticCurrentTotal = new wxStaticText(this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticCurrentTotal->Wrap(-1);
	bSizer6->Add(m_staticCurrentTotal, 0, wxALIGN_RIGHT|wxALL|wxRESERVE_SPACE_EVEN_IF_HIDDEN, 5);


	bSizer5->Add(bSizer6, 1, 0, 5);


	bSizer1->Add(bSizer5, 0, wxEXPAND, 5);

	m_progressTotal = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(-1, 15), wxGA_HORIZONTAL);
	m_progressTotal->SetValue(0);
	bSizer1->Add(m_progressTotal, 0, wxALL | wxEXPAND, 5);

	m_staticTotal = new wxStaticText(this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticTotal->Wrap(-1);
	bSizer1->Add(m_staticTotal, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	m_staticline1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	bSizer1->Add(m_staticline1, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxHORIZONTAL);

	m_btnCancel = new wxButton(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DLG_OPTION_BTN_CANCEL")), wxDefaultPosition, wxSize(-1, 30), 0);

	m_btnCancel->SetBitmap(wxArtProvider::GetBitmap(wxART_CROSS_MARK, wxART_BUTTON));
	bSizer4->Add(m_btnCancel, 0, wxALL, 5);

	m_btnClose = new wxButton(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DLG_OPTION_BTN_CLOSE")), wxDefaultPosition, wxSize(-1, 30), 0);

	m_btnClose->SetBitmap(wxArtProvider::GetBitmap(wxART_QUIT, wxART_BUTTON));
	bSizer4->Add(m_btnClose, 0, wxALL, 5);


	bSizer3->Add(bSizer4, 1, wxEXPAND, 5);


	bSizer1->Add(bSizer3, 0, wxALIGN_RIGHT, 5);


	this->SetSizer(bSizer1);
	this->Layout();

	this->Centre(wxBOTH);


	// Connect Events
	this->Connect(wxEVT_CHAR_HOOK, wxKeyEventHandler(FileOperationDlg::OnCharHook));
	this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(FileOperationDlg::OnClose));
	this->Connect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(FileOperationDlg::OnInitDialog));
	this->Connect(wxEVT_FILE_OPERATION_START, wxCommandEventHandler(FileOperationDlg::OnFileOperationStart), NULL, this);
	m_btnCancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FileOperationDlg::OnBtnCancel), NULL, this);
	m_btnClose->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FileOperationDlg::OnBtnClose), NULL, this);
}

FileOperationDlg::~FileOperationDlg()
{
	// Disconnect Events
	this->Disconnect(wxEVT_CHAR_HOOK, wxKeyEventHandler(FileOperationDlg::OnCharHook));
	this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(FileOperationDlg::OnClose));
	this->Disconnect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(FileOperationDlg::OnInitDialog));
	this->Disconnect(wxEVT_FILE_OPERATION_START, wxCommandEventHandler(FileOperationDlg::OnFileOperationStart), NULL, this);
	m_btnCancel->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FileOperationDlg::OnBtnCancel), NULL, this);
	m_btnClose->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FileOperationDlg::OnBtnClose), NULL, this);

	if (m_pOpThread)
	{
		if (m_pOpThread->IsRunning())
			m_pOpThread->Wait();

		delete m_pOpThread;
	}
}

void FileOperationDlg::OnCharHook(wxKeyEvent& event)
{
	if (event.GetKeyCode() == WXK_ESCAPE)
		DoCancel();
}

void FileOperationDlg::OnClose(wxCloseEvent& event)
{
	DoCancel();
}

void FileOperationDlg::OnInitDialog(wxInitDialogEvent& event)
{
	theFileOPCheck->SetInitialize();
	Init();
}

void FileOperationDlg::OnBtnCancel(wxCommandEvent& event)
{
	DoCancel();
}

void FileOperationDlg::DoCancel()
{
	m_pOpThread->Pause();

	int iRet = wxMessageBox(theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_CANCEL_MSG")), PROGRAM_FULL_NAME, wxYES_NO | wxICON_QUESTION);
	if (iRet == wxYES)
	{
		m_btnClose->Enable(true);
		m_btnCancel->Enable(false);

		theFileOPCheck->SetAllCancel(true);
	}

	m_pOpThread->Resume();
}

void FileOperationDlg::OnBtnClose(wxCommandEvent& event)
{
//	Destroy();
	EndDialog(wxID_CLOSE);
}

void FileOperationDlg::SetOperationItems(const std::list<wxString>& lst, const wxString& strTargetPath, FILE_OPERATION fop)
{
	m_lstSrc = lst;
	m_strTarget = strTargetPath;
	m_fop = fop;
}

void FileOperationDlg::Init()
{
	m_lstSource->InsertColumn(0, theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_TARGET_PATH")), wxLIST_FORMAT_LEFT, 700);
	m_lstSource->InsertColumn(1, theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_TARGET_TYPE")), wxLIST_FORMAT_RIGHT, 100);

	int iIndex = 0;
	m_iTotalCount = 0;

	int iIconIndex = 0;
	int iOverlayIndex = 0;

	for(auto strItem : m_lstSrc)
	{
		m_lstSource->InsertItem(iIndex, strItem);

		if (wxDirExists(strItem))
		{
			m_lstSource->SetItem(iIndex, 1, theMsgManager->GetMessage(wxT("MSG_DIR")));
			m_iTotalCount += theCommonUtil->GetFileCount(strItem);
		}
		else
		{
			m_lstSource->SetItem(iIndex, 1, theMsgManager->GetMessage(wxT("MSG_FILE_DESCRIPTION")));
			m_iTotalCount++;
		}
#ifdef __WXMSW__
		wxString strPath = m_lstSource->GetItemText(iIndex);
		theCommonUtil->GetIconIndex(strPath, iIconIndex, iOverlayIndex);
#endif // __WXMSW__

		m_lstSource->SetItemImage(iIndex, iIconIndex);
		iIndex++;
	}

	m_progressTotal->SetRange(m_iTotalCount);
	wxString strCpCountInfo = wxString::Format(wxT("(%d / %d)"), m_iProgressTotal, m_iTotalCount);
	m_staticTotal->SetLabelText(strCpCountInfo);

	m_staticTarget->SetLabelText(m_strTarget);

	m_staticTarget->SetFont(*_gFont);

	wxCommandEvent evt(wxEVT_FILE_OPERATION_START);
	wxPostEvent(this, evt);
}

void FileOperationDlg::OnFileOperationStart(wxCommandEvent& event)
{
	m_btnCancel->Enable(true);
	m_btnClose->Enable(false);

	m_pOpThread = new CFileOperationThread(this);
	m_pOpThread->SetFileOperationData(m_lstSrc, m_strTarget, m_fop);
	m_pOpThread->Run();
}

void FileOperationDlg::SetSourcePath(const wxString& strSrcPath)
{
	wxString strOperation = (m_fop == FILE_OP_COPY) ? theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_COPY_MSG")) : theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_MOVE_MSG"));
	strOperation += strSrcPath;

	m_staticSource->SetLabelText(strSrcPath);
}

void FileOperationDlg::SetCurrentFileItem(const wxString& strItem)
{
	wxString strOperation = (m_fop == FILE_OP_COPY) ? theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_COPY_MSG")) : theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_MOVE_MSG"));
	strOperation += strItem;

	m_staticItem->SetLabelText(strOperation);
}

void FileOperationDlg::SetCurrentFileTotal(unsigned long long ull)
{
	m_iCurrTotal = ull / MEGABYTE;
	m_progressCurr->SetRange(m_iCurrTotal);

	wxString strCurrentTotal = theCommonUtil->SetComma(wxString::Format(wxT("%d"), m_iCurrTotal)) + wxT(" MB");
	m_staticCurrentTotal->SetLabelText(strCurrentTotal);
}

void FileOperationDlg::SetCurrentFileLength(unsigned long long ullCurrent)
{
	int iCurrSize = ullCurrent / MEGABYTE;
	m_progressCurr->SetValue(iCurrSize);

	wxString strCurr = theCommonUtil->SetComma(wxString::Format(wxT("%d"), iCurrSize)) + wxT( " MB");
	m_staticCurrent->SetLabelText(strCurr);
}

void FileOperationDlg::SetCurrentFileTotal(UINT uWorkTotal)
{
	m_progressCurr->SetRange(uWorkTotal);
}

void FileOperationDlg::SetCurrentFileNow(UINT uWorkSoFar)
{
	m_progressCurr->SetValue(uWorkSoFar);
}

void FileOperationDlg::OnFileOperationEnd(wxThreadEvent& event)
{
	wxMilliSleep(100);
	EndDialog(wxID_CLOSE);
}

void FileOperationDlg::ShowExistCheckDialog(const wxString& strExistFile, const wxString& strSrc)
{
	m_strOperationTgt = strExistFile;
	m_strOperationSrc = strSrc;
}

void FileOperationDlg::OnFileCheckOperation(wxCommandEvent& event)
{
	FileOperationCheckDlg dlg(_gMainFrame);
	wxString strMsg = wxString::Format(theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_FILE_EXIST_MSG")), m_strOperationTgt);

	wxString strTgtPath = event.GetString();
	dlg.SetMessage(strMsg, strTgtPath, m_strOperationSrc);
	dlg.ShowModal();

	_gLock.getCondition()->Broadcast();
}

void FileOperationDlg::SetTotalOperation(int iOPCount)
{
	wxString strCpCountInfo = wxString::Format(wxT("(%d/ %d)"), iOPCount, m_iTotalCount);
	m_staticTotal->SetLabelText(strCpCountInfo);

	m_progressTotal->SetValue(iOPCount);
}
