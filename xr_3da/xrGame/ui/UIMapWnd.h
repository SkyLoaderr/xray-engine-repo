// CUIMapWnd.h:  диалог итерактивной карты
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"

#include "UIWndCallback.h"

class CUIMapWnd;
class CUIGlobalMapSpot;
class CLevelFogOfWar;
class CUIFrameWindow;
class CUIScrollBar;
class CUIFrameLineWnd;
class CMapActionPlanner;

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
public:

	virtual void	SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	virtual Fvector2 ConvertRealToLocal		(const Fvector2& src);// pixels->pixels (relatively own left-top pos)

					CUIGlobalMap			(CUIMapWnd*	pMapWnd);
	virtual			~CUIGlobalMap			();
	
	virtual void	Init					(shared_str name, CInifile& gameLtx, LPCSTR sh_name);
	virtual void	Draw					();
	virtual void	Update					();

	CUIMapWnd*		MapWnd					() {return m_mapWnd;}
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


DEFINE_MAP(shared_str,CUICustomMap*,GameMaps,GameMapsPairIt);


class CUIMapWnd: public CUIWindow, public CUIWndCallback
{
	typedef CUIWindow inherited;
	enum lmFlags{	lmMouseHold		= (1<<0),
					lmUserSpotAdd	= (1<<1),
				};
	Flags32						m_flags;

	CUICustomMap*				m_activeMap;
	CUIGlobalMap*				m_GlobalMap;
	GameMaps					m_GameMaps;
	
	CUIFrameWindow*				m_UIMainFrame;
	CUIScrollBar*				m_UIMainScrollV;
	CUIScrollBar*				m_UIMainScrollH;
	CUIWindow*					m_UILevelFrame;
	CMapActionPlanner*			m_ActionPlanner;
	CUIFrameLineWnd*			UIMainMapHeader;
	CUIStatic*					m_MapText;
	void						OnScrollV				();
	void						OnScrollH				();
public:
								CUIMapWnd				();
	virtual						~CUIMapWnd				();

	virtual void				Init					(LPCSTR xml_name, LPCSTR start_from);
	virtual void				Show					(bool status);
	virtual void				Draw					();
	virtual void				Update					();
			void				ShowHint				();

	virtual void				OnMouse					(float x, float y, EUIMessages mouse_action);
	virtual bool				OnKeyboard				(int dik, EUIMessages keyboard_action);
	virtual void				SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	void						SetActivePoint			(const Fvector &vPoint);
	void						SetActiveMap			(shared_str level_name);
	void						SetActiveMap			(shared_str level_name, Fvector2 pos);
	void						SetActiveMap			(CUICustomMap* m);
	
	Frect						ActiveMapRect			()		{return m_UILevelFrame->GetAbsoluteRect();};
	void						AddMapToRender			(CUICustomMap*);
	void						RemoveMapToRender		(CUICustomMap*);
	CUIGlobalMap*				GlobalMap				()		{return m_GlobalMap;};
	CUICustomMap*				ActiveMap				()		{return m_activeMap;};
	const GameMaps&				GameMaps				()		{return m_GameMaps;};	
	CUICustomMap*				GetMapByIdx				(u16 idx);
	u16							GetIdxByName			(const shared_str& map_name);
	void						SetStatusInfo			(LPCSTR status_text);
	void						UpdateScroll			();
	void						AddUserMapSpot			();
	shared_str					cName					() const	{return "ui_map_wnd";};
};
