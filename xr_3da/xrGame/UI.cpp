#include "stdafx.h"
#include "UI.h"
#include "..\xr_level_controller.h"
#include "..\xr_gamefont.h"
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
	UIFragList.Init	();

	m_Parent		= p;
	bShift			= false;
	bShowFragList	= FALSE;
	bShowBuyMenu	= FALSE;

	msgs_offs		= m_Parent->ClientToScreenScaledY(MSGS_OFFS,alLeft|alBottom)/Level().HUD()->pHUDFont->GetScale();
}
//--------------------------------------------------------------------

CUI::~CUI()
{
	for (UIMsgIt it=messages.begin(); it!=messages.end(); it++)
		_DELETE(*it);
}
//--------------------------------------------------------------------

void CUI::Load()
{
	UIBuyMenu.Load	();
}
//--------------------------------------------------------------------

DWORD ScaleAlpha(DWORD val, float factor)
{
	DWORD	r, g, b, a;

	float	_a		= float((val >> 24)&0xff)*factor;
	a				= iFloor(_a);
	r				= (val >> 16)&0xff;
	g				= (val >>  8)&0xff;
	b				= (val >>  0)&0xff;
    return ((DWORD)(a<<24) | (r<<16) | (g<<8) | (b));
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
		if (wpns&&wpns->ActiveWeapon()) UIWeapon.Out(wpns->ActiveWeapon());
		// out GAME-style depend information
		switch (GAME){
			case GAME_SINGLE:		
				break;
			case GAME_DEATHMATCH:
				if (bShowFragList) UIFragList.OnFrame();
				break;
			case GAME_CTF:			
				// time
				break;
			case GAME_ASSAULT:
				break;
			default: THROW;
		}
	}

	if (bShowBuyMenu){
		UIBuyMenu.OnFrame();
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
			DWORD color = messages[i]->color;
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
	switch (GAME){
	case GAME_SINGLE:		
		break;
	case GAME_DEATHMATCH:
		if (bShowFragList) UIFragList.Render();
		break;
	case GAME_CTF:			
		// time
		break;
	case GAME_ASSAULT:
		break;
	default: THROW;
	}

	return false;
	//	if (bActive) UICursor.Render();
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool CUI::OnKeyboardPress(int dik)
{
	if (bShowBuyMenu){
		if (UIBuyMenu.OnKeyboardPress(dik)) return true;
	}else{
		switch (dik){
		case DIK_TAB:	ShowFragList(TRUE);	return true;
		}
	}
	// global
	switch (dik){
	case DIK_B:			ShowBuyMenu	(!bShowBuyMenu);return true;
	case DIK_ESCAPE:	ShowBuyMenu	(FALSE);		return true;
	}
	return false;
}
//--------------------------------------------------------------------

bool CUI::OnKeyboardRelease(int dik)
{
	if (bShowBuyMenu){
		return UIBuyMenu.OnKeyboardRelease(dik);
	}else{
		switch (dik){
		case DIK_TAB: ShowFragList	(FALSE);		break;
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

void CUI::AddMessage(LPCSTR S, LPCSTR M, DWORD C, float life_time)
{
	if (messages.size()==MAX_UIMESSAGES){ 
		_DELETE(messages.front());
		messages.erase(DWORD(0));
	}
	messages.push_back(new SUIMessage(S,M,C,life_time));
}
