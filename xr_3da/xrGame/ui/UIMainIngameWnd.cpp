// UIMainIngameWnd.cpp:  окошки-информаци€ в игре
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIMainIngameWnd.h"

#include "..\\..\\XRXMLParser\\xrXMLParser.h"
#include "UIXmlInit.h"

#include "..\\Entity.h"
#include "..\\actor.h"
#include "..\\xr_weapon_list.h"
#include "..\\HUDManager.h"
/*
#include "..\\Group.h"
*/

#include "..\\UIStaticItem.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIMainIngameWnd::CUIMainIngameWnd()
{
}

CUIMainIngameWnd::~CUIMainIngameWnd()
{

}

void CUIMainIngameWnd::Init()
{
//	g_UIStaticItem.Init("ui\\ui_inv_ak47","hud\\default",400,400,alNone);
	



	CUIXml uiXml;
	//uiXml.Init("x:\\maingame.xml");
	uiXml.Init("$game_data$","maingame.xml");
	
	CUIXmlInit xml_init;
	CUIWindow::Init(0,0, Device.dwWidth, Device.dwHeight);

	//в это пользовательского ввода осуществл€тьс€ не будет
	Enable(false);


	AttachChild(&UIStaticHealth);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticHealth);
	
/*	AttachChild(&UIStaticMapBack);
	xml_init.InitStatic(uiXml, "static", 1, &UIStaticMapBack);*/

	AttachChild(&UIStaticRadiation);
	xml_init.InitStatic(uiXml, "static", 2, &UIStaticRadiation);

	AttachChild(&UIStaticWound);
	xml_init.InitStatic(uiXml, "static", 3, &UIStaticWound);

	AttachChild(&UIStaticWeapon);
	xml_init.InitStatic(uiXml, "static", 4, &UIStaticWeapon);

	AttachChild(&UITextWound);
	UITextWound.Init(UIStaticWound.GetWndRect().left+12, 
						UIStaticWound.GetWndRect().top+40,
						30,30);
	UITextWound.SetText("40");

	AttachChild(&UITextAmmo);
	UITextAmmo.Init(UIStaticWeapon.GetWndRect().left + 22, 
						UIStaticWeapon.GetWndRect().top - 10,
						30,30);
	UITextAmmo.SetText("T30/250");

	AttachChild(&UITextWeaponName);
	UITextWeaponName.Init(UIStaticWeapon.GetWndRect().left+119, 
						UIStaticWeapon.GetWndRect().top + 15,
						130,30);
	UITextWeaponName.SetFont(HUD().pFontSmall);
	//UITextWeaponName.SetText("AK-47");


	//ѕолоса прогресса
	AttachChild(&UIHealthBar);
	xml_init.InitProgressBar(uiXml, "progress_bar", 0, &UIHealthBar);


	//индикаторы 
	UIWeapon.Init();
	UIZoneMap.Init();
	UIZoneMap.SetScale(2);
}

void CUIMainIngameWnd::Draw()
{
	static float radiation_alpha = 254.f;
	static float radiation_alpha_inc = 0.5;

	radiation_alpha += radiation_alpha_inc;
	if(radiation_alpha > 255.f || radiation_alpha < 60.0f) 
	{
		radiation_alpha_inc *=-1;
		radiation_alpha += radiation_alpha_inc;
	}
	UIStaticRadiation.SetColor(RGB_ALPHA(radiation_alpha, 0xFF,0xFF,0xFF));
	//HUD().GetUI();
	
	CUIWindow::Draw();
	UIWeapon.Render();
	UIZoneMap.Render();


	//a test
/*	g_UIStaticItem.SetPos(0,0);
	g_UIStaticItem.SetRect(0,0,1024, 768);
	g_UIStaticItem.Render(0,0,256,128);*/
	

}

void CUIMainIngameWnd::Update()
{	
	CEntity* m_Actor = dynamic_cast<CEntity*>(Level().CurrentEntity());
	if (m_Actor)
	{
		// radar
		UIZoneMap.UpdateRadar(m_Actor,Level().Teams[m_Actor->id_Team]);
		// viewport
		float h,p;
		Device.vCameraDirection.getHP	(h,p);
		UIZoneMap.SetHeading			(-h);
		
		// health&armor
	//	UIHealth.Out(m_Actor->g_Health(),m_Actor->g_Armor());
		UIHealthBar.SetProgressPos((s16)m_Actor->g_Health());
		
		// weapon
		//CWeaponList* wpns = m_Actor->GetItemList();
		//if (wpns) UIWeapon.Out(wpns->ActiveWeapon());
		CActor *l_pA = dynamic_cast<CActor*>(m_Actor);
		if(l_pA && (l_pA->m_inventory.m_activeSlot < l_pA->m_inventory.m_slots.size())) 
		{
			CWeapon* pWeapon = dynamic_cast<CWeapon*>(l_pA->m_inventory.m_slots[
											l_pA->m_inventory.m_activeSlot].m_pIItem); 
			UIWeapon.Out(pWeapon);
			if(pWeapon)
				UITextWeaponName.SetText((char*)pWeapon->NameShort());
			else
				UITextWeaponName.SetText(NULL);
		} 
		else UIWeapon.Out(NULL);
}


		
	
	//обновить окошки
	CUIWindow::Update();
}
///	Device.TimerAsync();

bool CUIMainIngameWnd::OnKeyboardPress(int dik)
{
	switch(dik)
	{
	case DIK_NUMPADMINUS:
		UIZoneMap.ZoomOut();
		return true;
		break;
	case DIK_NUMPADPLUS:
		UIZoneMap.ZoomIn();
		return true;
		break;
	}
	return false;
}
