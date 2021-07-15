#include "../../ginc.h"
#include "../../dialog/DlgFileOperation.h"
#include "MSWFileOperationThread.h"
#include "../LocalFileSystem.h"

CMSWFileOperationThread::CMSWFileOperationThread(FileOperationDlg* pDlg)
	: CFileOperationThreadBase(pDlg)
{
	m_vecIgnore.push_back(".");
	m_vecIgnore.push_back("..");
}

CMSWFileOperationThread::~CMSWFileOperationThread()
{

}

wxThread::ExitCode CMSWFileOperationThread::Entry()
{
	wxString strTgt(wxT(""));
	bool bDir;
	for(auto strItem : m_lstSrc)
	{
		if (m_bAllOperationStop)
			break;

		if (theFileOPCheck->IsAllCancel())
			break;

		if (!theFileOPCheck->IsAfterSameOperation())
			theFileOPCheck->SetInitialize();

		bDir = false;
		m_pDialog->SetSourcePath(strItem);

		strTgt = m_strTarget;
		DWORD dwNum = GetFileAttributes(strItem);
		if (dwNum & FILE_ATTRIBUTE_DIRECTORY)
		{
			DoOperationDir(strItem, strTgt, true);
			bDir = true;
		}
		else
			DoOperationFile(strItem, strTgt);

		if (m_fop == FILE_OP_CUT)
		{
			if(!m_bOperationContinue)
			{
				m_bOperationContinue = true;
				continue;
			}

			if (!DoOperationCut(strItem, bDir))
			{
				wxString strMsg(strItem);
				strMsg += theMsgManager->GetMessage(wxT("MSG_DEL_PROBLEM_CONFIRM"));
				wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
				break;
			}
		}
	}

	SendThreadEndEvent();
	return (wxThread::ExitCode)0;
}

void CMSWFileOperationThread::SendThreadEndEvent()
{
	wxThreadEvent event(wxEVT_THREAD, FILE_OPERATION_THREAD);
	event.SetInt(-1);
	wxQueueEvent(m_pDialog, event.Clone());
}

void CMSWFileOperationThread::DoOperationDir(const wxString& strSrc, wxString& strTgt, bool bFirst)
{
	if (theFileOPCheck->IsAllCancel())
		return;

	WIN32_FIND_DATA fd;
	HANDLE hFind;
	bool bfound;
	wxString strFind_path(strSrc);

	strFind_path += SLASH + wxT("*.*");

	hFind = FindFirstFileEx(strFind_path, FindExInfoStandard, &fd, FindExSearchNameMatch, 0, 0);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	bfound = true;

	wxString strDirName = theCommonUtil->GetPathName(strSrc);

	if (bFirst)
	{
		if (!wxDirExists(strTgt + SLASH + strDirName))
		{
			wxMkDir(strTgt + SLASH + strDirName);
			strTgt += SLASH;
			strTgt += strDirName;
		}
		else
		{
			strTgt += SLASH;
			strTgt += strDirName;
		}
	}

	do
	{
		if (m_bAllOperationStop)
			break;

		if (theFileOPCheck->IsAllCancel())
			break;

		wxString strName(fd.cFileName);
		if (std::find(m_vecIgnore.begin(), m_vecIgnore.end(), strName.ToStdString()) != m_vecIgnore.end())
			continue;

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			wxString strNewDir = strTgt + SLASH + strName;
			if (!wxDirExists(strNewDir))
				wxMkDir(strNewDir);

			DoOperationDir(strSrc + SLASH + strName, strNewDir, false);
		}
		else
			DoOperationFile(strSrc + SLASH + strName, strTgt);

	} while ((bfound = FindNextFile(hFind, &fd) != 0));

	FindClose(hFind);
}

void CMSWFileOperationThread::DoOperationFile(const wxString& strSrc, const wxString& strTgt)
{
	if (theFileOPCheck->IsAllCancel())
		return;

	unsigned long long llFileSize = theCommonUtil->GetFileSize(strSrc);
	m_pDialog->SetCurrentFileTotal(llFileSize);

	wxString strName = theCommonUtil->GetFileName(strSrc);
	wxString strDest(strTgt);

	if(theCommonUtil->Compare(SLASH, strDest.Right(1)) == 0)
		strDest += strName;
	else
	{
		strDest += SLASH;
		strDest += strName;
	}

	if (!theFileOPCheck->IsAfterSameOperation() && !theFileOPCheck->IsOverwrite())
	{
		if (theFileOPCheck->IsExistFile(strDest))
		{
			m_pDialog->ShowExistCheckDialog(strDest, strSrc);
			wxCommandEvent evt(wxEVT_FILE_OPERATION_CHECK);
			evt.SetString(strTgt);
			wxPostEvent(m_pDialog, evt);

			_gLock.getCondition()->Wait();
		}
	}

	if (theFileOPCheck->IsCancel() || theFileOPCheck->IsByPass())
		return;

	wxString strRename(theFileOPCheck->GetRename());
	bool bOverwrite = theFileOPCheck->IsOverwrite();
	if (!theFileOPCheck->IsAfterSameOperation())
	{
		if (!strRename.IsEmpty())
			strDest = strTgt + SLASH + strRename;

		theFileOPCheck->SetRename(wxT(""));
	}

	m_bAllOperationStop = false;
	m_pDialog->SetCurrentFileItem(strSrc);
	DoOperation(strSrc, strDest, bOverwrite);

	if (!m_bOperationContinue)
	{
		int iRet = wxMessageBox(m_strMsg, PROGRAM_FULL_NAME, wxICON_ERROR | wxYES_NO, _gMainFrame);
		if (iRet == wxYES)
			m_bAllOperationStop = true;
	}
}
