#include "stdafx.h"
#include "game_cl_single.h"
#include "UIGameSP.h"

CUIGameCustom* game_cl_Single::createGameUI()
{
	CLASS_ID clsid			= CLSID_GAME_UI_SINGLE;
	CUIGameSP*			pUIGame	= dynamic_cast<CUIGameSP*> ( NEW_INSTANCE ( clsid ) );
	R_ASSERT(pUIGame);
	pUIGame->SetClGame(this);
	pUIGame->Init();
	return pUIGame;
}

