#include "stdafx.h"
#include "UIGameCS.h"

#include "UIBuyMenu.h"
#include "UICSPlayerList.h"
#include "UICSFragList.h"

//--------------------------------------------------------------------
CUIGameCS::CUIGameCS()
{
	pBuyMenu	= new CUIBuyMenu		();
	pFragList	= new CUICSFragList		();
	pPlayerList	= new CUICSPlayerList	();
	pBuyMenu->Load						("game_cs.ltx");
	pFragList->Init						();
	pPlayerList->Init					();
}
//--------------------------------------------------------------------

CUIGameCS::~CUIGameCS()
{
	_DELETE(pBuyMenu);
	_DELETE(pFragList);
	_DELETE(pPlayerList);
}
//--------------------------------------------------------------------

void CUIGameCS::OnFrame()
{
	switch (Game().phase){
	case GAME_PHASE_PENDING: 
		pPlayerList->OnFrame();
	break;
	case GAME_PHASE_INPROGRESS:
		if (uFlags&flShowBuyMenu)		pBuyMenu->OnFrame	();
		else if (uFlags&flShowFragList) pFragList->OnFrame	();
	break;
	}
}
//--------------------------------------------------------------------

void CUIGameCS::Render()
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

bool CUIGameCS::OnKeyboardPress(int dik)
{
	if (Game().phase==GAME_PHASE_INPROGRESS){
		if (uFlags&flShowBuyMenu)
			if (pBuyMenu->OnKeyboardPress(dik))				return true;
		// switch pressed keys
		switch (dik){
		case DIK_B:		
			InvertFlag				(flShowBuyMenu);
			if (uFlags&flShowBuyMenu)pBuyMenu->OnActivate	();
			else					pBuyMenu->OnDeactivate	();
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
			if (pBuyMenu->OnKeyboardRelease(dik))			return true;
		// switch pressed keys
		switch (dik){
		case DIK_TAB:	SetFlag		(flShowFragList,FALSE);	return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------
