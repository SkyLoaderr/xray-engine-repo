#include "stdafx.h"
#include "UIGameAHunt.h"

#include "UIAHuntPlayerList.h"
#include "UIAHuntFragList.h"
#include "UIDMStatisticWnd.h"

#include "hudmanager.h"
#include "team_base_zone.h"
#include "level.h"
#include "game_cl_ArtefactHunt.h"
#include "ui/UIFrags2.h"

#define MSGS_OFFS 510

#define BUY_MSG_COLOR		0xffffff00
#define SCORE_MSG_COLOR		0xffffffff
#define REINFORCEMENT_MSG_COLOR		0xff8080ff
#define TODO_MSG_COLOR		0xff00ff00

#define DI2PX(x) float(iFloor((x+1)*float(UI_BASE_WIDTH)*0.5f))
#define DI2PY(y) float(iFloor((y+1)*float(UI_BASE_HEIGHT)*0.5f))
#define SZ(x) x*UI_BASE_WIDTH

//--------------------------------------------------------------------
CUIGameAHunt::CUIGameAHunt()
{
}
//--------------------------------------------------------------------
void CUIGameAHunt::SetClGame (game_cl_GameState* g)
{
	inherited::SetClGame(g);
	m_game = smart_cast<game_cl_ArtefactHunt*>(g);
	R_ASSERT(m_game);
}

void CUIGameAHunt::Init	()
{
	CUIXml xml_doc;
	bool xml_result = xml_doc.Init(CONFIG_PATH, UI_PATH, "stats.xml");
	R_ASSERT2(xml_result, "xml file not found");

	CUIFrags2* pFragList		= xr_new<CUIFrags2>();			pFragList->SetAutoDelete(true);
	//-----------------------------------------------------------
	CUIDMStatisticWnd* pStatisticWnd = xr_new<CUIDMStatisticWnd>(); pStatisticWnd->SetAutoDelete(true);

	pFragList->Init(xml_doc, "stats_wnd", "frag_wnd_tdm");

	float ScreenW = UI_BASE_WIDTH;
	float ScreenH = UI_BASE_HEIGHT;
	//-----------------------------------------------------------
	Frect FrameRect = pFragList->GetWndRect ();
	float FrameW	= FrameRect.right - FrameRect.left;
	float FrameH	= FrameRect.bottom - FrameRect.top;

	pFragList->SetWndPos((ScreenW-FrameW)/2.0f, (ScreenH - FrameH)/2.0f);

	//-----------------------------------------------------------
	m_pFragLists->AttachChild(pFragList);
	//-----------------------------------------------------------

	CUIFrags2* pPlayerListT1	= xr_new<CUIFrags2>	();pPlayerListT1->SetAutoDelete(true);

	pPlayerListT1->Init(xml_doc, "players_wnd", "frag_wnd_tdm");
	//-----------------------------------------------------------
	FrameRect = pPlayerListT1->GetWndRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;

	pPlayerListT1->SetWndPos((ScreenW-FrameW)/2.0f, (ScreenH - FrameH)/2.0f);
	//-----------------------------------------------------------
	m_pPlayerLists->AttachChild(pPlayerListT1);
	//-----------------------------------------------------------
	m_reinforcement_caption			=	"ah_reinforcement";		
	GameCaptions()->addCustomMessage(m_reinforcement_caption, DI2PX(0.0f), DI2PY(-0.9f), SZ(0.02f), HUD().Font().pFontDI, CGameFont::alCenter, REINFORCEMENT_MSG_COLOR, "");

	m_score_caption					=	"ah_score";		
	GameCaptions()->addCustomMessage(m_score_caption, DI2PX(0.0f), DI2PY(-0.85f), SZ(0.02f), HUD().Font().pFontDI, CGameFont::alCenter, SCORE_MSG_COLOR, "");

	m_todo_caption					=	"ah_todo";
	GameCaptions()->addCustomMessage(m_todo_caption, 0.0f, -0.8f, 0.02f, HUD().Font().pFontDI, CGameFont::alCenter, TODO_MSG_COLOR, "");
	GameCaptions()->customizeMessage(m_todo_caption, CUITextBanner::tbsFlicker)->fPeriod = 0.5f;

	m_buy_msg_caption				=	"ah_buy";
	GameCaptions()->addCustomMessage(m_buy_msg_caption, DI2PX(0.0f), DI2PY(0.9f), SZ(0.02f), HUD().Font().pFontDI, CGameFont::alCenter, BUY_MSG_COLOR, "");
	//-----------------------------------------------------------
	FrameRect = pStatisticWnd->GetFrameRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;
	pStatisticWnd->SetWndRect((ScreenW-FrameW)/2.0f, (ScreenH - FrameH)/2.0f, FrameW, FrameH);

	m_pStatisticWnds->AttachChild(pStatisticWnd);

};
//--------------------------------------------------------------------

CUIGameAHunt::~CUIGameAHunt()
{
}

void CUIGameAHunt::SetReinforcementCaption(LPCSTR str)
{
	GameCaptions()->setCaption(m_reinforcement_caption, str, REINFORCEMENT_MSG_COLOR, true);
}

void CUIGameAHunt::SetScoreCaption(LPCSTR str)
{
	GameCaptions()->setCaption(m_score_caption, str, SCORE_MSG_COLOR, true);
}

void CUIGameAHunt::SetTodoCaption(LPCSTR str)
{
	GameCaptions()->setCaption(m_todo_caption, str, TODO_MSG_COLOR, true);
}

void CUIGameAHunt::SetBuyMsgCaption(LPCSTR str)
{
	GameCaptions()->setCaption(m_buy_msg_caption, str, BUY_MSG_COLOR, true);
}
