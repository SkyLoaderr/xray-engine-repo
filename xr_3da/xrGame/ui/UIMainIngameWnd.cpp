// UIMainIngameWnd.cpp:  окошки-информация в игре
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
#include "../character_info.h"

#include "UIInventoryUtilities.h"

#include "../inventory.h"
#include "../UIGameSP.h"
#include "../ai_alife_registries.h"
#include "../ai_alife.h"

using namespace InventoryUtilities;


//hud adjust mode
int			g_bHudAdjustMode			= 0;
float		g_fHudAdjustValue			= 0.0f;
int			g_bNewsDisable				= 0;

const u32	g_clWhite					= 0xffffffff;

#define		RADIATION_ABSENT			0.25f
#define		RADIATION_SMALL				0.5f
#define		RADIATION_MEDIUM			0.75f
#define		RADIATION_HIGH				1.0f

#define		DEFAULT_MAP_SCALE			1.f

//-----------------------------------------------------------------------------/
//  Fade and color parameters
//-----------------------------------------------------------------------------/
#define		C_SIZE						0.025f
#define		NEAR_LIM					0.5f

#define		SHOW_INFO_SPEED				0.5f
#define		HIDE_INFO_SPEED				10.f
#define		C_ON_ENEMY					D3DCOLOR_XRGB(0xff,0,0)
#define		C_DEFAULT					D3DCOLOR_XRGB(0xff,0xff,0xff)

//-----------------------------------------------------------------------------/
//  Textual constants
//-----------------------------------------------------------------------------/
const char * const PDA_INGAME_SINGLEPLAYER_CFG	= "ingame_msglog_sp.xml";
const char * const PDA_INGAME_MULTIPLAYER_CFG	= "ingame_msglog_mp.xml";
const char * const NEWS_TEMPLATES_CFG			= "news_templates.xml";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction//////////////////////////////////////////////////////////////////////

CUIMainIngameWnd::CUIMainIngameWnd()
{
	m_pActor					= NULL;
	m_pWeapon					= NULL;

	m_dwMaxShowTime				= 20000;
	m_dwMsgShowingTime			= 0;

	m_bShowHudInfo				= true;
	m_bShowHudCrosshair			= false;
	// Quick infos
	fuzzyShowInfo				= 0.f;
	m_iFade_mSec				= 0;
	m_iPrevTime					= 0;
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

	//в это окно пользовательского ввода осуществляться не будет
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
	//запомнить оригинальный размер для иконки оружия, 
	//так как она будет масштабироваться и центрироваться
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

	// У нас отдельные конфигурации листа для SP, и MP modes
	CUIXml uiXml2;
	if (Game().type != GAME_SINGLE)
		uiXml2.Init("$game_data$", PDA_INGAME_MULTIPLAYER_CFG);
	else
		uiXml2.Init("$game_data$", PDA_INGAME_SINGLEPLAYER_CFG);

	xml_init.InitListWnd(uiXml2, "list", 0, &UIPdaMsgListWnd);
	m_iFade_mSec = uiXml2.ReadAttribInt("list", 0, "fade", 0);

	UIPdaMsgListWnd.SetVertFlip(true);
		
	AttachChild(&UITextWound);
	UITextWound.Init(UIStaticWound.GetWndRect().left+12, 
						UIStaticWound.GetWndRect().top+40,
						30,30);


	//Полоса прогресса здоровья
	AttachChild(&UIHealthBar);
	xml_init.InitProgressBar(uiXml, "progress_bar", 0, &UIHealthBar);


	//индикаторы 
	UIZoneMap.Init();
	UIZoneMap.SetScale(DEFAULT_MAP_SCALE);

	//для отображения входящих сообщений PDA
	UIPdaMsgListWnd.Show(false);
	UIPdaMsgListWnd.Enable(false);

	m_dwMsgShowingTime = 0;
	m_dwMaxShowTime = pSettings->r_s32("pda_maingame","max_show_time");

	// Подсказки, которые возникают при наведении прицела на объект
	AttachChild(&UIStaticQuickHelp);
	xml_init.InitStatic(uiXml, "quick_info", 0, &UIStaticQuickHelp);
	UIStaticQuickHelp.SetTextColor(0xffffffff);
	UIStaticQuickHelp.SetTextAlign(CGameFont::alCenter);

	// читаем типсы их xml файла
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

	// Загружаем заготовки собщений ньюсов
	LoadNewsTemplates();
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


	bool zoom_mode = false;
	bool scope_mode = false;

	//отрендерить текстуру объектива снайперского прицела или бинокля
	if(m_pActor->HUDview() && m_pWeapon && m_pWeapon->IsZoomed() && 
		m_pWeapon->ZoomHideCrosshair())
	{
		zoom_mode = true;

		if(m_pWeapon->ZoomTexture() && !m_pWeapon->IsRotatingToZoom())
		{
			m_pWeapon->ZoomTexture()->SetPos(0,0);
			m_pWeapon->ZoomTexture()->Render(0,0, Device.dwWidth, Device.dwHeight);

			scope_mode = true;
		}

		if(psHUD_Flags.test(HUD_CROSSHAIR))
		{
			psHUD_Flags.set(HUD_CROSSHAIR, FALSE);
			m_bShowHudCrosshair = true;
		}

		zoom_mode = true;
	}
	
	if(!scope_mode)
	{
		if(m_bShowHudInfo)
		{
			//отрисовать остальные иконки
			CUIWindow::Draw();
			UIZoneMap.Render();
		}
		if (m_bShowHudCrosshair && !zoom_mode)
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
				//отцентрировать иконку
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
				//сторока для вывода патронов к оружию
				sprintf(text_str, "%d/%d %s",AE,AC, 
										 *m_pWeapon->m_ammoName?*m_pWeapon->m_ammoName:"");
				UIWeaponSignAmmo.SetText(text_str);
			}
		}
	} 
	else
		m_pWeapon = NULL;

	//сбросить индикаторы
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
			// Плавное исчезновение надписи
			float fAlphaFactor = static_cast<float>(m_iFade_mSec - *pShowTime) / m_iFade_mSec;
			if (fAlphaFactor < 0) fAlphaFactor = 0;
			pItem->UIMsgText.SetTextColor(subst_alpha(pItem->UIMsgText.GetTextColor(), u8(iFloor(255.f * (1 - fAlphaFactor)))));

			*pShowTime -= Device.dwTimeDelta;
		}
		else
		{
			xr_delete(pShowTime);
			UIPdaMsgListWnd.RemoveItem(i);
		}
	}

	// Check for new news
	CheckForNewNews();

	CUIWindow::Update();
}

bool CUIMainIngameWnd::OnKeyboardPress(int dik)
{
	// поддержка режима adjust hud mode
	if (g_bHudAdjustMode)
	{
		CWeaponHUD *pWpnHud = NULL;
		if (m_pWeapon)
		{
			CWeaponHUD *pWpnHud = m_pWeapon->GetHUD();
			if (!pWpnHud) return false;
		}

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
		else if (2 == g_bHudAdjustMode)
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
				if (m_pWeapon)
				{
					sprintf(tmpStr, "%s",
						*m_pWeapon->cNameSect());
					Log(tmpStr);
				}

				sprintf(tmpStr, "fire_point\t\t\t= %f,%f,%f",
					pWpnHud->FirePoint().x,
					pWpnHud->FirePoint().y,
					pWpnHud->FirePoint().z);

				Log(tmpStr);
				flag = true;
				break;
			}
		
			pWpnHud->SetFirePoint(tmpV);
		}
		else
		{
			CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());

			R_ASSERT(pActor);

			tmpV = pActor->GetMissileOffset();

			if (!pActor) return false;
			switch (dik)
			{
				// Shift +x
			case DIK_E:
				tmpV.y += g_fHudAdjustValue;
				flag = true;
				break;
				// Shift -x
			case DIK_Q:
				tmpV.y -= g_fHudAdjustValue;
				flag = true;
				break;
				// Shift +z
			case DIK_D:
				tmpV.x += g_fHudAdjustValue;
				flag = true;
				break;
				// Shift -z
			case DIK_A:
				tmpV.x -= g_fHudAdjustValue;
				flag = true;
				break;
				// Shift +y
			case DIK_W:
				tmpV.z += g_fHudAdjustValue;
				flag = true;
				break;
				// Shift -y
			case DIK_S:
				tmpV.z -= g_fHudAdjustValue;
				flag = true;
				break;
				// output coordinate info to the console
			case DIK_P:
				string256 tmpStr;
				if (m_pWeapon)
				{
					sprintf(tmpStr, "%s",
						*m_pWeapon->cNameSect());
					Log(tmpStr);
				}

				sprintf(tmpStr, "missile_throw_offset\t\t\t= %f,%f,%f",
					pActor->GetMissileOffset().x,
					pActor->GetMissileOffset().y,
					pActor->GetMissileOffset().z);

				Log(tmpStr);
				flag = true;
				break;
			}

			pActor->SetMissileOffset(tmpV);
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
	
	ACTOR_DEFS::EActorAction actor_action = m_pActor->GetDefaultActionForObject();
	if (pObject && eaaNoAction!=actor_action)
	{
		LPCSTR object_name = *pObject->cName();
		if(eaaTalk == actor_action)
		{
			object_name = m_pActor->PersonWeLookingAt()->CharacterInfo().Name();
		} 
		else if(eaaPickup == actor_action)
		{
			CInventoryItem* item = dynamic_cast<CInventoryItem*>(m_pActor->ObjectWeLookingAt());
			VERIFY(item);
			object_name =item->Name();
		}

		if (fuzzyShowInfo>0.5f)
		{
			UIStaticQuickHelp.SetTextColor(subst_alpha(C,u8(iFloor(255.f*(fuzzyShowInfo-0.5f)*2.f))));
			strconcat(text, object_name, ": ");
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

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::OnNewsReceived(const ALife::SGameNews &newsItem)
{
	string256	newsPhrase;
	string128	time;
	string128	locationName = "";
	string512	result;

	// If ignore flag is set, than ignore whole news item
	if (m_NewsTemplates[static_cast<u32>(newsItem.m_news_type)].ignore) return;

	// Get Level name
	const CGameGraph::CVertex	*game_vertex = ai().game_graph().vertex(newsItem.m_game_vertex_id);
	if (ai().game_graph().header().levels().find(game_vertex->level_id()) != ai().game_graph().header().levels().end())
	{
		sprintf(locationName, "%s ", ai().game_graph().header().levels().find(game_vertex->level_id())->second.name());
	}

	// Substitute placeholders with real names
	CSE_ALifeDynamicObject	*newsActorOne = ai().alife().object(newsItem.m_object_id[0]);
	if (newsItem.m_object_id[1] != static_cast<u16>(-1))
	{
		CSE_ALifeDynamicObject	*newsActorTwo = ai().alife().object(newsItem.m_object_id[1]);
		sprintf(newsPhrase, *m_NewsTemplates[static_cast<u32>(newsItem.m_news_type)].str, newsActorTwo->s_name_replace, newsActorOne->s_name_replace);
	}
	else
		sprintf(newsPhrase, *m_NewsTemplates[static_cast<u32>(newsItem.m_news_type)].str, newsActorOne->s_name_replace, "");


	// Calc curent time
	u8 m_CurrMins		= static_cast<u8>(newsItem.m_game_time / (1000 * 60) % 60 & 0xFF);
	u8 m_CurrHours		= static_cast<u8>(newsItem.m_game_time / (1000 * 3600) % 24 & 0xFF);
	sprintf(time, "%02i:%02i\\n", m_CurrHours, m_CurrMins);
	strconcat(result, locationName, time, newsPhrase);

	if (!g_bNewsDisable)
	{
		CUIPdaMsgListItem* pItem = NULL;
		pItem = xr_new<CUIPdaMsgListItem>();
		UIPdaMsgListWnd.AddItem(pItem, true); 
		UIPdaMsgListWnd.ScrollToBegin();

		pItem->InitCharacter(dynamic_cast<CInventoryOwner*>(Level().CurrentEntity()));
		int* pShowTime = xr_new<int>();
		*pShowTime = m_dwMaxShowTime;
		pItem->SetData(pShowTime);

		UIPdaMsgListWnd.Show(true);	

		pItem->UIMsgText.SetText(result);

		m_dwMsgShowingTime = m_dwMaxShowTime;
	}

	CUIGameSP* pGameSP		= dynamic_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	pGameSP->PdaMenu.AddNewsItem(result);
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::LoadNewsTemplates()
{
	const char * const NODE_NAME = "news_template";
	CUIXml uiXml;
	uiXml.Init("$game_data$", NEWS_TEMPLATES_CFG);

	string256	buf;

	int templatesCount = uiXml.GetNodesNum("", 0, NODE_NAME);
	strconcat(buf, NODE_NAME,":text");
	
	for (int i = 0; i < templatesCount; ++i)
	{
		int idx						= uiXml.ReadAttribInt(NODE_NAME, i, "index");
		ref_str news_template		= uiXml.Read(buf, i, NULL);
		m_NewsTemplates[idx].str	= news_template;
		m_NewsTemplates[idx].ignore	= !!uiXml.ReadAttribInt(NODE_NAME, i, "ignore");
	}
}

//////////////////////////////////////////////////////////////////////////

bool CUIMainIngameWnd::CheckForNewNews()
{
	// Нет симуляции или время проверки еще не пришло
	// Либо ньюсы принудительно отключены из консоли
	if (!ai().get_alife() || m_iPrevTime + NEWS_CHECK_INTERVAL > Level().GetGameTime())
	{
		return false;
	}

	m_iPrevTime = Level().GetGameTime();

	// последний известный NewsID
	static ALife::_NEWS_ID	lastKnownNewsID = 0;
	static bool				emptyNewsQueue	= true;

	CSE_ALifeNewsRegistry::NEWS_REGISTRY::const_iterator cit, cit_i;
	cit = ai().alife().news().upper_bound(lastKnownNewsID);

	if (emptyNewsQueue && ai().alife().news().size() > 0)
	{
		cit = ai().alife().news().begin();
		emptyNewsQueue = false;
	}

	if (cit == ai().alife().news().end()) return false;

	// Iterate through all new news and report to OnNewsReceived function
	for (cit_i = cit; cit_i != ai().alife().news().end(); ++cit_i)
	{
		OnNewsReceived(*cit_i->second);
		// Remember last known news id. Last remembering would last known news id
		lastKnownNewsID	= cit_i->first;
	}

	return true;
}
