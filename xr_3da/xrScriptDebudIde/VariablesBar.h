// VariablesBar.h: interface for the CVariablesBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VARIABLESBAR_H__233488EE_2350_4228_B266_DA29C45596BA__INCLUDED_)
#define AFX_VARIABLESBAR_H__233488EE_2350_4228_B266_DA29C45596BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

#endif // !defined(AFX_VARIABLESBAR_H__233488EE_2350_4228_B266_DA29C45596BA__INCLUDED_)
