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
#define MENU_OFFS 200

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
	bActive			= false;
	bShift			= false;
	bShowFragList	= FALSE;
	ResetSelected	();

	msgs_offs		= m_Parent->ClientToScreenScaledY(MSGS_OFFS,alLeft|alBottom)/Level().HUD()->pHUDFont->GetScale();
	menu_offs		= m_Parent->ClientToScreenScaledY(MENU_OFFS,alLeft|alTop)/Level().HUD()->pHUDFont->GetScale();
}
//--------------------------------------------------------------------

CUI::~CUI()
{
	for (UIMsgIt it=messages.begin(); it!=messages.end(); it++)
		_DELETE(*it);
}
//--------------------------------------------------------------------

void CUI::ResetSelected()
{
	ZeroMemory		(bSelGroups,sizeof(bool)*10);
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

	if (bActive){
		m_Parent->pHUDFont->OutSet			(0,menu_offs);
		m_Parent->pHUDFont->Color			(0xffffffff);
		m_Parent->pHUDFont->OutNext			("0: xyz");
		m_Parent->pHUDFont->OutNext			("1: xyz");
		m_Parent->pHUDFont->OutNext			("2: xyz");
		m_Parent->pHUDFont->OutNext			("3: xyz");
		m_Parent->pHUDFont->OutNext			("4: xyz");
		m_Parent->pHUDFont->OutNext			("5: xyz");
		m_Parent->pHUDFont->OutNext			("6: xyz");
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

	CEntity* m_Actor = dynamic_cast<CEntity*>(Level().CurrentEntity());
	if (m_Actor)
		UISquad.Render(Level().Teams[m_Actor->id_Team].Squads[m_Actor->id_Squad],bSelGroups,bActive);

	return false;
	//	if (bActive) UICursor.Render();
}
//--------------------------------------------------------------------

bool CUI::FindGroup(int idx)
{
	CEntity* m_Actor = dynamic_cast<CEntity*>(Level().CurrentEntity());
	if ((idx<0)||(idx>9)) return false;
	if (m_Actor)
	{
		CSquad& S = Level().Teams[m_Actor->id_Team].Squads[m_Actor->id_Squad];
		if (idx>=int(S.Groups.size())) return false;
		return !S.Groups[idx].Empty();
	}
	return false;
}
//--------------------------------------------------------------------

void CUI::SelectGroup(int idx)
{
	if (FindGroup(idx)){ 
		if (!bShift) ResetSelected();
		bSelGroups[idx]=true;
	}
}
//--------------------------------------------------------------------
bool CUI::OnKeyboardPress(int dik)
{
	if (!bActive){
		// global
		switch (dik){
		case DIK_TAB: ShowFragList	(TRUE);			break;
		default: return false;
		}
	}else{
		// only if active
		// mode event
		switch (dik){
		case DIK_LSHIFT: bShift = true;				break;
		// select group
		case DIK_1: SelectGroup(0);					break;
		case DIK_2: SelectGroup(1);					break;
		case DIK_3: SelectGroup(2);					break;
		case DIK_4: SelectGroup(3);					break;
		case DIK_5: SelectGroup(4);					break;
		case DIK_6: SelectGroup(5);					break;
		case DIK_7: SelectGroup(6);					break;
		case DIK_8: SelectGroup(7);					break;
		case DIK_9: SelectGroup(8);					break;
		case DIK_0: SelectGroup(9);					break;
		// set command
		case DIK_R: SetState(gsHoldPosition);		break;
		case DIK_T: SetState(gsFollowMe);			break;
		case DIK_Y: SetState(gsGoInThisDirection);	break;
		case DIK_U: SetState(gsGoToThisPosition);	break;
		case DIK_I: SetState(gsFreeHunting);		break;
		// set trigger
		case DIK_F: InvertFlag(gtAgressive);		break;
		case DIK_G: InvertFlag(gtQuiet);			break;
		default: return false;
		}
	}
	return true;
}
//--------------------------------------------------------------------

bool CUI::OnKeyboardRelease(int dik)
{
	if (!bActive){
		switch (dik){
		case DIK_TAB: ShowFragList	(FALSE);		break;
		}
	}else{
		switch (dik){
		case DIK_LALT: Deactivate();				break;
		case DIK_LSHIFT: bShift = false;			break;
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

void CUI::SetState(EGroupState st)
{
	CEntity* m_Actor = dynamic_cast<CEntity*>(Level().CurrentEntity());
	if (!m_Actor) return;
	for (int i=0; i<MAX_GROUPS; i++)
		if (bSelGroups[i]) Level().Teams[m_Actor->id_Team].Squads[m_Actor->id_Squad].Groups[i].SetState(st);
}
//--------------------------------------------------------------------

void CUI::SetFlag(EGroupTriggers tr, BOOL f)
{
	CEntity* m_Actor = dynamic_cast<CEntity*>(Level().CurrentEntity());
	if (!m_Actor) return;
	for (int i=0; i<MAX_GROUPS; i++)
		if (bSelGroups[i]) Level().Teams[m_Actor->id_Team].Squads[m_Actor->id_Squad].Groups[i].SetFlag(tr,f);
}
//--------------------------------------------------------------------

void CUI::InvertFlag(EGroupTriggers tr)
{
	CEntity* m_Actor = dynamic_cast<CEntity*>(Level().CurrentEntity());
	if (!m_Actor) return;
	for (int i=0; i<MAX_GROUPS; i++)
		if (bSelGroups[i]) Level().Teams[m_Actor->id_Team].Squads[m_Actor->id_Squad].Groups[i].InvertFlag(tr);
}
//--------------------------------------------------------------------

void CUI::Activate()
{
	bShift				= false;
	bActive				= true;
	for (int i=0; i<MAX_GROUPS; i++){
		if (bSelGroups[i]&&FindGroup(i)) bSelGroups[i]=true;
		else							 bSelGroups[i]=false;
	}
}
//--------------------------------------------------------------------

void CUI::Deactivate()
{
	bActive = false;
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
