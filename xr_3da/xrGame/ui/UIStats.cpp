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

}

void CUIStats::Init(CUIXml& xml_doc, int team){
	CUIXmlInit::InitScrollView(xml_doc, "stats_wnd", 0, this);
	this->SetFixedScrollBar(false);
	CUIWindow* pWnd = NULL;
	CUIStatic* pSt	= NULL;

    CUIFrameWindow* pFrm = xr_new<CUIFrameWindow>();
	CUIXmlInit::InitFrameWindow(xml_doc, "stats_wnd:back_ground",0, pFrm);
	pFrm->SetAutoDelete(true);
	AttachChild(pFrm);

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

    CUIStatsPlayerList* pPList = xr_new<CUIStatsPlayerList>();	
	CUIXmlInit::InitStatsPlayerList(xml_doc, "stats_wnd:player_list",0,pPList);
	pPList->SetMessageTarget(this);
	pWnd = pPList->GetHeader();
	AddWindow(pWnd, true);
	AddWindow(pPList, true);



}

