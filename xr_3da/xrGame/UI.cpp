#include "stdafx.h"
#include "UI.h"
#include "../xr_IOConsole.h"
#include "Entity.h"
#include "HUDManager.h"
#include "UIGameSP.h"
#include "UIGameDM.h"
#include "UIGameTDM.h"
#include "UIGameAHunt.h"
#include "actor.h"
#include "level.h"
#include "game_cl_base.h"


#define MSGS_OFFS 510

//--------------------------------------------------------------------
CUI::CUI(CHUDManager* p)
{
	UIZoneMap.Init	();
	UIHealth.Init	();
	UISquad.Init	();

#ifdef UI_INTERFACE_ON

	UIMainIngameWnd.Init();

#endif

	m_Parent		= p;
	pUIGame			= 0;

	msgs_offs		= (float)m_Parent->ClientToScreenY(MSGS_OFFS,alLeft|alBottom);

	m_bShowIndicators = true;


}
//--------------------------------------------------------------------

CUI::~CUI()
{
	for (UIMsgIt it=messages.begin(); messages.end() != it; ++it)
		xr_delete(*it);
	xr_delete(pUIGame);
}
//--------------------------------------------------------------------

void CUI::Load()
{
	pUIGame = Game().createGameUI();
	//pUIGame->IR_OnKeyboardPress(8);
	R_ASSERT(pUIGame);
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
		
		
		//all ingame information now in UIMainIngameWnd.Update();
		/*
		
		
		// radar
		UIZoneMap.UpdateRadar(m_Actor,Level().Teams[m_Actor->id_Team]);
		// viewport
		float h,p;
		Device.vCameraDirection.getHP	(h,p);
		UIZoneMap.SetHeading			(-h);
		// health&armor
		UIHealth.Out(m_Actor->g_Health(),m_Actor->g_Armor());
		// weapon
		//CWeaponList* wpns = m_Actor->GetItemList();
		//if (wpns) UIWeapon.Out(wpns->ActiveWeapon());
		CActor *l_pA = dynamic_cast<CActor*>(m_Actor);
		if(l_pA && (l_pA->inventory().m_iActiveSlot < l_pA->inventory().m_slots.size())) {
			UIWeapon.Out(dynamic_cast<CWeapon*>(l_pA->inventory().m_slots[l_pA->inventory().m_iActiveSlot].m_pIItem));
		} else UIWeapon.Out(NULL);
		
		*/


#ifdef UI_INTERFACE_ON
		//update windows
		if(m_bShowIndicators)
		{
			UIMainIngameWnd.SetFont(m_Parent->pFontMedium);
			UIMainIngameWnd.Update();
		}
#endif
			

	}
	// out GAME-style depend information
	if (pUIGame) pUIGame->OnFrame	();

#ifdef DEBUG
	if (!messages.empty()){
		m_Parent->pFontSmall->OutSet(0,msgs_offs);
		for (int i=messages.size()-1; i>=0; --i){
			SUIMessage* M = messages[i];
			M->life_time-=Device.dwTimeDelta;
			if (M->life_time<0){
				xr_delete(messages[i]);
				messages.erase(i);
				--i;
				continue;
			}
			u32 color = messages[i]->color;
			if (M->life_time<=(HIDE_TIME*1000))
				color = ScaleAlpha(color, float(M->life_time)/float(HIDE_TIME*1000));
			m_Parent->pFontSmall->SetColor(color);
			m_Parent->pFontSmall->OutPrev("%s: %s",messages[i]->sender,messages[i]->msg);
		}
	}
#endif
}
//--------------------------------------------------------------------

bool CUI::Render()
{
	CEntity* m_Actor = dynamic_cast<CEntity*>(Level().CurrentEntity());
	if (m_Actor)
	{
#ifdef UI_INTERFACE_ON
		//Draw main window and its children
		if(m_bShowIndicators)
			UIMainIngameWnd.Draw();
#endif
		//render cursor only when it visible
		if(UICursor.IsVisible())
            UICursor.Render();
	}
	// out GAME-style depend information
	if (pUIGame) pUIGame->Render	();

	return false;
}
//--------------------------------------------------------------------
bool CUI::IR_OnKeyboardPress(int dik)
{
/*#ifdef UI_INTERFACE_ON
	if(dik==MOUSE_1)
	{
		UIMainWindow.OnMouse(UICursor.GetPos().x,UICursor.GetPos().y,
			CUIWindow::LBUTTON_DOWN);
		return true;
	}
#endif*/
	if(UIMainIngameWnd.OnKeyboardPress(dik))
	{
		return true;
	}

	if (pUIGame && pUIGame->IR_OnKeyboardPress(dik)) 
	{
		return true;
	}
	return false;
}
//--------------------------------------------------------------------

bool CUI::IR_OnKeyboardRelease(int dik)
{
/*#ifdef UI_INTERFACE_ON
	if(dik==MOUSE_1)
	{
		UIMainWindow.OnMouse(UICursor.GetPos().x,UICursor.GetPos().y,
			CUIWindow::LBUTTON_UP);
		return true;
	}
#endif*/

	if (pUIGame&&pUIGame->IR_OnKeyboardRelease(dik)) 
	{
		return true;
	}
	return false;
}
//--------------------------------------------------------------------

bool CUI::IR_OnMouseMove(int dx,int dy)
{
/*	if (UICursor.IsVisible())
	{ 
		UICursor.MoveBy(dx, dy);


		if (pUIGame&&pUIGame->IR_OnMouseMove(UICursor.GetPos().x, UICursor.GetPos().y)) 
		{
			return true;
		}

#ifdef UI_INTERFACE_ON
		UIMainWindow.OnMouse(UICursor.GetPos().x,
							 UICursor.GetPos().y,
							 CUIWindow::MOUSE_MOVE);
#endif
		return true;
	}*/


	if (pUIGame&&pUIGame->IR_OnMouseMove(dx,dy)) 
	{
		return true;
	}
	
	return false;
}

void CUI::AddMessage(LPCSTR S, LPCSTR M, u32 C, float life_time)
{
	if (messages.size()==MAX_UIMESSAGES){
		xr_delete(messages.front());
		messages.erase(u32(0));
	}
	messages.push_back(xr_new<SUIMessage> (S,M,C,life_time));
}

