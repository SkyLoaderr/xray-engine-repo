// CUIMapWnd.h:  диалог итерактивной карты
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIWindow.h"
#include "UIWndCallback.h"


class CUICustomMap;
class CUIGlobalMap;
class CUIFrameWindow;
class CUIScrollBar;
class CUIFrameLineWnd;
class CMapActionPlanner;
class CUITabControl;
class CUIStatic;
class CUI3tButton;

DEFINE_MAP(shared_str,CUICustomMap*,GameMaps,GameMapsPairIt);


class CUIMapWnd: public CUIWindow, public CUIWndCallback
{
	typedef CUIWindow inherited;
	enum lmFlags{	lmUserSpotAdd	= (1<<1),
					lmUserSpotRemove= (1<<2),
					lmZoomIn		= (1<<3),
					lmZoomOut		= (1<<4),
					lmFirst			= (1<<5),
				};
	enum EMapToolBtn{	eGlobalMap=0,
						eNextMap,
						ePrevMap,
						eZoomIn,
						eZoomOut,
						eAddSpot,
						eRemoveSpot,
						eActor,
						eMaxBtn};
	Flags32						m_flags;
	float						m_currentZoom;
	CUIGlobalMap*				m_GlobalMap;
	GameMaps					m_GameMaps;
	
	CUIFrameWindow*				m_UIMainFrame;
	CUIScrollBar*				m_UIMainScrollV;
	CUIScrollBar*				m_UIMainScrollH;
	CUIWindow*					m_UILevelFrame;
	CMapActionPlanner*			m_ActionPlanner;
	CUIFrameLineWnd*			UIMainMapHeader;
	CUI3tButton*				m_ToolBar[eMaxBtn];
	CUIStatic*					m_MapText;
	void						OnScrollV				();
	void						OnScrollH				();
	void						OnToolGlobalMapClicked	(CUIWindow*, void*);
	void						OnToolActorClicked		(CUIWindow*, void*);
	void						OnToolNextMapClicked	(CUIWindow*, void*);
	void						OnToolPrevMapClicked	(CUIWindow*, void*);
	void						OnToolZoomInClicked		(CUIWindow*, void*);
	void						OnToolZoomOutClicked	(CUIWindow*, void*);
	void						OnToolAddSpotClicked	(CUIWindow*, void*);
	void						OnToolRemoveSpotClicked	(CUIWindow*, void*);
	void						ValidateToolBar			();
public:
	CUICustomMap*				m_tgtMap;
public:
								CUIMapWnd				();
	virtual						~CUIMapWnd				();

	virtual void				Init					(LPCSTR xml_name, LPCSTR start_from);
	virtual void				Show					(bool status);
	virtual void				Draw					();
	virtual void				Update					();
			void				ShowHint				();
			float				GetZoom					()	{return m_currentZoom;}
			void				SetZoom					(float value);

	virtual void				OnMouse					(float x, float y, EUIMessages mouse_action);
	virtual bool				OnKeyboard				(int dik, EUIMessages keyboard_action);
	virtual void				SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	void						SetTargetMap			(CUICustomMap* m);
	void						SetTargetMap			(CUICustomMap* m, const Fvector2& pos);
	void						SetTargetMap			(const shared_str& name, const Fvector2& pos);
	void						SetTargetMap			(const shared_str& name);
	
	Frect						ActiveMapRect			()		{return m_UILevelFrame->GetAbsoluteRect();};
	void						AddMapToRender			(CUICustomMap*);
	void						RemoveMapToRender		(CUICustomMap*);
	CUIGlobalMap*				GlobalMap				()		{return m_GlobalMap;};
	const GameMaps&				GameMaps				()		{return m_GameMaps;};	
	CUICustomMap*				GetMapByIdx				(u16 idx);
	u16							GetIdxByName			(const shared_str& map_name);
	void						SetStatusInfo			(LPCSTR status_text);
	void						UpdateScroll			();
	shared_str					cName					() const	{return "ui_map_wnd";};
};
