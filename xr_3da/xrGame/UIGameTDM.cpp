#include "stdafx.h"
#include "UIGameTDM.h"

#include "UITDMPlayerList.h"
#include "UITDMFragList.h"

#include "hudmanager.h"

#define MSGS_OFFS 510

//--------------------------------------------------------------------
CUIGameTDM::CUIGameTDM(CUI* parent):CUIGameCustom(parent)
{
	pFragListT1	= xr_new<CUITDMFragList>	();
	pFragListT2	= xr_new<CUITDMFragList>	();
	pPlayerListT1	= xr_new<CUITDMPlayerList>	();
	pPlayerListT2	= xr_new<CUITDMPlayerList>	();
	
	u32 X = Device.dwWidth / 24;
	u32	Y = Device.dwHeight / 4;
	u32 Width = Device.dwWidth/2 - X*2;
	u32 Height = Device.dwHeight/2;

	pFragListT1->Init						(1, Device.dwWidth/2 - X - Width, Y, Width, Height, alNone);
	pFragListT2->Init						(2, Device.dwWidth/2 + X, Y, Width, Height, alNone);

	pPlayerListT1->Init						(1, Device.dwWidth/2 - X - Width, Y, Width, Height, alNone);
	pPlayerListT2->Init						(2, Device.dwWidth/2 + X, Y, Width, Height, alNone);
}
//--------------------------------------------------------------------

CUIGameTDM::~CUIGameTDM()
{
	xr_delete(pFragListT1);
	xr_delete(pFragListT2);
	xr_delete(pPlayerListT1);
	xr_delete(pPlayerListT2);
}
//--------------------------------------------------------------------

void CUIGameTDM::OnFrame()
{
	inherited::OnFrame();	

	switch (Game().phase){
	case GAME_PHASE_PENDING: 
		pPlayerListT1->OnFrame();
		pPlayerListT2->OnFrame();
		break;
	case GAME_PHASE_INPROGRESS:
		if (uFlags&flShowFragList) 
		{
			pFragListT1->OnFrame	();
			pFragListT2->OnFrame	();
		}break;
		/*
	case GAME_PHASE_TEAM1_SCORES: 
		{
			m_Parent->m_Parent->pFontDI->SetAligment(CGameFont::alCenter);
			m_Parent->m_Parent->pFontDI->SetColor(0xA0969678);
			m_Parent->m_Parent->pFontDI->SetSize(0.05f);
			m_Parent->m_Parent->pFontDI->Out(0,-0.5f,"TEAM 1 SCORES");
		} break;
	case GAME_PHASE_TEAM2_SCORES: 
		{
			m_Parent->m_Parent->pFontDI->SetAligment(CGameFont::alCenter);
			m_Parent->m_Parent->pFontDI->SetColor(0xA0969678);
			m_Parent->m_Parent->pFontDI->SetSize(0.05f);
			m_Parent->m_Parent->pFontDI->Out(0,-0.5f,"TEAM 2 SCORES");
		} break;
		*/
	}
}
//--------------------------------------------------------------------

void CUIGameTDM::Render()
{
	inherited::Render();

	switch (Game().phase){
	case GAME_PHASE_PENDING: 
		pPlayerListT1->Render();
		pPlayerListT2->Render();
		break;
	case GAME_PHASE_INPROGRESS:
		if (uFlags&flShowFragList) 
		{
			pFragListT1->Render		();
			pFragListT2->Render		();
		}break;
	}
}
//--------------------------------------------------------------------

bool CUIGameTDM::IR_OnKeyboardPress(int dik)
{
	if(inherited::IR_OnKeyboardPress(dik)) return true;


	switch (dik)
	{
	case DIK_I: 
		StartStopMenu(&InventoryMenu);
		return true;
		break;
	case DIK_M:
		StartStopMenu(&MapMenu);
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

bool CUIGameTDM::IR_OnKeyboardRelease(int dik)
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
