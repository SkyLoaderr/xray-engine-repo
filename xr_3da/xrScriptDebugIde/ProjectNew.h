#if !defined(AFX_PROJECTNEW_H__D316F089_267A_4279_83F8_3A36D0408563__INCLUDED_)
#define AFX_PROJECTNEW_H__D316F089_267A_4279_83F8_3A36D0408563__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProjectNew.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProjectNew dialog

class CProject;

class CProjectNew : public CDialog
{
// Construction
public:
	void CreateByType(CProject* pProject);
	CString GetProjectName();
	CString GetProjectPathName();
	CProjectNew(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProjectNew)
	enum { IDD = IDD_NEW_PROJECT };
	CCJListCtrl	m_types;
	CString	m_strName;
	CString	m_strProjectDir;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectNew)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nType;

	// Generated message map functions
	//{{AFX_MSG(CProjectNew)
	virtual void OnOK();
	afx_msg void OnProjectSelloc();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECTNEW_H__D316F089_267A_4279_83F8_3A36D0408563__INCLUDED_)
