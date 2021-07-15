#ifndef __MSW_FILE_OPERATION_THREAD_H__
#define __MSW_FILE_OPERATION_THREAD_H__

#include "../FileOperationThreadBase.h"

class CMSWFileOperationThread;
typedef CMSWFileOperationThread CFileOperationThread;

class CMSWFileOperationThread : public CFileOperationThreadBase
{
public:
	CMSWFileOperationThread(FileOperationDlg* pDlg);
	virtual ~CMSWFileOperationThread();

protected:
	virtual wxThread::ExitCode Entry() wxOVERRIDE;


private:
	void SendThreadEndEvent();

private:
	virtual void DoOperationDir(const wxString& strSrc, wxString& strTgt, bool bFirst);
	virtual void DoOperationFile(const wxString& strSrc, const wxString& strTgt);

private:
	wxVector<wxString> m_vecIgnore;
};
#endif
