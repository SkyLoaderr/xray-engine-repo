// SectionQueryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "xrUpdate.h"
#include "SectionQueryDlg.h"


// CSectionQueryDlg dialog

IMPLEMENT_DYNAMIC(CSectionQueryDlg, CDialog)
CSectionQueryDlg::CSectionQueryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSectionQueryDlg::IDD, pParent)
	, m_section_name(_T(""))
	, m_task_name(_T(""))
{
}

CSectionQueryDlg::~CSectionQueryDlg()
{
}

void CSectionQueryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SECTION, m_section_name);
	DDX_Text(pDX, IDC_EDIT_SECTION2, m_task_name);
}


BEGIN_MESSAGE_MAP(CSectionQueryDlg, CDialog)
END_MESSAGE_MAP()


// CSectionQueryDlg message handlers
