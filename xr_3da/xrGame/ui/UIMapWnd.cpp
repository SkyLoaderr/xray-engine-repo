// CUIMapWnd.cpp:  ������ ������������� �����
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIMapWnd.h"
#include "UIMap.h"
#include "UIXmlInit.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "../map_manager.h"
#include "UIInventoryUtilities.h"
#include "../map_location.h"


#include "UIScrollBar.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UITabControl.h"
#include "UIMapWndActions.h"
#include "UIMapWndActionsSpace.h"

#include "../HUDManager.h"

#include <dinput.h>				//remove me !!!
#include "../../xr_input.h"		//remove me !!!

#include "../gameTaskmanager.h" //remove me !!!
#include "../gameTask.h"		//remove me !!!
#include "../actor.h"			//remove me !!!


const				SCROLLBARS_SHIFT			= 5;
const				VSCROLLBAR_STEP				= 20; // � ��������
const				HSCROLLBAR_STEP				= 20; // � ��������



CUIMapWnd::CUIMapWnd()
{
	m_activeMap				= NULL;
	m_GlobalMap				= NULL;
	m_flags.zero			();
}

CUIMapWnd::~CUIMapWnd()
{
	delete_data			(m_ActionPlanner);
	delete_data			(m_GameMaps);
	delete_data			(m_MapText);
}

void CUIMapWnd::SetActivePoint(const Fvector &vNewPoint)
{
	if(!ActiveMap())return;
	ActiveMap()->SetActivePoint(vNewPoint);
	UpdateScroll();
}

void CUIMapWnd::Init(LPCSTR xml_name, LPCSTR start_from)
{
	CUIXml uiXml;
	bool xml_result					= uiXml.Init(CONFIG_PATH, UI_PATH, xml_name);
	R_ASSERT3						(xml_result, "xml file not found", xml_name);

	string512	pth;
	// load map background
	CUIXmlInit xml_init;
	strconcat(pth,start_from,":main_wnd");
	xml_init.InitWindow				(uiXml, pth, 0, this);



	m_UIMainFrame					= xr_new<CUIFrameWindow>(); m_UIMainFrame->SetAutoDelete(true);
	AttachChild						(m_UIMainFrame);
	strconcat(pth,start_from,":main_wnd:main_map_frame");
	xml_init.InitFrameWindow		(uiXml, pth, 0, m_UIMainFrame);

	m_UILevelFrame					= xr_new<CUIWindow>(); m_UILevelFrame->SetAutoDelete(true);
	strconcat(pth,start_from,":main_wnd:main_map_frame:level_frame");
	xml_init.InitWindow				(uiXml, pth, 0, m_UILevelFrame);
	m_UIMainFrame->AttachChild		(m_UILevelFrame);

	Frect r							= m_UILevelFrame->GetWndRect();

	m_UIMainScrollV					= xr_new<CUIScrollBar>(); m_UIMainScrollV->SetAutoDelete(true);
	m_UIMainScrollV->Init			(r.right + SCROLLBARS_SHIFT, r.top, r.bottom - r.top, false);
	m_UIMainScrollV->SetWindowName	("scroll_v");
	m_UIMainScrollV->SetStepSize	(_max(1,iFloor(m_UILevelFrame->GetHeight()/10)));
	m_UIMainScrollV->SetPageSize	(iFloor(m_UILevelFrame->GetHeight()));
	m_UIMainFrame->AttachChild		(m_UIMainScrollV);
	Register						(m_UIMainScrollV);
	AddCallback						("scroll_v",SCROLLBAR_VSCROLL,boost::bind(&CUIMapWnd::OnScrollV,this));

	m_UIMainScrollH					= xr_new<CUIScrollBar>(); m_UIMainScrollH->SetAutoDelete(true);
	m_UIMainScrollH->Init			(r.left, r.bottom + SCROLLBARS_SHIFT, r.right - r.left, true);
	m_UIMainScrollH->SetWindowName	("scroll_h");
	m_UIMainScrollH->SetStepSize	(_max(1,iFloor(m_UILevelFrame->GetWidth()/10)));
	m_UIMainScrollH->SetPageSize	(iFloor(m_UILevelFrame->GetWidth()));
	m_UIMainFrame->AttachChild		(m_UIMainScrollH);
	Register						(m_UIMainScrollH);

	AddCallback						("scroll_h",SCROLLBAR_HSCROLL,boost::bind(&CUIMapWnd::OnScrollH,this));
	AddCallback						("map",SCROLLBAR_NEEDUPDATE,boost::bind(&CUIMapWnd::UpdateScroll,this));
	
	UIMainMapHeader					= xr_new<CUIFrameLineWnd>(); UIMainMapHeader->SetAutoDelete(true);
	m_UIMainFrame->AttachChild		(UIMainMapHeader);
	strconcat(pth,start_from,":main_wnd:map_header_frame_line");
	xml_init.InitFrameLine			(uiXml, pth, 0, UIMainMapHeader);

	ZeroMemory						(m_ToolBar,sizeof(m_ToolBar));
	xr_string  sToolbar;
	sToolbar	= xr_string(start_from) + ":main_wnd:map_header_frame_line:tool_bar";

	EMapToolBtn		btnIndex;
	btnIndex		= eGlobalMap;
	strconcat(pth, sToolbar.c_str(), ":global_map_btn");
	if(uiXml.NavigateToNode(pth,0)){
		m_ToolBar[btnIndex]				= xr_new<CUI3tButton>(); m_ToolBar[btnIndex]->SetAutoDelete(true);
		xml_init.Init3tButton			(uiXml, pth, 0, m_ToolBar[btnIndex]);
		UIMainMapHeader->AttachChild	(m_ToolBar[btnIndex]);
		Register						(m_ToolBar[btnIndex]);
		AddCallback						(*m_ToolBar[btnIndex]->WindowName(),BUTTON_CLICKED,boost::bind(&CUIMapWnd::OnToolGlobalMapClicked,this,_1,_2));
	}

	btnIndex		= eNextMap;
	strconcat(pth, sToolbar.c_str(), ":next_map_btn");
	if(uiXml.NavigateToNode(pth,0)){
		m_ToolBar[btnIndex]				= xr_new<CUI3tButton>(); m_ToolBar[btnIndex]->SetAutoDelete(true);
		xml_init.Init3tButton			(uiXml, pth, 0, m_ToolBar[btnIndex]);
		UIMainMapHeader->AttachChild	(m_ToolBar[btnIndex]);
		Register						(m_ToolBar[btnIndex]);
		AddCallback						(*m_ToolBar[btnIndex]->WindowName(),BUTTON_CLICKED,boost::bind(&CUIMapWnd::OnToolNextMapClicked,this,_1,_2));
	}

	btnIndex		= ePrevMap;
	strconcat(pth, sToolbar.c_str(), ":prev_map_btn");
	if(uiXml.NavigateToNode(pth,0)){
		m_ToolBar[btnIndex]				= xr_new<CUI3tButton>(); m_ToolBar[btnIndex]->SetAutoDelete(true);
		xml_init.Init3tButton			(uiXml, pth, 0, m_ToolBar[btnIndex]);
		UIMainMapHeader->AttachChild	(m_ToolBar[btnIndex]);
		Register						(m_ToolBar[btnIndex]);
		AddCallback						(*m_ToolBar[btnIndex]->WindowName(),BUTTON_CLICKED,boost::bind(&CUIMapWnd::OnToolPrevMapClicked,this,_1,_2));
	}

	btnIndex		= eZoomIn;
	strconcat(pth, sToolbar.c_str(), ":zoom_in_btn");
	if(uiXml.NavigateToNode(pth,0)){
		m_ToolBar[btnIndex]				= xr_new<CUI3tButton>(); m_ToolBar[btnIndex]->SetAutoDelete(true);
		xml_init.Init3tButton			(uiXml, pth, 0, m_ToolBar[btnIndex]);
		UIMainMapHeader->AttachChild	(m_ToolBar[btnIndex]);
		Register						(m_ToolBar[btnIndex]);
		AddCallback						(*m_ToolBar[btnIndex]->WindowName(),BUTTON_CLICKED,boost::bind(&CUIMapWnd::OnToolZoomInClicked,this,_1,_2));
	}
	btnIndex		= eZoomOut;
	strconcat(pth, sToolbar.c_str(), ":zoom_out_btn");
	if(uiXml.NavigateToNode(pth,0)){
		m_ToolBar[btnIndex]				= xr_new<CUI3tButton>(); m_ToolBar[btnIndex]->SetAutoDelete(true);
		xml_init.Init3tButton			(uiXml, pth, 0, m_ToolBar[btnIndex]);
		UIMainMapHeader->AttachChild	(m_ToolBar[btnIndex]);
		Register						(m_ToolBar[btnIndex]);
		AddCallback						(*m_ToolBar[btnIndex]->WindowName(),BUTTON_CLICKED,boost::bind(&CUIMapWnd::OnToolZoomOutClicked,this,_1,_2));
	}
	btnIndex		= eAddSpot;
	strconcat(pth, sToolbar.c_str(), ":add_spot_btn");
	if(uiXml.NavigateToNode(pth,0)){
		m_ToolBar[btnIndex]				= xr_new<CUI3tButton>(); m_ToolBar[btnIndex]->SetAutoDelete(true);
		xml_init.Init3tButton			(uiXml, pth, 0, m_ToolBar[btnIndex]);
		UIMainMapHeader->AttachChild	(m_ToolBar[btnIndex]);
		Register						(m_ToolBar[btnIndex]);
		AddCallback						(*m_ToolBar[btnIndex]->WindowName(),BUTTON_CLICKED,boost::bind(&CUIMapWnd::OnToolAddSpotClicked,this,_1,_2));
	}
	btnIndex		= eRemoveSpot;
	strconcat(pth, sToolbar.c_str(), ":remove_spot_btn");
	if(uiXml.NavigateToNode(pth,0)){
		m_ToolBar[btnIndex]				= xr_new<CUI3tButton>(); m_ToolBar[btnIndex]->SetAutoDelete(true);
		xml_init.Init3tButton			(uiXml, pth, 0, m_ToolBar[btnIndex]);
		UIMainMapHeader->AttachChild	(m_ToolBar[btnIndex]);
		Register						(m_ToolBar[btnIndex]);
		AddCallback						(*m_ToolBar[btnIndex]->WindowName(),BUTTON_CLICKED,boost::bind(&CUIMapWnd::OnToolRemoveSpotClicked,this,_1,_2));
	}


// Load maps
	string256								gameLtxPath;
	FS.update_path							(gameLtxPath, CONFIG_PATH, "game.ltx");
	CInifile gameLtx						(gameLtxPath);

	m_GlobalMap								= xr_new<CUIGlobalMap>(this);
	m_GlobalMap->SetAutoDelete				(true);
	m_GlobalMap->Init						("global_map",gameLtx,"hud\\default");

	m_UILevelFrame->AttachChild				(m_GlobalMap);

	m_MapText								= xr_new<CUIStatic>();
	m_MapText->SetFont						(UI()->Font()->pFontGraffiti22Russian);
	m_MapText->SetWndSize					(Fvector2().set(100.0f, 22.0f));

	// initialize local maps
	xr_string sect_name;
	if(GameID()==GAME_SINGLE)
		sect_name = "level_maps_single";
	else
		sect_name = "level_maps_mp";

	if (gameLtx.section_exist(sect_name.c_str())){
		CInifile::Sect& S		= gameLtx.r_section(sect_name.c_str());
		CInifile::SectIt	it	= S.begin(), end = S.end();
		for (;it!=end; it++){
			shared_str map_name = it->first;
			xr_strlwr(map_name);
			R_ASSERT2	(m_GameMaps.end() == m_GameMaps.find(map_name), "Duplicate level name not allowed");
			
			CUICustomMap*& l = m_GameMaps[map_name];

			l = xr_new<CUILevelMap>(this);
			
			l->Init(map_name, gameLtx, "hud\\set");

			l->OptimalFit( m_UILevelFrame->GetWndRect() );
		}
	}

	Register						(m_GlobalMap);
	m_ActionPlanner					= xr_new<CMapActionPlanner>();
	m_ActionPlanner->setup			(this);
	SetActiveMap					(m_GlobalMap);
}

void CUIMapWnd::Show(bool status)
{
	if (status)
	{
		m_GlobalMap->Show	(true);
		GameMaps::iterator	it = m_GameMaps.begin();
		for(;it!=m_GameMaps.end();++it){
			if(it->second->GlobalMapSpot())
				m_GlobalMap->AttachChild	(it->second->GlobalMapSpot());
		}

		if(ActiveMap()==NULL)
			SetActiveMap		(m_GlobalMap->MapName());
//		if(m_activeLevelMap==NULL)
//			SetActiveMap				(Level().name());
//		SetActivePoint				( Level().CurrentEntity()->Position() );
		InventoryUtilities::SendInfoToActor("ui_pda_map_local");
	}else{
		if(GlobalMap()){
			GlobalMap()->DetachAll();
			GlobalMap()->Show(false);
		}
		GameMaps::iterator	it = m_GameMaps.begin();
		for(;it!=m_GameMaps.end();++it)
			it->second->DetachAll();
	}

	inherited::Show(status);
}


void CUIMapWnd::AddMapToRender			(CUICustomMap* m)
{
	Register							( m );
	m_UILevelFrame->AttachChild			( m );
	m->Show								( true );
	m_UILevelFrame->BringToTop			( m );
	m->SetClipRect						( ActiveMapRect() );
}

void CUIMapWnd::RemoveMapToRender		(CUICustomMap* m)
{
	if( m!=GlobalMap() )
		m_UILevelFrame->DetachChild			(smart_cast<CUIWindow*>(m));
}

void CUIMapWnd::SetActiveMap(CUICustomMap* m)
{
	m_activeMap							= m;
	m_UILevelFrame->BringToTop			( m );
	m->SetClipRect						( ActiveMapRect() );
	UpdateScroll						();
}

void CUIMapWnd::SetActiveMap			(shared_str level_name, Fvector2 pos)
{
	u16 map_idx;
	if(level_name == m_GlobalMap->MapName())
		map_idx							= GLOBAL_MAP_IDX;
	else{
		map_idx							= GetIdxByName(level_name);
		CUILevelMap* lm					= smart_cast<CUILevelMap*>(GetMapByIdx(map_idx));
		lm->TargetCenter()				= pos;
	}

	m_ActionPlanner->m_storage.set_property(1, false);
	CWorldState							target_state;
	target_state.add_condition			(CWorldProperty(UIMapWndActionsSpace::make_map_id(map_idx,UIMapWndActionsSpace::ePropMapOpenedIdle),true));
	m_ActionPlanner->set_target_state	(target_state);

}

void CUIMapWnd::SetActiveMap			(shared_str level_name)
{
	u16 map_idx;
	if(level_name == m_GlobalMap->MapName())
		map_idx							= GLOBAL_MAP_IDX;
	else{
		map_idx							= GetIdxByName(level_name);
		CUILevelMap* lm					= smart_cast<CUILevelMap*>(GetMapByIdx(map_idx));
		Frect r							= lm->BoundRect();
		r.getcenter						(lm->TargetCenter());
	}

	m_ActionPlanner->m_storage.set_property(1, false);
	CWorldState							target_state;
	target_state.add_condition			(CWorldProperty(UIMapWndActionsSpace::make_map_id(map_idx,UIMapWndActionsSpace::ePropMapOpenedIdle),true));
	m_ActionPlanner->set_target_state	(target_state);
}


void CUIMapWnd::Draw()
{
	if(ActiveMap())
		ActiveMap()->SetClipRect( ActiveMapRect() );

	inherited::Draw();
	m_MapText->SetWndPos					(ActiveMapRect().lt);
	m_MapText->Draw();
}

bool CUIMapWnd::OnKeyboard				(int dik, EUIMessages keyboard_action)
{
	if (WINDOW_KEY_PRESSED == keyboard_action && DIK_A == dik)
	{
		if(ActiveMap() && ActiveMap()!=GlobalMap())
			m_flags.set(lmUserSpotAdd,TRUE);
		return true;
	}
	if (WINDOW_KEY_PRESSED == keyboard_action && DIK_G == dik)
	{
		SetActiveMap(GlobalMap()->MapName());
		return true;
	}
	if (WINDOW_KEY_PRESSED == keyboard_action && DIK_N == dik)
	{
		u16 curr_map_idx = 0;
		if(GlobalMap()!=ActiveMap()){
			curr_map_idx = GetIdxByName(ActiveMap()->MapName());
			++curr_map_idx;
			if(curr_map_idx == (u16)GameMaps().size()) curr_map_idx = 0;
		}
		SetActiveMap(GetMapByIdx(curr_map_idx)->MapName());
		return true;
	}
	return inherited::OnKeyboard	(dik, keyboard_action);
}

void CUIMapWnd::OnMouse(float x, float y, EUIMessages mouse_action)
{
	inherited::OnMouse(x,y,mouse_action);
	Fvector2 cursor_pos = GetUICursor()->GetPos();

	if(ActiveMap() && !ActiveMap()->Locked() && ActiveMapRect().in( cursor_pos ) ){
		switch (mouse_action){
		case WINDOW_MOUSE_MOVE:
			if( pInput->iGetAsyncBtnState(0) ){
				ActiveMap()->MoveWndDelta	(GetUICursor()->GetPosDelta());
				UpdateScroll					();
			}
		break;
		case WINDOW_LBUTTON_DOWN:
			if(m_flags.test(lmUserSpotAdd) ){
				Fvector2 cp = cursor_pos;
				cp.sub(ActiveMap()->GetAbsolutePos());
				Fvector2 p = ActiveMap()->ConvertLocalToReal(cp);
				Fvector pos;
				pos.set(p.x, 0.0f, p.y);
				CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());
				shared_str spot = "user";
				CMapLocation* ml = Level().MapManager().AddUserLocation(spot, ActiveMap()->MapName(), pos);
				CGameTask* t = pActor->GameTaskManager().GiveGameTaskToActor("user_task",false);
				t->m_Objectives[0].task_state	= eTaskUserDefined;
				t->m_Objectives[1].task_state	= eTaskUserDefined;
				t->m_Objectives[1].object_id	= ml->ObjectID();
				t->m_Objectives[1].map_location	= spot;
				m_flags.set(lmUserSpotAdd, FALSE);
				m_ToolBar[eAddSpot]->SetButtonMode		(CUIButton::BUTTON_NORMAL);
				break;
			}
		break;
		case WINDOW_MOUSE_WHEEL_UP:
			m_UIMainScrollV->TryScrollDec();
		break;
		case WINDOW_MOUSE_WHEEL_DOWN:
			m_UIMainScrollV->TryScrollInc();
		break;


		}	
	};
}

void CUIMapWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}

CUICustomMap*	CUIMapWnd::GetMapByIdx				(u16 idx)
{
	GameMapsPairIt it				= m_GameMaps.begin();
	std::advance(it, idx);
	return it->second;
}

u16 CUIMapWnd::GetIdxByName			(const shared_str& map_name)
{
	GameMapsPairIt it				= m_GameMaps.find(map_name);
	return (u16)std::distance		(m_GameMaps.begin(),it);
}

void CUIMapWnd::UpdateScroll()
{
	Fvector2 w_pos					= ActiveMap()->GetWndPos();
	m_UIMainScrollV->SetRange		(0,iFloor(ActiveMap()->GetHeight()));
	m_UIMainScrollH->SetRange		(0,iFloor(ActiveMap()->GetWidth()));

	m_UIMainScrollV->SetScrollPos	(iFloor(-w_pos.y));
	m_UIMainScrollH->SetScrollPos	(iFloor(-w_pos.x));
}


void CUIMapWnd::OnScrollV()
{
	if (ActiveMap()){
		int s_pos					= m_UIMainScrollV->GetScrollPos();
		Fvector2 w_pos				= ActiveMap()->GetWndPos();
		ActiveMap()->SetWndPos	(w_pos.x,float(-s_pos));
	}
}
void CUIMapWnd::OnScrollH()
{
	if (ActiveMap()){
		int s_pos					= m_UIMainScrollH->GetScrollPos();
		Fvector2 w_pos				= ActiveMap()->GetWndPos();
		ActiveMap()->SetWndPos	(float(-s_pos),w_pos.y);
	}
}

void CUIMapWnd::SetStatusInfo			(LPCSTR status_text)
{
//	UIMainMapHeader->UITitleText.SetText(status_text);
	m_MapText->SetText					(status_text);
}

void CUIMapWnd::Update()
{
	inherited::Update			();
	ShowHint					();
	m_ActionPlanner->update		();
}

void CUIMapWnd::ShowHint()
{
/*	Fvector2 cursor_pos = GetUICursor()->GetPos();
	LPCSTR hint = NULL;

	if( ActiveMap()==m_GlobalMap && m_GlobalMap->GetAbsoluteRect().in(cursor_pos) ){
		hint = m_GlobalMap->GetHint();
	}else
		if( ActiveMap() && ActiveMap()->GetAbsoluteRect().in(cursor_pos) ){
			hint = ActiveMap()->GetHint();
	}*/
	LPCSTR hint = NULL;
	if(ActiveMap())
		hint = *(ActiveMap()->MapName());
	SetStatusInfo(hint);
}

/*
void CUIMapWnd::OnToolBar				(CUIWindow* w, void* pData)
{
	shared_str prev_btn = m_ToolBar->GetCommandName((int)pData);


	shared_str active_btn = m_ToolBar->GetCommandName(m_ToolBar->GetActiveIndex());
	if(active_btn == "to_global"){
		SetActiveMap(GlobalMap()->MapName());
	}else
	if(active_btn == "to_next"){
		u16 curr_map_idx = 0;
		if(GlobalMap()!=ActiveMap()){
			curr_map_idx = GetIdxByName(ActiveMap()->MapName());
			++curr_map_idx;
			if(curr_map_idx == (u16)GameMaps().size()) curr_map_idx = 0;
		}
		SetActiveMap(GetMapByIdx(curr_map_idx)->MapName());
	}else
	if(active_btn == "add_user_spot"){
		m_flags.set(lmUserSpotAdd,TRUE);
	};
}
*/

void CUIMapWnd::OnToolGlobalMapClicked	(CUIWindow* w, void*)
{
		SetActiveMap(GlobalMap()->MapName());
}

void CUIMapWnd::OnToolNextMapClicked	(CUIWindow* w, void*)
{
		u16 curr_map_idx = 0;
		if(GlobalMap()!=ActiveMap()){
			curr_map_idx = GetIdxByName(ActiveMap()->MapName());
			++curr_map_idx;
			if(curr_map_idx == (u16)GameMaps().size()) curr_map_idx = 0;
		}
		SetActiveMap(GetMapByIdx(curr_map_idx)->MapName());
}

void CUIMapWnd::OnToolPrevMapClicked	(CUIWindow* w, void*)
{
}

void CUIMapWnd::OnToolZoomInClicked		(CUIWindow* w, void*)
{
}

void CUIMapWnd::OnToolZoomOutClicked	(CUIWindow* w, void*)
{
}

void CUIMapWnd::OnToolAddSpotClicked	(CUIWindow* w, void*)
{
	CUI3tButton* btn = smart_cast<CUI3tButton*>(w);
	bool bPushed = btn->GetCheck		();
	m_flags.set							(lmUserSpotAdd,bPushed);
}

void CUIMapWnd::OnToolRemoveSpotClicked	(CUIWindow* w, void*)
{
}
