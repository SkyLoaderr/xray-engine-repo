// NvTriStripDlg.h : header file
//

#if !defined(AFX_NVTRISTRIPDLG_H__1FFA50E6_7532_11D4_9C70_005004C20C3D__INCLUDED_)
#define AFX_NVTRISTRIPDLG_H__1FFA50E6_7532_11D4_9C70_005004C20C3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CNvTriStripDlg dialog

#include "d3d.h"
#include "d3dx.h"

#include "NvTriStripObjects.h"

#define SAFE_RELEASE(a) \
	if ((a)) (a)->Release(); (a) = NULL;

#define SAFE_DELETE(a) \
	if ((a)) delete(a); a = NULL;

#define SAFE_DELETE_ARRAY(a) \
	if ((a)) delete [] (a); a = NULL;

struct ColoredVertex
{
	float x, y, z;
	float nx, ny, nz;
	DWORD rgba;

	bool operator==(const ColoredVertex& rhs)
	{
		if(&rhs == this)
			return true;
		else if( (x == rhs.x) && (y == rhs.y) && (z == rhs.z) 
			&& (nx == rhs.nx) && (ny == rhs.ny) && (nz == rhs.nz) )
			return true;
		else 
			return false;
	}

	ColoredVertex& operator=(const ColoredVertex& rhs)
	{
		//check for assignment to self
		if(&rhs != this)
		{
			x    = rhs.x;
			y    = rhs.y;
			z    = rhs.z;

			nx   = rhs.nx;
			ny   = rhs.ny;
			nz   = rhs.nz;

			rgba = rhs.rgba;
		}
		return *this;
	}

	ColoredVertex& operator=(const MyVertex& rhs)
	{
		x  = rhs.x;
		y  = rhs.y;
		z  = rhs.z;

		nx = rhs.nx;
		ny = rhs.ny;
		nz = rhs.nz;

		rgba = 0;

		return *this;
	}
			
};


/////////////////////////////////////////////////////////////////////////////
// CNvStripifierDlg dialog

class CNvTriStripDlg : public CDialog
{
	// Construction
public:
	CNvTriStripDlg(CWnd* pParent = NULL);	// standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CNvTriStripDlg)
	enum { IDD = IDD_NVTRISTRIP_DIALOG };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNvTriStripDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	struct Extents
	{
		float maxX, minX;
		float maxY, minY;
		float maxZ, minZ;
	};
	
	HICON m_hIcon;
	CButton *normalButton;
	CButton *vizStripsButton;
	CButton *vizCacheButton;
	CButton *saveButton;
	CButton *stripifyButton;
	CButton *wireframeButton;
	CButton *stitchButton;
	CEdit   *minStripEdit;
	
	bool bRendering;
	bool bVBCreated;
	bool bStitch;
	D3DFILLMODE fillMode;
	
	bool bMouseDown;
	
	CPoint startPoint;
	
	enum RenderingSetting {
		RS_NORMAL,
		RS_VIZSTRIPS,
		RS_VIZCACHE
	} renderSetting;
	
	int cacheSize;
	
	D3DXMATRIX proj, view, world, spin, mouseSpin;
	
	MyVertexVec vertices;
	MyVertexVec optimizedVerts;
	MyFaceVec faces;
	NvStripInfoVec strips;
	NvFaceInfoVec leftoverFaces;
	WordVec stripIndices;
	
	
	HWND m_hD3DWin;
	LPDIRECT3DDEVICE7       m_pD3DDev;
	LPDIRECT3D7             m_pD3D;
	LPDIRECTDRAW7           m_pDD;
	D3DMATERIAL7            m_Material;
	D3DLIGHT7               m_Light0;
	D3DLIGHT7               m_Light1;
	
	
	LPDIRECT3DVERTEXBUFFER7 vertexBuffer;
	
	//don't need indices for these next four, as they'll be rendered
	// with DPVB, not DIPVB
	LPDIRECT3DVERTEXBUFFER7 vertexBufferVizStrips;
	LPDIRECT3DVERTEXBUFFER7 vertexBufferVizCache;
	LPDIRECT3DVERTEXBUFFER7 vertexBufferVizCacheFaces; //for the leftover faces
	LPDIRECT3DVERTEXBUFFER7 vertexBufferFaces;  //for the leftover faces
	
	WORD* indices;
	
	ID3DXContext*           m_pD3DX;
	
	void FixForSpock();
	void SetupMatrices(const Extents& e);
	int LoadFile(const char* fileName, Extents& e, bool bBrokenMesh);
	int SaveFile(const char* fileName);
	HRESULT InitD3DX();
	HRESULT InitRenderer();
	HRESULT Render();
	HRESULT CreateVB();
	HRESULT CreateStripCacheAndFaceVBs();
	void ComputeNormal(MyFace& face);
	void OptimizeVertices();
	void CreateStrips();
	bool NextIsCW(const int numIndices);
	bool IsCW(NvFaceInfo *faceInfo, int v0, int v1);
	void ReleaseModelSpecificMemory(bool bClearLoadedData);
	
	// Generated message map functions
	//{{AFX_MSG(CNvStripifierDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLoadButton();
	afx_msg void OnSavebutton();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRadViznone();
	afx_msg void OnRadVizcache();
	afx_msg void OnRadVizstrips();
	afx_msg void OnBtStripify();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnWireframe();
	afx_msg void OnStitch();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NVTRISTRIPDLG_H__1FFA50E6_7532_11D4_9C70_005004C20C3D__INCLUDED_)
