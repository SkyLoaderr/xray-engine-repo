#include "stdafx.h"
#include "UI.h"
#include "..\xr_level_controller.h"
#include "..\xr_gamefont.h"
#include "..\xr_IOConsole.h"
#include "Entity.h"
#include "HUDManager.h"
#include "Group.h"

static LPCSTR group_state_list[gsLast]={
	{"Hold position"},
	{"Need backup"},
	{"Go in this direction"},
	{"Go to that position"},
	{"Free hunting"}
};


#define MSGS_OFFS 510

//--------------------------------------------------------------------
CUI::CUI(CHUDManager* p){
	Level().HUD()->pUI = this;
	UIZoneMap.Init	();
	UIWeapon.Init	();
	UIHealth.Init	();
	UISquad.Init	();

	m_Parent		= p;
	bActive			= false;
	bShift			= false;
	ResetSelected	();

	msgs_offs		= m_Parent->ClientToScreenScaledY(MSGS_OFFS,alLeft|alBottom)/Level().HUD()->pHUDFont->GetScale();
}
//--------------------------------------------------------------------

CUI::~CUI(){
	for (UIMsgIt it=messages.begin(); it!=messages.end(); it++)
		_DELETE(*it);
}
//--------------------------------------------------------------------

void CUI::ResetSelected(){
	ZeroMemory		(bSelGroups,sizeof(bool)*10);
}
//--------------------------------------------------------------------

bool CUI::Render(){
	UIZoneMap.Render();
	UIWeapon.Render();
	UIHealth.Render();

	CEntity* m_Actor = (CEntity*)Level().CurrentEntity();
	if (m_Actor&&m_Actor->net_Local)
		UISquad.Render(Level().Teams[m_Actor->id_Team].Squads[m_Actor->id_Squad],bSelGroups,bActive);

	return false;
//	if (bActive) UICursor.Render();
}
//--------------------------------------------------------------------

DWORD ScaleAlpha(DWORD val, float factor){
    BYTE r, g, b, a;
    a = (BYTE) (float((BYTE)(val >> 24))*factor);
    r = (BYTE) (val >> 16);
    g = (BYTE) (val >>  8);
    b = (BYTE) (val >>  0);
    return ((DWORD)(a<<24) | (r<<16) | (g<<8) | (b));
}

void CUI::OnMove(){
	CEntity* m_Actor = (CEntity*)Level().CurrentEntity();
	if (m_Actor&&m_Actor->net_Local)
		UIZoneMap.UpdateRadar(m_Actor,Level().Teams[m_Actor->id_Team]);

	if (bActive){
		m_Parent->pSmallFont->OutSet		(-1.f,-0.3f);
		m_Parent->pSmallFont->Color			(0xffffffff);
		m_Parent->pSmallFont->OutNext		("R: %s",group_state_list[0]);
		m_Parent->pSmallFont->OutNext		("T: %s",group_state_list[1]);
		m_Parent->pSmallFont->OutNext		("Y: %s",group_state_list[2]);
		m_Parent->pSmallFont->OutNext		("U: %s",group_state_list[3]);
		m_Parent->pSmallFont->OutNext		("I: %s",group_state_list[4]);
		m_Parent->pSmallFont->OutNext		("F: Agressive");
		m_Parent->pSmallFont->OutNext		("G: Quiet");
		m_Parent->pSmallFont->OutNext		("F1:cfg_load s.ltx");
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

bool CUI::FindGroup(int idx){
	CEntity* m_Actor = (CEntity*)Level().CurrentEntity();
	if ((idx<0)||(idx>9)) return false;
	if (m_Actor&&m_Actor->net_Local){
		CSquad& S = Level().Teams[m_Actor->id_Team].Squads[m_Actor->id_Squad];
		if (idx>=int(S.Groups.size())) return false;
		return !S.Groups[idx].Empty();
	}
	return false;
}
//--------------------------------------------------------------------

void CUI::SelectGroup(int idx){
	if (FindGroup(idx)){ 
		if (!bShift) ResetSelected();
		bSelGroups[idx]=true;
	}
}
//--------------------------------------------------------------------
static int ii=0;

bool CUI::OnKeyboardPress(int dik)
{
	// mode event
	switch (dik){
	// global
	case DIK_LSHIFT: bShift = true; break;
	case DIK_F1:Console.Execute("cfg_load s");return true;
	case DIK_P:	break;
	// select group
	case DIK_1: SelectGroup(0);	break;
	case DIK_2: SelectGroup(1); break;
	case DIK_3: SelectGroup(2); break;
	case DIK_4: SelectGroup(3); break;
	case DIK_5: SelectGroup(4); break;
	case DIK_6: SelectGroup(5); break;
	case DIK_7: SelectGroup(6); break;
	case DIK_8: SelectGroup(7); break;
	case DIK_9: SelectGroup(8); break;
	case DIK_0: SelectGroup(9); break;
	// set command
	case DIK_R: SetState(gsHoldPosition);		break;
	case DIK_T: SetState(gsNeedBackup);			break;
	case DIK_Y: SetState(gsGoInThisDirection);	break;
	case DIK_U: SetState(gsGoToThatPosition);	break;
	case DIK_I: SetState(gsFreeHunting);		break;
	// set trigger
	case DIK_F: InvertFlag(gtAgressive);		break;
	case DIK_G: InvertFlag(gtQuiet);			break;
	case DIK_O:{
		char a[255];
		sprintf(a,"%d: %s",ii,"Karma");
		AddMessage(a,"Test");		
		ii++;
			   }break;
	default: return false;
	}
	return true;
}
//--------------------------------------------------------------------

bool CUI::OnKeyboardRelease(int dik)
{
	switch (dik){
	case DIK_LALT: Deactivate(); break;
	case DIK_LSHIFT: bShift = false; break;
	}
	return false;
}
//--------------------------------------------------------------------

bool CUI::OnMouseMove	(int dx, int dy)
{
	return false;
	float scale		= psMouseSens * psMouseSensScale/50.f;
	UICursor.vPos.x+=float(dx)*scale;
	UICursor.vPos.y+=float(dy)*scale*MouseHWScale;
	clamp(UICursor.vPos.x,-1.f,1.f);
	clamp(UICursor.vPos.y,-1.f,1.f);
}
//--------------------------------------------------------------------

void CUI::SetState(EGroupState st){
	CEntity* m_Actor = (CEntity*)Level().CurrentEntity();
	if (!m_Actor||!m_Actor->net_Local) return;
	for (int i=0; i<MAX_GROUPS; i++)
		if (bSelGroups[i]) Level().Teams[m_Actor->id_Team].Squads[m_Actor->id_Squad].Groups[i].SetState(st);
}
//--------------------------------------------------------------------

void CUI::SetFlag(EGroupTriggers tr, BOOL f){
	CEntity* m_Actor = (CEntity*)Level().CurrentEntity();
	if (!m_Actor||!m_Actor->net_Local) return;
	for (int i=0; i<MAX_GROUPS; i++)
		if (bSelGroups[i]) Level().Teams[m_Actor->id_Team].Squads[m_Actor->id_Squad].Groups[i].SetFlag(tr,f);
}
//--------------------------------------------------------------------

void CUI::InvertFlag(EGroupTriggers tr){
	CEntity* m_Actor = (CEntity*)Level().CurrentEntity();
	if (!m_Actor||!m_Actor->net_Local) return;
	for (int i=0; i<MAX_GROUPS; i++)
		if (bSelGroups[i]) Level().Teams[m_Actor->id_Team].Squads[m_Actor->id_Squad].Groups[i].InvertFlag(tr);
}
//--------------------------------------------------------------------

void CUI::Activate(){
	bShift				= false;
	bActive				= true;
	for (int i=0; i<MAX_GROUPS; i++)
		if (bSelGroups[i]&&FindGroup(i)) bSelGroups[i]=true;
		else							 bSelGroups[i]=false;
}
//--------------------------------------------------------------------

void CUI::Deactivate(){
	bActive = false;
}
//--------------------------------------------------------------------

void CUI::OutHealth(int health, int armor){
	UIHealth.Out(health, armor);
}
//--------------------------------------------------------------------

void CUI::OutWeapon(CWeapon* wpn){
	UIWeapon.Out(wpn);
}
//--------------------------------------------------------------------

void CUI::SetHeading(float heading){
	UIZoneMap.SetHeading(heading);
}

void CUI::SetHeading(const Fvector& dir){
	float heading;
	Fvector DHeading; DHeading.set(dir.x,0.f,dir.z); DHeading.normalize_safe();
	if (DHeading.x>=0)	heading = acosf(DHeading.z);
	else				heading = 2*PI-acosf(DHeading.z);
	UIZoneMap.SetHeading(heading);
}

void CUI::AddMessage(LPCSTR S, LPCSTR M, DWORD C, float life_time){
	if (messages.size()==MAX_UIMESSAGES){ 
		_DELETE(messages.front());
		messages.erase(DWORD(0));
	}
	messages.push_back(new SUIMessage(S,M,C,life_time));
}
