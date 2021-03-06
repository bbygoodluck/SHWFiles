#include "../ginc.h"
#include "Compress.h"
#include "CompressImpl.h"
#include "CompressInclude.h"

std::unique_ptr<CCompress> CCompress::m_pInstance = nullptr;
CCompress* CCompress::Get()
{
	if (m_pInstance.get() == nullptr)
	{
		m_pInstance.reset(new CCompress());
		m_pInstance->Init();
	}

	return m_pInstance.get();
}

void CCompress::Init()
{
	m_vCompress.emplace_back(wxT("zip"));
	m_vCompress.emplace_back(wxT("gz"));
}

bool CCompress::IsCompressedFile(const wxString& strExt)
{
	bool bCompressedFile = false;
	wxString strExtItem(strExt);
	for(auto item : m_vCompress)
	{
		if(theCommonUtil->Compare(item.MakeLower(), strExtItem.MakeLower()) == 0)
		{
			bCompressedFile = true;
			break;
		}
	}

	return bCompressedFile;
}

bool CCompress::SetCompressInfo(const wxString& strFullPath, const wxString& strCompressedFile)
{
	std::vector<wxString> vecDatas;
	vecDatas.emplace_back(strFullPath);

	return SetCompressInfo(vecDatas, strCompressedFile);
}

bool CCompress::SetCompressInfo(const std::vector<wxString>& vecDatas, const wxString& strCompressedFile)
{
	m_bDeCompress = false;
	m_strCompressedFile = strCompressedFile;
	m_vecCompressingDatas = vecDatas;

	if(!CreateCompressImpl())
		return false;

	return true;
}

bool CCompress::SetUnCompressedInfo(const wxString& strCompressedFile, const wxString& strDeCompressDir)
{
	m_bDeCompress = true;
	m_strCompressedFile = strCompressedFile;
	m_strDeCompressDir = strDeCompressDir;
	if(!CreateCompressImpl())
		return false;

	return true;
}

CCompressImpl* CCompress::GetCompressImpl()
{
	return m_pCompressImpl;
}

bool CCompress::CreateCompressImpl()
{
	wxString strExt = theCommonUtil->GetExt(m_strCompressedFile);
	m_pCompressType = COMPTYPE_NONE;
	if(theCommonUtil->Compare(strExt, wxT("zip")) == 0)
		m_pCompressType = m_bDeCompress ? COMPTYPE_UNZIP : COMPTYPE_ZIP;

	if(m_pCompressImpl)
		return false;

	switch(m_pCompressType)
	{
		case COMPTYPE_ZIP:
			m_pCompressImpl = new CZipFileImpl();
			break;

		case COMPTYPE_UNZIP:
			m_pCompressImpl = new CUnZipFileImpl();
			break;
		default:
			break;
	}

	if(!m_pCompressImpl)
		return false;

	return true;
}

bool CCompress::DoStart(wxDialog* pOwnerDlg)
{
	bool bRet = true;
	m_pCompressImpl->SetOwnerDialog(pOwnerDlg);

	if(m_bDeCompress)
	{
		bRet = m_pCompressImpl->DoDeCompress();
		return bRet;
	}
	//????????????
	bRet = m_pCompressImpl->DoCompress();
	return bRet;
}
//????????? ??????
void CCompress::ClearCompressInfo()
{	//????????? ??????
	if(!m_bDeCompress)
	{	//???????????? .zip ?????? ??????
		if(m_bCancel)
			wxRemoveFile(m_strCompressedFile);
	}
	//?????? ??????(?????? ????????????) ?????????
	if(m_pCompressImpl->GetThread() && m_pCompressImpl->GetThread()->IsRunning())
		m_pCompressImpl->GetThread()->Wait();
	//????????? ??????
	if(m_pCompressImpl)
		delete m_pCompressImpl;
	//?????????
	m_pCompressImpl = nullptr;
	m_bCancel = false;
	m_bAllDeCompressSame = false;
	m_DeCompressType = COMPTYPE_NONE;
}

void CCompress::SetLock()
{
	lock.getCondition()->Wait();
}

void CCompress::SetRelease()
{
	lock.getCondition()->Signal();
}
