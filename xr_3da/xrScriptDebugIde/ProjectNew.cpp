// ProjectNew.cpp : implementation file
//

#include "stdafx.h"
#include "ide2.h"
#include "ProjectNew.h"
#include "Project.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectNew dialog


CProjectNew::CProjectNew(CWnd* pParent /*=NULL*/)
	: CDialog(CProjectNew::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjectNew)
	m_strName = _T("");
	m_strProjectDir = _T("");
	//}}AFX_DATA_INIT
}


void CProjectNew::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectNew)
	DDX_Control(pDX, IDC_PROJECT_TYPES, m_types);
	DDX_Text(pDX, IDC_PROJECT_NAME, m_strName);
	DDX_Text(pDX, IDC_PROJECT_LOCATION, m_strProjectDir);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProjectNew, CDialog)
	//{{AFX_MSG_MAP(CProjectNew)
	ON_BN_CLICKED(IDC_PROJECT_SELLOC, OnProjectSelloc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectNew message handlers
static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData) 
{
	TCHAR szDir[MAX_PATH];

	switch(uMsg) 
	{
		case BFFM_INITIALIZED: 
			if (GetCurrentDirectory(sizeof(szDir)/sizeof(TCHAR), szDir)) 
			{
				// WParam is TRUE since you are passing a path.
				// It would be FALSE if you were passing a pidl.
				SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)szDir);
			}
			break;
		case BFFM_SELCHANGED: 
			// Set the status window to the currently selected path.
			if (SHGetPathFromIDList((LPITEMIDLIST) lp ,szDir))
				SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir);
			break;
		default:
			break;
	}
	
	return 0;
}

void CProjectNew::OnOK() 
{
	UpdateData(TRUE);

	if ( m_strName.IsEmpty() )
	{
		AfxMessageBox("Project name cannot be empty string.");
		return;
	}

	if ( PathIsRelative(m_strProjectDir) )
	{
		AfxMessageBox("Path to project directory must be absolute.");
		return;
	}

	if ( GetFileAttributes(m_strProjectDir)!=FILE_ATTRIBUTE_DIRECTORY &&
		!CreateDirectory(m_strProjectDir, NULL) )
	{
		AfxMessageBox("Cannot create project directory.");
		return;
	}
	
	int nItem = m_types.GetNextItem(-1, LVNI_SELECTED|LVNI_ALL);
	if ( nItem==-1 )
	{
		AfxMessageBox("Select project type.");
		return;
	}

	m_nType = m_types.GetItemData(nItem);

	CDialog::OnOK();
}

CString CProjectNew::GetProjectPathName()
{
	return m_strProjectDir + "\\" + m_strName + ".lpr";
}

CString CProjectNew::GetProjectName()
{
	return m_strName;
}

void CProjectNew::OnProjectSelloc() 
{
	UpdateData(TRUE);

	BROWSEINFO bi;
	TCHAR szDir[MAX_PATH];
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;

	if (SUCCEEDED(SHGetMalloc(&pMalloc))) 
	{
		ZeroMemory(&bi,sizeof(bi));
		bi.hwndOwner = NULL;
		bi.pszDisplayName = 0;
		bi.pidlRoot = 0;
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
		bi.lpfn = BrowseCallbackProc;

		pidl = SHBrowseForFolder(&bi);
		if (pidl) 
		{
			if (SHGetPathFromIDList(pidl,szDir)) 
			{
				m_strProjectDir = szDir;
				UpdateData(FALSE);
			}

			// In C++: pMalloc->Free(pidl); pMalloc->Release();
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
	}	
}

void CProjectNew::CreateByType(CProject *pProject)
{
}

BOOL CProjectNew::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_types.ModifyStyleEx(0, WS_EX_STATICEDGE);
	
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Type";
	lvc.cx = 70;
	lvc.fmt = LVCFMT_LEFT;
	m_types.InsertColumn(0,&lvc);

	int nItem = m_types.InsertItem(m_types.GetItemCount(), "Empty Project");
	m_types.SetItemData(nItem, 0);
	nItem = m_types.InsertItem(m_types.GetItemCount(), "Hello World Project");
	m_types.SetItemData(nItem, 1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
