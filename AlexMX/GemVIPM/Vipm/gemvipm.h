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
	struct Fvector3{
		float x,y,z;
		Fvector3& set	(float _x, float _y, float _z)
		{
			x = _x;
			y = _y;
			z = _z;
			return *this;	
		}
		Fvector3& setHP	(float h, float p)
		{
			float _ch=cosf(h), _cp=cosf(p), _sh=sinf(h), _sp=sinf(p);
			x = -_cp*_sh;
			y = _sp;
			z = _cp*_ch;
			return *this;	
		}
		Fvector3& mul	(Fvector3& v, float f)
		{
			x = v.x*f;
			y = v.y*f;
			z = v.z*f;
			return *this;	
		}

	};
	Fvector3			m_CameraHPB;
	float				m_CameraDist;

	void				BuildCamera			();
	void				CreateTestObject	();
	void				RenderCurrentObject	(int iSlidingWindowLevel, int& r_tris);
	void				RenderCurrentEdges	(int& r_edges);

	CD3DFont*			m_pFont;

	float				m_fSlidingWindowErrorTolerance;

	Object*				m_pObject;
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