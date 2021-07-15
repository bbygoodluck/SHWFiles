#ifndef __DLG_FILE_OPERATION_CHECK_H__
#define __DLG_FILE_OPERATION_CHECK_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/dialog.h>

class FileOperationCheckDlg : public wxDialog
{
public:
	FileOperationCheckDlg(wxWindow* parent, 
					wxWindowID id = wxID_ANY, 
					const wxString& title = wxT("File Operation Check"), 
					const wxPoint& pos = wxDefaultPosition, 
					const wxSize& size = wxSize(669, 181), 
					long style = wxDEFAULT_DIALOG_STYLE);
	~FileOperationCheckDlg();

public:
	void SetMessage(const wxString& strMsg, const wxString& strTgtPath, const wxString& strSrc);

protected:
	wxStaticText* m_staticMsg;
	wxStaticText* m_staticText9;
	wxStaticText* m_staticSrc;
	wxStaticText* m_staticText99;
	wxStaticText* m_staticFileName;
	wxStaticText* m_staticText11;
	wxTextCtrl* m_textChange;
	wxCheckBox* m_chkAfterAll;
	wxButton* m_btnRename;
	wxButton* m_btnOverWriter;
	wxButton* m_btnByPass;
	wxButton* m_btnCancel;

	// Virtual event handlers, overide them in your derived class
	virtual void OnClose(wxCloseEvent& event);
	virtual void OnInitDialog(wxInitDialogEvent& event);
	virtual void OnBtnRenameClick(wxCommandEvent& event);
	virtual void OnBtnOverwriteClick(wxCommandEvent& event);
	virtual void OnBtnByPassClick(wxCommandEvent& event);
	virtual void OnBtnCancelClick(wxCommandEvent& event);

private:
	wxString m_strTgtFile;
	wxString m_strTgtPath;
	wxString m_strSrc;
};

#endif
