// UIMainIngameWnd.cpp:  ������-���������� � ����
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIMainIngameWnd.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "UIPdaMsgListItem.h"

#include "../Entity.h"
#include "../HUDManager.h"
#include "../PDA.h"
#include "../UIStaticItem.h"
#include "../WeaponHUD.h"

#include "UIInventoryUtilities.h"

#include "../inventory.h"

using namespace InventoryUtilities;


//hud adjust mode
int				g_bHudAdjustMode			= 0;
float			g_fHudAdjustValue			= 0.0f;

#define RADIATION_ABSENT 0.25f
#define RADIATION_SMALL 0.5f
#define RADIATION_MEDIUM 0.75f
#define RADIATION_HIGH 1.0f

#define DEFAULT_MAP_SCALE 1.f

//-----------------------------------------------------------------------------/
//  Fade and color parameters
//-----------------------------------------------------------------------------/
#define C_SIZE		0.025f
#define NEAR_LIM	0.5f

#define SHOW_INFO_SPEED	0.5f
#define HIDE_INFO_SPEED	10.f
#define C_ON_ENEMY	D3DCOLOR_XRGB(0xff,0,0)
#define C_DEFAULT	D3DCOLOR_XRGB(0xff,0xff,0xff)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIMainIngameWnd::CUIMainIngameWnd()
{
	m_pActor			= NULL;
	m_pWeapon			= NULL;

	m_dwMaxShowTime		= 20000;
	m_dwMsgShowingTime	= 0;

	m_bShowHudInfo		= true;
	m_bShowHudCrosshair = false;
	// Quick infos
	fuzzyShowInfo		= 0.f;
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
//	UIWeaponIcon.EnableDragDrop(false);
	UIWeaponIcon.Enable(false);

	//
	AttachChild(&UIPdaOnline);
	xml_init.InitStatic(uiXml, "static", 10, &UIPdaOnline);
	
	AttachChild(&UIPdaMsgListWnd);
	xml_init.InitListWnd(uiXml, "list", 0, &UIPdaMsgListWnd);
	UIPdaMsgListWnd.SetVertFlip(true);
		
	AttachChild(&UITextWound);
	UITextWound.Init(UIStaticWound.GetWndRect().left+12, 
						UIStaticWound.GetWndRect().top+40,
						30,30);


	//������ ��������� ��������
	AttachChild(&UIHealthBar);
	xml_init.InitProgressBar(uiXml, "progress_bar", 0, &UIHealthBar);


	//���������� 
	UIZoneMap.Init();
	UIZoneMap.SetScale(DEFAULT_MAP_SCALE);

	//��� ����������� �������� ��������� PDA
	UIPdaMsgListWnd.Show(false);
	UIPdaMsgListWnd.Enable(false);

	m_dwMsgShowingTime = 0;
	m_dwMaxShowTime = pSettings->r_s32("pda_maingame","max_show_time");

	// ���������, ������� ��������� ��� ��������� ������� �� ������
	AttachChild(&UIStaticQuickHelp);
	xml_init.InitStatic(uiXml, "quick_info", 0, &UIStaticQuickHelp);
	UIStaticQuickHelp.SetTextColor(0xffffffff);
	UIStaticQuickHelp.SetTextAlign(CGameFont::alCenter);

	// ������ ����� �� xml �����
	int tabsCount = uiXml.GetNodesNum("quick_info", 0, "tip");

	XML_NODE* tab_node = uiXml.NavigateToNode("quick_info", 0);
	uiXml.SetLocalRoot(tab_node);

	std::pair<int, ref_str> tmpTip;

	for (int i = 0; i < tabsCount; ++i)
	{
		tmpTip.first	= uiXml.ReadAttribInt("tip", i, "id");
		tmpTip.second	= uiXml.Read("tip", i, NULL);
		m_strTips.insert(tmpTip);
	}
}

void CUIMainIngameWnd::Draw()
{
	if(m_bShowHudInfo)
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

	}

	//����������� �������� ��������� ������������ ������� ��� �������
	if(m_pActor->HUDview() && m_pWeapon && m_pWeapon->IsZoomed() && 
		!m_pWeapon->IsRotatingToZoom() && m_pWeapon->ZoomTexture())
	{
		m_pWeapon->ZoomTexture()->SetPos(0,0);
		m_pWeapon->ZoomTexture()->Render(0,0, Device.dwWidth, Device.dwHeight);

		if(psHUD_Flags.test(HUD_CROSSHAIR))
		{
			psHUD_Flags.set(HUD_CROSSHAIR, FALSE);
			m_bShowHudCrosshair = true;
		}
	}
	else 
	{
		if(m_bShowHudInfo)
		{
			//���������� ��������� ������
			CUIWindow::Draw();
			UIZoneMap.Render();
		}
		if (m_bShowHudCrosshair)
		{
			m_bShowHudCrosshair = false;
			psHUD_Flags.set(HUD_CROSSHAIR, TRUE);
		}
		RenderQuickInfos();
	}
}

void CUIMainIngameWnd::Update()
{
	if(!m_bShowHudInfo) return;

	static string256 text_str;

	m_pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
	if (!m_pActor) 
	{
		m_pWeapon = NULL;
		CUIWindow::Update();
		return;
	}

	if(m_pActor->GetPDA() && m_pActor->GetPDA()->ActiveContactsNum()>0)
	{
		sprintf(text_str, "%d", m_pActor->GetPDA()->ActiveContactsNum());
		UIPdaOnline.SetText(text_str);
	}
	else
	{
		UIPdaOnline.SetText("");
	}


	if(m_pActor->inventory().GetActiveSlot() < m_pActor->inventory().m_slots.size()) 
	{
		CWeapon* pWeapon = dynamic_cast<CWeapon*>(m_pActor->inventory().m_slots[
										m_pActor->inventory().GetActiveSlot()].m_pIItem); 
		
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
				sprintf(text_str, "%d/%d %s",AE,AC, 
										 *m_pWeapon->m_ammoName?*m_pWeapon->m_ammoName:"");
				UIWeaponSignAmmo.SetText(text_str);
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
    

	if(m_pActor->HUDview() && m_pWeapon && m_pWeapon->IsZoomed() && m_pWeapon->ZoomTexture()) return;


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
		
	//Wounds bleeding speed
	if(m_pActor->BleedingSpeed()>0)
	{
		sprintf(text_str, "%3.3f",m_pActor->BleedingSpeed());
		UITextWound.SetText(text_str);
		UITextWound.Show(true);
		UIStaticWound.Show(true);
	}
	else
	{
		UITextWound.Show(false);
		UIStaticWound.Show(false);
	}


	for(int i=0; i<UIPdaMsgListWnd.GetSize(); i++)
	{
		CUIPdaMsgListItem* pItem = dynamic_cast<CUIPdaMsgListItem*>(UIPdaMsgListWnd.GetItem(i));
		R_ASSERT(pItem);
		int* pShowTime = (int*)pItem->GetData();
		
		if(*pShowTime>0)
		{
			*pShowTime -= Device.dwTimeDelta;
		}
		else
		{
			xr_delete(pShowTime);
			UIPdaMsgListWnd.RemoveItem(i);
		}
	}

	CUIWindow::Update();
}

bool CUIMainIngameWnd::OnKeyboardPress(int dik)
{
	// ��������� ������ adjust hud mode
	if (m_pWeapon && g_bHudAdjustMode)
	{
		CWeaponHUD *pWpnHud = m_pWeapon->GetHUD();
		if (!pWpnHud) return false;

		Fvector tmpV;
		bool flag = false;

		if (1 == g_bHudAdjustMode)
		{
			tmpV = pWpnHud->ZoomOffset();

			switch (dik)
			{
				// Rotate +y
			case DIK_K:
				pWpnHud->SetZoomRotateX(pWpnHud->ZoomRotateX() + g_fHudAdjustValue);
				flag = true;
				break;
				// Rotate -y
			case DIK_I:
				pWpnHud->SetZoomRotateX(pWpnHud->ZoomRotateX() - g_fHudAdjustValue);
				flag = true;
				break;
				// Rotate +x
			case DIK_L:
				pWpnHud->SetZoomRotateY(pWpnHud->ZoomRotateY() + g_fHudAdjustValue);
				flag = true;
				break;
				// Rotate -x
			case DIK_J:
				pWpnHud->SetZoomRotateY(pWpnHud->ZoomRotateY() - g_fHudAdjustValue);
				flag = true;
				break;
				// Shift +x
			case DIK_W:
				tmpV.y += g_fHudAdjustValue;
				flag = true;
				break;
				// Shift -y
			case DIK_S:
				tmpV.y -= g_fHudAdjustValue;
				flag = true;
				break;
				// Shift +x
			case DIK_D:
				tmpV.x += g_fHudAdjustValue;
				flag = true;
				break;
				// Shift -x
			case DIK_A:
				tmpV.x -= g_fHudAdjustValue;
				flag = true;
				break;
				// Shift +z
			case DIK_Q:
				tmpV.z += g_fHudAdjustValue;
				flag = true;
				break;
				// Shift -z
			case DIK_E:
				tmpV.z -= g_fHudAdjustValue;
				flag = true;
				break;
				// output coordinate info to the console
			case DIK_P:
				string256 tmpStr;
				sprintf(tmpStr, "%s",
					*m_pWeapon->cNameSect());
				Log(tmpStr);
				sprintf(tmpStr, "zoom_offset\t\t\t= %f,%f,%f",
					pWpnHud->ZoomOffset().x,
					pWpnHud->ZoomOffset().y,
					pWpnHud->ZoomOffset().z);
				Log(tmpStr);
				sprintf(tmpStr, "zoom_rotate_x\t\t= %f",
					pWpnHud->ZoomRotateX());
				Log(tmpStr);
				sprintf(tmpStr, "zoom_rotate_y\t\t= %f",
					pWpnHud->ZoomRotateY());
				Log(tmpStr);
				flag = true;
				break;
			}

			if (tmpV.x || tmpV.y || tmpV.z)
				pWpnHud->SetZoomOffset(tmpV);
		}
		else
		{
			tmpV = pWpnHud->FirePoint();

			switch (dik)
			{
				// Shift +x
			case DIK_A:
				tmpV.y += g_fHudAdjustValue;
				flag = true;
				break;
				// Shift -x
			case DIK_D:
				tmpV.y -= g_fHudAdjustValue;
				flag = true;
				break;
				// Shift +z
			case DIK_Q:
				tmpV.x += g_fHudAdjustValue;
				flag = true;
				break;
				// Shift -z
			case DIK_E:
				tmpV.x -= g_fHudAdjustValue;
				flag = true;
				break;
				// Shift +y
			case DIK_S:
				tmpV.z += g_fHudAdjustValue;
				flag = true;
				break;
				// Shift -y
			case DIK_W:
				tmpV.z -= g_fHudAdjustValue;
				flag = true;
				break;
				// output coordinate info to the console
			case DIK_P:
				string256 tmpStr;
				sprintf(tmpStr, "%s",
					*m_pWeapon->cNameSect());
				Log(tmpStr);
				sprintf(tmpStr, "fire_point\t\t\t= %f,%f,%f",
					pWpnHud->FirePoint().x,
					pWpnHud->FirePoint().y,
					pWpnHud->FirePoint().z);
				Log(tmpStr);
				flag = true;
				break;
			}

			if (tmpV.x || tmpV.y || tmpV.z)
				pWpnHud->SetFirePoint(tmpV);
		}

		if (flag) return true;
	}

	if(Level().IR_GetKeyState(DIK_LSHIFT) || Level().IR_GetKeyState(DIK_RSHIFT))
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
	}
	else
	{
		switch(dik)
		{
		case DIK_NUMPADMINUS:
			HideAll();
			return true;
			break;
		case DIK_NUMPADPLUS:
			ShowAll();
			return true;
			break;
		}
	}
	
	return false;
}

void CUIMainIngameWnd::HideAll()
{
	m_bShowHudInfo = false;
}
void CUIMainIngameWnd::ShowAll()
{
	m_bShowHudInfo = true;
}


void CUIMainIngameWnd::ReceivePdaMessage(CInventoryOwner* pSender, EPdaMsg msg, int info_index)
{
	R_ASSERT(pSender);

	CUIPdaMsgListItem* pItem = NULL;
	pItem = xr_new<CUIPdaMsgListItem>();
	UIPdaMsgListWnd.AddItem(pItem, true); 
	UIPdaMsgListWnd.ScrollToBegin();

	pItem->InitCharacter(dynamic_cast<CInventoryOwner*>(pSender));
	int* pShowTime = xr_new<int>();
	*pShowTime = m_dwMaxShowTime;
	pItem->SetData(pShowTime);


	UIPdaMsgListWnd.Show(true);	

	if(msg == ePdaMsgInfo)
	{
		CInfoPortion info_portion;
		info_portion.Load(info_index);
		pItem->UIMsgText.SetText(info_portion.GetText());
	}

	m_dwMsgShowingTime = m_dwMaxShowTime;
}

void CUIMainIngameWnd::AddGameMessage	(CInventoryOwner* pSender, LPCSTR TextMessage)
{
	CUIPdaMsgListItem* pItem = NULL;
	pItem = xr_new<CUIPdaMsgListItem>();
	UIPdaMsgListWnd.AddItem(pItem, true); 
	UIPdaMsgListWnd.ScrollToBegin();

	int* pShowTime = xr_new<int>();
	*pShowTime = m_dwMaxShowTime;
	pItem->SetData(pShowTime);

	UIPdaMsgListWnd.Show(true);	

	pItem->UIMsgText.SetText(TextMessage);

	m_dwMsgShowingTime = m_dwMaxShowTime;
};

void CUIMainIngameWnd::RenderQuickInfos()
{
	static string256	text;
	static const u32	C = 0xffffffff;

	if (!m_pActor)
		return;

	CGameObject *pObject = m_pActor->ObjectWeLookingAt();

	UIStaticQuickHelp.SetTextColor(0x00000000);

	if (pObject && m_pActor->GetDefaultActionForObject())
	{
		if (fuzzyShowInfo>0.5f)
		{
			UIStaticQuickHelp.SetTextColor(subst_alpha(C,u8(iFloor(255.f*(fuzzyShowInfo-0.5f)*2.f))));
			strconcat(text, *pObject->cName(), ": ");
			strconcat(text, text, *m_strTips[m_pActor->GetDefaultActionForObject()]);
			UIStaticQuickHelp.SetText(text);
		}
		fuzzyShowInfo += SHOW_INFO_SPEED*Device.fTimeDelta;
	}
	else
	{
		fuzzyShowInfo -= HIDE_INFO_SPEED*Device.fTimeDelta;
	}
	clamp(fuzzyShowInfo,0.f,1.f);
}