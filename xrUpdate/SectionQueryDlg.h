#pragma once


// CSectionQueryDlg dialog

class CSectionQueryDlg : public CDialog
{
	DECLARE_DYNAMIC(CSectionQueryDlg)

public:
	CSectionQueryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSectionQueryDlg();

// Dialog Data
	enum { IDD = IDD_DLG_SECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_section_name;
	CString m_task_name;
};
