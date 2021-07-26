#include "../ginc.h"
#include "FileOperationCheck.h"

std::unique_ptr<CFileOperationCheck> CFileOperationCheck::m_pInstance(nullptr);

CFileOperationCheck* CFileOperationCheck::Get()
{
	if (m_pInstance.get() == NULL)
		m_pInstance.reset(new CFileOperationCheck());

	return m_pInstance.get();
}

bool CFileOperationCheck::IsExistFile(const wxString& strFileName)
{
	bool bExist = false;
	if (wxFileExists(strFileName))
	{
		m_strExistFileName = strFileName;
		bExist = true;
	}

	return bExist;
}

wxString CFileOperationCheck::GetExistFileName()
{
	return m_strExistFileName;
}

void CFileOperationCheck::SetRename(const wxString& strNewName)
{
	m_strRename = strNewName;
}

wxString CFileOperationCheck::GetRename()
{
	return m_strRename;
}

void CFileOperationCheck::SetOverwrite(bool IsOverwrite)
{
	m_bOverwrite = IsOverwrite;
}

bool CFileOperationCheck::IsOverwrite()
{
	return m_bOverwrite;
}

void CFileOperationCheck::SetAfterSameOperation(bool IsSameOperation)
{
	m_bAferSameOperation = IsSameOperation;
}

bool CFileOperationCheck::IsAfterSameOperation()
{
	return m_bAferSameOperation;
}

void CFileOperationCheck::SetCancel(bool IsCancel)
{
	m_bCancel = IsCancel;
}

bool CFileOperationCheck::IsCancel()
{
	return m_bCancel;
}

void CFileOperationCheck::SetByPass(bool IsByPass)
{
	m_bByPass = IsByPass;
}
bool CFileOperationCheck::IsByPass()
{
	return m_bByPass;
}

void CFileOperationCheck::SetAllCancel(bool IsAllCancel)
{
	m_bAllCancel = IsAllCancel;
}

bool CFileOperationCheck::IsAllCancel()
{
	return m_bAllCancel;
}

void CFileOperationCheck::SetInitialize()
{
	m_bOverwrite = false;
	m_bAferSameOperation = false;
	m_bCancel = false;
	m_bByPass = false;
	m_bAllCancel = false;

	m_strRename = wxT("");
}
