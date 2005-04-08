// UIMainIngameWnd.cpp:  ������-���������� � ����
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "UIMainIngameWnd.h"
#include "../UIZoneMap.h"


#include <dinput.h>
#include "../Entity.h"
#include "../HUDManager.h"
#include "../PDA.h"
#include "../UIStaticItem.h"
#include "../WeaponHUD.h"
#include "../character_info.h"
#include "../inventory.h"
#include "../UIGameSP.h"
#include "../weaponmagazined.h"
#include "../missile.h"
#include "../xrServer_objects_ALife.h"
#include "../alife_simulator.h"
#include "../alife_object_registry.h"
#include "../game_cl_base.h"
#include "../level.h"
#include "../seniority_hierarchy_holder.h"

#include "../date_time.h"
#include "../xrServer_Objects_ALife_Monsters.h"
#include "../../LightAnimLibrary.h"

#include "UIInventoryUtilities.h"


#include "UIXmlInit.h"
#include "UIPdaMsgListItem.h"
#include "../alife_registry_wrappers.h"
#include "../actorcondition.h"

#include "../string_table.h"
#include "../clsid_game.h"
#include "UIArtefactPanel.h"
//////////////////////////////////////////////////////////////////////////

using namespace InventoryUtilities;

//	hud adjust mode
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
const char * const MAININGAME_XML				= "maingame.xml";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction//////////////////////////////////////////////////////////////////////

CUIMainIngameWnd::CUIMainIngameWnd()
{
	m_pActor					= NULL;
	m_pWeapon					= NULL;
	m_pItem						= NULL;

	m_bShowHudInfo				= true;
	m_bShowHudCrosshair			= false;
	// Quick infos
	fuzzyShowInfo				= 0.f;
	UIZoneMap					= xr_new<CUIZoneMap>();
	m_pPickUpItem				= NULL;
	m_artefactPanel				= xr_new<CUIArtefactPanel>();
}

//////////////////////////////////////////////////////////////////////////

CUIMainIngameWnd::~CUIMainIngameWnd()
{
	DestroyFlashingIcons();
	xr_delete(UIZoneMap);
	xr_delete(m_artefactPanel);
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::Init()
{
	CUIXml uiXml;
	uiXml.Init(CONFIG_PATH, UI_PATH, MAININGAME_XML);
	
	CUIXmlInit xml_init;
	CUIWindow::Init(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	//� ��� ���� ����������������� ����� �������������� �� �����
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
	//---------------------------------------------------------
	AttachChild(&UIPickUpItemIcon);
	xml_init.InitStatic(uiXml, "pick_up_item", 0, &UIPickUpItemIcon);
	UIPickUpItemIcon.SetShader(GetEquipmentIconsShader());
	UIPickUpItemIcon.ClipperOn();

	m_iPickUpItemIconWidth = UIPickUpItemIcon.GetWidth();
	m_iPickUpItemIconHeight = UIPickUpItemIcon.GetHeight();
	m_iPickUpItemIconX = UIPickUpItemIcon.GetWndRect().left;
	m_iPickUpItemIconY = UIPickUpItemIcon.GetWndRect().top;
	//---------------------------------------------------------

	//��������� ������������ ������ ��� ������ ������, 
	//��� ��� ��� ����� ���������������� � ��������������
	m_iWeaponIconWidth = UIWeaponIcon.GetWidth();
	m_iWeaponIconHeight = UIWeaponIcon.GetHeight();
	m_iWeaponIconX = UIWeaponIcon.GetWndRect().left;
	m_iWeaponIconY = UIWeaponIcon.GetWndRect().top;

	UIWeaponIcon.Enable(false);

	//
	AttachChild(&UIPdaOnline);
	xml_init.InitStatic(uiXml, "static", 4, &UIPdaOnline);
	
	// � ��� ��������� ������������ ����� ��� SP, � MP modes
//	CUIXml uiXml2;
//	if (GameID() != GAME_SINGLE)
//		uiXml2.Init(CONFIG_PATH, UI_PATH, PDA_INGAME_MULTIPLAYER_CFG);
//	else
//		uiXml2.Init(CONFIG_PATH, UI_PATH, PDA_INGAME_SINGLEPLAYER_CFG);

	AttachChild(&UIPdaMsgListWnd);

	if (GameID() == GAME_SINGLE)
        xml_init.InitListWnd(uiXml, "pda_msg_list_sp", 0, &UIPdaMsgListWnd);
	else
		xml_init.InitListWnd(uiXml, "pda_msg_list_mp", 0, &UIPdaMsgListWnd);

	AttachChild(&UIPdaMsgListWnd2);
	xml_init.InitListWnd(uiXml, "pda_msg_list_sp2", 0, &UIPdaMsgListWnd2);
//	m_iPdaMessagesFade_mSec = uiXml2.ReadAttribInt("list", 0, "fade", 0);

	UIPdaMsgListWnd.SetVertFlip(true);
	UIPdaMsgListWnd.EnableScrollBar(false);
	UIPdaMsgListWnd2.SetVertFlip(true);
	UIPdaMsgListWnd2.EnableScrollBar(false);

	// ��� �������������� ���������
	AttachChild(&UIInfoMessages);
	xml_init.InitListWnd(uiXml, "info_list", 0, &UIInfoMessages);
//	m_iInfoMessagesFade_mSec = uiXml.ReadAttribInt("info_list", 0, "fade", 0);
	UIInfoMessages.SetVertFlip(true);

		
//	AttachChild(&UITextWound);
//	UITextWound.Init(UIStaticWound.GetWndRect().left+12, 
//						UIStaticWound.GetWndRect().top+40,
//						30,30);

	//������ ��������� ��������
	UIStaticHealth.AttachChild(&UIHealthBar);
	xml_init.InitProgressBar(uiXml, "progress_bar", 0, &UIHealthBar);

	//������ ��������� ������
	UIStaticArmor.AttachChild(&UIArmorBar);
	xml_init.InitProgressBar(uiXml, "progress_bar", 1, &UIArmorBar);

	//���������� 
	UIZoneMap->Init();
	UIZoneMap->SetScale(DEFAULT_MAP_SCALE);

	//��� ����������� �������� ��������� PDA
	UIPdaMsgListWnd.Show(true);
	UIPdaMsgListWnd.Enable(false);
	UIPdaMsgListWnd2.Show(true);
	UIPdaMsgListWnd2.Enable(false);

//	m_dwMaxShowTime		= pSettings->r_s32("maingame_ui", "pda_msgs_max_show_time");
//	m_iInfosShowTime	= pSettings->r_s32("maingame_ui", "info_msgs_max_show_time");

	// ���������, ������� ��������� ��� ��������� ������� �� ������
	AttachChild(&UIStaticQuickHelp);
	xml_init.InitStatic(uiXml, "quick_info", 0, &UIStaticQuickHelp);
	UIStaticQuickHelp.SetTextColor(0xffffffff);
	UIStaticQuickHelp.SetTextAlign(CGameFont::alCenter);

	uiXml.SetLocalRoot(uiXml.GetRoot());

	// ��������� ������ 
	AttachChild(&UIWeaponJammedIcon);
	xml_init.InitStatic(uiXml, "weapon_jammed_static", 0, &UIWeaponJammedIcon);
	UIWeaponJammedIcon.GetStaticItem()->SetScaleXY(0.75f,0.75f);
	UIWeaponJammedIcon.ClipperOn();

	AttachChild(&UIRadiaitionIcon);
	xml_init.InitStatic(uiXml, "radiation_static", 0, &UIRadiaitionIcon);
	UIRadiaitionIcon.GetStaticItem()->SetScaleXY(0.75f,0.75f);

	AttachChild(&UIWoundIcon);
	xml_init.InitStatic(uiXml, "wound_static", 0, &UIWoundIcon);
	UIWoundIcon.GetStaticItem()->SetScaleXY(0.75f,0.75f);

	AttachChild(&UIStarvationIcon);
	xml_init.InitStatic(uiXml, "starvation_static", 0, &UIStarvationIcon);
	UIStarvationIcon.GetStaticItem()->SetScaleXY(0.75f,0.75f);

	AttachChild(&UIFatigueIcon);
	xml_init.InitStatic(uiXml, "fatigue_static", 0, &UIFatigueIcon);
	UIFatigueIcon.GetStaticItem()->SetScaleXY(0.75f,0.75f);

	AttachChild(&UIInvincibleIcon);
	xml_init.InitStatic(uiXml, "invincible_static", 0, &UIInvincibleIcon);
//	UIInvincibleIcon.GetStaticItem()->SetScaleXY(0.75f,0.75f);

	shared_str warningStrings[6] = 
	{	
		"jammed",
		"radiation",
		"wounds",
		"starvation",
		"fatigue",
		"invincible"
	};

	// ��������� ��������� �������� ��� �����������
	EWarningIcons j = ewiWeaponJammed;
	while (j <= ewiInvincible)
	{
		// ������ ������ ������� ��� ������� ����������
		shared_str cfgRecord = pSettings->r_string("main_ingame_indicators_thresholds", *warningStrings[static_cast<int>(j) - 1]);
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

	shared_str animationName				= "ui_pda_contacts";
	UIContactsAnimation.SetColorAnimation	(animationName);
	UIContactsAnimation.SetColorToModify	(&UIPdaOnline.GetColorRef());
	UIContactsAnimation.Cyclic				(false);
	UIContactsAnimation.Reset				();

	// Claws animation
	uiXml.SetLocalRoot(uiXml.GetRoot());
	animationName							= "ui_claws_animation";
	m_ClawsTexture.SetRect					(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);
	m_ClawsAnimation.SetColorAnimation		(animationName);
	m_ClawsAnimation.SetColorToModify		(&m_ClawsTexture.GetColorRef());
	m_ClawsAnimation.Cyclic					(false);
	m_ClawsAnimation.Reset					();

	AttachChild(&UIStaticBattery);
	xml_init.InitStatic(uiXml, "battery_static", 0, &UIStaticBattery);
	//������ ��������� ������
	UIStaticBattery.AttachChild(&UIBatteryBar);
	xml_init.InitProgressBar(uiXml, "progress_bar", 2, &UIBatteryBar);
	ShowBattery(false);
	SetBatteryCharge(1.0f);
	
	AttachChild(&UICarPanel);
	xml_init.InitWindow(uiXml, "car_panel", 0, &UICarPanel);

	AttachChild(&UIMotionIcon);
	xml_init.InitWindow(uiXml, "motion_icon", 0, &UIMotionIcon);
//	UICarPanel.Show(true);

	xml_init.InitArtefactPanel(uiXml, "artefact_panel", 0, m_artefactPanel);
	this->AttachChild(m_artefactPanel);	
	xr_vector<Irect> vRects;

/*	// uncoment to test ArtefactPanel
	Irect rect;
	rect.left = 698; rect.right = 750;
	rect.top = 0;    rect.bottom = 48;

	vRects.push_back(rect);
	rect.left = 749; rect.right = 801;
	vRects.push_back(rect);

	m_artefactPanel.InitIcons(vRects);	*/
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::Draw()
{
	bool zoom_mode = false;
	bool scope_mode = false;

	for(CUSTOM_TEXTURE_IT it = m_CustomTextures.begin(); m_CustomTextures.end() != it; it++)
	{
		CUSTOM_TEXTURE& custom_texture = *it;
#pragma todo("ALEXMX -> ANDY")
		custom_texture.static_item->SetPos	(custom_texture.x1, custom_texture.y1);
		custom_texture.static_item->SetRect	(0,0,custom_texture.x2-custom_texture.x1, custom_texture.y2-custom_texture.y1);
		custom_texture.static_item->Render	();
//.		custom_texture.static_item->Render(custom_texture.x1, custom_texture.y1, custom_texture.x2, custom_texture.y2);
	}
	m_CustomTextures.clear();

	//����������� �������� ��������� ������������ ������� ��� �������
	if(m_pActor->HUDview() && m_pWeapon && m_pWeapon->IsZoomed() && m_pWeapon->ZoomHideCrosshair()){
		zoom_mode = true;
		if(m_pWeapon->ZoomTexture() && !m_pWeapon->IsRotatingToZoom()){
			m_pWeapon->ZoomTexture()->SetPos	(0,0);
			m_pWeapon->ZoomTexture()->SetRect	(0,0,UI_BASE_WIDTH, UI_BASE_HEIGHT);
			m_pWeapon->ZoomTexture()->Render	();
#pragma todo("ALEXMX -> ANDY")
//.			m_pWeapon->ZoomTexture()->Render(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);
			scope_mode = true;
		}

		if(psHUD_Flags.is(HUD_CROSSHAIR|HUD_CROSSHAIR_RT)){
			psHUD_Flags.set(HUD_CROSSHAIR_RT, FALSE);
			m_bShowHudCrosshair = true;
		}

		zoom_mode = true;
	}
	
	if(!scope_mode)
	{
		if(m_bShowHudInfo)
		{
			//���������� ��������� ������
			UIPdaMsgListWnd2.Show(false);
			CUIWindow::Draw();
			UIPdaMsgListWnd2.Show(true);

			UIZoneMap->Render();			
		}
		if (m_bShowHudCrosshair && !zoom_mode)
		{
			m_bShowHudCrosshair = false;
			psHUD_Flags.set(HUD_CROSSHAIR_RT, TRUE);
		}
		RenderQuickInfos();
	}

	// Render claws
	if (!m_ClawsAnimation.Done() && m_ClawsTexture.GetShader())
	{
		m_ClawsTexture.SetPos	(0, 0);
		//m_ClawsTexture.Render	(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);
		//m_ClawsTexture.SetRect	(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

		m_ClawsTexture.Render	(PI_DIV_3);
		m_ClawsTexture.SetScaleXY	(0.5f, 0.5f);
	}

	UIPdaMsgListWnd.Draw();

#ifdef DEBUG
	if (g_bHudAdjustMode&&m_pWeapon) //draw firePoint,ShellPoint etc
	{
		CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(!pActor)
			return;

		bool bCamFirstEye = pActor->cam_Active()==pActor->cam_FirstEye();
//		string32 hud_view="HUD view";
//		string32 _3rd_person_view="3-rd person view";
		CGameFont* F		= UI()->Font()->pFontDI;
		F->SetAligment		(CGameFont::alCenter);
		F->SetSizeI			(0.02f);
		F->OutSetI			(0.f,-0.8f);
		F->SetColor			(0xffffffff);
		F->OutNext			("Hud_adjust_mode=%d",g_bHudAdjustMode);
		if(g_bHudAdjustMode==1)
			F->OutNext			("adjusting zoom offset");// for %s",bCamFirstEye?hud_view:_3rd_person_view);
		else if(g_bHudAdjustMode==2)
			F->OutNext			("adjusting fire point");// for %s",bCamFirstEye?hud_view:_3rd_person_view);
		else if(g_bHudAdjustMode==3)
			F->OutNext			("adjusting missile offset");// for %s",bCamFirstEye?hud_view:_3rd_person_view);
		else if(g_bHudAdjustMode==4)
			F->OutNext			("adjusting shell point");// for %s",bCamFirstEye?hud_view:_3rd_person_view);

		if(bCamFirstEye)
		{
			CWeaponHUD *pWpnHud = NULL;
			pWpnHud = m_pWeapon->GetHUD();

			Fvector FP,SP;//,FP2;

			CKinematics* V			= smart_cast<CKinematics*>(pWpnHud->Visual());
			V->CalculateBones		();

			// fire point&direction
			Fmatrix& fire_mat		= V->LL_GetTransform(u16(pWpnHud->FireBone()));
			Fmatrix& parent			= pWpnHud->Transform	();

			Fvector& fp				= pWpnHud->FirePoint();
			Fvector& sp				= pWpnHud->ShellPoint();

			fire_mat.transform_tiny	(FP,fp);
			parent.transform_tiny	(FP);

			fire_mat.transform_tiny	(SP,sp);
			parent.transform_tiny	(SP);


			RCache.dbg_DrawAABB(FP,0.01f,0.01f,0.01f,D3DCOLOR_XRGB(255,0,0));
//			RCache.dbg_DrawAABB(FP2,0.02f,0.02f,0.02f,D3DCOLOR_XRGB(255,0,0));
			RCache.dbg_DrawAABB(SP,0.01f,0.01f,0.01f,D3DCOLOR_XRGB(0,255,0));
		
		}else{
			Fvector FP = m_pWeapon->get_CurrentFirePoint();
			Fvector SP = m_pWeapon->get_LastSP();
			RCache.dbg_DrawAABB(FP,0.02f,0.02f,0.02f,D3DCOLOR_XRGB(255,0,0));
			RCache.dbg_DrawAABB(SP,0.02f,0.02f,0.02f,D3DCOLOR_XRGB(0,255,0));
		}
	}
#endif
}

void CUIMainIngameWnd::DrawPdaMessages(){
	FadeUpdate(&UIPdaMsgListWnd);	
	FadeUpdate(&UIPdaMsgListWnd2);	
	UIPdaMsgListWnd2.Draw();
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::Update()
{
	if(!m_bShowHudInfo) return;

	static string256 text_str;

	m_pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if (!m_pActor) 
	{
		m_pItem = NULL;
		m_pWeapon = NULL;
		CUIWindow::Update();
		return;
	}

	if (GameID() == GAME_SINGLE)
	{
		if(m_pActor->GetPDA() && m_pActor->GetPDA()->ActiveContactsNum()>0)
		{
			sprintf(text_str, "%d", m_pActor->GetPDA()->ActiveContactsNum());
			UIPdaOnline.SetText(text_str);
		}
		else
		{
			UIPdaOnline.SetText("");
		}
	};

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
		PIItem item =  m_pActor->inventory().m_slots[m_pActor->inventory().GetActiveSlot()].m_pIItem;
		CWeapon* pWeapon = smart_cast<CWeapon*>(item); 
		CMissile* pMissile = smart_cast<CMissile*>(item); 
		CWeaponMagazined* pWeaponMagazined = smart_cast<CWeaponMagazined*>(pWeapon);
		
		bool active_item_changed = false;
		if(item && m_pItem != item)
		{
			m_pItem = item;
			active_item_changed = true;
		}

		if(pMissile && active_item_changed)
		{
			UIWeaponIcon.Show(false);
			UIWeaponSignAmmo.Show(false);

			UIWeaponBack.SetText(m_pItem->NameShort());
			UIWeaponBack.SetTextX(static_cast<int>((UIWeaponBack.GetAbsoluteRect().right - 
				UIWeaponBack.GetAbsoluteRect().left) * 0.5 - 
				UIWeaponIcon.GetFont()->SizeOf(m_pItem->NameShort()) / 2));

		}
		else if(pWeapon)
		{
			// Remember last used ammo types, and if this type doesn't changed 
			// then no need to update info
			static u32			prevAmmoID		= static_cast<u32>(-1);

			if(active_item_changed || !m_pWeapon || prevAmmoID != m_pWeapon->m_ammoType)
			{
				m_pWeapon = pWeapon;

				if(m_pWeapon->ShowAmmo())
				{
					//if (xr_strcmp(m_pWeapon->NameShort(), "knife") != 0)
					if (pWeaponMagazined)
					{
						UIWeaponIcon.Show(true);
					}
					else
					{
						UIWeaponIcon.Show(false);
					}

					UIWeaponSignAmmo.Show(true);

					prevAmmoID			= m_pWeapon->m_ammoType;
					shared_str sect_name	= m_pWeapon->m_ammoTypes[m_pWeapon->m_ammoType];

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
					UIWeaponIcon.SetStretchTexture(true);

					// now perform only width scale for ammo, which (W)size >2
					// all others ammo (1x1, 1x2) will be not scaled (original picture)
					int w = iFloor(0.5f+ ((m_iGridWidth>2)?1.6f:m_iGridWidth)*INV_GRID_WIDTH*0.9f);
					int h = iFloor(0.5f+INV_GRID_HEIGHT*0.9f);//1 cell
					UIWeaponIcon.SetWidth(w);
					UIWeaponIcon.SetHeight(h);
				}
				else
				{
					UIWeaponIcon.Show(false);
					UIWeaponSignAmmo.Show(false);
				}
			}


			UIWeaponBack.SetText(m_pItem->NameShort());
			UIWeaponBack.SetTextX(static_cast<int>((UIWeaponBack.GetAbsoluteRect().right - 
				UIWeaponBack.GetAbsoluteRect().left) * 0.5 - 
				UIWeaponIcon.GetFont()->SizeOf(m_pItem->NameShort()) / 2));


			int	AE = m_pWeapon->GetAmmoElapsed();
			int	AC = m_pWeapon->GetAmmoCurrent();
			if((AE>=0)&&(AC>=0))
			{
				if (!m_pWeapon->unlimited_ammo())
					sprintf(text_str, "%d/%d",AE,AC - AE);
				else
					sprintf(text_str, "%d/--",AE);
				UIWeaponSignAmmo.SetText(text_str);
			}
		}
	} 
	else
	{
		m_pWeapon = NULL;
		UIWeaponBack.SetText("");
	}


	//�������� ����������
	if(!m_pWeapon)
	{
		UIWeaponSignAmmo.SetText("");
		UIWeaponIcon.Show(false);
	}
    

	if(m_pActor->HUDview() && m_pWeapon && m_pWeapon->IsZoomed() && m_pWeapon->ZoomTexture()) return;


    // radar
	UIZoneMap->UpdateRadar(m_pActor);
	// viewport
	float h,p;
	Device.vCameraDirection.getHP	(h,p);
	UIZoneMap->SetHeading			(-h);
		
	// health&armor
	//	UIHealth.Out(m_Actor->g_Health(),m_Actor->g_Armor());
	UIHealthBar.SetProgressPos((s16)m_pActor->g_Health());
	UIMotionIcon.SetProgressPos(s16(m_pActor->conditions().GetPower()*100));
	
	
	EWarningIcons i = ewiWeaponJammed;
		
	while (i <= ewiInvincible)
	{
		float value = 0;
		switch (i)
		{
		//radiation
		case ewiRadiation:
			value = m_pActor->conditions().GetRadiation();
			break;
		case ewiWound:
			value = m_pActor->conditions().BleedingSpeed();
			break;
		case ewiWeaponJammed:
			if (m_pWeapon)
				value = 1 - m_pWeapon->GetCondition();
			break;
		case ewiStarvation:
			value = 1 - m_pActor->conditions().GetSatiety();
			break;
		case ewiFatigue:
			value = 1 - m_pActor->conditions().GetPower();
			break;
		case ewiInvincible:
			value = (GodMode()||(!Game().local_player)) ? 1.0f : float(Game().local_player->testFlag(GAME_PLAYER_FLAG_INVINCIBLE));
			break;
		default:
			R_ASSERT(!"Unknown type of warning icon");
		}

		xr_vector<float>::reverse_iterator	rit;

		// ������� ��������� �� ������ �����������
		rit  = std::find(m_Thresholds[i].rbegin(),
						 m_Thresholds[i].rend(),
						 value);

		// ���� ��� ���, �� ����� ��������� ������� �������� ()
		if (rit == m_Thresholds[i].rend())
		{
			rit = std::find_if(m_Thresholds[i].rbegin(),
						m_Thresholds[i].rend(),
						std::bind2nd(std::less<float>(), value));
		}

		// ����������� � ������������ �������� �������
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
	FadeUpdate(&UIPdaMsgListWnd);//, m_iPdaMessagesFade_mSec);
	FadeUpdate(&UIPdaMsgListWnd2);//, m_iPdaMessagesFade_mSec);
	FadeUpdate(&UIInfoMessages);//, m_iInfoMessagesFade_mSec);

	UpdateFlashingIcons();
	UIContactsAnimation.Update();
	UIPdaOnline.SetTextColor(subst_alpha(UIPdaOnline.GetTextColor(), color_get_A(UIContactsAnimation.GetColor())));
	m_ClawsAnimation.Update();

	UpdatePickUpItem();

	CUIWindow::Update();
}

bool CUIMainIngameWnd::OnKeyboardPress(int dik)
{
	// ��������� ������ adjust hud mode
	if (g_bHudAdjustMode)
	{
//		bool bCamFirstEye = pActor->cam_Active()==pActor->cam_FirstEye();

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

		if (1 == g_bHudAdjustMode) //zoom offset
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
		else if (2 == g_bHudAdjustMode) //firePoint
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
		else if (4 == g_bHudAdjustMode) //ShellPoint
		{
			tmpV = pWpnHud->ShellPoint();

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

				sprintf(tmpStr, "shell_point\t\t\t= %f,%f,%f",
					pWpnHud->ShellPoint().x,
					pWpnHud->ShellPoint().y,
					pWpnHud->ShellPoint().z);
				Log(tmpStr);
				flag = true;
				break;
			}
		
			pWpnHud->SetShellPoint(tmpV);
		}
		else if (3 == g_bHudAdjustMode) //MissileOffset
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
			UIZoneMap->ZoomOut();
			return true;
			break;
		case DIK_NUMPADPLUS:
			UIZoneMap->ZoomIn();
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


void CUIMainIngameWnd::ReceivePdaMessage(CInventoryOwner* pSender, EPdaMsg msg, INFO_ID info_id)
{
	R_ASSERT(pSender);

	CUIPdaMsgListItem* pItem = NULL;
	CUIPdaMsgListItem* pItem2 = NULL;

	pItem = xr_new<CUIPdaMsgListItem>();
	pItem2 = xr_new<CUIPdaMsgListItem>();

	UIPdaMsgListWnd.AddItem<CUIListItem>(pItem, 0);	/*---*/ UIPdaMsgListWnd2.AddItem<CUIListItem>(pItem2, 0); 
	UIPdaMsgListWnd.ScrollToBegin();				/*---*/ UIPdaMsgListWnd2.ScrollToBegin();

	pItem->InitCharacter(smart_cast<CInventoryOwner*>(pSender));
	pItem2->InitCharacter(smart_cast<CInventoryOwner*>(pSender));

	CUIColorAnimatorWrapper *p = xr_new<CUIColorAnimatorWrapper>("ui_main_msgs");
	CUIColorAnimatorWrapper *p2 = xr_new<CUIColorAnimatorWrapper>("ui_main_msgs");

	R_ASSERT(p);
	R_ASSERT(p2);

	p->Cyclic(false);
	p2->Cyclic(false);

//	p->SetColorToModify(&pItem->UIMsgText.GetColorRef());
	pItem->SetData(p);
	pItem2->SetData(p2);


	UIPdaMsgListWnd.Show(true);	/*---*/ UIPdaMsgListWnd2.Show(true);

	if(msg == ePdaMsgInfo)
	{
		CInfoPortion info_portion;
		info_portion.Load(info_id);
		pItem->UIMsgText.SetText(*CStringTable()(info_portion.GetText()));
		pItem2->UIMsgText.SetText(*CStringTable()(info_portion.GetText()));
	}
	else
	{
	//	pItem->UIMsgText.SetText(CPda::m_PdaMsgStr[msg]);
	}
}

//////////////////////////////////////////////////////////////////////////

bool CUIMainIngameWnd::SetDelayForPdaMessage(int iValue, int iDelay){
	int index = UIPdaMsgListWnd.FindItemWithValue(iValue);

	if (index >= 0)
	{
        CUIPdaMsgListItem* item = smart_cast<CUIPdaMsgListItem*>(UIPdaMsgListWnd.GetItem(index));
        item->SetDelay(iDelay);

		index = UIPdaMsgListWnd2.FindItemWithValue(iValue);

#ifdef DEBUG
		R_ASSERT2(index >= 0, "Item exist only in first list");
#endif
		item = smart_cast<CUIPdaMsgListItem*>(UIPdaMsgListWnd2.GetItem(index));
        item->SetDelay(iDelay);

		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

CUIPdaMsgListItem* CUIMainIngameWnd::AddMessageToList(LPCSTR message, CUIListWnd* pListWnd, int iId, int iDelay){
	CUIPdaMsgListItem* pItem = NULL;
	pItem = xr_new<CUIPdaMsgListItem>(iDelay);
	pListWnd->AddItem<CUIListItem>(pItem, 0); 
	pListWnd->ScrollToBegin();
	// create animation
	CUIColorAnimatorWrapper *p = xr_new<CUIColorAnimatorWrapper>("ui_main_msgs");
	R_ASSERT(p);
	// no loop animation
	p->Cyclic(false);
	// set animation wrapper as Item Data
	pItem->SetData(p);
	// set id to item. With this we can find item in list and for some manipulation
	pItem->SetValue(iId);
	pItem->UIMsgText.SetWndPos(0, 0);	
	pItem->UIMsgText.SetText(message);	

	return pItem;
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::AddGameMessage(LPCSTR message, int iId, int iDelay)
{
	AddMessageToList(message, &UIPdaMsgListWnd, iId, iDelay);
	AddMessageToList(message, &UIPdaMsgListWnd2, iId, iDelay);
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::AddPersonalizedGameMessage(CInventoryOwner* pSender, LPCSTR message, int iId, int iDelay)
{
	CUIPdaMsgListItem *pItem = AddMessageToList(message, &UIPdaMsgListWnd, iId, iDelay);
	if (pItem)	
		pItem->InitCharacter(pSender);

	pItem = AddMessageToList(message, &UIPdaMsgListWnd2, iId, iDelay);
	if (pItem)	
		pItem->InitCharacter(pSender);	
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::AddIconedGameMessage(LPCSTR textureName, Irect originalRect, LPCSTR message, int iId, int iDelay)
{
	CUIPdaMsgListItem* pItem = AddMessageToList(message, &UIPdaMsgListWnd, iId, iDelay);

	if (pItem)
	{
		pItem->UIIcon.InitTexture(textureName);
		pItem->UIIcon.SetOriginalRect(originalRect.left, originalRect.top, originalRect.right, originalRect.bottom);
		pItem->UIMsgText.SetWndPos(originalRect.right, 0/*originalRect.bottom*/);
	}

	pItem = AddMessageToList(message, &UIPdaMsgListWnd2, iId, iDelay);

	if (pItem)
	{
		pItem->UIIcon.InitTexture(textureName);
		pItem->UIIcon.SetOriginalRect(originalRect.left, originalRect.top, originalRect.right, originalRect.bottom);
		pItem->UIMsgText.SetWndPos(originalRect.right, 0/*originalRect.bottom*/);
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
	pItem->UIMsgText.SetWndPos(-static_cast<int>(UIInfoMessages.GetFont()->SizeOf(message) / 2),
								pItem->UIMsgText.GetWndRect().top);
	UIInfoMessages.ScrollToBegin();

	CUIColorAnimatorWrapper *p = xr_new<CUIColorAnimatorWrapper>("ui_main_msgs");
	R_ASSERT(p);
	p->Cyclic(false);
//	p->SetColorToModify(&pItem->UIMsgText.GetColorRef());
	pItem->SetData(p);

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

void CUIMainIngameWnd::OnNewsReceived(GAME_NEWS_DATA &news)
{
	if (g_bNewsDisable) return;

#ifdef DEBUG
	Msg("[news]%s", news.FullText());
#endif
		
	if(news.texture_name)
	{
		Irect rect;
		rect.left = news.x1;
		rect.right = news.x2;
		rect.top = news.y1;
		rect.bottom = news.y2;
		AddIconedGameMessage(news.texture_name, rect, news.FullText(), news.news_text, news.show_time);
	}
	else
		AddGameMessage(news.FullText(), news.news_text, news.show_time);
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::SetWarningIconColor(EWarningIcons icon, const u32 cl)
{
	bool bMagicFlag = true;

	// ������ ���� ��������� ������
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
	case ewiInvincible:
		UIInvincibleIcon.SetColor(cl);
		break;
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

void CUIMainIngameWnd::ChangeTotalMoneyIndicator(shared_str newMoneyString)
{
	CUIMultiTextStatic::SinglePhrase * sp = UIMoneyIndicator.GetPhraseByIndex(0);
	sp->str = newMoneyString;
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::DisplayMoneyChange(shared_str deltaMoney)
{
	CUIMultiTextStatic::SinglePhrase * sp = UIMoneyIndicator.GetPhraseByIndex(1);
	sp->str				= deltaMoney;
	sp->effect.PlayAnimation();
	EffectParams *eff	= sp->effect.SetStyleParams(CUITextBanner::tbsFade);
	eff->bOn			= true;
	sp->effect.ResetAnimation(CUITextBanner::tbsFade);
}

//-----------------------------------------------------------------------------/
// Local compare functor 
//-----------------------------------------------------------------------------/

struct priority_greater : public std::binary_function<CUSTOM_TEXTURE, CUSTOM_TEXTURE, bool>
{	// functor for operator>
	bool operator()(const CUSTOM_TEXTURE& Left, const CUSTOM_TEXTURE& Right) const
	{	// apply operator> to operands
		return (Left.texPriority > Right.texPriority);
	}
} priority_greater_functor;

//////////////////////////////////////////////////////////////////////////

void  CUIMainIngameWnd::AddStaticItem (CUIStaticItem* si, int left, int top, int right, int bottom, int priority)
{
	m_CustomTextures.push_back(CUSTOM_TEXTURE(si, left, top, right, bottom, priority));

	std::sort(m_CustomTextures.begin(), m_CustomTextures.end(), priority_greater_functor);
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::FadeUpdate(CUIListWnd *pWnd)
{

	for(int i=0; i<pWnd->GetSize(); i++)
	{
		CUIListItem			*pItem	= pWnd->GetItem(i);
		CUIPdaMsgListItem	*pPItem = smart_cast<CUIPdaMsgListItem*>(pItem);

		if (! pPItem->IsTimeToDestroy() )
			return;

		CUIColorAnimatorWrapper *p = reinterpret_cast<CUIColorAnimatorWrapper*>(pItem->GetData());
		if (p)
		{
			p->Update();
			if (pPItem)
			{
				pPItem->UIMsgText.SetTextColor(subst_alpha(pPItem->UIMsgText.GetTextColor(), color_get_A(p->GetColor())));
			}
			else
			{
				pItem->SetTextColor(subst_alpha(pItem->GetTextColor(), color_get_A(p->GetColor())));
			}

			if (p->Done())
			{
				xr_delete(p);
				pWnd->RemoveItem(i);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::SetFlashIconState(EFlashingIcons type, bool enable)
{
	// �������� �������� ��������� ������
	FlashingIcons_it icon = m_FlashingIcons.find(type);
	R_ASSERT2(icon != m_FlashingIcons.end(), "Flashing icon with this type not existed");

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
	// ����������� �� ���� ����� � �������������� �� ��� �������
	for (int i = 0; i < staticsCount; ++i)
	{
		pIcon = xr_new<CUIStatic>();
		xml_init.InitStatic(node, flashingIconNodeName, i, pIcon);
		shared_str iconType = node.ReadAttrib(flashingIconNodeName, i, "type", "none");

		// ������ ���������� ������ � �� ���
		EFlashingIcons type = efiPdaTask;

		if		(iconType == "pda")		type = efiPdaTask;
		else if (iconType == "mail")	type = efiMail;
		else	R_ASSERT(!"Unknown type of mainingame flashing icon");

		R_ASSERT2(m_FlashingIcons.find(type) == m_FlashingIcons.end(), "Flashing icon with this type already exists");

		IconInfo &val	= m_FlashingIcons[type];
		val.first		= pIcon;

		const shared_str colorAnimationName	= "ui_new_jobs";
		val.second.SetColorAnimation(colorAnimationName);
		val.second.SetColorToModify(&pIcon->GetColorRef());
		AttachChild(pIcon);
		pIcon->Show(false);

		// Some type related hacks
		switch (type)
		{
		case efiPdaTask:
			pIcon->GetUIStaticItem().SetScaleXY(0.5f, 0.5f);
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
		it->second.second.Update();
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::AnimateContacts()
{
	UIContactsAnimation.Reset();
}

//////////////////////////////////////////////////////////////////////////

//void CUIMainIngameWnd::UpdateContactsAnimation()
//{
//	if (UIContactsFlicker.GetState() == CUIAnimationBase::easPlayed)
//	{
//		UIPdaOnline.Show(!!UIContactsFlicker.GetCurrentPhase());
//	}
//	else
//		UIPdaOnline.Show(true);
//}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::AddMonsterClawsEffect(const shared_str &monsterName, const shared_str &textureName)
{
	if (m_ClawsTextures.find(monsterName) != m_ClawsTextures.end()) return;

	// Check for texture existance
	if (m_ClawsRepos.find(textureName) == m_ClawsRepos.end())
	{
		m_ClawsRepos[textureName].create("hud\\default", *textureName);
	}

	ref_shader *sh = &m_ClawsRepos[textureName];
	// Add new texture
	m_ClawsTextures[monsterName] = sh;
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::PlayClawsAnimation(const shared_str &monsterName)
{
	MonsterClawsTextures_it it = m_ClawsTextures.find(monsterName);
	R_ASSERT2(it != m_ClawsTextures.end(), "Monster claws texture for this monster doesn't exist");
	
	m_ClawsTexture.SetShader	(*it->second);
	m_ClawsAnimation.Reset		();

//	m_ClawsAnimation.SetAnimationDirection(CUIAnimationFade::efdFadeOut);
//	m_ClawsAnimation.Play();
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::ShowBattery(bool on)
{
	UIStaticBattery.Show(on);
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::SetBatteryCharge(float value)
{
	s16 pos = static_cast<s16>(value * 100);
	clamp<s16>(pos, 0, 100);
	UIBatteryBar.SetProgressPos(pos);
}
////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::SetPickUpItem	(CInventoryItem* PickUpItem)
{
	if (PickUpItem)
		m_pPickUpItem = PickUpItem;
	else
		m_pPickUpItem = PickUpItem;
};
void CUIMainIngameWnd::UpdatePickUpItem	()
{
	if (!m_pPickUpItem || !Level().CurrentViewEntity() || Level().CurrentViewEntity()->CLS_ID != CLSID_OBJECT_ACTOR) 
	{
		UIPickUpItemIcon.Show(false);
		return;
	};

	shared_str sect_name	= m_pPickUpItem->object().cNameSect();

	//properties used by inventory menu
	int m_iGridWidth	= pSettings->r_u32(sect_name, "inv_grid_width");
	int m_iGridHeight	= pSettings->r_u32(sect_name, "inv_grid_height");

	int m_iXPos			= pSettings->r_u32(sect_name, "inv_grid_x");
	int m_iYPos			= pSettings->r_u32(sect_name, "inv_grid_y");

	float scale_x = float(m_iPickUpItemIconWidth)/
		float(m_iGridWidth*INV_GRID_WIDTH);

	float scale_y = float(m_iPickUpItemIconHeight)/
		float(m_iGridHeight*INV_GRID_HEIGHT);

	scale_x = (scale_x>1) ? 1.0f : scale_x;
	scale_y = (scale_y>1) ? 1.0f : scale_y;

	float scale = scale_x<scale_y?scale_x:scale_y;

	UIPickUpItemIcon.GetUIStaticItem().SetOriginalRect(
		m_iXPos * INV_GRID_WIDTH,
		m_iYPos * INV_GRID_HEIGHT,
		m_iGridWidth * INV_GRID_WIDTH,
		m_iGridHeight * INV_GRID_HEIGHT);

//	UIPickUpItemIcon.SetTextureScaleXY(scale, scale);
	UIPickUpItemIcon.SetStretchTexture(true);

	UIPickUpItemIcon.SetWidth(iFloor(0.5f+ m_iGridWidth*INV_GRID_WIDTH*scale));
	UIPickUpItemIcon.SetHeight(iFloor(0.5f+ m_iGridHeight*INV_GRID_HEIGHT*scale));

	UIPickUpItemIcon.SetWndPos(m_iPickUpItemIconX + 
		(m_iPickUpItemIconWidth - UIPickUpItemIcon.GetWidth())/2,
		m_iPickUpItemIconY + 
		(m_iPickUpItemIconHeight - UIPickUpItemIcon.GetHeight())/2);

	UIPickUpItemIcon.SetColor(color_rgba(255,255,255,192));
	UIPickUpItemIcon.Show(true);
};