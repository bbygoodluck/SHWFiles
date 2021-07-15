#ifndef SPLITTERMANAGER_H_INCLUDED
#define SPLITTERMANAGER_H_INCLUDED

#include "SplitterManagerImpl.h"
#include "TabManager.h"

class CSplitterManager
{
private:
	explicit CSplitterManager() {};

public:
	~CSplitterManager() {};
	static CSplitterManager *Get();

	bool Init();

	void DoSplitter(const wxSize& _size = wxSize(0,0));
	CTabManager* GetActiveTab() { return m_pActiveTab; }
	CTabManager* GetAnotherTab();

	CTabManager* GetLeftTab()
	{
		if(m_pTabLeft.get())
			return m_pTabLeft.get();

		return nullptr;
	}

	CTabManager* GetRightTab()
	{
		if(m_pTabRight.get())
			return m_pTabRight.get();

		return nullptr;
	}

	void SetDefaultActiveTab();
	void SetActivatePage();
	void ChangeActiveTab();

	CSplitterManagerImpl* GetSplitter()
	{
		if(m_pSplitterImpl.get())
			return m_pSplitterImpl.get();

		return nullptr;
	}

	void ChangeTabPagePathName(const wxString& strPath);
	void AddTabPage();

private:
	void CreateTabManager();
	void DoCreateSplitTab();

private:
	static CSplitterManager* m_pSplitterMngInstance;

	std::unique_ptr<CSplitterManagerImpl> m_pSplitterImpl = nullptr;
	std::unique_ptr<CTabManager> m_pTabLeft = nullptr;
	std::unique_ptr<CTabManager> m_pTabRight = nullptr;

	CTabManager* m_pActiveTab = nullptr;
};

#endif // SPLITTERMANAGER_H_INCLUDED
