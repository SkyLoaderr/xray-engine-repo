#pragma once

#define UI_BASE_WIDTH	1024.0f
#define UI_BASE_HEIGHT	768.0f

struct CFontManager;
class CUICursor;


class CDeviceResetNotifier :public pureDeviceReset
{
public:
						CDeviceResetNotifier					()	{Device.seqDeviceReset.Add(this,REG_PRIORITY_NORMAL);};
	virtual				~CDeviceResetNotifier					()	{Device.seqDeviceReset.Remove(this);};
	virtual void		OnDeviceReset							()	{};

};



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

class C2DFrustum{//only rect form
	svector<Fplane2,FRUSTUM_MAXPLANES> planes;
	Frect						m_rect;
public:
	void		CreateFromRect	(const Frect& rect);
	sPoly2D*	ClipPoly		(sPoly2D& S, sPoly2D& D) const;
};

class ui_core: public CDeviceResetNotifier
{
	C2DFrustum		m_2DFrustum;
	C2DFrustum		m_2DFrustumPP;
	bool			m_bPostprocess;

	CFontManager*	m_pFontManager;
	CUICursor*		m_pUICursor;

	Fvector2		m_pp_scale_;
	Fvector2		m_scale_;
	Fvector2*		m_current_scale;
public:
	xr_stack<Frect> m_Scissors;
	
	ui_core			();
	~ui_core		();
	CFontManager*	Font							()							{return m_pFontManager;}
	CUICursor*		GetUICursor						()							{return m_pUICursor;}
	void			OutText							(CGameFont *pFont, Frect r, float x, float y, LPCSTR fmt, ...);

//.	IC float		GetScaleX						()							{return (m_bPostprocess)?float(::Render->getTarget()->get_width())/float(UI_BASE_WIDTH):float(Device.dwWidth)/float(UI_BASE_WIDTH);   }
//.	IC float		GetScaleY						()							{return (m_bPostprocess)?float(::Render->getTarget()->get_height())/float(UI_BASE_HEIGHT):float(Device.dwHeight)/float(UI_BASE_HEIGHT);   }

//.	IC float		GetScaleX						()							{return (m_bPostprocess) ? m_pp_scale.x : m_scale.x;   }
//.	IC float		GetScaleY						()							{return (m_bPostprocess) ? m_pp_scale.y : m_scale.y;   }

	void			ClientToScreenScaled			(Fvector2& dest, float left, float top);
	float			ClientToScreenScaledX			(float left);
	float			ClientToScreenScaledY			(float top);

	Frect			ScreenRect						();
	const C2DFrustum& ScreenFrustum					(){return (m_bPostprocess)?m_2DFrustumPP:m_2DFrustum;}
	void			PushScissor						(const Frect& r, bool overlapped=false);
	void			PopScissor						();

	void			pp_start						();
	void			pp_stop							();
	void			RenderFont						();

	virtual void	OnDeviceReset					();
};

extern CUICursor*	GetUICursor		();
extern ui_core*		UI				();
