// NvTriStripDlg.cpp : implementation file
//
#include "stdafx.h"
#include "NvTriStrip.h"
#include "NvTriStripDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
// No message handlers
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNvTriStripDlg dialog

CNvTriStripDlg::CNvTriStripDlg(CWnd* pParent /*=NULL*/)
: CDialog(CNvTriStripDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNvTriStripDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNvTriStripDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNvTriStripDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNvTriStripDlg, CDialog)
//{{AFX_MSG_MAP(CNvTriStripDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_LOADBUTTON, OnLoadButton)
ON_BN_CLICKED(IDC_SAVEBUTTON, OnSavebutton)
ON_WM_HSCROLL()
ON_BN_CLICKED(IDC_RAD_VIZNONE, OnRadViznone)
ON_BN_CLICKED(IDC_RAD_VIZCACHE, OnRadVizcache)
ON_BN_CLICKED(IDC_RAD_VIZSTRIPS, OnRadVizstrips)
ON_BN_CLICKED(IDC_BT_STRIPIFY, OnBtStripify)
ON_WM_TIMER()
ON_WM_CLOSE()
ON_WM_DESTROY()
ON_BN_CLICKED(IDC_WIREFRAME, OnWireframe)
ON_BN_CLICKED(IDC_STITCH, OnStitch)
ON_WM_LBUTTONDOWN()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
ON_WM_KEYDOWN()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNvTriStripDlg message handlers

BOOL CNvTriStripDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Add "About..." menu item to system menu.
	
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	GetDlgItem(IDC_RENDERWINDOW, &m_hD3DWin);
	
	m_pD3DDev                 = NULL;
	m_pD3D                    = NULL;
	m_pDD                     = NULL;
	vertexBuffer              = NULL;
	vertexBufferVizStrips     = NULL;
	vertexBufferVizCache      = NULL;
	vertexBufferVizCacheFaces = NULL;
	vertexBufferFaces         = NULL;
	indices                   = NULL;
	m_pD3DX                   = NULL;
	
	bRendering = true;
	bVBCreated = false;
	bStitch = true;
	
	bMouseDown = false;
	
	renderSetting = RS_NORMAL;
	fillMode = D3DFILL_SOLID;
	
	HRESULT res = InitD3DX();
	if( FAILED(res) )
	{
		MessageBox("Can't init D3DX! Disabling rendering...");
		bRendering = false;
	}
	
	if(bRendering)
	{
		res = InitRenderer();
		if(FAILED(res))
		{
			MessageBox("Can't init renderer! Disabling rendering...");
			bRendering = false;
		}
	}
	
	if(bRendering)
		SetTimer(123, 40, NULL);
	
	CSliderCtrl *pSlider;
	pSlider = (CSliderCtrl*)GetDlgItem(IDC_CACHESLIDER);
	
	pSlider->SetRange(5, 30);
	pSlider->SetPos(16);
	cacheSize = 16;
	
	SetDlgItemText(IDC_EDIT1, "16");
	
	normalButton    = (CButton*)GetDlgItem(IDC_RAD_VIZNONE);
	vizStripsButton = (CButton*)GetDlgItem(IDC_RAD_VIZSTRIPS);
	vizCacheButton  = (CButton*)GetDlgItem(IDC_RAD_VIZCACHE);
	saveButton      = (CButton*)GetDlgItem(IDC_SAVEBUTTON);
	stripifyButton  = (CButton*)GetDlgItem(IDC_BT_STRIPIFY);
	wireframeButton = (CButton*)GetDlgItem(IDC_WIREFRAME);
	stitchButton    = (CButton*)GetDlgItem(IDC_STITCH);
	minStripEdit    = (CEdit*)GetDlgItem(IDC_MINSTRIPLENGTH);

	minStripEdit->SetLimitText(3);
	minStripEdit->SetWindowText("3");
	minStripEdit->EnableWindow(FALSE);
	
	normalButton->SetCheck(1);
	vizStripsButton->SetCheck(0);
	vizCacheButton->SetCheck(0);
	
	normalButton->EnableWindow(FALSE);
	vizStripsButton->EnableWindow(FALSE);
	vizCacheButton->EnableWindow(FALSE);
	
	saveButton->EnableWindow(FALSE);
	stripifyButton->EnableWindow(FALSE);
	
	wireframeButton->SetCheck(0);
	wireframeButton->EnableWindow(FALSE);

	stitchButton->SetCheck(1);
	stitchButton->EnableWindow(FALSE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNvTriStripDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNvTriStripDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
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
		if(bRendering)
			Render();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNvTriStripDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


///////////////////////////////////////////////////////////////////////////////////////////
// ComputeNormal()
//
// Computes normal of input face.  Normal is normalized.
//
void CNvTriStripDlg::ComputeNormal(MyFace& face)
{
	MyVector v1, v2;
	
	v1.x = vertices[face.v1].x - vertices[face.v2].x;
	v1.y = vertices[face.v1].y - vertices[face.v2].y;
	v1.z = vertices[face.v1].z - vertices[face.v2].z;
	
	v2.x = -(vertices[face.v3].x - vertices[face.v1].x);
	v2.y = -(vertices[face.v3].y - vertices[face.v1].y);
	v2.z = -(vertices[face.v3].z - vertices[face.v1].z);
	
	face.nx = v1.y*v2.z - v1.z*v2.y;
	face.ny = -(v1.x*v2.z - v2.x*v1.z);
	face.nz = v1.x*v2.y - v2.x*v1.y;
	
	float magnitude = sqrt(face.nx*face.nx + face.ny*face.ny + face.nz*face.nz);
	
	face.nx = face.nx / magnitude;
	face.ny = face.ny / magnitude;
	face.nz = face.nz / magnitude;
	
}


///////////////////////////////////////////////////////////////////////////////////////////
// SetupMatrices()
//
// Function to center on the mesh that's loaded
//
void CNvTriStripDlg::SetupMatrices(const Extents& e)
{
	float dx = e.maxX - e.minX;
	float dy = e.maxY - e.minY;
	float dz = e.maxZ - e.minZ;
	
	float midX = (dx / 2) + e.minX;
	float midY = (dy / 2) + e.minY;
	float midZ = (dz / 2) + e.minZ;
	
	float maxExtent = (dx > dy) ? dx : dy;
	maxExtent = (maxExtent > dz) ? maxExtent : dz;
	
	//place object at origin
	D3DXMatrixTranslation(&world, -midX, -midY, -midZ);
	
	D3DXVECTOR3 eye;
	D3DXVECTOR3 lookAt;
	D3DXVECTOR3 up;
	
	eye.x    = 0.0f; eye.y    = 0.0f; eye.z    = e.minZ - (1.5f * maxExtent);
	lookAt.x = 0.0f; lookAt.y = 0.0f; lookAt.z = 0.0f;
	up.x     = 0.0f; up.y     = 1.0f; up.z     = 0.0f;
	
	//set view
	
	// I could just use the function:
	//  D3DXMatrixLookAtLH(&view, &eye, &lookAt, &up);
	// but it seems broken under D3DX7
	//
    
	D3DXVECTOR3 viewVec = lookAt - eye;
	D3DXVec3Normalize(&viewVec, &viewVec);
	float dotProd = D3DXVec3Dot(&viewVec, &up);
	D3DXVECTOR3 newUp = up - (dotProd * viewVec);
	D3DXVec3Normalize(&newUp, &newUp);
	D3DXVECTOR3 right;
	D3DXVec3Cross(&right, &newUp, &viewVec);
	
	D3DXMatrixIdentity(&view);
	view.m[0][0] = right.x;  view.m[0][1] = newUp.x;  view.m[0][2] = viewVec.x;
	view.m[1][0] = right.y;  view.m[1][1] = newUp.y;  view.m[1][2] = viewVec.y;
	view.m[2][0] = right.z;  view.m[2][1] = newUp.z;  view.m[2][2] = viewVec.z;
	
	view.m[3][0] = -D3DXVec3Dot(&eye, &right);
	view.m[3][1] = -D3DXVec3Dot(&eye, &newUp);
	view.m[3][2] = -D3DXVec3Dot(&eye, &viewVec);
    
	m_pD3DDev->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *)&world);
	
	m_pD3DDev->SetTransform(D3DTRANSFORMSTATE_VIEW, (D3DMATRIX *)&view);
	
}

////////////////////////////////////////////////////////////////////////////////////////
// This function is named as such because it fixes up a certain ill-tempered .m file
//
// Basically it just handles files where the vertices aren't strictly from 0-numVerts
//
std::vector<int> vertNums;

void CNvTriStripDlg::FixForSpock()
{	
	for(int i = 0; i < faces.size(); i++)
	{
		for(int j = 0; j < vertNums.size(); j++)
		{
			if(faces[i].v1 == vertNums[j])
				faces[i].v1 = j;
			if(faces[i].v2 == vertNums[j])
				faces[i].v2 = j;
			if(faces[i].v3 == vertNums[j])
				faces[i].v3 = j;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
// LoadFile()
//
// Loads .m file from disk, loading info into the global vertices vector and faces vector
//
int CNvTriStripDlg::LoadFile(const char* fileName, Extents& e, bool bBrokenMesh)
{
	FILE* fp;
	
	if((fp = fopen(fileName, "r")) == NULL)
		return -1;
	
	char line[256];
	char* token;
	
	fgets(line, 256, fp);
	
	MyVertex currentVertex;
	MyFace currentFace;
	
	e.maxX = e.maxY = e.maxZ = -1000;
	e.minX = e.minY = e.minZ = 1000;
	
	while(!feof(fp))
	{
		if(line[0] == '#')
		{
			//it's a comment
			fgets(line, 256, fp);
			continue;
		}
		
		else if(line[0] == 'V')
		{
			//it's a vertex
			
			//get Vertex
			token = strtok(line, " ");
			
			//get the number
			token = strtok(NULL, " ");
			vertNums.push_back(atoi(token) - 1);
			//this checks to see if vertices are monotonically increasing by one
			if((vertNums.size() > 1) && 
				(vertNums[vertNums.size()-1] != (vertNums[vertNums.size()-2]+1)) &&
				!bBrokenMesh)
			{
				vertices.clear();
				vertNums.clear();
				fclose(fp);
				return 1;   //return value of 1 indicates broken mesh
			}
			
			//get the vertex info
			token = strtok(NULL, " ");
			currentVertex.x = atof(token);
			
			token = strtok(NULL, " ");
			currentVertex.y = atof(token);
			
			token = strtok(NULL, " ");
			currentVertex.z = atof(token);
			
			if(currentVertex.x > e.maxX)
				e.maxX = currentVertex.x;
			
			if(currentVertex.y > e.maxY)
				e.maxY = currentVertex.y;
			
			if(currentVertex.z > e.maxZ)
				e.maxZ = currentVertex.z;
			
			if(currentVertex.x < e.minX)
				e.minX = currentVertex.x;
			
			if(currentVertex.y < e.minY)
				e.minY = currentVertex.y;
			
			if(currentVertex.z < e.minZ)
				e.minZ = currentVertex.z;
			
			currentVertex.nx = 0.0f;
			currentVertex.ny = 0.0f;
			currentVertex.nz = 0.0f;
			
			vertices.push_back(currentVertex);
		}
		else if(line[0] == 'F')
		{
			
			//it's a face
			
			//we subtract one from all of these indices because vertices are numbered
			// starting at 1 in .m files, but we index starting at zero
			
			//get Face
			token = strtok(line, " ");
			
			//get number
			token = strtok(NULL, " ");
			
			//get index data
			token = strtok(NULL, " ");
			currentFace.v1 = atoi(token) - 1;
			
			token = strtok(NULL, " ");
			currentFace.v2 = atoi(token) - 1;
			
			token = strtok(NULL, " ");
			currentFace.v3 = atoi(token) - 1;
			
			if(!bBrokenMesh)
			{
				ComputeNormal(currentFace);
				
				vertices[currentFace.v1].nx += currentFace.nx;
				vertices[currentFace.v1].ny += currentFace.ny;
				vertices[currentFace.v1].nz += currentFace.nz;
				
				vertices[currentFace.v2].nx += currentFace.nx;
				vertices[currentFace.v2].ny += currentFace.ny;
				vertices[currentFace.v2].nz += currentFace.nz;
				
				vertices[currentFace.v3].nx += currentFace.nx;
				vertices[currentFace.v3].ny += currentFace.ny;
				vertices[currentFace.v3].nz += currentFace.nz;
			}
			
			faces.push_back(currentFace);
		}
		else if(line[0] == 'E')
		{
			//edge, do nothing
		}
		else
		{
			
		}
		
		fgets(line, 256, fp);	
  }
  fclose(fp);
  
  if(bBrokenMesh)
  {
	  FixForSpock();
	  for(int i = 0; i < faces.size(); i++)
	  {
		  ComputeNormal(faces[i]);
		  
		  vertices[faces[i].v1].nx += faces[i].nx;
		  vertices[faces[i].v1].ny += faces[i].ny;
		  vertices[faces[i].v1].nz += faces[i].nz;
		  
		  vertices[faces[i].v2].nx += faces[i].nx;
		  vertices[faces[i].v2].ny += faces[i].ny;
		  vertices[faces[i].v2].nz += faces[i].nz;
		  
		  vertices[faces[i].v3].nx += faces[i].nx;
		  vertices[faces[i].v3].ny += faces[i].ny;
		  vertices[faces[i].v3].nz += faces[i].nz;
	  }
  }
  
  float mag;
  
  for(int i = 0; i < vertices.size(); i++)
  {
	  //normalize normals
	  mag = sqrt(vertices[i].nx*vertices[i].nx + vertices[i].ny*vertices[i].ny + vertices[i].nz*vertices[i].nz);
	  vertices[i].nx /= mag;
	  vertices[i].ny /= mag;
	  vertices[i].nz /= mag;
  }
  
  vertNums.clear();

  return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////
// InitD3DX()
//
// Initializes D3DX
//
HRESULT CNvTriStripDlg::InitD3DX()
{
	HRESULT hr;
	
	hr = D3DXInitialize();
	
	if(FAILED(hr ))
		return hr;
	
	DWORD dwDevice;
	DWORD dwDeviceCount = D3DXGetDeviceCount();
	
	D3DX_DEVICEDESC dev;
	
	dev.deviceIndex = D3DX_DEFAULT;
	dev.hwLevel     = D3DX_DEFAULT;
	dev.onPrimary   = TRUE;
	
	D3DX_DEVICEDESC devDesc;
	
	for(dwDevice = 0; dwDevice < dwDeviceCount; dwDevice++)
	{
		if(FAILED(D3DXGetDeviceDescription(dwDevice, &devDesc)))
			continue;
		
		if(devDesc.hwLevel > dev.hwLevel)
			dev = devDesc;
	}
	
	// D3DX Initialization
	hr = D3DXCreateContextEx(  
		dev.hwLevel,            // D3DX device
		0,						          // flags
		m_hD3DWin,
		NULL,                   // focusWnd
		D3DX_DEFAULT,           // colorbits
		8,						          // alphabits
		D3DX_DEFAULT,           // numdepthbits
		0,                      // numstencilbits
		D3DX_DEFAULT,           // numbackbuffers
		D3DX_DEFAULT,		      	// width
		D3DX_DEFAULT,		      	// height
		D3DX_DEFAULT,           // refresh rate
		&m_pD3DX                // returned D3DX interface
		);
	
	if (FAILED(hr))
		return hr;
	if (m_pD3DX == NULL )
	{
		return E_FAIL;
	}
	
	m_pD3DDev = m_pD3DX->GetD3DDevice();
	if (m_pD3DDev == NULL)
		return E_FAIL;
	
	m_pD3D = m_pD3DX->GetD3D();
	if (m_pD3D == NULL)
		return E_FAIL;
	
	m_pDD = m_pD3DX->GetDD();
	if( m_pDD == NULL )
		return E_FAIL;
	
	return D3D_OK;
}


///////////////////////////////////////////////////////////////////////////////////////////
// InitRenderer()
//
// Initializes the renderer, including render states, matrices, lights, and materials
//
HRESULT CNvTriStripDlg::InitRenderer()
{
	m_pD3DX->SetClearColor(D3DRGBA(0.0f,0.0f,0.0f,0));
	HRESULT hr = m_pD3DX->Clear(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER);
	
	if(FAILED(hr))
		return hr;
	
	ZeroMemory(&m_Material, sizeof(D3DMATERIAL7));
	m_Material.dcvDiffuse.r = (D3DVALUE)1.0f;
	m_Material.dcvDiffuse.g = (D3DVALUE)1.0f;
	m_Material.dcvDiffuse.b = (D3DVALUE)1.0f;
	
	m_Material.dcvAmbient.r = (D3DVALUE)0.0f;
	m_Material.dcvAmbient.g = (D3DVALUE)0.0f;
	m_Material.dcvAmbient.b = (D3DVALUE)0.0f;
	
	m_pD3DDev->SetMaterial(&m_Material);
	if(FAILED(hr))
		MessageBox("failed");
	
	D3DVIEWPORT7 vp;
	m_pD3DDev->GetViewport(&vp);
	float fAspect = (float) vp.dwHeight / (float) vp.dwWidth;
	
	//setup matrix for spinning
	D3DXVECTOR3 axis(0.0f, 1.0f, 0.0f);
	D3DXMatrixRotationAxis(&spin, &axis, D3DXToRadian(1.0f));
	
	D3DXMatrixPerspectiveFovLH(&proj, D3DXToRadian(60.0f), fAspect, 0.1f, 20.0f);
	
	D3DXMatrixIdentity(&world);
	D3DXMatrixIdentity(&view);
	D3DXMatrixIdentity(&mouseSpin);
	
	ZeroMemory(&m_Light0, sizeof(D3DLIGHT7));
	
	// Set up for a white directional light.
	m_Light0.dltType = D3DLIGHT_DIRECTIONAL;
	m_Light0.dcvDiffuse.r = 1.0f;
	m_Light0.dcvDiffuse.g = 1.0f;
	m_Light0.dcvDiffuse.b = 1.0f;
	
	m_Light0.dvDirection.x = 0.0f;
	m_Light0.dvDirection.y = 0.0f;
	m_Light0.dvDirection.z = 1.0f;
	
	hr = m_pD3DDev->SetLight(0, &m_Light0);
	hr = m_pD3DDev->LightEnable(0, TRUE);
	if(FAILED(hr))
		return hr;
	
	ZeroMemory(&m_Light1, sizeof(D3DLIGHT7));
	
	// Set up for a white directional light.
	m_Light1.dltType = D3DLIGHT_DIRECTIONAL;
	m_Light1.dcvDiffuse.r = 1.0f;
	m_Light1.dcvDiffuse.g = 1.0f;
	m_Light1.dcvDiffuse.b = 1.0f;
	
	m_Light1.dvDirection.x = 0.0f;
	m_Light1.dvDirection.y = 0.0f;
	m_Light1.dvDirection.z = -1.0f;
	
	hr = m_pD3DDev->SetLight(1, &m_Light1);
	hr = m_pD3DDev->LightEnable(1, TRUE);
	if(FAILED(hr))
		return hr;
	
	hr = m_pD3DDev->SetTransform(D3DTRANSFORMSTATE_PROJECTION, (D3DMATRIX *)&proj);
	if (FAILED(hr))
		return hr;
	
	hr = m_pD3DDev->SetTransform(D3DTRANSFORMSTATE_VIEW, (D3DMATRIX *)&view);
	if (FAILED(hr))
		return hr;
	
	hr = m_pD3DDev->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *)&world);
	if (FAILED(hr))
		return hr;
	
	m_pD3DDev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
	m_pD3DDev->SetRenderState(D3DRENDERSTATE_AMBIENT, D3DRGB(0.5, 0.5, 0.5));
	m_pD3DDev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
	m_pD3DDev->SetRenderState(D3DRENDERSTATE_LIGHTING, TRUE);
	m_pD3DDev->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD); 
	m_pD3DDev->SetRenderState(D3DRENDERSTATE_FILLMODE, fillMode);
	m_pD3DDev->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);

	//no alpha blending right now
	m_pD3DDev->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDev->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	return D3D_OK;
}


///////////////////////////////////////////////////////////////////////////////////////////
// CreateVB()
//
// Creates the vertex buffer vertexBuffer along with the corresponding index list in indices
//
HRESULT CNvTriStripDlg::CreateVB()
{
	D3DVERTEXBUFFERDESC vbdesc;
	ZeroMemory( &vbdesc, sizeof(D3DVERTEXBUFFERDESC) );
	vbdesc.dwSize        = sizeof(D3DVERTEXBUFFERDESC);
	vbdesc.dwCaps        = D3DVBCAPS_WRITEONLY;
	vbdesc.dwFVF         = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX0;
	vbdesc.dwNumVertices = vertices.size();
	
	if(FAILED( m_pD3D->CreateVertexBuffer(&vbdesc, &vertexBuffer, 0L)))
		return E_FAIL;
	
	MyVertex* tempVertices;
	if( SUCCEEDED( vertexBuffer->Lock( DDLOCK_WAIT | DDLOCK_DISCARDCONTENTS, (VOID**)&tempVertices,
		NULL ) ) )
	{
		for(int i = 0; i < vertices.size(); i++)
		{
			tempVertices[i].x = vertices[i].x;
			tempVertices[i].y = vertices[i].y;
			tempVertices[i].z = vertices[i].z;
			
			tempVertices[i].nx = vertices[i].nx;
			tempVertices[i].ny = vertices[i].ny;
			tempVertices[i].nz = vertices[i].nz;
		}
		
		vertexBuffer->Unlock();
	}
	
	//create indices
	int indexCtr = 0;
	
	indices = new WORD[faces.size() * 3];
	for(int i = 0; i < faces.size(); i++)
	{
		indices[indexCtr++] = faces[i].v1;
		indices[indexCtr++] = faces[i].v2;
		indices[indexCtr++] = faces[i].v3;
	}
	
	bVBCreated = true;
	
	return D3D_OK;
}


///////////////////////////////////////////////////////////////////////////////////////////
// CreateStripCacheAndFaceVBs()
//
// Function which generates the global VBs, vertexBufferVizStrips, vertexBufferVizCache, 
//  vertexBufferVizCacheFaces, and vertexBufferFaces.
// Basically, sets up the color info in these VBs
//
HRESULT CNvTriStripDlg::CreateStripCacheAndFaceVBs()
{
	D3DVERTEXBUFFERDESC vbdesc;
	ZeroMemory( &vbdesc, sizeof(D3DVERTEXBUFFERDESC) );
	vbdesc.dwSize        = sizeof(D3DVERTEXBUFFERDESC);
	vbdesc.dwCaps        = D3DVBCAPS_WRITEONLY;
	vbdesc.dwFVF         = D3DFVF_XYZ | D3DFVF_TEX0 | D3DFVF_DIFFUSE | D3DFVF_NORMAL;
	vbdesc.dwNumVertices = faces.size() * 3 - (leftoverFaces.size()*3);
	
	ColoredVertex* tempVertices;
	//make the VB for strip visualization
	if(vbdesc.dwNumVertices != 0)
	{
		if(FAILED( m_pD3D->CreateVertexBuffer(&vbdesc, &vertexBufferVizStrips, 0L)))
			return E_FAIL;
		
		if( SUCCEEDED( vertexBufferVizStrips->Lock( DDLOCK_WAIT | DDLOCK_DISCARDCONTENTS, (VOID**)&tempVertices, NULL ) ) )
		{
			int vertCtr = 0;
			
			//for every strip
			for(int i = 0; i < strips.size(); i++)
			{
				DWORD r = rand() % 256;
				DWORD g = rand() % 256;
				DWORD b = rand() % 256;
				DWORD a = 0xFF000000;
				
				DWORD randColor = a | (r << 16) | (g << 8) | (b);
				
				for(int j = 0; j < strips[i]->m_faces.size(); j++)
				{
					tempVertices[vertCtr] = vertices[strips[i]->m_faces[j]->m_v0];
					tempVertices[vertCtr++].rgba = randColor;
					
					tempVertices[vertCtr] = vertices[strips[i]->m_faces[j]->m_v1];
					tempVertices[vertCtr++].rgba = randColor;
					
					tempVertices[vertCtr] = vertices[strips[i]->m_faces[j]->m_v2];
					tempVertices[vertCtr++].rgba = randColor;
				}
				
			}
		}
		
		vertexBufferVizStrips->Unlock();
	}
	
	//now, make a VB for the leftOver faces
	if(leftoverFaces.size() != 0)
	{
		vbdesc.dwNumVertices = leftoverFaces.size()*3;
		if(FAILED( m_pD3D->CreateVertexBuffer(&vbdesc, &vertexBufferFaces, 0L)))
			return E_FAIL;
		if( SUCCEEDED( vertexBufferFaces->Lock( DDLOCK_WAIT | DDLOCK_DISCARDCONTENTS, (VOID**)&tempVertices,
			NULL ) ) )
		{
			int vertCtr = 0;
			DWORD currColor;
			DWORD r, g, b, a;
			a = 120;
			r = 255;
			g = 255;
			b = 255;
			currColor = (a << 24) | (r << 16) | (g << 8) | (b);
			
			//for every face
			for(int i = 0; i < leftoverFaces.size(); i++)
			{
				tempVertices[vertCtr] = vertices[leftoverFaces[i]->m_v0];
				tempVertices[vertCtr++].rgba = currColor;
				
				tempVertices[vertCtr] = vertices[leftoverFaces[i]->m_v1];
				tempVertices[vertCtr++].rgba = currColor;
				
				tempVertices[vertCtr] = vertices[leftoverFaces[i]->m_v2];
				tempVertices[vertCtr++].rgba = currColor;
			}
		}
		vertexBufferFaces->Unlock();
	}
	
	//now, make a VB for cache miss visualization
	VertexCache* vcache = new VertexCache(cacheSize);
	if((faces.size() - leftoverFaces.size()) != 0)
	{
		vbdesc.dwNumVertices = faces.size()*3 - (leftoverFaces.size()*3);
		if(FAILED( m_pD3D->CreateVertexBuffer(&vbdesc, &vertexBufferVizCache, 0L)))
			return E_FAIL;
		
		if( SUCCEEDED( vertexBufferVizCache->Lock( DDLOCK_WAIT | DDLOCK_DISCARDCONTENTS, (VOID**)&tempVertices,
			NULL ) ) )
		{
			int vertCtr = 0;
			
			//for every strip
			for(int i = 0; i < strips.size(); i++)
			{
				DWORD currColor;
				DWORD r, g, b, a;
				a = 0xFF000000;
				
				for(int j = 0; j < strips[i]->m_faces.size(); j++)
				{
					int numMisses = 0;
					
					if(!vcache->InCache(strips[i]->m_faces[j]->m_v0))
					{
						numMisses++;
						vcache->AddEntry(strips[i]->m_faces[j]->m_v0);
					}
					if(!vcache->InCache(strips[i]->m_faces[j]->m_v1))
					{
						numMisses++;
						vcache->AddEntry(strips[i]->m_faces[j]->m_v1);
					}
					if(!vcache->InCache(strips[i]->m_faces[j]->m_v2))
					{
						numMisses++;
						vcache->AddEntry(strips[i]->m_faces[j]->m_v2);
					}
					
					a = 0xFF000000;
					
					switch(numMisses)
					{
					case 0:
						r = 0;
						g = 160;
						b = 0;
						break;
					case 1:
						r = 200;
						g = 200;
						b = 0;
						break;
					case 2:
						r = 0;
						g = 140;
						b = 140;
						break;
					case 3:
						r = 255;
						g = 0;
						b = 0;
						break;
					}
					
					currColor = a | (r << 16) | (g << 8) | (b);
					
					tempVertices[vertCtr] = vertices[strips[i]->m_faces[j]->m_v0];
					tempVertices[vertCtr++].rgba = currColor;
					
					tempVertices[vertCtr] = vertices[strips[i]->m_faces[j]->m_v1];
					tempVertices[vertCtr++].rgba = currColor;
					
					tempVertices[vertCtr] = vertices[strips[i]->m_faces[j]->m_v2];
					tempVertices[vertCtr++].rgba = currColor;
				}
				
			}
		}
		
		vertexBufferVizCache->Unlock();
	}
	
	//now make a VB for visualizing the leftOver faces
	if(leftoverFaces.size() != 0)
	{
		vbdesc.dwNumVertices = leftoverFaces.size()*3;
		if(FAILED( m_pD3D->CreateVertexBuffer(&vbdesc, &vertexBufferVizCacheFaces, 0L)))
			return E_FAIL;
		
		if( SUCCEEDED( vertexBufferVizCacheFaces->Lock( DDLOCK_WAIT | DDLOCK_DISCARDCONTENTS, (VOID**)&tempVertices,
			NULL ) ) )
		{
			int vertCtr = 0;
			
			//for every face
			for(int i = 0; i < leftoverFaces.size(); i++)
			{
				DWORD currColor;
				DWORD r, g, b, a;
				a = 120;
				
				int numMisses = 0;
				
				if(!vcache->InCache(leftoverFaces[i]->m_v0))
				{
					numMisses++;
					vcache->AddEntry(leftoverFaces[i]->m_v0);
				}
				if(!vcache->InCache(leftoverFaces[i]->m_v1))
				{
					numMisses++;
					vcache->AddEntry(leftoverFaces[i]->m_v1);
				}
				if(!vcache->InCache(leftoverFaces[i]->m_v2))
				{
					numMisses++;
					vcache->AddEntry(leftoverFaces[i]->m_v2);
				}
				
				switch(numMisses)
				{
				case 0:
					r = 0;
					g = 160;
					b = 0;
					break;
				case 1:
					r = 200;
					g = 200;
					b = 0;
					break;
				case 2:
					r = 0;
					g = 140;
					b = 140;
					break;
				case 3:
					r = 255;
					g = 0;
					b = 0;
					break;
				}

				currColor = (a << 24) | (r << 16) | (g << 8) | (b);
				
				tempVertices[vertCtr] = vertices[leftoverFaces[i]->m_v0];
				tempVertices[vertCtr++].rgba = currColor;
				
				tempVertices[vertCtr] = vertices[leftoverFaces[i]->m_v1];
				tempVertices[vertCtr++].rgba = currColor;
				
				tempVertices[vertCtr] = vertices[leftoverFaces[i]->m_v2];
				tempVertices[vertCtr++].rgba = currColor;
			}
			
		}
		
		vertexBufferVizCacheFaces->Unlock();
	}
	delete vcache;
	
	return D3D_OK;	
}


void CNvTriStripDlg::ReleaseModelSpecificMemory(bool bClearLoadedData)
{
	bVBCreated = false;
	SAFE_RELEASE(vertexBuffer);
	SAFE_DELETE_ARRAY(indices);

	//set this flag only if you want to clear out the data loaded from disk
	if(bClearLoadedData)
	{
		vertices.clear();
		faces.clear();
	}

	for(int i = 0; i < strips.size(); i++)
	{
		for(int j = 0; j < strips[i]->m_faces.size(); j++)
		{
			SAFE_DELETE(strips[i]->m_faces[j]);
		}
		SAFE_DELETE(strips[i]);
	}

	strips.clear();
	stripIndices.clear();
	optimizedVerts.clear();
	SAFE_RELEASE(vertexBufferVizStrips);
	SAFE_RELEASE(vertexBufferVizCache);
	SAFE_RELEASE(vertexBufferVizCacheFaces);
	SAFE_RELEASE(vertexBufferFaces);
	for(i = 0; i < leftoverFaces.size(); i++)
	{
		SAFE_DELETE(leftoverFaces[i]);
	}
	leftoverFaces.clear();
}


///////////////////////////////////////////////////////////////////////////////////////////
// OnLoadButton()
//
// Gets called when load is pressed.
// We need to clear out our data structures each time this is pressed because we begin
//  anew with a new model
//
void CNvTriStripDlg::OnLoadButton() 
{
	TCHAR szFilters[] = _T(".M files (*.m)|*.m|All files (*.*)|*.*||");
	
	CFileDialog dlg (TRUE, "m", "*.m", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
	
	if(dlg.DoModal() == IDOK) 
	{
		//we don't want to leak memory
		ReleaseModelSpecificMemory(true);

		Extents e;
		
		//create hourglass cursor, will return to regular cursor when it loses scope
		CWaitCursor wait;

		int returnVal = LoadFile((LPCTSTR)dlg.GetPathName(), e, false);
		if(returnVal == -1)
			MessageBox("Error loading file from disk!");
		else
		{
			if(returnVal == 1)
			{
				//reload, and fix
				LoadFile((LPCTSTR)dlg.GetPathName(), e, true);
			}
			
			char buf[256];
			sprintf(buf, "NvTriStrip -- %s", (LPCTSTR)dlg.GetPathName());
			SetWindowText(buf);
			if(bRendering)
			{
				SetupMatrices(e);
				normalButton->EnableWindow(TRUE);
				stripifyButton->EnableWindow(TRUE);
				wireframeButton->EnableWindow(TRUE);
				
				vizCacheButton->EnableWindow(FALSE);
				vizStripsButton->EnableWindow(FALSE);
				normalButton->SetCheck(1);
				vizStripsButton->SetCheck(0);
				vizCacheButton->SetCheck(0);
				
				renderSetting = RS_NORMAL;
				CreateVB();
			}
			saveButton->EnableWindow(FALSE);
			minStripEdit->EnableWindow(TRUE);
			stitchButton->EnableWindow(FALSE);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
// NextIsCW()
//
// Returns true if the next face should be ordered in CW fashion
//
bool CNvTriStripDlg::NextIsCW(const int numIndices)
{
	return ((numIndices % 2) == 0);
}


///////////////////////////////////////////////////////////////////////////////////////////
// IsCW()
//
// Returns true if the face is ordered in CW fashion
//
bool CNvTriStripDlg::IsCW(NvFaceInfo *faceInfo, int v0, int v1)
{
	if (faceInfo->m_v0 == v0)
		return (faceInfo->m_v1 == v1);
	
	else if (faceInfo->m_v1 == v0)
		return (faceInfo->m_v2 == v1);
	
	else 
		return (faceInfo->m_v0 == v1);
	
	// shouldn't get here
	assert(0);
	return false;
}


//used in CreateStrips
template<class T> 
inline void SWAP(T& first, T& second) 
{
	T temp = first;
	first = second;
	second = temp;
}

///////////////////////////////////////////////////////////////////////////////////////////
// CreateStrips()
//
// Up until now, the strips had been strips at heart, but tri lists in reality.
// Now, remove redundant indices, and stitch together strips to form one, huge uber-strip
//  using degenerate tris.
//
void CNvTriStripDlg::CreateStrips()
{
	NvFaceInfo tLastFace(0, 0, 0);
	int nStripCount = strips.size();
	assert(nStripCount > 0);

	for (int i = 0; i < nStripCount; i++)
	{
		NvStripInfo *strip = strips[i];
		int nStripFaceCount = strip->m_faces.size();
		int stripIndicesSize = stripIndices.size();
		assert(nStripFaceCount > 0);

		// Handle the first face in the strip
		{
			NvFaceInfo tFirstFace(strip->m_faces[0]->m_v0, strip->m_faces[0]->m_v1, strip->m_faces[0]->m_v2);

			// If there is a second face, reorder vertices such that the
			// unique vertex is first
			if (nStripFaceCount > 1)
			{
				int nUnique = NvStripifier::GetUniqueVertexInB(strip->m_faces[1], &tFirstFace);
				if (nUnique == tFirstFace.m_v1)
				{
					SWAP(tFirstFace.m_v0, tFirstFace.m_v1);
				}
				else if (nUnique == tFirstFace.m_v2)
				{
					SWAP(tFirstFace.m_v0, tFirstFace.m_v2);
				}

				// If there is a third face, reorder vertices such that the
				// shared vertex is last
				if (nStripFaceCount > 2)
				{
					int nShared = NvStripifier::GetSharedVertex(strip->m_faces[2], &tFirstFace);
					if (nShared == tFirstFace.m_v1)
					{
						SWAP(tFirstFace.m_v1, tFirstFace.m_v2);
					}
				}
			}

			if (i != 0)
			{
				// Double tap the first in the new strip
				stripIndices.push_back(tFirstFace.m_v0);

				// Check CW/CCW ordering
				if (NextIsCW(stripIndices.size()) != IsCW(strip->m_faces[0], tFirstFace.m_v0, tFirstFace.m_v1))
				{
					stripIndices.push_back(tFirstFace.m_v0);
				}
			}
			else
			{
				if(!IsCW(strip->m_faces[0], tFirstFace.m_v0, tFirstFace.m_v1))
					stripIndices.push_back(tFirstFace.m_v0);
			}

			stripIndices.push_back(tFirstFace.m_v0);
			stripIndices.push_back(tFirstFace.m_v1);
			stripIndices.push_back(tFirstFace.m_v2);

			// Update last face info
			tLastFace = tFirstFace;
		}

		for (int j = 1; j < nStripFaceCount; j++)
		{
			int nUnique = NvStripifier::GetUniqueVertexInB(&tLastFace, strip->m_faces[j]);
			if (nUnique != -1)
			{
				stripIndices.push_back(nUnique);

				// Update last face info
				tLastFace.m_v0 = tLastFace.m_v1;
				tLastFace.m_v1 = tLastFace.m_v2;
				tLastFace.m_v2 = nUnique;
			}
		}

		// Double tap between strips.
		stripIndices.push_back(tLastFace.m_v2);

		// Update last face info
		tLastFace.m_v0 = tLastFace.m_v1;
		tLastFace.m_v1 = tLastFace.m_v2;
		tLastFace.m_v2 = tLastFace.m_v2;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
// OnSaveButton()
//
// Gets called when Save is pressed.  Optimizes the vertices, stitches the strips together,
//  and saves the resulting file to disk.
//
void CNvTriStripDlg::OnSavebutton() 
{
	TCHAR szFilters[] = _T(".STRIP files (*.strip)|*.strip|All files (*.*)|*.*||");
	
	CFileDialog dlg (FALSE, "strip", "*.strip", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
	
	if(dlg.DoModal() == IDOK) 
	{
		if(optimizedVerts.size() == 0)
			OptimizeVertices();

		if(bStitch && (stripIndices.size() == 0))
			CreateStrips();
		
		if(SaveFile((LPCTSTR)dlg.GetPathName()) == -1)
			MessageBox("Error saving file to disk!");
	}
}


///////////////////////////////////////////////////////////////////////////////////////////
// OptimizeVertices()
//
// Function which optimizes the vertices in the mesh to minimize page misses
//
// Puts output verts into optimizedVerts vector
//
//
void CNvTriStripDlg::OptimizeVertices()
{
	//caches oldIndex --> newIndex conversion
	int *indexCache;
	indexCache = new int[vertices.size()];
	
	memset(indexCache, -1, sizeof(int)*vertices.size());
	
	//first do the strips
	for(int i = 0; i < strips.size(); i++)
	{
		for(int j = 0; j < strips[i]->m_faces.size(); j++)
		{
			int v0 = strips[i]->m_faces[j]->m_v0;
			int v1 = strips[i]->m_faces[j]->m_v1;
			int v2 = strips[i]->m_faces[j]->m_v2;
			
			//v0
			int index = indexCache[v0];
			if(index == -1)
			{
				optimizedVerts.push_back(vertices[v0]);
				strips[i]->m_faces[j]->m_v0 = optimizedVerts.size() - 1;
				
				indexCache[v0] = strips[i]->m_faces[j]->m_v0;
			}
			else
			{
				strips[i]->m_faces[j]->m_v0 = index;
			}
			
			//v1
			index = indexCache[v1];
			if(index == -1)
			{
				optimizedVerts.push_back(vertices[v1]);
				strips[i]->m_faces[j]->m_v1 = optimizedVerts.size() - 1;        
				
				indexCache[v1] = strips[i]->m_faces[j]->m_v1;
			}
			else
			{
				strips[i]->m_faces[j]->m_v1 = index;
			}
			
			//v2
			index = indexCache[v2];
			if(index == -1)
			{
				optimizedVerts.push_back(vertices[v2]);
				strips[i]->m_faces[j]->m_v2 = optimizedVerts.size() - 1;  
				
				indexCache[v2] = strips[i]->m_faces[j]->m_v2;
			}
			else
			{
				strips[i]->m_faces[j]->m_v2 = index;
			}
			
		}
	}

	//now do the leftover list
	for(i = 0; i < leftoverFaces.size(); i++)
	{
		int v0 = leftoverFaces[i]->m_v0;
		int v1 = leftoverFaces[i]->m_v1;
		int v2 = leftoverFaces[i]->m_v2;
		
		//v0
		int index = indexCache[v0];
		if(index == -1)
		{
			optimizedVerts.push_back(vertices[v0]);
			leftoverFaces[i]->m_v0 = optimizedVerts.size() - 1;
			
			indexCache[v0] = leftoverFaces[i]->m_v0;
		}
		else
		{
			leftoverFaces[i]->m_v0 = index;
		}
		
		//v1
		index = indexCache[v1];
		if(index == -1)
		{
			optimizedVerts.push_back(vertices[v1]);
			leftoverFaces[i]->m_v1 = optimizedVerts.size() - 1;        
			
			indexCache[v1] = leftoverFaces[i]->m_v1;
		}
		else
		{
			leftoverFaces[i]->m_v1 = index;
		}
		
		//v2
		index = indexCache[v2];
		if(index == -1)
		{
			optimizedVerts.push_back(vertices[v2]);
			leftoverFaces[i]->m_v2 = optimizedVerts.size() - 1;  
			
			indexCache[v2] = leftoverFaces[i]->m_v2;
		}
		else
		{
			leftoverFaces[i]->m_v2 = index;
		}
	}

	delete[] indexCache;
	
	assert(optimizedVerts.size() == vertices.size());
}

///////////////////////////////////////////////////////////////////////////////////////////
// SaveFile()
//
// Binary file format:
// Number of vertices: 4 bytes
// Vertices, x, y, z, nx, ny, nz components, each a 4-byte float
// Number of strips: 4 bytes
// For each strip:
//   Number of strip indices: 4 bytes
//   Indices, each a 2-byte WORD
//
// Number of list indices: 4 bytes
// Indices, each a 2-byte WORD
//
int CNvTriStripDlg::SaveFile(const char *fileName)
{
	HANDLE h = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD numBytes;
	
	int numVerts = optimizedVerts.size();
	
	WriteFile(h, &numVerts, sizeof(int), &numBytes, NULL);
	
	for(int i = 0; i < optimizedVerts.size(); i++)
	{
		WriteFile(h, &optimizedVerts[i], sizeof(MyVertex), &numBytes, NULL);
	}
	
	if(bStitch)
	{
		int numStrips = 1;
		if(strips.size() == 0)
			numStrips = 0;

		WriteFile(h, &numStrips, sizeof(int), &numBytes, NULL);
		
		if(numStrips != 0)
		{
			int numStripIndices = stripIndices.size();
			WriteFile(h, &numStripIndices, sizeof(int), &numBytes, NULL);
	
			for(int i = 0; i < stripIndices.size(); i++)
			{
				WriteFile(h, &stripIndices[i], sizeof(WORD), &numBytes, NULL);
			}
		}
	}
	else
	{
		int numStrips = strips.size();
		WriteFile(h, &numStrips, sizeof(int), &numBytes, NULL);

		for(int i = 0; i < numStrips; i++)
		{
			int numIndicesInStrip = strips[i]->m_faces.size() + 2;
			
			if(strips[i]->m_faces.size() > 2)
			{
				NvFaceInfo* face0 = strips[i]->m_faces[0];
				NvFaceInfo* face1 = strips[i]->m_faces[1];
				NvFaceInfo* face2 = strips[i]->m_faces[2];

				WORD v0 = NvStripifier::GetUniqueVertexInB(face1, face0);
				WORD v2 = NvStripifier::GetSharedVertex(face0, face2);
				WORD v1 = (face0->m_v0 != v0 && face0->m_v0 != v2 ? face0->m_v0 :
					(face0->m_v1 != v0 && face0->m_v1 != v2 ? face0->m_v1 : face0->m_v2));

				//check ordering of first face
				if(!IsCW(face0, v0, v1))
				{
					numIndicesInStrip++;
					WriteFile(h, &numIndicesInStrip, sizeof(int), &numBytes, NULL);
					WriteFile(h, &v0, sizeof(WORD), &numBytes, NULL);
				}
				else
					WriteFile(h, &numIndicesInStrip, sizeof(int), &numBytes, NULL);

				WriteFile(h, &v0, sizeof(WORD), &numBytes, NULL);
				WriteFile(h, &v1, sizeof(WORD), &numBytes, NULL);
				WriteFile(h, &v2, sizeof(WORD), &numBytes, NULL);
				for(int j = 1; j < strips[i]->m_faces.size(); j++)
				{
					face1 = strips[i]->m_faces[j];
					WORD v = NvStripifier::GetUniqueVertexInB(face0, face1);
					WriteFile(h, &v, sizeof(WORD), &numBytes, NULL);
					face0 = face1;
				}
			}
			else if(strips[i]->m_faces.size() > 1)
			{
				NvFaceInfo* face0 = strips[i]->m_faces[0];
				NvFaceInfo* face1 = strips[i]->m_faces[1];
				WORD v0 = NvStripifier::GetUniqueVertexInB(face1, face0);
				WORD v1 = (face0->m_v0 == v0 ? face0->m_v1 : (face0->m_v1 == v0 ? face0->m_v2 : face0->m_v0));
				WORD v2 = (face0->m_v0 != v0 && face0->m_v0 != v1 ? face0->m_v0 :
					(face0->m_v1 != v0 && face0->m_v1 != v1 ? face0->m_v1 : face0->m_v2));
				
				//check ordering of first face
				if(!IsCW(face0, v0, v1))
				{
					numIndicesInStrip++;
					WriteFile(h, &numIndicesInStrip, sizeof(int), &numBytes, NULL);
					WriteFile(h, &v0, sizeof(WORD), &numBytes, NULL);
				}
				else
					WriteFile(h, &numIndicesInStrip, sizeof(int), &numBytes, NULL);

				WriteFile(h, &v0, sizeof(WORD), &numBytes, NULL);
				WriteFile(h, &v1, sizeof(WORD), &numBytes, NULL);
				WriteFile(h, &v2, sizeof(WORD), &numBytes, NULL);
				WORD v = NvStripifier::GetUniqueVertexInB(face0, face1);
				WriteFile(h, &v, sizeof(WORD), &numBytes, NULL);
			}
			else
			{
				WriteFile(h, &numIndicesInStrip, sizeof(int), &numBytes, NULL);
				NvFaceInfo* face0 = strips[i]->m_faces[0];
				WORD v0 = face0->m_v0;
				WORD v1 = face0->m_v1;
				WORD v2 = face0->m_v2;
				WriteFile(h, &v0, sizeof(WORD), &numBytes, NULL);
				WriteFile(h, &v1, sizeof(WORD), &numBytes, NULL);
				WriteFile(h, &v2, sizeof(WORD), &numBytes, NULL);
			}
		}
	}

	int numListIndices = leftoverFaces.size()*3;
	WriteFile(h, &numListIndices, sizeof(int), &numBytes, NULL);

	for(i = 0; i < leftoverFaces.size(); i++)
	{
		WriteFile(h, &leftoverFaces[i]->m_v0, sizeof(WORD), &numBytes, NULL);
		WriteFile(h, &leftoverFaces[i]->m_v1, sizeof(WORD), &numBytes, NULL);
		WriteFile(h, &leftoverFaces[i]->m_v2, sizeof(WORD), &numBytes, NULL);
	}
	
	CloseHandle(h);
	
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////
// OnHScroll()
//
// Gets called when the cache slider is moved
//
void CNvTriStripDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int pos, nControl;
	nControl = 0;
	CSliderCtrl *pSlider;
	
	const int stln = 4;		// character length of text output string
	char str[10];
	
	if(pScrollBar != NULL)
	{
		nControl = pScrollBar->GetDlgCtrlID();
		pSlider = (CSliderCtrl*) pScrollBar;
		
		if(nControl == IDC_CACHESLIDER)
		{
			pos = pSlider->GetPos();
			
			SetDlgItemText(IDC_EDIT1, itoa(pos, str, 10));
			cacheSize = pos;
		}
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CNvTriStripDlg::OnRadViznone() 
{
	normalButton->SetCheck(1);
	vizStripsButton->SetCheck(0);
	vizCacheButton->SetCheck(0);
	
	renderSetting = RS_NORMAL;
}


void CNvTriStripDlg::OnRadVizcache() 
{
	normalButton->SetCheck(0);
	vizStripsButton->SetCheck(0);
	vizCacheButton->SetCheck(1);
	
	renderSetting = RS_VIZCACHE;
}


void CNvTriStripDlg::OnRadVizstrips() 
{
	normalButton->SetCheck(0);
	vizStripsButton->SetCheck(1);
	vizCacheButton->SetCheck(0);
	
	renderSetting = RS_VIZSTRIPS;
}



///////////////////////////////////////////////////////////////////////////////////////////
// OnBtStripify()
//
// Gets called when Stripify is pressed.
// Calls Stripify in class NvStripifier in file NvTriStripObjects.cpp.
// Also calls CreateStripAndCacheVBs to enable visualization
//
void CNvTriStripDlg::OnBtStripify() 
{
	//create hourglass cursor, will return to regular cursor when it loses scope
	CWaitCursor wait;
	
	NvStripifier *stripifier = new NvStripifier();
	
	//put indices in correct format for Stripify()
	WordVec tempIndices;
	tempIndices.reserve(faces.size()*3);
	for(int i = 0; i < faces.size()*3; i++)
		tempIndices.push_back(indices[i]);

	//delete old data, since it will all be re-created
	ReleaseModelSpecificMemory(false);
	
	CString cStringBuf;
	char stringBuf[3];
	minStripEdit->GetWindowText(cStringBuf);

	int minStripLength = atoi((LPCSTR)cStringBuf);
	minStripEdit->SetWindowText(itoa(minStripLength, stringBuf, 10));

	//actually do the stripification
	stripifier->Stripify(tempIndices, cacheSize, minStripLength, strips, leftoverFaces);
	
	saveButton->EnableWindow(TRUE);
	if(bRendering)
	{	
		vizStripsButton->EnableWindow(TRUE);
		vizCacheButton->EnableWindow(TRUE);
		
		CreateVB();
		CreateStripCacheAndFaceVBs();
	}
	
	stitchButton->EnableWindow(TRUE);
	
	delete stripifier;
}


///////////////////////////////////////////////////////////////////////////////////////////
// OnTimer()
//
// We render every tick of the timer, so the frame rate is fixed.
//
void CNvTriStripDlg::OnTimer(UINT nIDEvent) 
{
	if(bRendering)
		Render();
	
	CDialog::OnTimer(nIDEvent);
}


///////////////////////////////////////////////////////////////////////////////////////////
// Render()
//
// Renders a vertex buffer, depending on the current renderSetting
//
HRESULT CNvTriStripDlg::Render()
{
	HRESULT hr = m_pD3DDev->BeginScene();
	
	if(!FAILED(hr))
	{
		m_pD3DX->Clear(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER);
		
		if(bVBCreated)
		{
			//render stuff
			switch(renderSetting)
			{
			case RS_NORMAL:
				m_pD3DDev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);
				m_pD3DDev->DrawIndexedPrimitiveVB(D3DPT_TRIANGLELIST, vertexBuffer, 0, vertices.size(), indices, faces.size()*3, 0);
				break;
			case RS_VIZSTRIPS:
				if(vertexBufferVizStrips)
					m_pD3DDev->DrawPrimitiveVB(D3DPT_TRIANGLELIST, vertexBufferVizStrips, 0, faces.size()*3 - (leftoverFaces.size()*3), 0);
				
				if(vertexBufferFaces)
				{
					m_pD3DDev->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME);
					m_pD3DDev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
					m_pD3DDev->DrawPrimitiveVB(D3DPT_TRIANGLELIST, vertexBufferFaces, 0, leftoverFaces.size()*3, 0);
					m_pD3DDev->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
					m_pD3DDev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
					
					//render back faces
					m_pD3DDev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CW);
					m_pD3DDev->DrawPrimitiveVB(D3DPT_TRIANGLELIST, vertexBufferFaces, 0, leftoverFaces.size()*3, 0);
					//render front faces
					m_pD3DDev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);
					m_pD3DDev->DrawPrimitiveVB(D3DPT_TRIANGLELIST, vertexBufferFaces, 0, leftoverFaces.size()*3, 0);
					m_pD3DDev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);

					m_pD3DDev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
					m_pD3DDev->SetRenderState(D3DRENDERSTATE_FILLMODE, fillMode);
				}
				break;
			case RS_VIZCACHE:
				if(vertexBufferVizCache)
					m_pD3DDev->DrawPrimitiveVB(D3DPT_TRIANGLELIST, vertexBufferVizCache, 0, faces.size()*3 - (leftoverFaces.size()*3), 0);

				if(vertexBufferVizCacheFaces)
				{
					m_pD3DDev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
					m_pD3DDev->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME);
					m_pD3DDev->DrawPrimitiveVB(D3DPT_TRIANGLELIST, vertexBufferVizCacheFaces, 0, leftoverFaces.size()*3, 0);
					m_pD3DDev->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
					
					m_pD3DDev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);

					//render backfaces
					m_pD3DDev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CW);
					m_pD3DDev->DrawPrimitiveVB(D3DPT_TRIANGLELIST, vertexBufferVizCacheFaces, 0, leftoverFaces.size()*3, 0);
					//render front faces
					m_pD3DDev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);
					m_pD3DDev->DrawPrimitiveVB(D3DPT_TRIANGLELIST, vertexBufferVizCacheFaces, 0, leftoverFaces.size()*3, 0);
					m_pD3DDev->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);

					m_pD3DDev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
					m_pD3DDev->SetRenderState(D3DRENDERSTATE_FILLMODE, fillMode);
				}
				break;
			}
			
		}
		
		m_pD3DDev->EndScene();
	}
	
	hr = m_pD3DX->UpdateFrame( 0 );
	
	return hr;
}

void CNvTriStripDlg::OnClose() 
{
	CDialog::OnClose();
}


///////////////////////////////////////////////////////////////////////////////////////////
// OnDestroy()
//
// Deletes all allocated objects.
//
void CNvTriStripDlg::OnDestroy() 
{
	D3DXUninitialize();
	
	ReleaseModelSpecificMemory(true);

	if(m_pD3DDev != NULL)
	{
		m_pD3DDev->Release();
		m_pD3DDev = NULL;
	}
	if(m_pD3D != NULL)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}
	if(m_pDD != NULL)
	{
		m_pDD->Release();
		m_pDD = NULL;
	}
	
	CDialog::OnDestroy();
}

void CNvTriStripDlg::OnWireframe() 
{
	if(bRendering)
	{
		fillMode = (fillMode == D3DFILL_WIREFRAME) ? D3DFILL_SOLID : D3DFILL_WIREFRAME;
		m_pD3DDev->SetRenderState(D3DRENDERSTATE_FILLMODE, fillMode);
		
		if(fillMode == D3DFILL_WIREFRAME)
			wireframeButton->SetCheck(1);
		else
			wireframeButton->SetCheck(0);
	}
	
}

void CNvTriStripDlg::OnStitch()
{
	bStitch = !bStitch;
	stitchButton->SetCheck(bStitch);
}

void CNvTriStripDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	bMouseDown = true;
	startPoint = point;
	SetCapture();
	
	CDialog::OnLButtonDown(nFlags, point);
}


void CNvTriStripDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	bMouseDown = false;
	ReleaseCapture();
	D3DXMatrixIdentity(&mouseSpin);
	CDialog::OnLButtonUp(nFlags, point);
}

struct IntVector2D
{
	int x, y;
};

void CNvTriStripDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(bMouseDown)
	{
		IntVector2D delta;
		delta.x = point.x - startPoint.x;
		delta.y = point.y - startPoint.y;
		
		if((delta.x == 0) && (delta.y == 0))
		{
			startPoint.x = point.x;
			startPoint.y = point.y;
			D3DXMatrixIdentity(&mouseSpin);
			return;
		}
		
		float mag = sqrt(delta.x * delta.x + delta.y * delta.y);
		float dx = (float)delta.x / mag;
		float dy = (float)delta.y / mag;
		
		//find a vector in the plane perpendicular to delta vector
		float perpx;
		float perpy;
		
		perpx = dy;
		perpy = dx;
		
		//rotate around this vector
		D3DXVECTOR3 axis(perpx, perpy, 0.0f);
		D3DXMatrixRotationAxis(&mouseSpin, &axis, D3DXToRadian(-0.7f*mag));
		
		D3DXMatrixMultiply(&world, &world, &mouseSpin);
		m_pD3DDev->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX *)&world);
		
		startPoint.x = point.x;
		startPoint.y = point.y;
	}
	
	CDialog::OnMouseMove(nFlags, point);
}
