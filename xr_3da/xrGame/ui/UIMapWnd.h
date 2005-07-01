// CUIMapWnd.h:  диалог итерактивной карты
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"

#include "UIButton.h"
#include "UIScrollBar.h"

#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIWndCallback.h"

class CUIMapWnd;
class CUIGlobalMapSpot;
class CLevelFogOfWar;

class CUICustomMap : public CUIStatic, public CUIWndCallback
{
protected:	
	shared_str		m_name;
	Frect			m_BoundRect;// real map size (meters)
	float			m_zoom_factor;
	
public:
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
	float			GetCurrentZoom					(){return m_zoom_factor;}
	const Frect&    BoundRect						()const					{return m_BoundRect;};
	virtual void	OptimalFit						(const Frect& r);
	virtual	void	MoveWndDelta					(const Fvector2& d);

	shared_str		MapName							() {return m_name;}
	virtual CUIGlobalMapSpot*	GlobalMapSpot		() {return NULL;}
	virtual LPCSTR	GetHint							(); 

	virtual void	SetZoomFactor					(float z);

	virtual void	Draw							();
	virtual void	Update							();

			bool	IsRectVisible					(Frect r);
			bool	NeedShowPointer					(Frect r);
protected:
	virtual void	UpdateSpots						() {};
};


class CUIGlobalMap: public CUICustomMap{
	typedef  CUICustomMap inherited;
public:
	enum EState{
		stNone,
		stMinimized,
		stNormal,
		stMaximized
	};
private:
	shared_str		m_prev_active_map;
	CUIButton		m_maximized_btn;
	CUIButton		m_minimized_btn;
	Fvector2		m_maximized_btn_offset;
	Fvector2		m_minimized_btn_offset;

	Fvector2		m_MinimizedSize;
	Fvector2		m_NormalSize;
	EState			m_State;
	void			OnBtnMinimizedClick		();
	void			OnBtnMaximizedClick		();
	CUIMapWnd*		m_mapWnd;
public:
	virtual void	SetActivePoint			(const Fvector &vNewPoint){};//do nothing
	void			SwitchTo				(EState new_state);

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


	virtual void				Init				(u32 color);
	virtual void				OnMouse				(float x, float y, EUIMessages mouse_action);
	virtual void				Update				();
	virtual void				Draw				();
	virtual LPCSTR				GetHint				();
protected:
	CUIFrameWindow				UIBorder;
};

class CUILevelMap: public CUICustomMap{
	typedef  CUICustomMap inherited;

	Frect						m_GlobalRect;			// virtual map size (meters)
	CUIGlobalMapSpot			*m_globalMapSpot;		//rect on the global map
	CUIStatic					*m_anomalies_map;
private:
								CUILevelMap			(const CUILevelMap &obj) {}
			CUILevelMap			&operator=			(const CUILevelMap &obj) {}

public:
								CUILevelMap			();
	virtual						~CUILevelMap		();
	virtual void				Init				(shared_str name, CInifile& gameLtx, LPCSTR sh_name);
	const Frect&				GlobalRect			() const								{return m_GlobalRect;}
	virtual CUIGlobalMapSpot*	GlobalMapSpot		()										{return m_globalMapSpot;}
	virtual void				Draw				();
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
	enum lmFlags{lmMouseHold = 1,};
	Flags32						m_flags;


	CUICustomMap*				m_activeLevelMap;

	CUIGlobalMap*				m_GlobalMap;
	GameMaps					m_GameMaps;

	CUIFrameWindow				m_UIMainFrame;
	CUIScrollBar				m_UIMainScrollV,	m_UIMainScrollH;
	CUIStatic					m_UILevelFrame;

	CUIFrameLineWnd				UIMainMapHeader;
	void						OnScrollV				();
	void						OnScrollH				();
	void						UpdateScroll			();
public:
								CUIMapWnd				();
	virtual						~CUIMapWnd				();

	virtual void				Init					();
	virtual void				Show					(bool status);
	virtual void				Draw					();
	virtual void				Update					();
			void				ShowHint				();

	virtual void				OnMouse					(float x, float y, EUIMessages mouse_action);
	virtual void				SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	void						InitGlobalMapObjectives	();
	void						InitLocalMapObjectives	();

	void						SetActivePoint			(const Fvector &vNewPoint);
	void						SetActiveMap			(shared_str level_name);
	CUICustomMap*				ActiveLevelMap			() {return m_activeLevelMap;};

	void						SetStatusInfo			(LPCSTR status_text);
};
