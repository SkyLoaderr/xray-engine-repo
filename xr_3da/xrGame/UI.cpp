#include "stdafx.h"
#include "UI.h"
#include "..\xr_IOConsole.h"
#include "Entity.h"
#include "HUDManager.h"
#include "Group.h"
#include "xr_weapon_list.h"
#include "UIGameSP.h"
#include "UIGameCS.h"
#include "UIGameDM.h"
#include "actor.h"

#define MSGS_OFFS 510

//--------------------------------------------------------------------
CUI::CUI(CHUDManager* p)
{
	HUD().pUI		= this;
	UIZoneMap.Init	();
	UIWeapon.Init	();
	UIHealth.Init	();
	UISquad.Init	();

/**
	UIMainWindow.Init("ui\\ui_frame", 100,100, 700,530);

	UIButton1.Init("ui\\ui_button_01", 200,475,150,50);
	UIButton2.Init("ui\\ui_button_02", 360,475,150,50);
	UIButton3.Init("ui\\ui_button_03", 520,475,150,50);

	UIMainWindow.AttachChild(&UIButton1);
	UIMainWindow.AttachChild(&UIButton2);
	UIMainWindow.AttachChild(&UIButton3);
/**/

/*	UIMainWindow.AttachChild(&UIScrollBar);
	UIScrollBar.Init(10,10,350,true);
	UIScrollBar.SetRange(0, 10);
	UIScrollBar.SetPageSize(2);
	UIScrollBar.SetScrollPos(0);*/


//UIButton1.Init("ui\\hud_health_back", 10,10,128,128);
//	UIButton2.Init(10,210,128,128);

//	UIMainWindow.AttachChild(&UIButton1);
//	UIMainWindow.AttachChild(&UIButton2);

/*	UIMainWindow.AttachChild(&UIRadioGroup);
	UIRadioGroup.Init(100,10, 300,300);

	UIRadioGroup.AttachChild(&UIRadioButton1);
	UIRadioGroup.AttachChild(&UIRadioButton2);

	UIRadioButton1.Init(10,10,128,128);
	UIRadioButton2.Init(10,110,128,128);
*/
/*	UIMainWindow.AttachChild(&UIScrollBar);
	UIScrollBar.Init(10,10,250,true);*/

//	CGameFont *pGameFont = m_Parent->pFontMedium;
	//UIButton1.SetFont(m_Parent->pFontMedium);
	//UIButton2.SetFont(m_Parent->pFontSmall);

					
	//show the cursor
	UICursor.SetPos(Device.dwWidth/2, Device.dwHeight/2);
//	UICursor.Show();
	UICursor.Hide();

	

	m_Parent		= p;
	pUIGame			= 0;

	msgs_offs		= (float)m_Parent->ClientToScreenY(MSGS_OFFS,alLeft|alBottom);
}
//--------------------------------------------------------------------

CUI::~CUI()
{
	for (UIMsgIt it=messages.begin(); it!=messages.end(); it++)
		xr_delete(*it);
	xr_delete(pUIGame);
}
//--------------------------------------------------------------------

void CUI::Load()
{
	switch (GameID())
	{
	case GAME_SINGLE:		pUIGame = xr_new<CUIGameSP> (this);	break;
	case GAME_CS:			pUIGame = xr_new<CUIGameCS> (this);	break;
	case GAME_ASSAULT:											break;
	case GAME_DEATHMATCH:	pUIGame = xr_new<CUIGameDM> (this);	break;
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
		//CWeaponList* wpns = m_Actor->GetItemList();
		//if (wpns) UIWeapon.Out(wpns->ActiveWeapon());
		CActor *l_pA = dynamic_cast<CActor*>(m_Actor);
		if(l_pA && (l_pA->m_inventory.m_activeSlot < l_pA->m_inventory.m_slots.size())) {
			UIWeapon.Out(dynamic_cast<CWeapon*>(l_pA->m_inventory.m_slots[l_pA->m_inventory.m_activeSlot].m_pIItem));
		} else UIWeapon.Out(NULL);


		//update windows
//		UIMainWindow.SetFont(m_Parent->pFontMedium);
//		UIMainWindow.Update();
			

	}
	// out GAME-style depend information
	if (pUIGame) pUIGame->OnFrame	();

	if (!messages.empty()){
		m_Parent->pFontSmall->OutSet(0,msgs_offs);
		for (int i=messages.size()-1; i>=0; i--){
			SUIMessage* M = messages[i];
			M->life_time-=Device.dwTimeDelta;
			if (M->life_time<0){
				xr_delete(messages[i]);
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
	CEntity* m_Actor = dynamic_cast<CEntity*>(Level().CurrentEntity());
	if (m_Actor){
		UIZoneMap.Render();
		UIWeapon.Render();
		UIHealth.Render();

		//Draw main window and its children
//		UIMainWindow.Draw();

		//render cursor only when it visible
		if(UICursor.IsVisible())
					UICursor.Render();
	}
	// out GAME-style depend information
	if (pUIGame) pUIGame->Render	();
	return false;
}
//--------------------------------------------------------------------
bool CUI::OnKeyboardPress(int dik)
{
/**
	if(dik==MOUSE_1)
	{
		UIMainWindow.OnMouse(UICursor.GetPos().x,UICursor.GetPos().y,
			CUIWindow::LBUTTON_DOWN);
		return true;
	}
/**/

	if (pUIGame&&pUIGame->OnKeyboardPress(dik)) 
	{
		return true;
	}
	return false;
}
//--------------------------------------------------------------------

bool CUI::OnKeyboardRelease(int dik)
{
/**
	if(dik==MOUSE_1)
	{
		UIMainWindow.OnMouse(UICursor.GetPos().x,UICursor.GetPos().y,
			CUIWindow::LBUTTON_UP);
		return true;
	}
/**/

	if (pUIGame&&pUIGame->OnKeyboardRelease(dik)) 
	{
		return true;
	}
	return false;
}
//--------------------------------------------------------------------

bool CUI::OnMouseMove(int dx,int dy)
{
	if (UICursor.IsVisible())
	{ 
		UICursor.MoveBy(dx, dy);

/**
		UIMainWindow.OnMouse(UICursor.GetPos().x,
							 UICursor.GetPos().y,
							 CUIWindow::MOUSE_MOVE);
/**/
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
