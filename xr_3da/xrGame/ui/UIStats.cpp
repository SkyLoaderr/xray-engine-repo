#include "StdAfx.h"

#include "UIStats.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "UIStatsPlayerList.h"
#include "UIStatic.h"
#include "UIFrameWindow.h"
#include "../level.h"
#include "../game_base_space.h"


CUIStats::CUIStats(){

}

CUIStats::~CUIStats(){
	xr_delete(m_pBack);
}

void CUIStats::Init(CUIXml& xml_doc, int team){
	CUIXmlInit::InitScrollView(xml_doc, "stats_wnd", 0, this);
	this->SetFixedScrollBar(false);
	CUIWindow* pWnd = NULL;
	CUIStatic* pSt	= NULL;

    m_pBack = xr_new<CUIFrameWindow>();
	CUIXmlInit::InitFrameWindow(xml_doc, "stats_wnd:back_ground",0, m_pBack);

	switch (GameID()){
		case GAME_ARTEFACTHUNT:
		case GAME_TEAMDEATHMATCH:
			pWnd = xr_new<CUIWindow>();
			CUIXmlInit::InitWindow(xml_doc, "stats_wnd:team_header", 0, pWnd);
			pWnd->SetAutoDelete(true);
			pSt = xr_new<CUIStatic>();
			CUIXmlInit::InitStatic(xml_doc, "stats_wnd:team_header:logo",0,pSt);
			pSt->SetAutoDelete(true);
			if (1 == team)
                pSt->InitTexture(pSettings->r_string("team_logo_small", "team1"));
			else if (2 == team)
				pSt->InitTexture(pSettings->r_string("team_logo_small", "team2"));
			else
				R_ASSERT2(false, "invalid team");
			pWnd->AttachChild(pSt);
			pSt = xr_new<CUIStatic>();
			CUIXmlInit::InitStatic(xml_doc, "stats_wnd:team_header:header",0,pSt);
			pSt->SetAutoDelete(true);
			pWnd->AttachChild(pSt);
	//	case GAME_DEATHMATCH:
	//	default:
	}

	// players
	CUIStatsPlayerList* pPList = xr_new<CUIStatsPlayerList>();
	CUIXmlInit::InitStatsPlayerList(xml_doc, "stats_wnd:player_list",0,pPList);
	pPList->SetMessageTarget(this);

	u32 col;
    CGameFont* pF;
	CUIXmlInit::InitFont(xml_doc, "stats_wnd:player_list_header:text", 0, col, pF);
	pWnd = pPList->GetHeader(pF,col);

	AddWindow(pWnd, true);
	AddWindow(pPList, true);

	// spectators
	pPList = xr_new<CUIStatsPlayerList>();
	CUIXmlInit::InitStatsPlayerList(xml_doc, "stats_wnd:spectator_list",0,pPList);
	pPList->SetMessageTarget(this);

	col = 0;
    pF = NULL;
	CUIXmlInit::InitFont(xml_doc, "stats_wnd:spectator_list_header:text", 0, col, pF);
	pWnd = pPList->GetHeader(pF,col);

	AddWindow(pWnd, true);
	AddWindow(pPList, true);



}

void CUIStats::Draw(){
	Fvector2 pos = GetWndPos();
	m_pBack->Draw(pos.x,pos.y);	
	CUIScrollView::Draw();
}

