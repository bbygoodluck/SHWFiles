#include "common.h"
#include "SpecialFolderManager.h"

std::unique_ptr<CSpecialFolderManager> CSpecialFolderManager::m_pInstanceSPFoder(nullptr);

CSpecialFolderManager* CSpecialFolderManager::Get()
{
	if (m_pInstanceSPFoder == nullptr)
		m_pInstanceSPFoder.reset(new CSpecialFolderManager());

	m_pInstanceSPFoder->Init();
	return m_pInstanceSPFoder.get();
}

void CSpecialFolderManager::Init()
{
	typedef struct stCSIDL {
		int iCSIDL;
	} CSIDL_LIST;

	CSIDL_LIST csidlList[] = {
		CSIDL_WINDOWS,
		CSIDL_PROGRAM_FILES,
		CSIDL_PROGRAM_FILESX86,
		CSIDL_MYPICTURES,
		CSIDL_LOCAL_APPDATA,
		CSIDL_FAVORITES,
		CSIDL_DESKTOPDIRECTORY,
		CSIDL_COMMON_PROGRAMS,
		CSIDL_BITBUCKET
	//	CLSID_RecycleBin
	};

	size_t icount = WXSIZEOF(csidlList);
	for(size_t i = 0; i < icount; i++)
	{
		if(csidlList[i].iCSIDL == CSIDL_BITBUCKET)
		{
			LPSHELLFOLDER pDesktop = nullptr;
			LPSHELLFOLDER pRecycleBin = nullptr;
			LPITEMIDLIST pidlRecycleBin = nullptr;
			STRRET strRet;

			HRESULT hr = S_OK;

			hr = SHGetDesktopFolder(&pDesktop);
			hr = SHGetSpecialFolderLocation(_gMainFrame->GetHWND(), CSIDL_BITBUCKET, &pidlRecycleBin);
			hr = pDesktop->BindToObject(pidlRecycleBin, NULL, __uuidof(IShellFolder), (LPVOID *)&pRecycleBin);
			pDesktop->GetDisplayNameOf(pidlRecycleBin, SHGDN_NORMAL, &strRet);

			LPMALLOC pMalloc = NULL;
			TCHAR szPath[MAX_PATH];

			SHGetMalloc(&pMalloc);
			wxZeroMemory(szPath);

			wxString strPath;
			switch(strRet.uType)
			{
				case STRRET_CSTR:
					//_tcscpy(szPath, strRet.cStr);
					strPath = strRet.cStr;
					break;

				case STRRET_WSTR:
				//	WideCharToMultiByte (CP_ACP, 0, strRet.pOleStr, -1, szPath, sizeof (szPath), NULL, NULL);
					wxString strTmp(strRet.pOleStr);
					strPath = strTmp;
					pMalloc->Free (strRet.pOleStr);
					break;
			}

			pMalloc->Release();
		}
		else
		{
			TCHAR tPath[MAX_PATH];
			SHGetFolderPath(NULL, csidlList[i].iCSIDL, NULL, 0, tPath);

			wxString strPath(tPath);
			std::unordered_map<int, wxString>::value_type val(csidlList[i].iCSIDL, strPath);

			m_spFolderMap.insert(val);
		}
	}
}

wxString CSpecialFolderManager::GetSpecialFolder(int _CLSID)
{
	std::unordered_map<int, wxString>::const_iterator fIter = m_spFolderMap.find(_CLSID);
	if(fIter == m_spFolderMap.end())
		return wxT("");

	return fIter->second;
}
