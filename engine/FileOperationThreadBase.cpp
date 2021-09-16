#include "../ginc.h"
#include "../dialog/DlgFileOperation.h"
#include "FileOperationThreadBase.h"
#include "LocalFileSystem.h"

CFileOperationThreadBase::CFileOperationThreadBase(FileOperationDlg* pDlg)
	: wxThread(wxTHREAD_JOINABLE)
	, m_pDialog(pDlg)
	, m_iOperationCount(0)
	, m_bOperationContinue(true)
	, m_bAllOperationStop(false)
{
}

CFileOperationThreadBase::~CFileOperationThreadBase()
{

}

void CFileOperationThreadBase::SetFileOperationData(const std::list<wxString>& lst, const wxString& strTargetPath, FILE_OPERATION fop)
{
	m_lstSrc = lst;
	m_strTarget = strTargetPath;
	m_fop = fop;
}

wxThread::ExitCode CFileOperationThreadBase::Entry()
{
	return (wxThread::ExitCode)0;
}

void CFileOperationThreadBase::DoOperation(const wxString& strSrc, const wxString& strTgt, bool IsOverwrite)
{
	m_bOperationContinue = true;

	bool IsCut = (m_fop == FILE_OP_CUT ? true : false);

	if (!CLocalFileSystem::IsWritable(strSrc, (IsCut ? FILE_SHARE_DELETE : FILE_SHARE_READ)))
	{
		m_strMsg = wxString::Format(theMsgManager->GetMessage(wxT("MSG_DLG_COPY_MOVE_USE_ANOTHER_PG")), strSrc);
		m_bOperationContinue = false;

		return;
	}

	if(IsOverwrite)
	{
		if (!CLocalFileSystem::IsWritable(strTgt, FILE_SHARE_WRITE))
		{
			m_strMsg = wxString::Format(theMsgManager->GetMessage(wxT("MSG_DLG_COPY_MOVE_USE_ANOTHER_PG")), strTgt);
			m_bOperationContinue = false;
			return;
		}
	}

	wxFile fileIn(strSrc, wxFile::read);

	wxStructStat fbuf;
	if (wxStat(strSrc, &fbuf) != 0)
	{
		fileIn.Close();
		return;
	}

	wxFile fileOut;
	if (!fileOut.Create(strTgt, IsOverwrite, fbuf.st_mode & 0777))
	{
		fileOut.Close();
		return;
	}

	unsigned long long ulReadSize = 0;
	char buf[600 * 1024];
	wxZeroMemory(buf);

	bool bReturn = false;
	while(1)
	{
		if (theFileOPCheck->IsAllCancel())
			break;

		ssize_t count = fileIn.Read(buf, WXSIZEOF(buf));
		if (count == wxInvalidOffset)
		{
			bReturn = true;
			break;
		}

		ulReadSize += count;
		m_pDialog->SetCurrentFileLength(ulReadSize);

		// end of file?
		if (!count)
			break;

		if (fileOut.Write(buf, count) < (size_t)count)
		{
			bReturn = true;
			break;
		}
	}

	fileIn.Close();
	fileOut.Close();

	if(bReturn)
		return;

	if (theFileOPCheck->IsAllCancel())
	{
		wxRemoveFile(strTgt);
		return;
	}

	m_iOperationCount++;
	m_pDialog->SetTotalOperation(m_iOperationCount);
}

bool CFileOperationThreadBase::DoOperationCut(const wxString& strItem, bool IsDir)
{
	bool bResult = IsDir ? wxFileName::Rmdir(strItem, wxPATH_RMDIR_RECURSIVE) : wxRemoveFile(strItem);
	return bResult;
}
