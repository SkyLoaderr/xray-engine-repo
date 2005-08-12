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

extern void	Surface_Init();

CTexturesListDlg::CTexturesListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTexturesListDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_texList.Reset();

	m_textureRect.SetRect(500, 50, 800, 300);
	m_cur_tex_rect.SetRect(0,0,0,0);
}

CTexturesListDlg::~CTexturesListDlg(){
	Core._destroy();
//	FlushLog();
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
	DDX_Control(pDX, IDC_CHECK1, m_checkFitImage);
	DDX_Control(pDX, IDC_EDIT1, m_editSearch);
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
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
END_MESSAGE_MAP()


// CTexturesListDlg message handlers

BOOL CTexturesListDlg::OnInitDialog()
{
	Surface_Init();	
	Core._initialize("xr",0,TRUE,"fs_tex_design.ltx");
	Msg("Core Init");
	FlushLog();
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
void DrawBitmap(HDC hdc, const CRect& r, u32* data, u32 w, u32 h,  const CRect& source);
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
		PAINTSTRUCT ps;
		CDC* pDC = BeginPaint(&ps);
		pDC->Rectangle(m_textureRect);
		if (m_texture.pData && m_cur_tex_rect.Width() && m_cur_tex_rect.Height())
			DrawBitmap(pDC->GetSafeHdc(),GetDestRect(),m_texture.pData, m_texture.dwWidth, m_texture.dwHeight, m_cur_tex_rect);
		EndPaint(&ps);
		CDialog::OnPaint();
	}
}


CRect CTexturesListDlg::GetDestRect(){
	CRect dest_rect;

	dest_rect = m_textureRect;
	
	if (m_checkFitImage.GetCheck())
	{
		float scale_ratio;
		float rw = m_textureRect.Width()/float(m_cur_tex_rect.Width());
		float rh = m_textureRect.Height()/float(m_cur_tex_rect.Height());
		if ( rw < rh)
			scale_ratio = rw;
		else
			scale_ratio = rh;

		dest_rect.bottom = dest_rect.top + int(m_cur_tex_rect.Height()*scale_ratio);
		dest_rect.right = dest_rect.left + int(m_cur_tex_rect.Width()*scale_ratio);
	}
	else
	{
		dest_rect.bottom = dest_rect.top + m_cur_tex_rect.Height();
		dest_rect.right = dest_rect.left + m_cur_tex_rect.Width();
	}
    return dest_rect;
}


void DrawBitmap(HDC hdc, const CRect& r, u32* _data, u32 _w, u32 _h, const CRect& source)
{
	U32Vec data(source.Width()*source.Height());

	for (u32 y = source.top; y<source.bottom; y++)
		Memory.mem_copy(&data[(y - source.top)*source.Width()],&_data[(source.bottom - (y - source.top) -1 )*_w + source.left],source.Width()*4);

	BITMAPINFO          bmi;
	bmi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth			= source.Width();
	bmi.bmiHeader.biHeight			= source.Height();
	bmi.bmiHeader.biPlanes			= 1;
	bmi.bmiHeader.biBitCount		= 32;
	bmi.bmiHeader.biCompression		= BI_RGB;
	bmi.bmiHeader.biSizeImage		= 0;
	bmi.bmiHeader.biXPelsPerMeter	= 0;
	bmi.bmiHeader.biYPelsPerMeter	= 0;
	bmi.bmiHeader.biClrUsed			= 0;
	bmi.bmiHeader.biClrImportant	= 0;

	SetMapMode(hdc,MM_ANISOTROPIC);
	SetStretchBltMode(hdc,HALFTONE);
	int err=StretchDIBits (hdc,r.left,r.top,r.Width(),r.Height(),0,0,source.Width(),source.Height(),&*data.begin(), &bmi,DIB_RGB_COLORS,SRCCOPY);
	if (err==GDI_ERROR)
		Log("!StretchDIBits - Draw failed.");
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
	m_list.SelectString(0,name);
	OnLbnSelchangeList1();
	//UpdatePicture();

	m_editTextureID.SetWindowText("");
	m_editX.SetWindowText("");
	m_editY.SetWindowText("");
	m_editWidth.SetWindowText("");
	m_editHeight.SetWindowText("");
}


void CTexturesListDlg::OnEnChangeEdit1()
{
	UpdateList();
}

void CTexturesListDlg::UpdateList(){
	m_list.SetRedraw(FALSE);
	m_list.ResetContent();
	for (unsigned int i = 0; i <m_texList.m_list.size(); i++)
		m_list.AddString(m_texList.m_list[i].m_name.GetBuffer());
	CString txt;
	CString text;
	m_editSearch.GetWindowText(txt);
	bool flag;
	do {
		int sz = m_list.GetCount();
		bool fl = true;
		for (int i=0; i<sz; i++)
		{
			m_list.GetText(i,text);
			int pos = text.Find(_T(txt));
			if (-1 == pos)
			{
				m_list.DeleteString(i);
				fl = false;
				break;
			}
		}
		flag = fl;
	}while (!flag);

	m_list.SetRedraw(TRUE);
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
		xrLoadTexture();
		UpdateList();
		m_btnSave.EnableWindow();
	}
}

void CTexturesListDlg::UpdatePicture(){
	InvalidateRect(m_textureRect);
}

void CTexturesListDlg::OnLbnSelchangeList1()
{
	int cur_sel = m_list.GetCurSel();
	char buff[16];
	CString str;
	m_list.GetText(cur_sel, str);
	CMyTexture texture;
	m_texList.Get(texture, str);

	m_cur_tex_rect.SetRect(texture.x,texture.y,texture.x + texture.width, texture.y+texture.height);

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
	m_editTextureID.SetWindowText("");
	m_editX.SetWindowText("");
	m_editY.SetWindowText("");
	m_editWidth.SetWindowText("");
	m_editHeight.SetWindowText("");

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
	xrLoadTexture();
	
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


BOOL	Surface_Detect(LPSTR F, LPSTR N);

void CTexturesListDlg::xrLoadTexture(){
	CString tex;
	m_editTexName.GetWindowText(tex);
	tex.Replace(_T("ui\\"),_T(""));
	string256		full;
	string256		nm;
	strcpy(nm,tex);
	if (Surface_Detect(full,nm)) 
	{
		Msg("Load Texture %s", full);
		FlushLog();
		m_texture.LoadTGA(full);
		InvalidateRect(m_textureRect);
	}
	else
	{
		Msg("Can't find texture %s",nm);
		FlushLog();
	}
}

void CTexturesListDlg::OnBnClickedCheck1()
{
	InvalidateRect(m_textureRect);

}
