#include "stdafx.h"
#include "UIGameDM.h"

#include "UIDMPlayerList.h"
#include "UIDMFragList.h"

#define MSGS_OFFS 510

//--------------------------------------------------------------------
CUIGameDM::CUIGameDM()
{
	pFragList	= new CUIDMFragList		();
	pPlayerList	= new CUIDMPlayerList	();
	pFragList->Init						();
	pPlayerList->Init					();
}
//--------------------------------------------------------------------

CUIGameDM::~CUIGameDM()
{
	_DELETE(pFragList);
	_DELETE(pPlayerList);
}
//--------------------------------------------------------------------

void CUIGameDM::OnFrame()
{
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

bool CUIGameDM::OnKeyboardPress(int dik)
{
	if (Game().phase==GAME_PHASE_INPROGRESS){
		// switch pressed keys
		switch (dik){
		case DIK_TAB:	SetFlag		(flShowFragList,TRUE);	return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------

bool CUIGameDM::OnKeyboardRelease(int dik)
{
	if (Game().phase==GAME_PHASE_INPROGRESS){
		// switch pressed keys
		switch (dik){
		case DIK_TAB:	SetFlag		(flShowFragList,FALSE);	return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------
