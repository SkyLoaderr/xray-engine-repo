#pragma once
#include "UIStatic.h"
#include "UIWndCallback.h"

class CUIGlobalMapSpot;
class CUIMapWnd;

class CUICustomMap : public CUIStatic, public CUIWndCallback
{
protected:	
	shared_str		m_name;
	Frect			m_BoundRect;// real map size (meters)
	Flags16			m_flags;
	enum EFlags{	eLocked	=(1<<0),};

public:
	Frect			m_prevRect;
					CUICustomMap					();
	virtual			~CUICustomMap					();
	virtual void	SetActivePoint					(const Fvector &vNewPoint);

	virtual void	Init							(shared_str name, CInifile& gameLtx, LPCSTR sh_name);
	virtual Fvector2 ConvertRealToLocal				(const Fvector2& src);// meters->pixels (relatively own left-top pos)
	Fvector2		ConvertLocalToReal				(const Fvector2& src);
	Fvector2		ConvertRealToLocalNoTransform	(const Fvector2& src);// meters->pixels (relatively own left-top pos)

	bool			GetPointerTo					(const Fvector2& src, float item_radius, Fvector2& pos, float& heading);//position and heading for drawing pointer to src pos

	void			FitToWidth						(float width);
	void			FitToHeight						(float height);
	float			GetCurrentZoom					(){return GetWndRect().width()/m_BoundRect.width();}
	const Frect&    BoundRect						()const					{return m_BoundRect;};
	virtual void	OptimalFit						(const Frect& r);
	virtual	void	MoveWndDelta					(const Fvector2& d);

	shared_str		MapName							() {return m_name;}
	virtual CUIGlobalMapSpot*	GlobalMapSpot		() {return NULL;}
	virtual LPCSTR	GetHint							(); 

	virtual void	Draw							();
	virtual void	Update							();

			bool	IsRectVisible					(Frect r);
			bool	NeedShowPointer					(Frect r);
			bool	Locked							()				{return !!m_flags.test(eLocked);}
			void	SetLocked						(bool b)		{m_flags.set(eLocked,b);}
protected:
	virtual void	UpdateSpots						() {};
};


class CUIGlobalMap: public CUICustomMap{
	typedef  CUICustomMap inherited;
public:
private:
	shared_str		m_prev_active_map;
	CUIMapWnd*		m_mapWnd;
	float			m_minZoom;
public:

	virtual void	SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	virtual Fvector2 ConvertRealToLocal		(const Fvector2& src);// pixels->pixels (relatively own left-top pos)

					CUIGlobalMap			(CUIMapWnd*	pMapWnd);
	virtual			~CUIGlobalMap			();
	
	IC void			SetMinZoom				(float zoom){m_minZoom=zoom;}
	IC float		GetMinZoom				(){return m_minZoom;}

	virtual void	Init					(shared_str name, CInifile& gameLtx, LPCSTR sh_name);
	virtual void	Draw					();
	virtual void	Update					();

	CUIMapWnd*		MapWnd					() {return m_mapWnd;}
	void			MoveWndDelta			(const Fvector2& d);
};

class CUILevelMap;
class CUIGlobalMapSpot: public CUIWindow
{
	typedef CUIWindow inherited;

	CUILevelMap*				m_owner_map;
public:
								CUIGlobalMapSpot	(CUICustomMap* m);
	virtual						~CUIGlobalMapSpot	();


	virtual void				Init				(u32 color, LPCSTR tex_name);
	virtual void				OnMouse				(float x, float y, EUIMessages mouse_action);
	virtual void				Update				();
	virtual void				Draw				();
	virtual LPCSTR				GetHint				();
protected:
	CUIStatic*					UIBorder;
};

class CUILevelMap: public CUICustomMap{
	typedef  CUICustomMap inherited;
	CUIMapWnd*					m_mapWnd;
	Frect						m_GlobalRect;			// virtual map size (meters)
	CUIGlobalMapSpot*			m_globalMapSpot;		//rect on the global map
	CUIStatic*					m_anomalies_map;
	Fvector2					m_tgtCenter;			// for moving
private:
								CUILevelMap			(const CUILevelMap &obj) {}
			CUILevelMap			&operator=			(const CUILevelMap &obj) {}

public:
								CUILevelMap			(CUIMapWnd*);
	virtual						~CUILevelMap		();
	virtual void				Init				(shared_str name, CInifile& gameLtx, LPCSTR sh_name);
	const Frect&				GlobalRect			() const								{return m_GlobalRect;}
	virtual CUIGlobalMapSpot*	GlobalMapSpot		()										{return m_globalMapSpot;}
	virtual void				Draw				();
	virtual void				Update				();
	virtual void				OnMouse				(float x, float y, EUIMessages mouse_action);
	
	Frect						CalcWndRectOnGlobal	();
	void						CalcOpenRect		(const Fvector2& destLevelMapLT, const float map_zoom, Frect& lmap_des_rect, Frect& gmap_des_rect);
	CUIMapWnd*					MapWnd				() {return m_mapWnd;}
	Fvector2&					TargetCenter		() {return m_tgtCenter;}
protected:
	virtual void				UpdateSpots			();


};

class CUIMiniMap: public CUICustomMap{
	typedef  CUICustomMap inherited;

public:
								CUIMiniMap			();
	virtual						~CUIMiniMap			();
	virtual void				Init				(shared_str name, CInifile& gameLtx, LPCSTR sh_name);
	virtual	void				MoveWndDelta		(const Fvector2& d);
protected:
	virtual void				UpdateSpots			();

};
