#ifndef SPECIALFOLDERMANAGER_H_INCLUDED
#define SPECIALFOLDERMANAGER_H_INCLUDED

class CSpecialFolderManager
{
protected:

private:
	CSpecialFolderManager() {};

public:
	~CSpecialFolderManager() {};

	static CSpecialFolderManager* Get();
	void Init();

	wxString GetSpecialFolder(int _CLSID);

private:
	static std::unique_ptr<CSpecialFolderManager> m_pInstanceSPFoder;
	std::unordered_map<int, wxString> m_spFolderMap;

};

#endif // SPECIALFOLDERMANAGER_H_INCLUDED
