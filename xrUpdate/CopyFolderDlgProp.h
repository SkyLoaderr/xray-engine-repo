#pragma once


// CCopyFolderDlgProp dialog
class CTask;
class CTaskCopyFolder;

class CCopyFolderDlgProp : public CDialog
{
	DECLARE_DYNAMIC(CCopyFolderDlgProp)

public:
	CCopyFolderDlgProp(LPCTSTR lpszTemplateName, CWnd* pParent = NULL);   // standard constructor
	virtual ~CCopyFolderDlgProp();

// Dialog Data
	enum { IDD = IDD_DIALOG_CPY_FOLDER };


	void	init_(CTask*);

protected:
	CTaskCopyFolder*			m_task;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_src_folder;
	CString m_target_folder;
	afx_msg void OnEnChangeEditSrcFolder2();
	afx_msg void OnEnChangeEditDstFolder2();
	afx_msg void OnBnClickedBtnSelSrc2();
	afx_msg void OnBnClickedBtnSelDst2();
};
