#include "stdafx.h"
#include "UI.h"
#include "..\xr_level_controller.h"
#include "..\xr_IOConsole.h"
#include "Entity.h"
#include "HUDManager.h"
#include "Group.h"
#include "xr_weapon_list.h"
#include "UIGameCS.h"
#include "UIGameDM.h"

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
	pUIGame			= 0;

	msgs_offs		= (float)m_Parent->ClientToScreenY(MSGS_OFFS,alLeft|alBottom);
}
//--------------------------------------------------------------------

CUI::~CUI()
{
	for (UIMsgIt it=messages.begin(); it!=messages.end(); it++)
		_DELETE(*it);
	_DELETE(pUIGame);
}
//--------------------------------------------------------------------

void CUI::Load()
{
	switch (GameID())
	{
	case GAME_CS:			pUIGame = new CUIGameCS(this);	break;
	case GAME_ASSAULT:										break;
	case GAME_DEATHMATCH:	pUIGame = new CUIGameDM(this);	break;
	}
}
//--------------------------------------------------------------------

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
		if (pUIGame) pUIGame->OnFrame	();
	}

	if (!messages.empty()){
		m_Parent->pFontSmall->OutSet(0,msgs_offs);
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
			m_Parent->pFontSmall->SetColor(color);
			m_Parent->pFontSmall->OutPrev("%s: %s",messages[i]->sender,messages[i]->msg);
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
	if (pUIGame) pUIGame->Render	();
	return false;
}
//--------------------------------------------------------------------
bool CUI::OnKeyboardPress(int dik)
{
	if (pUIGame&&pUIGame->OnKeyboardPress(dik)) return true;
	return false;
}
//--------------------------------------------------------------------

bool CUI::OnKeyboardRelease(int dik)
{
	if (pUIGame&&pUIGame->OnKeyboardRelease(dik)) return true;
	return false;
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
