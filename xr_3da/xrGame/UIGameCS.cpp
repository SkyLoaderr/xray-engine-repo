#include "stdafx.h"
#include "UIGameCS.h"

#include "UIBuyMenu.h"
#include "UICSPlayerList.h"
#include "UICSFragList.h"

//--------------------------------------------------------------------
CUIGameCS::CUIGameCS()
{
	BuyMenu.Load			("game_cs.ltx");
	FragList.Init			();
	PlayerList.Init			();
	BuyZone.Init			("ui\\ui_cs_base",		"font", 5,200,	alLeft|alTop); BuyZone.SetColor(0x8000FF00);
	Artifact.Init			("ui\\ui_cs_artefact",	"font",	5,240,	alLeft|alTop); Artifact.SetColor(0x80FF00A2);
}
//--------------------------------------------------------------------

CUIGameCS::~CUIGameCS()
{
}
//--------------------------------------------------------------------

BOOL CUIGameCS::CanBuy()
{
	return (Level().timeServer()-Game().start_time)<Game().buy_time;
}
//--------------------------------------------------------------------

void CUIGameCS::OnFrame()
{
	switch (Game().phase){
	case GAME_PHASE_PENDING: 
		PlayerList.OnFrame();
	break;
	case GAME_PHASE_INPROGRESS:
		if (uFlags&flShowBuyMenu)		BuyMenu.OnFrame	();
		else if (uFlags&flShowFragList) FragList.OnFrame	();

		map<u32,game_cl_GameState::Player>::iterator I=Game().players.begin();
		map<u32,game_cl_GameState::Player>::iterator E=Game().players.end();
		for (;I!=E;I++){
			game_cl_GameState::Player* P = (game_cl_GameState::Player*)&I->second;
			if (P->flags&GAME_PLAYER_FLAG_LOCAL){
//				if ((P->flags&GAME_PLAYER_FLAG_CS_ON_BASE)&&CanBuy())	BuyZone.OnFrame();
//				if (P->flags&GAME_PLAYER_FLAG_CS_HAS_ARTEFACT)			Artifact.OnFrame();
			}
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
		map<u32,game_cl_GameState::Player>::iterator I=Game().players.begin();
		map<u32,game_cl_GameState::Player>::iterator E=Game().players.end();
		for (;I!=E;I++){
			game_cl_GameState::Player* P = (game_cl_GameState::Player*)&I->second;
			if (P->flags&GAME_PLAYER_FLAG_LOCAL){
				if ((P->flags&GAME_PLAYER_FLAG_CS_ON_BASE)&&CanBuy())	BuyZone.Render();
				if (P->flags&GAME_PLAYER_FLAG_CS_HAS_ARTEFACT)			Artifact.Render();
			}
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
