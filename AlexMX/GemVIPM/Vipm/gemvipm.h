#ifndef gemvipmH
#define gemvipmH

#include "TomsD3DLib.h"
#include "object.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "DXUtil.h"

//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Application class. The base class (CD3DApplication) provides the 
//       generic functionality needed in all Direct3D samples. CMyD3DApplication 
//       adds functionality specific to this sample program.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	CD3DFont*          m_pFont;

	BOOL				m_bShowSlidingWindowInfo;
	float				m_fSlidingWindowErrorTolerance;
	int					m_iFindBestErrorCountdown;

	int					m_iTargetNumCollapses;
	float				m_fTargetErrorFactor;
	BOOL				m_bTargetErrorAutoGen;
	BOOL				m_bWireframe;
	int					m_iCreateThisManyCollapses;


	MeshEdge			*m_pedgeBestError;
	MeshPt				*m_pptBestError;


	ObjectInstance		m_ObjectInstRoot;
	Object				*m_pObject;
	D3DXMATRIX			m_matView;
	D3DXMATRIX			m_matProjClose;
	D3DXMATRIX			m_matProjCloseZbias;
	D3DXMATRIX			m_matProjFar;

	HRESULT ConfirmDevice( D3DCAPS8*, DWORD, D3DFORMAT );
	LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

protected:
	HRESULT OneTimeSceneInit();
	HRESULT InitDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT Render();
	HRESULT FrameMove();
	HRESULT FinalCleanup();
	void SetMenuItems();

	void CollapseAll();
public:
	CMyD3DApplication();
};

extern BOOL				g_bActive;
extern HINSTANCE		g_hInst;
extern BOOL				g_bExclusive;
extern DIMOUSESTATE2	g_dims;      // DirectInput mouse state structure

HRESULT SetAcquire		(HWND hWnd);
HRESULT InitDirectInput	(HWND hWnd, bool bExclusive = FALSE);
HRESULT UpdateInputState(HWND hWnd);
HRESULT InitDirectInput	(HWND hWnd, bool bExclusive);
HRESULT FreeDirectInput	();
HRESULT SetExclusiveMode(bool bExclusive, HWND hWnd);

#endif