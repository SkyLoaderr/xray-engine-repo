#include "stdafx.h"
#include "ide2.h"
#include "ScriptThreadsBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CScriptThreadsBar, CCJControlBar)
	//{{AFX_MSG_MAP(CVariablesBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CScriptThreadsBar::CScriptThreadsBar()
{}

CScriptThreadsBar::~CScriptThreadsBar()
{}

int CScriptThreadsBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCJControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_scriptThreads.Create (WS_TABSTOP|WS_CHILD|WS_VISIBLE|LVS_REPORT,
		CRect(0,0,0,0), this, IDC_THREADS))
	{
		TRACE0("Failed to create Threads Bar ctrl\n");
		return -1;
	}
	SetChild(&m_scriptThreads);

	m_scriptThreads.ModifyStyleEx(0, WS_EX_STATICEDGE);

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Thread Name";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
	m_scriptThreads.InsertColumn(0,&lvc);
/*
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
*/
	return 0;
}

void CScriptThreadsBar::AddThread(const char *szName)
{
	int idx = m_scriptThreads.InsertItem(m_scriptThreads.GetItemCount(), szName);
//	m_variables.SetItem(idx, 1, LVIF_TEXT, szType, 0, LVIF_TEXT, LVIF_TEXT, 0);
//	m_variables.SetItem(idx, 2, LVIF_TEXT, szValue, 0, LVIF_TEXT, LVIF_TEXT, 0);
}

void CScriptThreadsBar::RemoveAll()
{
	m_scriptThreads.DeleteAllItems();
}
