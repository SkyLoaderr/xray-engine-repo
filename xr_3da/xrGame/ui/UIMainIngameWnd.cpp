// UIMainIngameWnd.cpp:  окошки-информация в игре
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIMainIngameWnd.h"

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
#include "../weaponmagazined.h"

#include "../xrServer_objects_ALife.h"
#include "../alife_simulator.h"
#include "../alife_news_registry.h"
#include "../alife_object_registry.h"
#include "../game_cl_base.h"
#include "../level.h"
#include "../seniority_hierarchy_holder.h"

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
const char * const MAININGAME_XML				= "maingame.xml";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction//////////////////////////////////////////////////////////////////////

CUIMainIngameWnd::CUIMainIngameWnd()
{
	m_pActor					= NULL;
	m_pWeapon					= NULL;

	m_dwMaxShowTime				= 20000;

	m_bShowHudInfo				= true;
	m_bShowHudCrosshair			= false;
	// Quick infos
	fuzzyShowInfo				= 0.f;
	m_iPdaMessagesFade_mSec		= 0;
	m_iInfoMessagesFade_mSec	= 0;
	m_iPrevTime					= 0;
}

CUIMainIngameWnd::~CUIMainIngameWnd()
{
	DestroyFlashingIcons();
}

void CUIMainIngameWnd::Init()
{
	CUIXml uiXml;
	uiXml.Init("$game_data$", MAININGAME_XML);
	
	CUIXmlInit xml_init;
	CUIWindow::Init(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	//в это окно пользовательского ввода осуществляться не будет
	Enable(false);


	AttachChild(&UIStaticHealth);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticHealth);

	AttachChild(&UIStaticArmor);
	xml_init.InitStatic(uiXml, "static", 5, &UIStaticArmor);

	AttachChild(&UIWeaponBack);
	xml_init.InitStatic(uiXml, "static", 1, &UIWeaponBack);

	UIWeaponBack.AttachChild(&UIWeaponSignAmmo);
	xml_init.InitStatic(uiXml, "static", 2, &UIWeaponSignAmmo);
	UIWeaponSignAmmo.SetElipsis(CUIStatic::eepEnd, 2);

	UIWeaponBack.AttachChild(&UIWeaponIcon);
	xml_init.InitStatic(uiXml, "static", 3, &UIWeaponIcon);
	UIWeaponIcon.SetShader(GetEquipmentIconsShader());
	UIWeaponIcon.ClipperOn();

	//запомнить оригинальный размер для иконки оружия, 
	//так как она будет масштабироваться и центрироваться
	m_iWeaponIconWidth = UIWeaponIcon.GetWidth();
	m_iWeaponIconHeight = UIWeaponIcon.GetHeight();
	m_iWeaponIconX = UIWeaponIcon.GetWndRect().left;
	m_iWeaponIconY = UIWeaponIcon.GetWndRect().top;

	UIWeaponIcon.Enable(false);

	//
	AttachChild(&UIPdaOnline);
	xml_init.InitStatic(uiXml, "static", 4, &UIPdaOnline);
	
	AttachChild(&UIPdaMsgListWnd);

	// У нас отдельные конфигурации листа для SP, и MP modes
	CUIXml uiXml2;
	if (GameID() != GAME_SINGLE)
		uiXml2.Init("$game_data$", PDA_INGAME_MULTIPLAYER_CFG);
	else
		uiXml2.Init("$game_data$", PDA_INGAME_SINGLEPLAYER_CFG);

	xml_init.InitListWnd(uiXml2, "list", 0, &UIPdaMsgListWnd);
	m_iPdaMessagesFade_mSec = uiXml2.ReadAttribInt("list", 0, "fade", 0);

	UIPdaMsgListWnd.SetVertFlip(true);

	// Для информационных сообщений
	AttachChild(&UIInfoMessages);
	xml_init.InitListWnd(uiXml, "info_list", 0, &UIInfoMessages);
	m_iInfoMessagesFade_mSec = uiXml.ReadAttribInt("info_list", 0, "fade", 0);
	UIInfoMessages.SetVertFlip(true);

		
//	AttachChild(&UITextWound);
//	UITextWound.Init(UIStaticWound.GetWndRect().left+12, 
//						UIStaticWound.GetWndRect().top+40,
//						30,30);

	//Полоса прогресса здоровья
	UIStaticHealth.AttachChild(&UIHealthBar);
	xml_init.InitProgressBar(uiXml, "progress_bar", 0, &UIHealthBar);

	//Полоса прогресса армора
	UIStaticArmor.AttachChild(&UIArmorBar);
	xml_init.InitProgressBar(uiXml, "progress_bar", 1, &UIArmorBar);

	//индикаторы 
	UIZoneMap.Init();
	UIZoneMap.SetScale(DEFAULT_MAP_SCALE);

	//для отображения входящих сообщений PDA
	UIPdaMsgListWnd.Show(false);
	UIPdaMsgListWnd.Enable(false);

	m_dwMaxShowTime		= pSettings->r_s32("maingame_ui", "pda_msgs_max_show_time");
	m_iInfosShowTime	= pSettings->r_s32("maingame_ui", "info_msgs_max_show_time");

	// Подсказки, которые возникают при наведении прицела на объект
	AttachChild(&UIStaticQuickHelp);
	xml_init.InitStatic(uiXml, "quick_info", 0, &UIStaticQuickHelp);
	UIStaticQuickHelp.SetTextColor(0xffffffff);
	UIStaticQuickHelp.SetTextAlign(CGameFont::alCenter);

	uiXml.SetLocalRoot(uiXml.GetRoot());

	// Загружаем иконки 
	AttachChild(&UIWeaponJammedIcon);
	xml_init.InitStatic(uiXml, "weapon_jammed_static", 0, &UIWeaponJammedIcon);

	AttachChild(&UIRadiaitionIcon);
	xml_init.InitStatic(uiXml, "radiation_static", 0, &UIRadiaitionIcon);

	AttachChild(&UIWoundIcon);
	xml_init.InitStatic(uiXml, "wound_static", 0, &UIWoundIcon);

	AttachChild(&UIStarvationIcon);
	xml_init.InitStatic(uiXml, "starvation_static", 0, &UIStarvationIcon);

	AttachChild(&UIFatigueIcon);
	xml_init.InitStatic(uiXml, "fatigue_static", 0, &UIFatigueIcon);

	// Загружаем заготовки собщений ньюсов
	LoadNewsTemplates();

	ref_str warningStrings[5] = 
	{	
		"jammed",
		"radiation",
		"wounds",
		"starvation",
		"fatigue"
	};

	// Загружаем пороговые значения для индикаторов
	EWarningIcons j = ewiWeaponJammed;
	while (j <= ewiFatigue)
	{
		// Читаем данные порогов для каждого индикатора
		ref_str cfgRecord = pSettings->r_string("main_ingame_indicators_thresholds", *warningStrings[static_cast<int>(j) - 1]);
		u32 count = _GetItemCount(*cfgRecord);

		char	singleThreshold[8];
		float	f = 0;
		for (u32 k = 0; k < count; ++k)
		{
			_GetItem(*cfgRecord, k, singleThreshold);
			sscanf(singleThreshold, "%f", &f);

			m_Thresholds[j].push_back(f);
		}

		j = static_cast<EWarningIcons>(j + 1);
	}

	// Money
	if (GameID() == GAME_DEATHMATCH || GameID() == GAME_TEAMDEATHMATCH || GameID() == GAME_ARTEFACTHUNT)
	{
		AttachChild(&UIMoneyIndicator);
		xml_init.InitMultiTextStatic(uiXml, "money_mt_static", 0, &UIMoneyIndicator);
//		ChangeTotalMoneyIndicator("100$");
	}

	// Flashing icons initialize
	uiXml.SetLocalRoot(uiXml.NavigateToNode("flashing_icons"));
	InitFlashingIcons(uiXml);

	UIContactsFlicker.SetAnimationPeriod(1000);
	UIContactsFlicker.SetPlayDuration(4000);
	UIContactsFlicker.SetMessageTarget(this);
}

void CUIMainIngameWnd::Draw()
{
	bool zoom_mode = false;
	bool scope_mode = false;

	for(CUSTOM_TEXTURE_IT it = m_CustomTextures.begin(); m_CustomTextures.end() != it; it++)
	{
		CUSTOM_TEXTURE& custom_texture = *it;
		custom_texture.static_item->SetPos(0,0);
		custom_texture.static_item->Render(custom_texture.x1, custom_texture.y1,
									custom_texture.x2, custom_texture.y2);
	}
	m_CustomTextures.clear();

	//отрендерить текстуру объектива снайперского прицела или бинокля
	if(m_pActor->HUDview() && m_pWeapon && m_pWeapon->IsZoomed() && 
		m_pWeapon->ZoomHideCrosshair())
	{
		zoom_mode = true;

		if(m_pWeapon->ZoomTexture() && !m_pWeapon->IsRotatingToZoom())
		{
			m_pWeapon->ZoomTexture()->SetPos(0,0);
			m_pWeapon->ZoomTexture()->Render(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

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

	m_pActor = smart_cast<CActor*>(Level().CurrentEntity());
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

	// Armor indicator stuff
	PIItem	pItem = m_pActor->inventory().ItemFormSlot(OUTFIT_SLOT);
	if (pItem)
	{
		UIArmorBar.Show(true);
//		UIArmorString.Show(true);
		UIStaticArmor.Show(true);
		UIArmorBar.SetProgressPos(static_cast<s16>(pItem->GetCondition() * 100));
	}
	else
	{
		UIArmorBar.Show(false);
//		UIArmorString.Show(false);
		UIStaticArmor.Show(false);
	}

	if(m_pActor->inventory().GetActiveSlot() < m_pActor->inventory().m_slots.size()) 
	{
		CWeapon* pWeapon = smart_cast<CWeapon*>(m_pActor->inventory().m_slots[
										m_pActor->inventory().GetActiveSlot()].m_pIItem); 

		CWeaponMagazined* pWeaponMagazined = smart_cast<CWeaponMagazined*>(pWeapon);
		
		if(pWeapon)
		{
			// Remember last used ammo types, and if this type doesn't changed 
			// then no need to update info
			static u32			prevAmmoID		= static_cast<u32>(-1);

			if(m_pWeapon != pWeapon || prevAmmoID != m_pWeapon->m_ammoType)
			{

				m_pWeapon = pWeapon;

				//if (xr_strcmp(m_pWeapon->NameShort(), "knife") != 0)
				if (pWeaponMagazined)
				{
					UIWeaponIcon.Show(true);
				}
				else
				{
					UIWeaponIcon.Show(false);
				}

				prevAmmoID			= m_pWeapon->m_ammoType;
				ref_str sect_name	= m_pWeapon->m_ammoTypes[m_pWeapon->m_ammoType];
				
				//properties used by inventory menu
				int m_iGridWidth	= pSettings->r_u32(sect_name, "inv_grid_width");
				int m_iGridHeight	= pSettings->r_u32(sect_name, "inv_grid_height");

				int m_iXPos			= pSettings->r_u32(sect_name, "inv_grid_x");
				int m_iYPos			= pSettings->r_u32(sect_name, "inv_grid_y");

				UIWeaponIcon.GetUIStaticItem().SetOriginalRect(
						m_iXPos * INV_GRID_WIDTH,
						m_iYPos * INV_GRID_HEIGHT,
						m_iGridWidth * INV_GRID_WIDTH,
						m_iGridHeight * INV_GRID_HEIGHT);
				float scale_x = float(m_iWeaponIconWidth)/
					float(m_iGridWidth*INV_GRID_WIDTH);

				float scale_y = float(m_iWeaponIconHeight)/
					float(m_iGridHeight*INV_GRID_HEIGHT);

				float scale = scale_x<scale_y?scale_x:scale_y;
					UIWeaponIcon.SetTextureScale(scale);
					UIWeaponIcon.SetWidth(iFloor(0.5f+ m_iGridWidth*INV_GRID_WIDTH*scale));
					UIWeaponIcon.SetHeight(iFloor(0.5f+ m_iGridHeight*INV_GRID_HEIGHT*scale));

				UIWeaponIcon.MoveWindow(m_iWeaponIconX + 
					(m_iWeaponIconWidth - UIWeaponIcon.GetWidth())/2,
					m_iWeaponIconY + 
					(m_iWeaponIconHeight - UIWeaponIcon.GetHeight())/2);
			}

			UIWeaponBack.SetText(m_pWeapon->NameShort());
			UIWeaponBack.SetTextX(static_cast<int>((UIWeaponBack.GetAbsoluteRect().right - UIWeaponBack.GetAbsoluteRect().left) * 0.5 - UIWeaponIcon.GetFont()->SizeOf(m_pWeapon->NameShort()) / 2));
			
			int	AE = m_pWeapon->GetAmmoElapsed();
			int	AC = m_pWeapon->GetAmmoCurrent();
			if((AE>=0)&&(AC>=0))
			{
				//сторока для вывода патронов к оружию
//				sprintf(text_str, "%d/%d %s",AE,AC, 
//										 *m_pWeapon->m_ammoName?*m_pWeapon->m_ammoName:"");
				sprintf(text_str, "%d/%d",AE,AC - AE);
				UIWeaponSignAmmo.SetText(text_str);
			}
		}
	} 
	else
	{
		m_pWeapon = NULL;
		UIWeaponBack.SetText("");
	}


	//сбросить индикаторы
	if(!m_pWeapon)
	{
		UIWeaponSignAmmo.SetText("");
		//UIWeaponSignName.SetText("");
		UIWeaponIcon.Show(false);
	}
    

	if(m_pActor->HUDview() && m_pWeapon && m_pWeapon->IsZoomed() && m_pWeapon->ZoomTexture()) return;


    // radar
	UIZoneMap.UpdateRadar(m_pActor);
	// viewport
	float h,p;
	Device.vCameraDirection.getHP	(h,p);
	UIZoneMap.SetHeading			(-h);
		
	// health&armor
	//	UIHealth.Out(m_Actor->g_Health(),m_Actor->g_Armor());
	UIHealthBar.SetProgressPos((s16)m_pActor->g_Health());
	EWarningIcons i = ewiWeaponJammed;
		
	while (i <= ewiFatigue)
	{
		float value = 0;
		switch (i)
		{
		//radiation
		case ewiRadiation:
			value = m_pActor->GetRadiation();
			break;
		case ewiWound:
			value = m_pActor->BleedingSpeed();
			break;
		case ewiWeaponJammed:
			if (m_pWeapon)
				value = 1 - m_pWeapon->GetCondition();
			break;
		case ewiStarvation:
			value = 1 - m_pActor->GetSatiety();
			break;
		case ewiFatigue:
			value = 1 - m_pActor->GetPower();
			break;
		default:
			R_ASSERT(!"Unknown type of warning icon");
		}

		xr_vector<float>::reverse_iterator	rit;

		// Сначала проверяем на точное соответсвие
		rit  = std::find(m_Thresholds[i].rbegin(),
						 m_Thresholds[i].rend(),
						 value);

		// Если его нет, то берем последнее меньшее значение ()
		if (rit == m_Thresholds[i].rend())
		{
			rit = std::find_if(m_Thresholds[i].rbegin(),
						m_Thresholds[i].rend(),
						std::bind2nd(std::less<float>(), value));
		}

		// Минимальное и максимальное значения границы
		float min = m_Thresholds[i].front();
		float max = m_Thresholds[i].back();

		if (rit != m_Thresholds[i].rend())
			SetWarningIconColor(i, RGB_ALPHA(0xFF, clampr<u32>(static_cast<u32>(255 * ((*rit - min) / (max - min) * 2)), 0, 255), 
												   clampr<u32>(static_cast<u32>(255 * (2.0f - (*rit - min) / (max - min) * 2)), 0, 255),
												   0));
		else
			TurnOffWarningIcon(i);

		i = static_cast<EWarningIcons>(i + 1);
	}

	// Fade animations
	FadeUpdate(&UIPdaMsgListWnd, m_iPdaMessagesFade_mSec);
	FadeUpdate(&UIInfoMessages, m_iInfoMessagesFade_mSec);

	// Check for new news
	CheckForNewNews();

	UpdateFlashingIcons();
	UpdateContactsAnimation();

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
			pWpnHud = m_pWeapon->GetHUD();
			if (!pWpnHud) return false;
		}
		else
			return false;

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
			CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());

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


void CUIMainIngameWnd::ReceivePdaMessage(CInventoryOwner* pSender, EPdaMsg msg, INFO_INDEX info_index)
{
	R_ASSERT(pSender);

	CUIPdaMsgListItem* pItem = NULL;
	pItem = xr_new<CUIPdaMsgListItem>();
	UIPdaMsgListWnd.AddItem<CUIListItem>(pItem, 0); 
	UIPdaMsgListWnd.ScrollToBegin();

	pItem->InitCharacter(smart_cast<CInventoryOwner*>(pSender));
	pItem->SetValue(m_dwMaxShowTime);


	UIPdaMsgListWnd.Show(true);	

	if(msg == ePdaMsgInfo)
	{
		CInfoPortion info_portion;
		info_portion.Load(info_index);
		pItem->UIMsgText.SetText(info_portion.GetText());
	}
	else
	{
	//	pItem->UIMsgText.SetText(CPda::m_PdaMsgStr[msg]);
	}
}

//////////////////////////////////////////////////////////////////////////

CUIPdaMsgListItem * CUIMainIngameWnd::AddGameMessage(LPCSTR message)
{
	CUIPdaMsgListItem* pItem = NULL;
	pItem = xr_new<CUIPdaMsgListItem>();
	UIPdaMsgListWnd.AddItem<CUIListItem>(pItem, 0); 
	UIPdaMsgListWnd.ScrollToBegin();

	pItem->SetValue(m_dwMaxShowTime);

	UIPdaMsgListWnd.Show(true);	

	pItem->UIMsgText.SetText(message);
	return pItem;
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::AddPersonalizedGameMessage(CInventoryOwner* pSender, LPCSTR TextMessage)
{
	CUIPdaMsgListItem *pItem = AddGameMessage(TextMessage);
	if (pItem)
	{
		pItem->InitCharacter(pSender);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::AddIconedGameMessage(LPCSTR textureName, RECT originalRect, LPCSTR message)
{
	CUIPdaMsgListItem* pItem = AddGameMessage(message);

	if (pItem)
	{
		pItem->UIIcon.InitTexture(textureName);
		pItem->UIIcon.SetOriginalRect(originalRect.left, originalRect.top, originalRect.right, originalRect.bottom);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::AddInfoMessage(LPCSTR message)
{
	CUIPdaMsgListItem* pItem = NULL;
	pItem = xr_new<CUIPdaMsgListItem>();
	UIInfoMessages.AddItem<CUIListItem>(pItem, 0); 
	R_ASSERT(UIInfoMessages.GetFont());
	pItem->UIMsgText.SetFont(UIInfoMessages.GetFont());
	pItem->UIMsgText.SetTextColor(UIInfoMessages.GetTextColor());
	pItem->UIMsgText.MoveWindow(-static_cast<int>(UIInfoMessages.GetFont()->SizeOf(message) / 2),
								pItem->UIMsgText.GetWndRect().top);
	UIInfoMessages.ScrollToBegin();

	pItem->SetValue(m_iInfosShowTime);

	UIInfoMessages.Show(true);	

	pItem->UIMsgText.SetText(message);
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::RenderQuickInfos()
{
	static string256	text;
	static const u32	C = 0xffffffff;

	if (!m_pActor)
		return;

	CGameObject *pObject = m_pActor->ObjectWeLookingAt();

	UIStaticQuickHelp.SetTextColor(0x00000000);
	
	LPCSTR actor_action = m_pActor->GetDefaultActionForObject();
	if (pObject && actor_action)
	{
//		LPCSTR object_name = pObject->Name();

		if (fuzzyShowInfo>0.5f)
		{
			UIStaticQuickHelp.SetTextColor(subst_alpha(C,u8(iFloor(255.f*(fuzzyShowInfo-0.5f)*2.f))));
//			strconcat(text, object_name, ": ");
			strcpy(text, m_pActor->GetDefaultActionForObject());
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

void CUIMainIngameWnd::OnNewsReceived(const CALifeNews &newsItem)
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
	CSE_ALifeDynamicObject	*newsActorOne = ai().alife().objects().object(newsItem.m_object_id[0]);
	if (newsItem.m_object_id[1] != static_cast<u16>(-1))
	{
		CSE_ALifeDynamicObject	*newsActorTwo = ai().alife().objects().object(newsItem.m_object_id[1]);
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
//		CUIPdaMsgListItem* pItem = NULL;
//		pItem = xr_new<CUIPdaMsgListItem>();
//		UIPdaMsgListWnd.AddItem<CUIListItem>(pItem); 
//		UIPdaMsgListWnd.ScrollToBegin();
//
//		pItem->InitCharacter(smart_cast<CInventoryOwner*>(Level().CurrentEntity()));
//		pItem->SetValue(m_dwMaxShowTime);
//
//		UIPdaMsgListWnd.Show(true);	
//
//		pItem->UIMsgText.SetText(result);
		AddPersonalizedGameMessage(smart_cast<CInventoryOwner*>(Level().CurrentEntity()), result);
	}

	CUIGameSP* pGameSP		= smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
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

	ALife::NEWS_REGISTRY::const_iterator cit, cit_i;
	cit = ai().alife().news().news().upper_bound(lastKnownNewsID);

	if (emptyNewsQueue && ai().alife().news().news().size() > 0)
	{
		cit = ai().alife().news().news().begin();
		emptyNewsQueue = false;
	}

	if (cit == ai().alife().news().news().end()) return false;

	// Iterate through all new news and report to OnNewsReceived function
	for (cit_i = cit; cit_i != ai().alife().news().news().end(); ++cit_i)
	{
		OnNewsReceived(*cit_i->second);
		// Remember last known news id. Last remembering would last known news id
		lastKnownNewsID	= cit_i->first;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::SetWarningIconColor(EWarningIcons icon, const u32 cl)
{
	bool bMagicFlag = true;

	// Задаем цвет требуемой иконки
	switch(icon)
	{
	case ewiAll:
		bMagicFlag = false;
	case ewiWeaponJammed:
		UIWeaponJammedIcon.SetColor(cl);
		if (bMagicFlag) break;
	case ewiRadiation:
		UIRadiaitionIcon.SetColor(cl);
		if (bMagicFlag) break;
	case ewiWound:
		UIWoundIcon.SetColor(cl);
		if (bMagicFlag) break;
	case ewiStarvation:
		UIStarvationIcon.SetColor(cl);
		if (bMagicFlag) break;
	case ewiFatigue:
		UIFatigueIcon.SetColor(cl);
		if (bMagicFlag) break;
	default:
		R_ASSERT(!"Unknown warning icon type");
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::TurnOffWarningIcon(EWarningIcons icon)
{
	SetWarningIconColor(icon, 0x00ffffff);
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::ChangeTotalMoneyIndicator(ref_str newMoneyString)
{
	CUIMultiTextStatic::SinglePhrase * sp = UIMoneyIndicator.GetPhraseByIndex(0);
	sp->str = newMoneyString;
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::DisplayMoneyChange(ref_str deltaMoney)
{
	CUIMultiTextStatic::SinglePhrase * sp = UIMoneyIndicator.GetPhraseByIndex(1);
	sp->str				= deltaMoney;
	sp->effect.PlayAnimation();
	EffectParams *eff	= sp->effect.SetStyleParams(CUITextBanner::tbsFade);
	eff->bOn			= true;
	sp->effect.ResetAnimation(CUITextBanner::tbsFade);
}

//////////////////////////////////////////////////////////////////////////

void  CUIMainIngameWnd::AddStaticItem (CUIStaticItem* si, int left, int top, int right, int bottom)
{
	m_CustomTextures.push_back(CUSTOM_TEXTURE(si, left, top, right, bottom));
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::FadeUpdate(CUIListWnd *pWnd, int fadeDuration)
{
	for(int i=0; i<pWnd->GetSize(); i++)
	{
		CUIPdaMsgListItem* pItem = smart_cast<CUIPdaMsgListItem*>(pWnd->GetItem(i));
		R_ASSERT(pItem);
		int show_time = pItem->GetValue();

		if(show_time>0)
		{
			// Плавное исчезновение надписи
			float fAlphaFactor = static_cast<float>(fadeDuration - show_time) / fadeDuration;
			if (fAlphaFactor < 0) fAlphaFactor = 0;
			pItem->UIMsgText.SetTextColor(subst_alpha(pItem->UIMsgText.GetTextColor(), u8(iFloor(255.f * (1 - fAlphaFactor)))));

			show_time -= Device.dwTimeDelta;
			pItem->SetValue(show_time);
		}
		else
			pWnd->RemoveItem(i);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::SetFlashIconState(EFlashingIcons type, bool enable)
{
	// Включаем анимацию требуемой иконки
	FlashingIcons_it icon = m_FlashingIcons.find(type);
	R_ASSERT2(icon != m_FlashingIcons.end(), "Flashing icon with this type not existed");

	enable ? icon->second.second.Play() : icon->second.second.Stop();
	if(!enable)
	{
		icon->second.first->TextureOff();
		icon->second.first->Show(false);
	}
	else
	{
		icon->second.first->TextureOn();
		icon->second.first->Show(true);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::InitFlashingIcons(CUIXml &node)
{
	const char * const flashingIconNodeName = "flashing_icon";
	int staticsCount = node.GetNodesNum("", 0, flashingIconNodeName);

	CUIXmlInit xml_init;
	CUIStatic *pIcon = NULL;
	// Пробегаемся по всем нодам и инициализируем из них статики
	for (int i = 0; i < staticsCount; ++i)
	{
		pIcon = xr_new<CUIStatic>();
		xml_init.InitStatic(node, flashingIconNodeName, i, pIcon);
		ref_str iconType = node.ReadAttrib(flashingIconNodeName, i, "type", "none");

		// Теперь запоминаем иконку и ее тип
		EFlashingIcons type = efiPdaTask;

		if		(iconType == "pda")		type = efiPdaTask;
		else if (iconType == "mail")	type = efiMail;
		else	R_ASSERT(!"Unknown type of mainingame flashing icon");

		R_ASSERT2(m_FlashingIcons.find(type) == m_FlashingIcons.end(), "Flashing icon with this type already exists");

		IconInfo &val	= m_FlashingIcons[type];
		val.first		= pIcon;

		val.second.SetAnimationPeriod(node.ReadAttribInt(flashingIconNodeName, i, "period", 1000));
		AttachChild(pIcon);
		pIcon->Show(false);

		// Some type related hacks
		switch (type)
		{
		case efiPdaTask:
			pIcon->GetUIStaticItem().SetScale(0.5f);
			break;
		default:
			NODEFAULT;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::DestroyFlashingIcons()
{
	for (FlashingIcons_it it = m_FlashingIcons.begin(); it != m_FlashingIcons.end(); ++it)
	{
		DetachChild(it->second.first);
		xr_delete(it->second.first);
	}

	m_FlashingIcons.clear();
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::UpdateFlashingIcons()
{
	for (FlashingIcons_it it = m_FlashingIcons.begin(); it != m_FlashingIcons.end(); ++it)
	{
		CUIAnimationFade &f = it->second.second;
		f.Update();
		//		if (1 == f.GetCurrentPhase()) it->second.first->Show(true);
		//		else it->second.first->Show(false);
		it->second.first->SetColor(subst_alpha(it->second.first->GetColor(), f.GetCurrentPhase()));
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::AnimateContacts()
{
	UIContactsFlicker.Play();
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::UpdateContactsAnimation()
{
	UIContactsFlicker.Update();
	if (UIContactsFlicker.GetState() == CUIAnimationBase::easPlayed)
	{
		UIPdaOnline.Show(!!UIContactsFlicker.GetCurrentPhase());
	}
	else
		UIPdaOnline.Show(true);
}