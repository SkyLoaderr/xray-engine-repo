#pragma once
#include "afxwin.h"


// CLogDlg dialog

class CLogDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogDlg)

public:
	CLogDlg(LPCTSTR lpszTemplateName, CWnd* pParent = NULL);   // standard constructor
	virtual ~CLogDlg();

// Dialog Data
	enum { IDD = IDD_LOG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_list_box;
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnClearAll();
};
