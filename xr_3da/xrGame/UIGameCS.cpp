#include "stdafx.h"
#include "UIGameCS.h"

#include "UI.h"
#include "entity.h"
#include "hudmanager.h"

static const int money_x_offs = 690; 
static const int money_y_offs = 485;

//--------------------------------------------------------------------
CUIGameCS::CUIGameCS(CUI* parent):CUIGameCustom(parent)
{
	BuyMenu.Load			("game_cs.ltx",this,BuyItem);
	FragList.Init			();
	PlayerList.Init			();
	OwnBase.Init			("ui\\ui_cs_base",		"font",	5,175,	alLeft|alTop); OwnBase.SetColor	(0x8000FF00);
	EnemyBase.Init			("ui\\ui_cs_base",		"font",	5,175,	alLeft|alTop); EnemyBase.SetColor(0x80FF0000);
	BuyZone.Init			("ui\\ui_cs_buyzone",	"font", 5,210,	alLeft|alTop); BuyZone.SetColor	(0x8000FF00);
	Artifact.Init			("ui\\ui_cs_artefact",	"font",	5,245,	alLeft|alTop); Artifact.SetColor(0x80FF00A2);
	m_Parent->m_Parent->ClientToScreen(vMoneyPlace, money_x_offs, money_y_offs, alRight|alBottom);
}
//--------------------------------------------------------------------

CUIGameCS::~CUIGameCS()
{
}
//--------------------------------------------------------------------

BOOL CUIGameCS::CanBuy()
{
	game_cl_GameState::Player* P = Game().local_player;
	return (P->flags&GAME_PLAYER_FLAG_CS_ON_BASE)&&((Level().timeServer()-Game().start_time)<Game().buy_time);
}
//--------------------------------------------------------------------

void CUIGameCS::BuyItem(CCustomMenuItem* sender)
{
	CUIGameCS* G	= (CUIGameCS*)sender->tag; R_ASSERT(G);
	if (G)			G->SetFlag(CUIGameCustom::flShowBuyMenu,FALSE);

	if (G->CanBuy()&&sender->value1){
		string64 buf;
		sprintf(buf,"%s/cost=%s",sender->value1,sender->value0);
		// buy item
		CEntity* E		= dynamic_cast<CEntity*>(Level().CurrentEntity());
		if (E){
			NET_Packet P;
			E->u_EventGen	(P,GE_BUY,E->ID());
			P.w_string		(buf);
			E->u_EventSend	(P);
		}
	}
}
//--------------------------------------------------------------------

void CUIGameCS::OnFrame()
{
	switch (Game().phase){
	case GAME_PHASE_PENDING: 
		PlayerList.OnFrame();
	break;
	case GAME_PHASE_INPROGRESS:
		if (uFlags&flShowBuyMenu)		BuyMenu.OnFrame		();
		else if (uFlags&flShowFragList) FragList.OnFrame	();
/*
		map<u32,game_cl_GameState::Player>::iterator I=Game().players.begin();
		map<u32,game_cl_GameState::Player>::iterator E=Game().players.end();
		for (;I!=E;I++){
			game_cl_GameState::Player* P = (game_cl_GameState::Player*)&I->second;
			if (P->flags&GAME_PLAYER_FLAG_LOCAL){
//				if ((P->flags&GAME_PLAYER_FLAG_CS_ON_BASE)&&CanBuy())	BuyZone.OnFrame();
//				if (P->flags&GAME_PLAYER_FLAG_CS_HAS_ARTEFACT)			Artifact.OnFrame();
			}
		}
*/
	break;
	}
	game_cl_GameState::Player* P = Game().local_player;
	m_Parent->m_Parent->pFontBigDigit->Color	(0xA0969678);
	m_Parent->m_Parent->pFontBigDigit->OutSet	((float)vMoneyPlace.x,(float)vMoneyPlace.y);
	m_Parent->m_Parent->pFontBigDigit->OutNext	("$%-4d",P->money_total);
}
//--------------------------------------------------------------------

void CUIGameCS::Render()
{
	switch (Game().phase){
	case GAME_PHASE_PENDING: 
		PlayerList.Render();
		break;
	case GAME_PHASE_INPROGRESS:
		if (uFlags&flShowFragList) FragList.Render		();
		game_cl_GameState::Player* P = Game().local_player;
		if (P->flags&GAME_PLAYER_FLAG_CS_ON_BASE)				OwnBase.Render();
		else if (P->flags&GAME_PLAYER_FLAG_CS_ON_ENEMY_BASE)	EnemyBase.Render();
		if ((P->flags&GAME_PLAYER_FLAG_CS_ON_BASE)&&CanBuy())	BuyZone.Render();
		if (P->flags&GAME_PLAYER_FLAG_CS_HAS_ARTEFACT)			Artifact.Render();
		break;
	}
}
//--------------------------------------------------------------------

bool CUIGameCS::OnKeyboardPress(int dik)
{
	if (Game().phase==GAME_PHASE_INPROGRESS){
		if (uFlags&flShowBuyMenu)
			if (BuyMenu.OnKeyboardPress(dik))				return true;
		// switch pressed keys
		switch (dik){
		case DIK_B:		
			InvertFlag				(flShowBuyMenu);
			if (uFlags&flShowBuyMenu)BuyMenu.OnActivate		();
			else					BuyMenu.OnDeactivate	();
			return true;
		case DIK_TAB:	SetFlag		(flShowFragList,TRUE);	return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------

bool CUIGameCS::OnKeyboardRelease(int dik)
{
	if (Game().phase==GAME_PHASE_INPROGRESS){
		if (uFlags&flShowBuyMenu)
			if (BuyMenu.OnKeyboardRelease(dik))				return true;
		// switch pressed keys
		switch (dik){
		case DIK_TAB:	SetFlag		(flShowFragList,FALSE);	return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------
