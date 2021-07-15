#ifndef TABMANAGER_H_INCLUDED
#define TABMANAGER_H_INCLUDED

class CViewPanel;
class CTabManager : public wxAuiNotebook
{
public:
	explicit CTabManager(wxWindow* parent, long style);
	virtual ~CTabManager();

public:
	void AddTabPage(const wxString& strPath);
	CViewPanel* GetActiveViewPanel();
	void ActivatePage();

private:
	wxString GetTabDispPathName(const wxString& strSourcePath);

private:
	wxBitmap m_PageBMP;

private:
	void OnPageClose(wxAuiNotebookEvent& event);
	wxDECLARE_EVENT_TABLE();
};

#endif // TABMANAGER_H_INCLUDED
