#pragma once

struct CFontManager;
class CUIWindow;
class CUIDialogWnd;
class CUICursor;
#include "../IInputReceiver.h"
#include "../IGame_Persistent.h"
#include "UIDialogHolder.h"
#include "UI.h"

//---------------------------------------------------------------------------------------
// 2D Frustum & 2D Vertex
//---------------------------------------------------------------------------------------
struct S2DVert{
	Fvector2	pt;
	Fvector2	uv;
				S2DVert		(){}
				S2DVert		(float pX, float pY, float tU, float tV){pt.set(pX,pY);uv.set(tU,tV);}
	void		set			(float pt_x, float pt_y, float uv_x, float uv_y){pt.set(pt_x,pt_y);uv.set(uv_x,uv_y);}
	void		set			(const Fvector2& _pt, const Fvector2& _uv){pt.set(_pt);uv.set(_uv);}
	void		rotate_pt	(const Fvector2& pivot, float cosA, float sinA);
};
#define UI_FRUSTUM_MAXPLANES	12
#define UI_FRUSTUM_SAFE			(UI_FRUSTUM_MAXPLANES*4)
typedef svector<S2DVert,UI_FRUSTUM_SAFE>		sPoly2D;

class C2DFrustum{
	svector<Fplane2,FRUSTUM_MAXPLANES> planes;
public:
	void		CreateFromRect	(const Irect& rect);
	sPoly2D*	ClipPoly		(sPoly2D& S, sPoly2D& D) const;
};
//---------------------------------------------------------------------------------------

class CMainUI :
	public IMainUI,
	public IInputReceiver,
	public pureRender,
	public CDialogHolder

{
	CFontManager*	m_pFontManager;
	CUICursor*		m_pUICursor;

	CUIDialogWnd*	m_startDialog;

//	float			m_fDevScaleX;
//	float			m_fDevScaleY;

	enum{
		flRestoreConsole	= (1<<0),
		flRestorePause		= (1<<1),
		flActive			= (1<<2),
		flNeedChangeCapture	= (1<<3),
	};
	Flags8			m_Flags;
	xr_stack<Irect> m_Scissors;
	C2DFrustum		m_2DFrustum;
public:
					CMainUI							();
	virtual			~CMainUI						();
	CUICursor*		GetUICursor						()							{return m_pUICursor;}

	virtual void	Activate						(bool bActive); 
	virtual	bool	IsActive						(); 

	CFontManager*	Font							()							{return m_pFontManager;}
	//IInputReceiver
	virtual void	IR_OnMousePress					(int btn);
	virtual void	IR_OnMouseRelease				(int btn);
	virtual void	IR_OnMouseHold					(int btn);
	virtual void	IR_OnMouseMove					(int x, int y);
	virtual void	IR_OnMouseStop					(int x, int y);

	virtual void	IR_OnKeyboardPress				(int dik);
	virtual void	IR_OnKeyboardRelease			(int dik);
	virtual void	IR_OnKeyboardHold				(int dik);

	virtual void	IR_OnMouseWheel					(int direction)	;

	//pureRender
	virtual void	OnRender						(void);

	//pureFrame
	virtual void	OnFrame							(void);


	void			OutText							(CGameFont *pFont, Irect r, float x, float y, LPCSTR fmt, ...);

	void			OnDeviceCreate					();

	float			GetScaleX						()							{return float(Device.dwWidth)/float(UI_BASE_WIDTH);   }
	float			GetScaleY						()							{return float(Device.dwHeight)/float(UI_BASE_HEIGHT); }
//	void			SetScaleXY						(float f_x, float f_y);

	void			ClientToScreenScaled			(Irect& r, u32 align);
	void			ClientToScreenScaled			(Ivector2& p, u32 align);
	void			ClientToScreenScaled			(Ivector2& dest, int left, int top, u32 align);
	int				ClientToScreenScaledX			(int left, u32 align);
	int				ClientToScreenScaledY			(int top, u32 align);
	void			ClientToScreen					(Irect& r, u32 align);
	void			ClientToScreen					(Ivector2& dest, int left, int top, u32 align);
	int				ClientToScreenX					(int left, u32 align);
	int				ClientToScreenY					(int top, u32 align);

	Irect			ScreenRect						();
	const C2DFrustum& ScreenFrustum					(){return m_2DFrustum;}
	void			PushScissor						(const Irect& r, bool overlapped=false);
	void			PopScissor						();
};


extern CUICursor*	GetUICursor();
extern CMainUI*		UI();
