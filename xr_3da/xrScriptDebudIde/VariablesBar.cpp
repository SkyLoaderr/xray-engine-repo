// VariablesBar.cpp: implementation of the CVariablesBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ide2.h"
#include "VariablesBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CVariablesBar, CCJControlBar)
	//{{AFX_MSG_MAP(CVariablesBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CVariablesBar::CVariablesBar()
{

}

CVariablesBar::~CVariablesBar()
{

}

int CVariablesBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCJControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_variables.Create (WS_TABSTOP|WS_CHILD|WS_VISIBLE|LVS_REPORT,
		CRect(0,0,0,0), this, IDC_VARIABLES))
	{
		TRACE0("Failed to create Variables ctrl\n");
		return -1;
	}
	SetChild(&m_variables);

	m_variables.ModifyStyleEx(0, WS_EX_STATICEDGE);
	
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Name";
	lvc.cx = 70;
	lvc.fmt = LVCFMT_LEFT;
	m_variables.InsertColumn(0,&lvc);

	lvc.iSubItem = 1;
	lvc.pszText = "Type";
	lvc.cx = 60;
	lvc.fmt = LVCFMT_LEFT;
	m_variables.InsertColumn(1,&lvc);

	lvc.iSubItem = 2;
	lvc.pszText = "Value";
	lvc.cx = 300;
	lvc.fmt = LVCFMT_LEFT;
	m_variables.InsertColumn(2,&lvc);

	return 0;
}

void CVariablesBar::AddVariable(const char *szName, const char *szType, const char *szValue)
{
	int idx = m_variables.InsertItem(m_variables.GetItemCount(), szName);
	m_variables.SetItem(idx, 1, LVIF_TEXT, szType, 0, LVIF_TEXT, LVIF_TEXT, 0);
	m_variables.SetItem(idx, 2, LVIF_TEXT, szValue, 0, LVIF_TEXT, LVIF_TEXT, 0);
}

void CVariablesBar::RemoveAll()
{
	m_variables.DeleteAllItems();
}
