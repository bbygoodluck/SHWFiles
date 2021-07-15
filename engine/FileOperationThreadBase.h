#ifndef __FILE_OPERATION_THREAD_H__
#define __FILE_OPERATION_THREAD_H__

class FileOperationDlg;
class CFileOperationThreadBase : public wxThread
{
public:
	CFileOperationThreadBase(FileOperationDlg* pDlg);
	virtual ~CFileOperationThreadBase();

public:
	void SetFileOperationData(const std::list<wxString>& lst, const wxString& strTargetPath, FILE_OPERATION fop);

protected:
	virtual wxThread::ExitCode Entry() wxOVERRIDE;
	
	virtual void DoOperationDir(const wxString& strSrc, wxString& strTgt, bool bFirst) = 0;
	virtual void DoOperationFile(const wxString& strSrc, const wxString& strTgt) = 0;
	virtual void DoOperation(const wxString& strSrc, const wxString& strTgt, bool IsOverwrite);
	bool DoOperationCut(const wxString& strItem, bool IsDir = false);

protected:
	bool m_bOperationContinue;
	bool m_bAllOperationStop;

protected:
	FileOperationDlg* m_pDialog;

	std::list<wxString> m_lstSrc;
	wxString m_strTarget;
	wxString m_strMsg;
	FILE_OPERATION m_fop;

	int m_iOperationCount;
};
#endif
