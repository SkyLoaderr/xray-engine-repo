// TexturesListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TexturesList.h"
#include "TexturesListDlg.h"
#include ".\textureslistdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTexturesListDlg dialog



CTexturesListDlg::CTexturesListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTexturesListDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_texList.Reset();
}

void CTexturesListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_TEX_ID, m_editTextureID);
	DDX_Control(pDX, IDC_X, m_editX);
	DDX_Control(pDX, IDC_Y, m_editY);
	DDX_Control(pDX, IDC_EDIT5, m_editWidth);
	DDX_Control(pDX, IDC_EDIT4, m_editHeight);
	DDX_Control(pDX, IDC_EDIT_TEXTURENAME, m_editTexName);
	DDX_Control(pDX, IDC_EDIT2, m_editTexParams);
	DDX_Control(pDX, IDC_BTN_RENAME_FILE, m_btnRename);
	DDX_Control(pDX, IDC_EDIT_NEW_NAME, m_editNewName);
	DDX_Control(pDX, IDC_BUTTON4, m_btnSave);
	DDX_Control(pDX, IDC_STATIC1242345, m_pic);
}

BEGIN_MESSAGE_MAP(CTexturesListDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_ADD, OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_LOAD, OnBnClickedLoad)
	ON_LBN_SELCHANGE(IDC_LIST1, OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BTN_REMOVE, OnBnClickedBtnRemove)
	ON_BN_CLICKED(IDC_BTN_RENAME_FILE, OnBnClickedBtnRenameFile)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BTN_SAVEAS, OnBnClickedBtnSaveas)
	ON_LBN_DBLCLK(IDC_LIST1, OnLbnDblclkList1)
END_MESSAGE_MAP()


// CTexturesListDlg message handlers

BOOL CTexturesListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTexturesListDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();	
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTexturesListDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTexturesListDlg::OnBnClickedBtnAdd()
{
	CString str;
	CString name;
	int x, y, width, height;
	m_editTextureID.GetWindowText(str);

    CMyTexture t;
	if (m_texList.Get(t, str))
	{
		CString msg = str + " already exist. Do you want to replace it?";
		if (IDOK == AfxMessageBox(msg, MB_OKCANCEL))
		{
			m_texList.Del(str);
		}
		else
            return;
	}

	name = str;
	if (str.IsEmpty())
	{
		AfxMessageBox("Name field is emty");
		return;
	}
	m_editX.GetWindowText(str);
	if (str.IsEmpty())
	{
		AfxMessageBox("X field is emty");
		return;
	}
	x = atoi(str.GetBuffer());
	m_editY.GetWindowText(str);
	if (str.IsEmpty())
	{
		AfxMessageBox("Y field is emty");
		return;
	}
	y = atoi(str.GetBuffer());
	m_editWidth.GetWindowText(str);
	if (str.IsEmpty())
	{
		AfxMessageBox("WIDTH field is emty");
		return;
	}
	width = atoi(str.GetBuffer());
	m_editHeight.GetWindowText(str);
	if (str.IsEmpty())
	{
		AfxMessageBox("HEIGHT field is emty");
		return;
	}
	height = atoi(str.GetBuffer());
	m_texList.Add(name, x, y, width, height);
	UpdateList();

	m_editTextureID.SetWindowText("");
	m_editX.SetWindowText("");
	m_editY.SetWindowText("");
	m_editWidth.SetWindowText("");
	m_editHeight.SetWindowText("");
}
void CTexturesListDlg::UpdateList(){
	m_list.ResetContent();
	for (unsigned int i = 0; i <m_texList.m_list.size(); i++)
	{
		m_list.AddString(m_texList.m_list[i].m_name.GetBuffer());
	}
}

void CTexturesListDlg::OnBnClickedLoad()
{
	char szFilters[]=
		"XML Files (*.xml)|*.xml|All Files (*.*)|*.*||";

	CFileDialog fileDlg (TRUE, "xml", "*.xml",
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);


	if( fileDlg.DoModal ()==IDOK )
	{
		CString pathName = fileDlg.GetPathName();

		m_texList.ParseFile(pathName);
		this->m_editTexName.SetWindowText(m_texList.m_fileName);
		UpdateList();
		m_btnSave.EnableWindow();
	}
}

void CTexturesListDlg::UpdatePicture(){
	int cur_sel = m_list.GetCurSel();

	if (cur_sel == 1)
	{

	}
	else
	{

	}
}

void CTexturesListDlg::OnLbnSelchangeList1()
{
	int cur_sel = m_list.GetCurSel();
	char buff[16];
	CString str;
	m_list.GetText(cur_sel, str);
	CMyTexture texture;
	m_texList.Get(texture, str);

//	str = texture.m_name + " ";
	str = "x=";
	str += itoa(texture.x, buff, 10);
	str += ", y=";
	str += itoa(texture.y, buff, 10);
	str += ", width=";
	str += itoa(texture.width, buff, 10);
	str += ", height=";
	str += itoa(texture.height, buff, 10);

	m_editTexParams.SetWindowText(str);

	UpdatePicture();
}

void CTexturesListDlg::OnBnClickedBtnRemove()
{
	int cur_sel = m_list.GetCurSel();
	CString str;
	m_list.GetText(cur_sel, str);

	m_texList.Del(str);
	UpdateList();
}

void CTexturesListDlg::OnBnClickedBtnRenameFile()
{
	CString str;
	m_editNewName.GetWindowText(str);
	if (str.IsEmpty())
	{
		AfxMessageBox("Empty name");
		return;
	}
	m_texList.m_fileName = str;
	m_editTexName.SetWindowText(str);
	m_editNewName.SetWindowText("");
	
}

void CTexturesListDlg::OnBnClickedButton4()
{
	CString str;
	if (!m_texList.SaveFile(str))
	{
		AfxMessageBox("Can't save file");
		return;
	}
}

void CTexturesListDlg::OnBnClickedBtnSaveas()
{
	char szFilters[]=
		"XML Files (*.xml)|*.xml|All Files (*.*)|*.*||";

	CFileDialog fileDlg (FALSE, "xml", "*.xml",
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, NULL);


	if( fileDlg.DoModal () == IDOK )
	{
		CString pathName = fileDlg.GetPathName();	  

		m_texList.SaveFile(pathName);
		this->m_btnSave.EnableWindow();
	}
}
void CTexturesListDlg::OnLbnDblclkList1()
{
	int cur_sel = m_list.GetCurSel();
	char buff[16];
	CString str;
	m_list.GetText(cur_sel, str);
	m_editTextureID.SetWindowText(str);
	CMyTexture texture;
	m_texList.Get(texture, str);

	//	str = texture.m_name + " ";
	str = itoa(texture.x, buff, 10);
	m_editX.SetWindowText(str);
	str = itoa(texture.y, buff, 10);
	m_editY.SetWindowText(str);
	str = itoa(texture.width, buff, 10);
	m_editWidth.SetWindowText(str);
	str = itoa(texture.height, buff, 10);
	m_editHeight.SetWindowText(str);
//	m_editTexParams.SetWindowText(str);
}
