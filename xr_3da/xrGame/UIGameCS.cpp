#include "stdafx.h"
#include "UIGameCS.h"

#include "UI.h"
#include "entity.h"
#include "hudmanager.h"

static const int money_x_offs = 785; 
static const int money_y_offs = 485;
static const int time_x_offs = 400; 
static const int time_y_offs = 550;

//--------------------------------------------------------------------
CUIGameCS::CUIGameCS(CUI* parent):CUIGameCustom(parent)
{
	BuyMenu.Load			("game_cs.ltx",this,BuyItem);
	FragList.Init			();
	PlayerList.Init			();
	OwnBase.Init			("ui\\ui_cs_base",		"font",	5,175,	alLeft|alTop); OwnBase.SetColor		(0x8000FF00);
	EnemyBase.Init			("ui\\ui_cs_base",		"font",	5,175,	alLeft|alTop); EnemyBase.SetColor	(0x80FF0000);
	BuyZone.Init			("ui\\ui_cs_buyzone",	"font", 5,210,	alLeft|alTop); BuyZone.SetColor		(0x8000FF00);
	Artifact.Init			("ui\\ui_cs_artefact",	"font",	5,245,	alLeft|alTop); Artifact.SetColor	(0x80FF00A2);
	m_Parent->m_Parent->ClientToScreen(vMoneyPlace, money_x_offs, money_y_offs, alRight|alBottom);
	m_Parent->m_Parent->ClientToScreen(vTimePlace, time_x_offs, time_y_offs, alCenter);
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
		CEntityAlive* E	= dynamic_cast<CEntityAlive*>(Level().CurrentEntity());
		if (E)		E->BuyItem(buf);
	}
}
//--------------------------------------------------------------------

LPCSTR make_time(string16 buf, DWORD sec)
{
	sprintf(buf,"%2.0d:%2.0d",(sec%3600)/60,sec%60);
	int len = strlen(buf);
	for (int i=0; i<len; i++) if (buf[i]==' ') buf[i]='0';
	return buf;
}

void CUIGameCS::OnFrame()
{
	switch (Game().phase){
	case GAME_PHASE_PENDING: 
		PlayerList.OnFrame();
	break;
	case GAME_PHASE_INPROGRESS:
		if(!CanBuy()) SetFlag(flShowBuyMenu,FALSE);
		if (uFlags&flShowBuyMenu)		BuyMenu.OnFrame		();
		else if (uFlags&flShowFragList) FragList.OnFrame	();
		string16 buf;
		game_cl_GameState::Player* P = Game().local_player;
		CEntity* m_Actor = dynamic_cast<CEntity*>(Level().CurrentEntity());
		if (m_Actor){
			m_Parent->m_Parent->pFontBigDigit->SetColor		(0xA0969678);
			m_Parent->m_Parent->pFontBigDigit->SetAligment	(CGameFont::alRight);
			m_Parent->m_Parent->pFontBigDigit->Out			((float)vMoneyPlace.x,(float)vMoneyPlace.y,"$%d",P->money_total);
			m_Parent->m_Parent->pFontBigDigit->SetAligment	(CGameFont::alCenter);
			m_Parent->m_Parent->pFontBigDigit->Out			((float)vTimePlace.x,(float)vTimePlace.y,"\x60%s",make_time(buf,(Game().timelimit-(Level().timeServer()-Game().start_time))/1000));
		}
	break;
	}
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
		CEntity* m_Actor = dynamic_cast<CEntity*>(Level().CurrentEntity());
		if (m_Actor){
			game_cl_GameState::Player* P = Game().local_player;
			if (P->flags&GAME_PLAYER_FLAG_CS_ON_BASE)				OwnBase.Render();
			else if (P->flags&GAME_PLAYER_FLAG_CS_ON_ENEMY_BASE)	EnemyBase.Render();
			if ((P->flags&GAME_PLAYER_FLAG_CS_ON_BASE)&&CanBuy())	BuyZone.Render();
			if (P->flags&GAME_PLAYER_FLAG_CS_HAS_ARTEFACT)			Artifact.Render();
		}
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
