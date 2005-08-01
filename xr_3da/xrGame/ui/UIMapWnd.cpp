// CUIMapWnd.cpp:  диалог интерактивной карты
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
const				VSCROLLBAR_STEP				= 20; // В пикселях
const				HSCROLLBAR_STEP				= 20; // В пикселях



CUIMapWnd::CUIMapWnd()
{
	m_tgtMap				= NULL;
	m_GlobalMap				= NULL;
	m_flags.zero			();
	m_currentZoom			= 1.0f;
}

CUIMapWnd::~CUIMapWnd()
{
	delete_data			(m_ActionPlanner);
	delete_data			(m_GameMaps);
	delete_data			(m_MapText);
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

	btnIndex		= eActor;
	strconcat(pth, sToolbar.c_str(), ":actor_btn");
	if(uiXml.NavigateToNode(pth,0)){
		m_ToolBar[btnIndex]				= xr_new<CUI3tButton>(); m_ToolBar[btnIndex]->SetAutoDelete(true);
		xml_init.Init3tButton			(uiXml, pth, 0, m_ToolBar[btnIndex]);
		UIMainMapHeader->AttachChild	(m_ToolBar[btnIndex]);
		Register						(m_ToolBar[btnIndex]);
		AddCallback						(*m_ToolBar[btnIndex]->WindowName(),BUTTON_CLICKED,boost::bind(&CUIMapWnd::OnToolActorClicked,this,_1,_2));
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
	m_GlobalMap->OptimalFit					(m_UILevelFrame->GetWndRect());
	m_GlobalMap->SetMinZoom					(m_GlobalMap->GetCurrentZoom());
	m_currentZoom							= m_GlobalMap->GetCurrentZoom();

	m_MapText								= xr_new<CUIStatic>();
	strconcat(pth,start_from,":title");
	xml_init.InitStatic						(uiXml, pth, 0, m_MapText);

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
	SetTargetMap					(m_GlobalMap);
	m_flags.set						(lmFirst,TRUE);
}

void CUIMapWnd::Show(bool status)
{
	if (status)
	{
		m_GlobalMap->Show			(true);
		m_GlobalMap->SetClipRect	(ActiveMapRect());
		GameMaps::iterator	it		= m_GameMaps.begin();
		for(;it!=m_GameMaps.end();++it){
			m_GlobalMap->AttachChild(it->second);
			it->second->Show		(true);
			it->second->SetClipRect	(ActiveMapRect());
		}

		if(	m_flags.test(lmFirst)){
//			OnToolActorClicked	(NULL,NULL);
			m_flags.set(lmFirst,FALSE);
			}
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

void CUIMapWnd::SetTargetMap			(const shared_str& name, const Fvector2& pos)
{
	u16	idx								= GetIdxByName			(name);
	if (idx!=u16(-1)){
		CUICustomMap* lm				= GetMapByIdx			(idx);
		SetTargetMap					(lm, pos);
	}
}

void CUIMapWnd::SetTargetMap			(const shared_str& name)
{
	u16	idx								= GetIdxByName			(name);
	if (idx!=u16(-1)){
		CUICustomMap* lm				= GetMapByIdx			(idx);
		SetTargetMap					(lm);
	}
}

void CUIMapWnd::SetTargetMap			(CUICustomMap* m)
{
	m_tgtMap							= m;
	Fvector2							pos;
	Frect r								= m->BoundRect();
	r.getcenter							(pos);
	SetTargetMap						(m,pos);
}

void CUIMapWnd::SetTargetMap			(CUICustomMap* m, const Fvector2& pos)
{
	m_tgtMap							= m;

	if(m==GlobalMap()){
		CUIGlobalMap* gm				= GlobalMap();
		SetZoom							(gm->GetMinZoom());
		Frect vis_rect					= ActiveMapRect		();
		vis_rect.getcenter				(m_tgtCenter);
		m_tgtCenter.sub					(gm->GetAbsolutePos());
		m_tgtCenter.div					(gm->GetCurrentZoom());
 	}else{
		//translate real level position to identity GlobalMapPosition
		SetZoom							(GlobalMap()->GetMaxZoom());
		m_tgtCenter						= m->ConvertRealToLocalNoTransform(pos);
		m_tgtCenter.add					(m->GetWndPos()).div(GlobalMap()->GetCurrentZoom());
	}
	ResetActionPlanner				();
}

void CUIMapWnd::Draw()
{
	inherited::Draw();
	m_MapText->Draw();
}

bool CUIMapWnd::OnKeyboard				(int dik, EUIMessages keyboard_action)
{
	return inherited::OnKeyboard	(dik, keyboard_action);
}

void CUIMapWnd::OnMouse(float x, float y, EUIMessages mouse_action)
{
	inherited::OnMouse(x,y,mouse_action);
	Fvector2 cursor_pos = GetUICursor()->GetPos();

	if(GlobalMap() && !GlobalMap()->Locked() && ActiveMapRect().in( cursor_pos ) ){
		switch (mouse_action){
		case WINDOW_MOUSE_MOVE:
			if( pInput->iGetAsyncBtnState(0) ){
				GlobalMap()->MoveWndDelta	(GetUICursor()->GetPosDelta());
				UpdateScroll					();
			}
		break;
		case WINDOW_LBUTTON_DOWN:
			if (m_flags.is_any(lmZoomIn+lmZoomOut)){
				CUIGlobalMap* gm				= GlobalMap();
				if(m_flags.test(lmZoomIn))		SetZoom(GetZoom()*1.5f);
				else							SetZoom(GetZoom()/1.5f);
				m_tgtCenter						= cursor_pos;
				m_tgtCenter.sub					(gm->GetAbsolutePos());
				m_tgtCenter.div					(gm->GetCurrentZoom());
				ResetActionPlanner				();
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
	VERIFY							(idx!=u16(-1));
	GameMapsPairIt it				= m_GameMaps.begin();
	std::advance					(it, idx);
	return							it->second;
}

u16 CUIMapWnd::GetIdxByName			(const shared_str& map_name)
{
	GameMapsPairIt it				= m_GameMaps.find(map_name);
	if(it==m_GameMaps.end()){	
		Msg							("! Level Map '%s' not registered",map_name.c_str());
		return						u16(-1);
	}
	return (u16)std::distance		(m_GameMaps.begin(),it);
	
}

void CUIMapWnd::UpdateScroll()
{
	Fvector2 w_pos					= GlobalMap()->GetWndPos();
	m_UIMainScrollV->SetRange		(0,iFloor(GlobalMap()->GetHeight()));
	m_UIMainScrollH->SetRange		(0,iFloor(GlobalMap()->GetWidth()));

	m_UIMainScrollV->SetScrollPos	(iFloor(-w_pos.y));
	m_UIMainScrollH->SetScrollPos	(iFloor(-w_pos.x));
}


void CUIMapWnd::OnScrollV()
{
	if (GlobalMap()){
		int s_pos					= m_UIMainScrollV->GetScrollPos();
		Fvector2 w_pos				= GlobalMap()->GetWndPos();
		GlobalMap()->SetWndPos	(w_pos.x,float(-s_pos));
	}
}
void CUIMapWnd::OnScrollH()
{
	if (GlobalMap()){
		int s_pos					= m_UIMainScrollH->GetScrollPos();
		Fvector2 w_pos				= GlobalMap()->GetWndPos();
		GlobalMap()->SetWndPos	(float(-s_pos),w_pos.y);
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

void CUIMapWnd::SetZoom	( float value)
{
	m_currentZoom	= value;
	clamp		(m_currentZoom, GlobalMap()->GetMinZoom(), GlobalMap()->GetMaxZoom());
}

void CUIMapWnd::ShowHint()
{
	LPCSTR hint = NULL;
	if(m_tgtMap)
		hint = *(m_tgtMap->MapName());
	SetStatusInfo(hint);
}


void CUIMapWnd::OnToolGlobalMapClicked	(CUIWindow* w, void*)
{
	if (GlobalMap()->Locked())			return;
	SetTargetMap(GlobalMap());
}

void CUIMapWnd::OnToolNextMapClicked	(CUIWindow* w, void*)
{
/*	u16 curr_map_idx = 0;
	if(GlobalMap()!=ActiveMap()){
		curr_map_idx = GetIdxByName(ActiveMap()->MapName());
		++curr_map_idx;
		if(curr_map_idx == (u16)GameMaps().size()) curr_map_idx = 0;
	}
	SetActiveMap(GetMapByIdx(curr_map_idx)->MapName());
*/
}

void CUIMapWnd::OnToolPrevMapClicked	(CUIWindow* w, void*)
{
/*	u16 curr_map_idx = 0;
	if(GlobalMap()!=ActiveMap()){
		curr_map_idx = GetIdxByName(ActiveMap()->MapName());
		--curr_map_idx;
		if(curr_map_idx == -1) curr_map_idx = (u16)GameMaps().size()-1;
	}
	SetActiveMap(GetMapByIdx(curr_map_idx)->MapName());
*/
}

void CUIMapWnd::ResetActionPlanner()
{
	m_ActionPlanner->m_storage.set_property(1,false);
	m_ActionPlanner->m_storage.set_property(2,false);
}

void CUIMapWnd::OnToolZoomInClicked	(CUIWindow* w, void*)
{
	if (GlobalMap()->Locked())		return;

	m_flags.zero					();

	CUI3tButton* btn				= smart_cast<CUI3tButton*>(w);
	bool bPushed					= btn->GetCheck		();
	m_flags.set						(lmZoomIn,bPushed);
	ValidateToolBar					();
}

void CUIMapWnd::OnToolZoomOutClicked(CUIWindow* w, void*)
{
	if (GlobalMap()->Locked())		return;

	m_flags.zero					();

	CUI3tButton* btn				= smart_cast<CUI3tButton*>(w);
	bool bPushed					= btn->GetCheck		();
	m_flags.set						(lmZoomOut,bPushed);
	ValidateToolBar					();
}

void CUIMapWnd::OnToolAddSpotClicked	(CUIWindow* w, void*)
{
	m_flags.zero		();
	CUI3tButton* btn = smart_cast<CUI3tButton*>(w);
	bool bPushed = btn->GetCheck		();
	m_flags.set							(lmUserSpotAdd,bPushed);
	ValidateToolBar						();
}

void CUIMapWnd::ValidateToolBar			()
{
	CUI3tButton* btn	= NULL;
	btn					= m_ToolBar[eZoomIn];
	if(btn)
		btn->SetCheck	(!!m_flags.test(lmZoomIn));

	btn					= m_ToolBar[eZoomOut];
	if(btn)
		btn->SetCheck	(!!m_flags.test(lmZoomOut));

	btn					= m_ToolBar[eAddSpot];
	if(btn)
		btn->SetCheck	(!!m_flags.test(lmUserSpotAdd));

	btn					= m_ToolBar[eRemoveSpot];
	if(btn)
		btn->SetCheck	(!!m_flags.test(lmUserSpotRemove));


}

void CUIMapWnd::OnToolRemoveSpotClicked	(CUIWindow* w, void*)
{
}

void CUIMapWnd::OnToolActorClicked		(CUIWindow*, void*)
{
	if (GlobalMap()->Locked())			return;

	Fvector v					= Level().CurrentEntity()->Position();
	Fvector2 v2;
	v2.set						(v.x,v.z);

	SetTargetMap				(Level().name(), v2);
}

void CUIMapWnd::AddUserSpot			(CUILevelMap* lm)
{
	VERIFY(m_flags.test(lmUserSpotAdd) );

	Fvector2 cursor_pos = GetUICursor()->GetPos();
	VERIFY(lm->GetAbsoluteRect().in(cursor_pos));

	cursor_pos.sub					(lm->GetAbsolutePos());
	Fvector2 p =					lm->ConvertLocalToReal(cursor_pos);
	Fvector pos;
	pos.set							(p.x, 0.0f, p.y);
	CActor* pActor					= smart_cast<CActor*>(Level().CurrentEntity());
	shared_str spot					= "user"; 
	CMapLocation* ml				= Level().MapManager().AddUserLocation(spot, lm->MapName(), pos);
	CGameTask* t					= pActor->GameTaskManager().GiveGameTaskToActor("user_task",false);
	t->m_Objectives[0].SetTaskState	(eTaskUserDefined);
	t->m_Objectives[0].object_id	= ml->ObjectID();
	t->m_Objectives[0].map_location	= spot;
	m_flags.set						(lmUserSpotAdd, FALSE);
	m_ToolBar[eAddSpot]->SetButtonMode(CUIButton::BUTTON_NORMAL);
}
