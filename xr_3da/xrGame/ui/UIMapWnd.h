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
	enum lmFlags{	lmMouseHold		= (1<<0),
					lmUserSpotAdd	= (1<<1),
				};
	enum EMapToolBtn{	eGlobalMap=0,
						eNextMap,
						ePrevMap,
						eZoomIn,
						eZoomOut,
						eAddSpot,
						eRemoveSpot,
						eMaxBtn};
	Flags32						m_flags;
	float						m_currentZoom;
	CUICustomMap*				m_activeMap;
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
	void						OnToolNextMapClicked	(CUIWindow*, void*);
	void						OnToolPrevMapClicked	(CUIWindow*, void*);
	void						OnToolZoomInClicked		(CUIWindow*, void*);
	void						OnToolZoomOutClicked	(CUIWindow*, void*);
	void						OnToolAddSpotClicked	(CUIWindow*, void*);
	void						OnToolRemoveSpotClicked	(CUIWindow*, void*);
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
	shared_str					cName					() const	{return "ui_map_wnd";};
};
