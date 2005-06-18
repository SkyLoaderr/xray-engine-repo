// NotifyTaskPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "xrUpdate.h"
#include "NotifyTaskPropDlg.h"
#include "upd_task.h"
#include ".\notifytaskpropdlg.h"

// CNotifyTaskPropDlg dialog
IMPLEMENT_DYNAMIC(CNotifyTaskPropDlg, CDialog)

CNotifyTaskPropDlg::CNotifyTaskPropDlg(LPCTSTR lpszTemplateName, CWnd* pParent)   // standard constructor
	: CDialog(lpszTemplateName, pParent),
	m_text(_T(""))
{}

CNotifyTaskPropDlg::~CNotifyTaskPropDlg()
{
}

void CNotifyTaskPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NOTIFY_TEXT, m_text);

	if(pDX->m_bSaveAndValidate){//TRUE== controls->members
		m_task->setText(m_text.GetBuffer());
	}
}

BEGIN_MESSAGE_MAP(CNotifyTaskPropDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT_NOTIFY_TEXT, OnEnChangeEditNotifyText)
END_MESSAGE_MAP()


void CNotifyTaskPropDlg::init_(CTask* t)
{
	m_task = dynamic_cast<CTaskNotification*>(t);
	m_text	= m_task->text();
	UpdateData(FALSE);
}


void CNotifyTaskPropDlg::OnEnChangeEditNotifyText()
{
	UpdateData(TRUE);
}
