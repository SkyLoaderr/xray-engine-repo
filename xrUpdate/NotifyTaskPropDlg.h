#pragma once

class CTaskNotification;
class CTask;
// CNotifyTaskPropDlg dialog

class CNotifyTaskPropDlg : public CDialog
{
	DECLARE_DYNAMIC(CNotifyTaskPropDlg)

public:
	CNotifyTaskPropDlg(LPCTSTR lpszTemplateName, CWnd* pParent = NULL);   // standard constructor
	virtual ~CNotifyTaskPropDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_NOTIFY_TASK };

protected:
	CTaskNotification*			m_task;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void	init_(CTask*);
	CString m_text;
	afx_msg void OnEnChangeEditNotifyText();
};
