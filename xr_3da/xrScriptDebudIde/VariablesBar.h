#pragma once

class CVariablesBar : public CCJControlBar  
{
public:
	void RemoveAll();
	void AddVariable(const char* szName, const char* szType, const char* szValue);
	CVariablesBar();
	virtual ~CVariablesBar();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVariablesBar)
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	CCJListCtrl m_variables;

	//{{AFX_MSG(CVariablesBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
