#include "stdafx.h"
#include "UI.h"
#include "..\xr_level_controller.h"
#include "..\xr_IOConsole.h"
#include "Entity.h"
#include "HUDManager.h"
#include "Group.h"
#include "xr_weapon_list.h"

#define MSGS_OFFS 510

//--------------------------------------------------------------------
CUI::CUI(CHUDManager* p)
{
	Level().HUD()->pUI = this;
	UIZoneMap.Init	();
	UIWeapon.Init	();
	UIHealth.Init	();
	UISquad.Init	();

	m_Parent		= p;
	bShift			= FALSE;
	bShowFragList	= FALSE;
	bShowBuyMenu	= FALSE;
	pUIBuyMenu		= 0;
	pUIDMFragList	= 0;
	pUIDMPlayerList	= 0;

	msgs_offs		= m_Parent->ClientToScreenY(MSGS_OFFS,alLeft|alBottom);
}
//--------------------------------------------------------------------

CUI::~CUI()
{
	for (UIMsgIt it=messages.begin(); it!=messages.end(); it++)
		_DELETE(*it);
	// cs
	_DELETE(pUIBuyMenu);
	// dm
	_DELETE(pUIDMFragList);
	_DELETE(pUIDMPlayerList);
}
//--------------------------------------------------------------------

void CUI::Load()
{
	switch (GameID())
	{
	case GAME_CS:
	case GAME_ASSAULT:
		pUIBuyMenu	= new CUIBuyMenu	();
		pUIBuyMenu->Load				();
		break;
	case GAME_DEATHMATCH:
		pUIDMFragList	= new CUIFragList		();
		pUIDMPlayerList	= new CUIDMPlayerList	();
		pUIDMFragList->Init						();
		pUIDMPlayerList->Init					();
		break;
	}
}
//--------------------------------------------------------------------

void CUI::ShowBuyMenu(BOOL bShow)
{
	VERIFY		(pUIBuyMenu);
	bShowBuyMenu=bShow;
	if (bShow)	pUIBuyMenu->OnActivate();
	else		pUIBuyMenu->OnDeactivate();
}

u32 ScaleAlpha(u32 val, float factor)
{
	u32	r, g, b, a;

	float	_a		= float((val >> 24)&0xff)*factor;
	a				= iFloor(_a);
	r				= (val >> 16)&0xff;
	g				= (val >>  8)&0xff;
	b				= (val >>  0)&0xff;
    return ((u32)(a<<24) | (r<<16) | (g<<8) | (b));
}

void CUI::OnFrame()
{
	CEntity* m_Actor = dynamic_cast<CEntity*>(Level().CurrentEntity());
	if (m_Actor){
		// radar
		UIZoneMap.UpdateRadar(m_Actor,Level().Teams[m_Actor->id_Team]);
		// viewport
		float h,p;
		Device.vCameraDirection.getHP	(h,p);
		UIZoneMap.SetHeading			(-h);
		// health&armor
		UIHealth.Out(m_Actor->g_Health(),m_Actor->g_Armor());
		// weapon
		CWeaponList* wpns = m_Actor->GetItemList();
		if (wpns) UIWeapon.Out(wpns->ActiveWeapon());
		// out GAME-style depend information
		switch (GameID())
		{
			case GAME_SINGLE:		
				break;
			case GAME_DEATHMATCH:
				switch (Game().phase){
				case GAME_PHASE_PENDING: 
					VERIFY				(pUIDMPlayerList);
					pUIDMPlayerList->OnFrame();
				break;
				case GAME_PHASE_INPROGRESS:
					if (bShowFragList){ 
						VERIFY				(pUIDMFragList);
						pUIDMFragList->OnFrame	();
					}
				break;
				}
				break;
			case GAME_CTF:			
				// time
				break;
			case GAME_ASSAULT:
				break;
			case GAME_CS:
				if (bShowBuyMenu){
					VERIFY				(bShowBuyMenu);
					pUIBuyMenu->OnFrame	();
				}
				break;
			default: THROW;
		}
	}

	if (!messages.empty()){
		m_Parent->pHUDFont->OutSet(0,msgs_offs);
		for (int i=messages.size()-1; i>=0; i--){
			SUIMessage* M = messages[i];
			M->life_time-=Device.dwTimeDelta;
			if (M->life_time<0){
				_DELETE(messages[i]);
				messages.erase(i);
				i--;
				continue;
			}
			u32 color = messages[i]->color;
			if (M->life_time<=(HIDE_TIME*1000))
				color = ScaleAlpha(color, float(M->life_time)/float(HIDE_TIME*1000));
			m_Parent->pHUDFont->Color(color);
			m_Parent->pHUDFont->OutPrev("%s: %s",messages[i]->sender,messages[i]->msg);
		}
	}
}
//--------------------------------------------------------------------

bool CUI::Render()
{
	UIZoneMap.Render();
	UIWeapon.Render();
	UIHealth.Render();

	// out GAME-style depend information
	switch (GameID())
	{
	case GAME_SINGLE:		
		break;
	case GAME_DEATHMATCH:
		switch (Game().phase){
		case GAME_PHASE_PENDING: 
			VERIFY				(pUIDMPlayerList);
			pUIDMPlayerList->Render	();
		break;
		case GAME_PHASE_INPROGRESS:
			if (bShowFragList){ 
				VERIFY				(pUIDMFragList);
				pUIDMFragList->Render	();
			}
		break;
		}
		break;
	case GAME_CTF:			
		// time
		break;
	case GAME_ASSAULT:
		break;
	case GAME_CS:
		break;
	default: THROW;
	}
	return false;
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool CUI::OnKeyboardPress(int dik)
{
	// global
	switch (GameID())
	{
	case GAME_ASSAULT:
	case GAME_CS:		
		if (bShowBuyMenu){
			VERIFY(pUIBuyMenu);
			if (pUIBuyMenu->OnKeyboardPress(dik)) return true;
		}else{
			switch (dik){
			case DIK_B:			ShowBuyMenu	(!bShowBuyMenu);return true;
			}
		}
	break;
	case GAME_DEATHMATCH:
		if (Game().phase==GAME_PHASE_INPROGRESS){
			switch (dik){
			case DIK_TAB:		ShowFragList(TRUE);	return true;
			}
		}
	}
	return false;
}
//--------------------------------------------------------------------

bool CUI::OnKeyboardRelease(int dik)
{
	if (bShowBuyMenu){
		VERIFY(pUIBuyMenu);
		return pUIBuyMenu->OnKeyboardRelease(dik);
	}else{
		if (Game().phase==GAME_PHASE_INPROGRESS){
			switch (dik){
			case DIK_TAB: ShowFragList	(FALSE);		break;
			}
		}
	}
	return false;
}
//--------------------------------------------------------------------

bool CUI::OnMouseMove	(int dx, int dy)
{
	return false;
//	float scale		= psMouseSens * psMouseSensScale/50.f;
//	UICursor.vPos.x+=float(dx)*scale;
//	UICursor.vPos.y+=float(dy)*scale*(3.f/4.f);
//	clamp(UICursor.vPos.x,-1.f,1.f);
//	clamp(UICursor.vPos.y,-1.f,1.f);
}
//--------------------------------------------------------------------

void CUI::AddMessage(LPCSTR S, LPCSTR M, u32 C, float life_time)
{
	if (messages.size()==MAX_UIMESSAGES){ 
		_DELETE(messages.front());
		messages.erase(u32(0));
	}
	messages.push_back(new SUIMessage(S,M,C,life_time));
}
