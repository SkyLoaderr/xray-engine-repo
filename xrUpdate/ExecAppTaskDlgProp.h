#pragma once


// CExecAppTaskDlgProp dialog
class CTaskExecute;
class CTask;

class CExecAppTaskDlgProp : public CDialog
{
	DECLARE_DYNAMIC(CExecAppTaskDlgProp)

public:
	CExecAppTaskDlgProp(LPCTSTR lpszTemplateName, CWnd* pParent = NULL);   // standard constructor
	virtual ~CExecAppTaskDlgProp();

// Dialog Data
	enum { IDD = IDD_DIALOG_EXEC_PROCESS };

protected:
	CTaskExecute*			m_task;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void	init_(CTask*);
	afx_msg void OnEnChangeEditAppName();
	afx_msg void OnEnChangeEditAppParams();
	afx_msg void OnEnChangeEditWrkFolder();
	afx_msg void OnBnClickedBtnSelApp();
	afx_msg void OnBnClickedBtnSelWrkFolder();
	CString m_app_name;
	CString m_params;
	CString m_wrk_folder;
};
