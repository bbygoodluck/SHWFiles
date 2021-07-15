#ifndef __DLG_FILE_OPERATION_H__
#define __DLG_FILE_OPERATION_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/gauge.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/dialog.h>

#ifdef __WXMSW__
	#include "../engine/msw/MSWFileOperationThread.h"
#else
#endif

wxDECLARE_EVENT(wxEVT_FILE_OPERATION_START, wxCommandEvent);
class FileOperationDlg : public wxDialog
{
public:
	FileOperationDlg(wxWindow* parent,
					 wxWindowID id = wxID_ANY,
					 const wxString& title = wxT("복사 & 이동"),
					 const wxPoint& pos = wxDefaultPosition,
				     const wxSize& size = wxSize(859, 480), long style = wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN);
	~FileOperationDlg();

public:
	void SetOperationItems(const std::list<wxString>& lst, const wxString& strTargetPath, FILE_OPERATION fop);
	void SetSourcePath(const wxString& strSrcPath);
	void SetCurrentFileItem(const wxString& strItem);

	void SetCurrentFileTotal(unsigned long long);
	void SetCurrentFileLength(unsigned long long ullCurrent);
	void SetCurrentFileTotal(UINT);
	void SetCurrentFileNow(UINT);
	void SetTotalOperation(int iOPCount);
	void ShowExistCheckDialog(const wxString& strExistFile, const wxString& strSrc);

private:
	void Init();
	void DoCancel();

protected:
	wxListCtrl* m_lstSource;
	wxStaticText* m_staticText3;
	wxStaticText* m_staticSource;
	wxStaticText* m_staticText6;
	wxStaticText* m_staticTarget;
	wxStaticLine* m_staticline11;
	wxStaticText* m_staticItem;
	wxGauge* m_progressCurr;
	wxStaticText* m_staticCurrent;
	wxStaticText* m_staticCurrentTotal;
	wxGauge* m_progressTotal;
	wxStaticText* m_staticTotal;
	wxStaticLine* m_staticline1;
	wxButton* m_btnCancel;
	wxButton* m_btnClose;


private:
	std::list<wxString> m_lstSrc;
	wxString m_strTarget;
	FILE_OPERATION m_fop;

	int m_iTotalCount;
	int m_iProgressTotal;

	CFileOperationThread* m_pOpThread = nullptr;
//	CLocalFileOperationImpl2* m_pFileOp = nullptr;
	int m_iCurrTotal;

	wxString m_strOperationTgt;
	wxString m_strOperationSrc;
private:

	// Virtual event handlers, overide them in your derived class
	virtual void OnCharHook(wxKeyEvent& event);
	virtual void OnClose(wxCloseEvent& event);
	virtual void OnInitDialog(wxInitDialogEvent& event);
	virtual void OnBtnCancel(wxCommandEvent& event);
	virtual void OnBtnClose(wxCommandEvent& event);


	void OnFileOperationStart(wxCommandEvent& event);
	void OnFileOperationEnd(wxThreadEvent& event);
	void OnFileCheckOperation(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

#endif
