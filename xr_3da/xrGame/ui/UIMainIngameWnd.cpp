// UIMainIngameWnd.cpp:  ������-���������� � ����
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIMainIngameWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "../Entity.h"
#include "../HUDManager.h"
#include "../PDA.h"
#include "../UIStaticItem.h"

#include "UIInventoryUtilities.h"
using namespace InventoryUtilities;




#define RADIATION_ABSENT 0.25f
#define RADIATION_SMALL 0.5f
#define RADIATION_MEDIUM 0.75f
#define RADIATION_HIGH 1.0f




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIMainIngameWnd::CUIMainIngameWnd()
{
	m_pActor = NULL;
	m_pWeapon = NULL;
}

CUIMainIngameWnd::~CUIMainIngameWnd()
{

}

void CUIMainIngameWnd::Init()
{
	CUIXml uiXml;
	uiXml.Init("$game_data$","maingame.xml");
	
	CUIXmlInit xml_init;
	CUIWindow::Init(0,0, Device.dwWidth, Device.dwHeight);

	//� ��� ���� ����������������� ����� �������������� �� �����
	Enable(false);


	AttachChild(&UIStaticHealth);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticHealth);
	
/*	AttachChild(&UIStaticMapBack);
	xml_init.InitStatic(uiXml, "static", 1, &UIStaticMapBack);*/

	AttachChild(&UIStaticRadiationLow);
	xml_init.InitStatic(uiXml, "static", 2, &UIStaticRadiationLow);
	AttachChild(&UIStaticRadiationMedium);
	xml_init.InitStatic(uiXml, "static", 3, &UIStaticRadiationMedium);
	AttachChild(&UIStaticRadiationHigh);
	xml_init.InitStatic(uiXml, "static", 4, &UIStaticRadiationHigh);

	AttachChild(&UIStaticWound);
	xml_init.InitStatic(uiXml, "static", 5, &UIStaticWound);

	AttachChild(&UIWeaponBack);
	xml_init.InitStatic(uiXml, "static", 6, &UIWeaponBack);
	UIWeaponBack.AttachChild(&UIWeaponSignAmmo);
	xml_init.InitStatic(uiXml, "static", 7, &UIWeaponSignAmmo);
	UIWeaponBack.AttachChild(&UIWeaponSignName);
	xml_init.InitStatic(uiXml, "static", 8, &UIWeaponSignName);
	UIWeaponBack.AttachChild(&UIWeaponIcon);
	xml_init.InitStatic(uiXml, "static", 9, &UIWeaponIcon);
	UIWeaponIcon.SetShader(GetEquipmentIconsShader());
	UIWeaponIcon.ClipperOn();
	//��������� ������������ ������ ��� ������ ������, 
	//��� ��� ��� ����� ���������������� � ��������������
	m_iWeaponIconWidth = UIWeaponIcon.GetWidth();
	m_iWeaponIconHeight = UIWeaponIcon.GetHeight();
	m_iWeaponIconX = UIWeaponIcon.GetWndRect().left;
	m_iWeaponIconY = UIWeaponIcon.GetWndRect().top;

	//
	AttachChild(&UIPdaOnline);
	xml_init.InitStatic(uiXml, "static", 10, &UIPdaOnline);
	


/*	UIWeaponBack;
	UIWeaponSignAmmo;
	UIWeaponSignName;
	UIWeaponIcon;*/

	AttachChild(&UITextWound);
	UITextWound.Init(UIStaticWound.GetWndRect().left+12, 
						UIStaticWound.GetWndRect().top+40,
						30,30);


	//������ ��������� ��������
	AttachChild(&UIHealthBar);
	xml_init.InitProgressBar(uiXml, "progress_bar", 0, &UIHealthBar);


	//���������� 
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
	
	UIStaticRadiationLow.SetColor(RGB_ALPHA(radiation_alpha, 0xFF,0xFF,0xFF));
	UIStaticRadiationMedium.SetColor(RGB_ALPHA(radiation_alpha, 0xFF,0xFF,0xFF));
	UIStaticRadiationHigh.SetColor(RGB_ALPHA(radiation_alpha, 0xFF,0xFF,0xFF));
	
	//����������� �������� ��������� ������������ ������� ��� �������
	if(m_pWeapon && m_pWeapon->IsZoomed() && m_pWeapon->ZoomTexture())
	{
		m_pWeapon->ZoomTexture()->SetPos(0,0);
		m_pWeapon->ZoomTexture()->Render(0,0, Device.dwWidth, Device.dwHeight);
	}
	else
	{
		//���������� ��������� ������
		CUIWindow::Draw();
		UIZoneMap.Render();
	}
}

void CUIMainIngameWnd::Update()
{
	static string256 wound_string;
	static string256 pda_string;

	m_pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if (!m_pActor) 
	{
		m_pWeapon = NULL;
		CUIWindow::Update();
		return;
	}

#pragma todo("JIM: I temporary commented it")
//	if(m_pActor->GetPDA() && m_pActor->GetPDA()->ActiveContactsNum()>0)
//	{
//		string256 text;
//		sprintf(&text[0], "%d", m_pActor->GetPDA()->ActiveContactsNum());
//		UIPdaOnline.SetText(&text[0]);
//	}
//	else
//	{
//		UIPdaOnline.SetText("");
//	}


	if(m_pActor->m_inventory.GetActiveSlot() < m_pActor->m_inventory.m_slots.size()) 
	{
		CWeapon* pWeapon = dynamic_cast<CWeapon*>(m_pActor->m_inventory.m_slots[
										m_pActor->m_inventory.GetActiveSlot()].m_pIItem); 
		
		if(pWeapon)	
		{
			if(m_pWeapon != pWeapon)
			{
				m_pWeapon = pWeapon;
				UIWeaponIcon.Show(true);
				UIWeaponIcon.GetUIStaticItem().SetOriginalRect(
								m_pWeapon->GetXPos()*INV_GRID_WIDTH,
								m_pWeapon->GetYPos()*INV_GRID_HEIGHT,
								m_pWeapon->GetGridWidth()*INV_GRID_WIDTH,
								m_pWeapon->GetGridHeight()*INV_GRID_HEIGHT);
			
				float scale_x = float(m_iWeaponIconWidth)/
								float(m_pWeapon->GetGridWidth()*INV_GRID_WIDTH);
				float scale_y = float(m_iWeaponIconHeight)/
								float(m_pWeapon->GetGridHeight()*INV_GRID_HEIGHT);

				float scale = scale_x<scale_y?scale_x:scale_y;
				UIWeaponIcon.SetTextureScale(scale);
				UIWeaponIcon.SetWidth(iFloor(0.5f+ m_pWeapon->GetGridWidth()*INV_GRID_WIDTH*scale));
				UIWeaponIcon.SetHeight(iFloor(0.5f+ m_pWeapon->GetGridHeight()*INV_GRID_HEIGHT*scale));
				//�������������� ������
				UIWeaponIcon.MoveWindow(m_iWeaponIconX + 
									   (m_iWeaponIconWidth - UIWeaponIcon.GetWidth())/2,
									    m_iWeaponIconY + 
									   (m_iWeaponIconHeight - UIWeaponIcon.GetHeight())/2);
			}



			UIWeaponSignName.SetText(m_pWeapon->NameShort());
			
			int	AE = m_pWeapon->GetAmmoElapsed();
			int	AC = m_pWeapon->GetAmmoCurrent();
			if((AE>=0)&&(AC>=0))
			{
				//������� ��� ������ �������� � ������
				string256 m_sAmmoText;
				sprintf(&m_sAmmoText[0], "%d/%d %s",AE,AC, 
										 *m_pWeapon->m_ammoName?*m_pWeapon->m_ammoName:"");
				UIWeaponSignAmmo.SetText(&m_sAmmoText[0]);
			}
		}
	} 
	else
		m_pWeapon = NULL;

	//�������� ����������
	if(!m_pWeapon)
	{
		UIWeaponSignAmmo.SetText("");
		UIWeaponSignName.SetText("");
		UIWeaponIcon.Show(false);
	}
    

	if(m_pWeapon && m_pWeapon->IsZoomed() && m_pWeapon->ZoomTexture()) return;


    // radar
	UIZoneMap.UpdateRadar(m_pActor,Level().Teams[m_pActor->id_Team]);
	// viewport
	float h,p;
	Device.vCameraDirection.getHP	(h,p);
	UIZoneMap.SetHeading			(-h);
		
	// health&armor
	//	UIHealth.Out(m_Actor->g_Health(),m_Actor->g_Armor());
	UIHealthBar.SetProgressPos((s16)m_pActor->g_Health());
		
	//radiation
	float radiation = m_pActor->GetRadiation();

	if(radiation<RADIATION_ABSENT)	
	{
		UIStaticRadiationLow.Show(false);
		UIStaticRadiationMedium.Show(false);
		UIStaticRadiationHigh.Show(false);
	}
	else if(radiation<RADIATION_SMALL)	
	{
		UIStaticRadiationLow.Show(true);
		UIStaticRadiationMedium.Show(false);
		UIStaticRadiationHigh.Show(false);
	}
	else if(radiation<RADIATION_MEDIUM)	
	{
		UIStaticRadiationLow.Show(false);
		UIStaticRadiationMedium.Show(true);
		UIStaticRadiationHigh.Show(false);
	}
	else 
	{
		UIStaticRadiationLow.Show(false);
		UIStaticRadiationMedium.Show(false);
		UIStaticRadiationHigh.Show(true);
	}

	// weapon
	//CWeaponList* wpns = m_Actor->GetItemList();
	//if (wpns) UIWeapon.Out(wpns->ActiveWeapon());
		
	//Wounds bleeding speed
	if(m_pActor->BleedingSpeed()>0)
	{
		sprintf(wound_string, "%3.3f",m_pActor->BleedingSpeed());
		UITextWound.SetText(wound_string);
		UITextWound.Show(true);
		UIStaticWound.Show(true);
	}
	else
	{
		UITextWound.Show(false);
		UIStaticWound.Show(false);
	}


	CUIWindow::Update();
}

	


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
