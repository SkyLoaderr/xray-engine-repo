// LogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "xrUpdate.h"
#include "LogDlg.h"
#include ".\logdlg.h"


// CLogDlg dialog

IMPLEMENT_DYNAMIC(CLogDlg, CDialog)
CLogDlg::CLogDlg(LPCTSTR lpszTemplateName, CWnd* pParent)
	: CDialog(lpszTemplateName, pParent)
{
}

CLogDlg::~CLogDlg()
{
}

void CLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_list_box);
}


BEGIN_MESSAGE_MAP(CLogDlg, CDialog)
	ON_WM_SIZING()
	ON_COMMAND(ID_MENU_CLEARALL,  OnClearAll)
END_MESSAGE_MAP()


// CLogDlg message handlers

void CLogDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);
	RECT r;
	GetClientRect(&r);
	
	m_list_box.MoveWindow(&r);
	UpdateData();
}

void CLogDlg::OnClearAll()
{
	m_list_box.ResetContent();
}