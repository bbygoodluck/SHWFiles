#ifndef __FILE_OPERATION_CHECK_H__
#define __FILE_OPERATION_CHECK_H__

class CFileOperationCheck
{
private:
	CFileOperationCheck()
		: m_bOverwrite(false)
		, m_bAferSameOperation(false)
		, m_bCancel(false)
		, m_bByPass(false)
		, m_bAllCancel(false)

	{
	};

public:
	~CFileOperationCheck() {};

public:
	static CFileOperationCheck* Get();

	bool IsExistFile(const wxString& strFileName);
	wxString GetExistFileName();

	void SetRename(const wxString& strNewName);
	wxString GetRename();

	void SetOverwrite(bool IsOverwrite);
	bool IsOverwrite();

	void SetAfterSameOperation(bool IsSameOperation);
	bool IsAfterSameOperation();

	void SetCancel(bool IsCancel);
	bool IsCancel();

	void SetByPass(bool IsByPass);
	bool IsByPass();

	void SetAllCancel(bool IsAllCancel);
	bool IsAllCancel();

	void SetInitialize();
private:
	static std::unique_ptr<CFileOperationCheck> m_pInstance;
	wxString m_strExistFileName;
	wxString m_strRename;

	bool m_bOverwrite;
	bool m_bAferSameOperation;
	bool m_bCancel;
	bool m_bByPass;
	bool m_bAllCancel;
};
#endif
