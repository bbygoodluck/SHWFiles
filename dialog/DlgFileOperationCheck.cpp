#include "../ginc.h"
#include "DlgFileOperationCheck.h"

FileOperationCheckDlg::FileOperationCheckDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	SetIcon(wxIcon("wxwin"));
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer(wxVERTICAL);

	m_staticMsg = new wxStaticText(this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticMsg->Wrap(-1);
	bSizer7->Add(m_staticMsg, 0, wxALL | wxEXPAND, 5);

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText9 = new wxStaticText(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_SOURCE")), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText9->Wrap(-1);
	bSizer8->Add(m_staticText9, 0, wxALL, 5);

	m_staticSrc = new wxStaticText(this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticSrc->Wrap(-1);
	bSizer8->Add(m_staticSrc, 1, wxALL, 5);


	bSizer7->Add(bSizer8, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText99 = new wxStaticText(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_NAME")), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText99->Wrap(-1);
	bSizer13->Add(m_staticText99, 0, wxALL, 5);

	m_staticFileName = new wxStaticText(this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticFileName->Wrap(-1);
	bSizer13->Add(m_staticFileName, 1, wxALL, 5);


	bSizer7->Add(bSizer13, 0, wxEXPAND | wxRIGHT, 5);

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText11 = new wxStaticText(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_CHANGE")), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText11->Wrap(-1);
	bSizer9->Add(m_staticText11, 0, wxALL, 5);

	m_textChange = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	bSizer9->Add(m_textChange, 1, wxLEFT | wxRIGHT, 5);


	bSizer7->Add(bSizer9, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer(wxHORIZONTAL);

	m_chkAfterAll = new wxCheckBox(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_AFTER_ALL_APPLY")), wxDefaultPosition, wxDefaultSize, 0);
	bSizer12->Add(m_chkAfterAll, 0, wxALL, 5);


	bSizer11->Add(bSizer12, 1, wxALL | wxEXPAND, 5);

	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer(wxHORIZONTAL);

	m_btnRename = new wxButton(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_CHANGE_NAME")), wxDefaultPosition, wxDefaultSize, 0);

	m_btnRename->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW, wxART_BUTTON));
	bSizer10->Add(m_btnRename, 0, wxALL, 5);

	m_btnOverWriter = new wxButton(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_CHANGE_OVERWRITE")), wxDefaultPosition, wxDefaultSize, 0);

	m_btnOverWriter->SetBitmap(wxArtProvider::GetBitmap(wxART_PASTE, wxART_BUTTON));
	bSizer10->Add(m_btnOverWriter, 0, wxALL, 5);

	m_btnByPass = new wxButton(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_CHANGE_SKIP")), wxDefaultPosition, wxDefaultSize, 0);

	m_btnByPass->SetBitmap(wxArtProvider::GetBitmap(wxART_UNDO, wxART_BUTTON));
	bSizer10->Add(m_btnByPass, 0, wxALL, 5);

	m_btnCancel = new wxButton(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_CANCEL")), wxDefaultPosition, wxDefaultSize, 0);

	m_btnCancel->SetBitmap(wxArtProvider::GetBitmap(wxART_CLOSE, wxART_BUTTON));
	bSizer10->Add(m_btnCancel, 0, wxALL, 5);


	bSizer11->Add(bSizer10, 1, 0, 5);


	bSizer7->Add(bSizer11, 0, wxALIGN_RIGHT, 5);


	this->SetSizer(bSizer7);
	this->Layout();

	this->Centre(wxBOTH);

	// Connect Events
	this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(FileOperationCheckDlg::OnClose));
	this->Connect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(FileOperationCheckDlg::OnInitDialog));
	m_btnRename->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FileOperationCheckDlg::OnBtnRenameClick), NULL, this);
	m_btnOverWriter->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FileOperationCheckDlg::OnBtnOverwriteClick), NULL, this);
	m_btnByPass->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FileOperationCheckDlg::OnBtnByPassClick), NULL, this);
	m_btnCancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FileOperationCheckDlg::OnBtnCancelClick), NULL, this);
}

FileOperationCheckDlg::~FileOperationCheckDlg()
{
	// Disconnect Events
	this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(FileOperationCheckDlg::OnClose));
	this->Disconnect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(FileOperationCheckDlg::OnInitDialog));
	m_btnRename->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FileOperationCheckDlg::OnBtnRenameClick), NULL, this);
	m_btnOverWriter->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FileOperationCheckDlg::OnBtnOverwriteClick), NULL, this);
	m_btnByPass->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FileOperationCheckDlg::OnBtnByPassClick), NULL, this);
	m_btnCancel->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FileOperationCheckDlg::OnBtnCancelClick), NULL, this);

}

void FileOperationCheckDlg::SetMessage(const wxString& strMsg, const wxString& strTgtPath, const wxString& strSrc)
{
	m_staticMsg->SetLabelText(strMsg);
	m_strTgtPath = strTgtPath;
	m_strSrc = strSrc;
}

void FileOperationCheckDlg::OnClose(wxCloseEvent& event)
{
	theFileOPCheck->SetCancel(true);
	Destroy();
}

void FileOperationCheckDlg::OnInitDialog(wxInitDialogEvent& event)
{
	m_strTgtFile = theFileOPCheck->GetExistFileName();
	wxString strName = wxFileExists(m_strTgtFile) ? theCommonUtil->GetFileName(m_strTgtFile) : theCommonUtil->GetName(m_strTgtFile);

	m_staticFileName->SetLabelText(strName);
	m_textChange->SetLabelText(strName);
	m_staticSrc->SetLabelText(m_strSrc);

	theFileOPCheck->SetOverwrite(false);
	theFileOPCheck->SetAfterSameOperation(false);
	theFileOPCheck->SetCancel(false);
	theFileOPCheck->SetByPass(false);
	theFileOPCheck->SetRename(wxT(""));
}

//이름변경의 경우 이후 모두 적용은 적용하지 않음
void FileOperationCheckDlg::OnBtnRenameClick(wxCommandEvent& event)
{
	wxString strSrcName;
	wxString strTgtName;

	strSrcName = m_staticFileName->GetLabelText();
	strTgtName = m_textChange->GetValue();// GetLineText(1);

	bool bRename = true;
	bool bExist = false;
#ifdef __WXMSW__
	if (strTgtName.CmpNoCase(strSrcName) == 0)
		bExist = true;

#else
	if (strTgtName.Cmp(strSrcName) == 0)
		bExist = true;
#endif

	if (bExist)
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_SRC_TGT_SAME_MSG")), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
		bRename = false;
		m_textChange->SetFocus();

		return;
	}

	if (wxFileExists(m_strTgtPath + SLASH + strTgtName))
	{
		wxString strMsg = m_strTgtPath + SLASH + strTgtName + theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_SRC_TGT_EXIST_MSG"));
		wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
		bRename = false;
		m_textChange->SetFocus();
		return;
	}

	m_chkAfterAll->SetValue(false);

	if (bRename)
		theFileOPCheck->SetRename(strTgtName);

	Destroy();
}

void FileOperationCheckDlg::OnBtnOverwriteClick(wxCommandEvent& event)
{
	theFileOPCheck->SetAfterSameOperation(false);
	if (m_chkAfterAll->IsChecked())
		theFileOPCheck->SetAfterSameOperation(true);

	theFileOPCheck->SetOverwrite(true);
	Destroy();
}

void FileOperationCheckDlg::OnBtnByPassClick(wxCommandEvent& event)
{
	if (m_chkAfterAll->IsChecked())
		theFileOPCheck->SetAfterSameOperation(true);

	theFileOPCheck->SetByPass(true);
	Destroy();
}

void FileOperationCheckDlg::OnBtnCancelClick(wxCommandEvent& event)
{
	theFileOPCheck->SetCancel(true);
	theFileOPCheck->SetAfterSameOperation(false);
	if (m_chkAfterAll->IsChecked())
	{
		theFileOPCheck->SetAfterSameOperation(true);
		theFileOPCheck->SetAllCancel(true);
	}

	Destroy();
}
