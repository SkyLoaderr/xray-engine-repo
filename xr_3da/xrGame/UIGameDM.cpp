#include "stdafx.h"
#include "UIGameDM.h"

#include "UIDMPlayerList.h"
#include "UIDMFragList.h"

#define MSGS_OFFS 510

//--------------------------------------------------------------------
CUIGameDM::CUIGameDM(CUI* parent):CUIGameCustom(parent)
{
	pFragList	= xr_new<CUIDMFragList>		();
	pPlayerList	= xr_new<CUIDMPlayerList>	();
	pFragList->Init							();
	pPlayerList->Init						();
}
//--------------------------------------------------------------------

CUIGameDM::~CUIGameDM()
{
	xr_delete(pFragList);
	xr_delete(pPlayerList);
}
//--------------------------------------------------------------------

void CUIGameDM::OnFrame()
{
	inherited::OnFrame();	

	switch (Game().phase){
	case GAME_PHASE_PENDING: 
		pPlayerList->OnFrame();
	break;
	case GAME_PHASE_INPROGRESS:
		if (uFlags&flShowFragList) pFragList->OnFrame	();
	break;
	}
}
//--------------------------------------------------------------------

void CUIGameDM::Render()
{
	inherited::Render();

	switch (Game().phase){
	case GAME_PHASE_PENDING: 
		pPlayerList->Render();
		break;
	case GAME_PHASE_INPROGRESS:
		if (uFlags&flShowFragList) pFragList->Render		();
		break;
	}
}
//--------------------------------------------------------------------

bool CUIGameDM::IR_OnKeyboardPress(int dik)
{
	if(inherited::IR_OnKeyboardPress(dik)) return true;


	switch (dik)
	{
	case DIK_I: 
		StartStopMenu(&InventoryMenu);
		return true;
		break;
	}

	if (Game().phase==GAME_PHASE_INPROGRESS){
		// switch pressed keys
		switch (dik){
		case DIK_TAB:	SetFlag		(flShowFragList,TRUE);	return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------

bool CUIGameDM::IR_OnKeyboardRelease(int dik)
{
	if(inherited::IR_OnKeyboardRelease(dik)) return true;

	if (Game().phase==GAME_PHASE_INPROGRESS){
		// switch pressed keys
		switch (dik){
		case DIK_TAB:	SetFlag		(flShowFragList,FALSE);	return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------
