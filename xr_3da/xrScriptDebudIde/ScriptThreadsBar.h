#pragma once

#pragma once

class CScriptThreadsBar : public CCJControlBar  
{
public:
	void RemoveAll();
	void AddThread(const char* szName);
	CScriptThreadsBar();
	virtual ~CScriptThreadsBar();

protected:
	CCJListCtrl m_scriptThreads;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};
