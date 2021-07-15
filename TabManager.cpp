#include "ginc.h"
#include "ViewPanel.h"
#include "TabManager.h"

wxBEGIN_EVENT_TABLE(CTabManager, wxAuiNotebook)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, OnPageClose)
wxEND_EVENT_TABLE()

CTabManager::CTabManager(wxWindow* parent, long style)
	: wxAuiNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style)
{
	m_PageBMP = wxArtProvider::GetBitmap(wxART_NEW_DIR, wxART_OTHER, wxSize(16, 16));
	this->SetArtProvider(new wxAuiGenericTabArt);
}

CTabManager::~CTabManager()
{
}

void CTabManager::AddTabPage(const wxString& strPath)
{
	Freeze();

	wxString strPathName = GetTabDispPathName(strPath);

	AddPage(new CViewPanel(this, ++wxTABWindowID, strPath), wxT(""), false, m_PageBMP);
	int nPageCount = GetPageCount();

	SetPageToolTip(nPageCount - 1, strPath);
	SetSelection(nPageCount - 1);

	this->SetPageText(nPageCount - 1, strPathName);
	Thaw();
}

wxString CTabManager::GetTabDispPathName(const wxString& strSourcePath)
{
	wxString::size_type n = strSourcePath.Find(SLASH);
	wxString strTarget = (n == wxString::npos ? strSourcePath + SLASH : theCommonUtil->GetPathName(strSourcePath));

	return strTarget;
}

void CTabManager::OnPageClose(wxAuiNotebookEvent& event)
{
	int iPageCount = this->GetPageCount();
	if (iPageCount == 1)
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_DELETE_TAB")), PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
		event.Veto();

		GetActiveViewPanel()->SetActivateView();
		return;
	}
}

CViewPanel* CTabManager::GetActiveViewPanel()
{
	int nSel = GetSelection();
	return (CViewPanel *)GetPage(nSel);
}

void CTabManager::ActivatePage()
{
	GetActiveViewPanel()->SetActivateView();
}
