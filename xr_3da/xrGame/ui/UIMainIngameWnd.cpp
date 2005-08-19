// UIMainIngameWnd.cpp:  окошки-информация в игре
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "UIMainIngameWnd.h"
#include "UIMessagesWindow.h"
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
#include "UIMoneyIndicator.h"


#include "UIXmlInit.h"
#include "UIPdaMsgListItem.h"
#include "../alife_registry_wrappers.h"
#include "../actorcondition.h"

#include "../string_table.h"
#include "../clsid_game.h"
#include "UIArtefactPanel.h"
#ifdef DEBUG
#include "../attachable_item.h"
#include "../../xr_input.h"
#endif
//////////////////////////////////////////////////////////////////////////

#include "UIDragDropListEx.h"
#include "map_hint.h"
static CTestDragDropWnd* w = NULL;
CUIMapHint* w_ = NULL;

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
	m_pMPChatWnd				= NULL;
	m_pMPLogWnd					= NULL;	
	m_pMoneyIndicator			= NULL;
}

//////////////////////////////////////////////////////////////////////////

CUIMainIngameWnd::~CUIMainIngameWnd()
{
	DestroyFlashingIcons();
	xr_delete(UIZoneMap);
	xr_delete(m_artefactPanel);
	xr_delete(m_pMoneyIndicator);
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::Init()
{
	CUIXml uiXml;
	uiXml.Init(CONFIG_PATH, UI_PATH, MAININGAME_XML);
	
	CUIXmlInit xml_init;
	CUIWindow::Init(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	//в это окно пользовательского ввода осуществляться не будет
	Enable(false);


	AttachChild(&UIStaticHealth);
	xml_init.InitStatic(uiXml, "static", 0, &UIStaticHealth);
//	UIStaticHealth.SetWndPos(500, 500);
//	UIStaticHealth.InitTexture("ui\\ui_debug_red_n_black");
		//.InitSharedTexture("ui_texture.xml", "riffle", true);
		//SetShader(UI()->GetShader("ui\\ui_debug_red_n_black"));		
//	UIStaticHealth.SetStretchTexture(true);
//	UIStaticHealth.m_lines.SetWndRect(UIStaticHealth.GetAbsoluteRect());
//	UIStaticHealth.m_lines.SetTextColor(color_argb(255,0,255,0));
//	UIStaticHealth.m_lines.SetText("%c<255,255,255,255>white %c<default>green1 green2 green3 green4 green5 %c<red>red text %c<default>green6 green7 green8888 last_green");

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
	if (GameID() != GAME_SINGLE)
	{
//		UIPdaOnline.Show(false);
	}

	// Для информационных сообщений
	AttachChild(&UIInfoMessages);
	xml_init.InitListWnd(uiXml, "info_list", 0, &UIInfoMessages);
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
	UIZoneMap->Init();
	UIZoneMap->SetScale(DEFAULT_MAP_SCALE);

	// Подсказки, которые возникают при наведении прицела на объект
	AttachChild(&UIStaticQuickHelp);
	xml_init.InitStatic(uiXml, "quick_info", 0, &UIStaticQuickHelp);
	UIStaticQuickHelp.SetTextColor(0xffffffff);
	UIStaticQuickHelp.SetTextAlign(CGameFont::alCenter);

	uiXml.SetLocalRoot(uiXml.GetRoot());

	// Загружаем иконки 
	AttachChild(&UIWeaponJammedIcon);
	xml_init.InitStatic(uiXml, "weapon_jammed_static", 0, &UIWeaponJammedIcon);
///.UIWeaponJammedIcon.GetStaticItem()->SetScaleXY(0.75f,0.75f);
	UIWeaponJammedIcon.ClipperOn();

	AttachChild(&UIRadiaitionIcon);
	xml_init.InitStatic(uiXml, "radiation_static", 0, &UIRadiaitionIcon);
///.UIRadiaitionIcon.GetStaticItem()->SetScaleXY(0.75f,0.75f);

	AttachChild(&UIWoundIcon);
	xml_init.InitStatic(uiXml, "wound_static", 0, &UIWoundIcon);
///.UIWoundIcon.GetStaticItem()->SetScaleXY(0.75f,0.75f);

	AttachChild(&UIStarvationIcon);
	xml_init.InitStatic(uiXml, "starvation_static", 0, &UIStarvationIcon);
///.UIStarvationIcon.GetStaticItem()->SetScaleXY(0.75f,0.75f);

	AttachChild(&UIFatigueIcon);
	xml_init.InitStatic(uiXml, "fatigue_static", 0, &UIFatigueIcon);
///.UIFatigueIcon.GetStaticItem()->SetScaleXY(0.75f,0.75f);

	AttachChild(&UIInvincibleIcon);
	xml_init.InitStatic(uiXml, "invincible_static", 0, &UIInvincibleIcon);

	shared_str warningStrings[6] = 
	{	
		"jammed",
		"radiation",
		"wounds",
		"starvation",
		"fatigue",
		"invincible"
	};

	// Загружаем пороговые значения для индикаторов
	EWarningIcons j = ewiWeaponJammed;
	while (j <= ewiInvincible)
	{
		// Читаем данные порогов для каждого индикатора
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
		m_pMoneyIndicator = xr_new<CUIMoneyIndicator>();
		AttachChild(m_pMoneyIndicator);
		m_pMoneyIndicator->InitFromXML(uiXml);
//		xml_init.InitMultiTextStatic(uiXml, "money_mt_static", 0, &UIMoneyIndicator);

		if (GameID() == GAME_TEAMDEATHMATCH || GameID() == GAME_ARTEFACTHUNT)
		{
			AttachChild(&UITeam1Sign);
			AttachChild(&UITeam2Sign);
			xml_init.InitStatic(uiXml, "t1_static", 0, &UITeam1Sign);
			xml_init.InitStatic(uiXml, "t2_static", 0, &UITeam2Sign);

			AttachChild(&UITeam1Score);
			AttachChild(&UITeam2Score);
			xml_init.InitStatic(uiXml, "t1_score", 0, &UITeam1Score);
			xml_init.InitStatic(uiXml, "t2_score", 0, &UITeam2Score);

			UITeam1Score.SetText("10");
			UITeam2Score.SetText("10");

//			UITeam1Sign.Show(false);
//			UITeam2Sign.Show(false);
//			UITeam1Score.Show(false);
//			UITeam2Score.Show(false);
		}

		//  [7/27/2005]
		AttachChild(&UIRankIndicator);
		xml_init.InitStatic(uiXml, "rank_icon", 0, &UIRankIndicator);
		SetRank(0);
		//  [7/27/2005]
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
	//Полоса прогресса армора
	UIStaticBattery.AttachChild(&UIBatteryBar);
	xml_init.InitProgressBar(uiXml, "progress_bar", 2, &UIBatteryBar);
	ShowBattery(false);
	SetBatteryCharge(1.0f);
	
	AttachChild(&UICarPanel);
	xml_init.InitWindow(uiXml, "car_panel", 0, &UICarPanel);

	AttachChild(&UIMotionIcon);
	xml_init.InitWindow(uiXml, "motion_icon", 0, &UIMotionIcon);

	xml_init.InitArtefactPanel(uiXml, "artefact_panel", 0, m_artefactPanel);
	this->AttachChild(m_artefactPanel);	
	xr_vector<Irect> vRects;

	AttachChild(&UILuminosityBar);
	xml_init.InitProgressBar(uiXml, "luminosity_bar", 0, &UILuminosityBar);

	AttachChild(&UISndNoiseBar);
	xml_init.InitProgressBar(uiXml, "sound_noise_bar", 0, &UISndNoiseBar);

	AttachChild(&UIStaticDiskIO);
	UIStaticDiskIO.SetWndRect				(1000,750,16,16);
	UIStaticDiskIO.GetUIStaticItem().SetRect(0,0,16,16);
	UIStaticDiskIO.InitTexture				("ui\\ui_disk_io");
	UIStaticDiskIO.SetOriginalRect			(0,0,32,32);
	UIStaticDiskIO.SetStretchTexture		(TRUE);

}
float UIStaticDiskIO_start_time = 0.0f;
void CUIMainIngameWnd::Draw()
{
	if(w_)
		w_->Draw();

	bool zoom_mode = false;
	bool scope_mode = false;
	// show IO icon
	bool IOActive	= (FS.dwOpenCounter>0);
	if	(IOActive)	UIStaticDiskIO_start_time = Device.fTimeGlobal;

	if ((UIStaticDiskIO_start_time+1.0f) < Device.fTimeGlobal)	UIStaticDiskIO.Show(false); 
	else {
		u32		alpha			= clampr(iFloor(255.f*(1.f-(Device.fTimeGlobal-UIStaticDiskIO_start_time)/1.f)),0,255);
		UIStaticDiskIO.Show		( true  ); 
		UIStaticDiskIO.SetColor	(color_rgba(255,255,255,alpha));
	}
	FS.dwOpenCounter = 0;

	float	luminocity = smart_cast<CGameObject*>(Level().CurrentEntity())->ROS()->get_luminocity();
	static float cur_lum = luminocity;
	cur_lum = luminocity*0.01f + cur_lum*0.99f;
	UILuminosityBar.SetProgressPos( (s16)iFloor(cur_lum*100.0f) );

	
	CActor* _pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(_pActor){
		s32 progr	= clampr(iFloor(_pActor->m_snd_noise*100.0f),(s32)UISndNoiseBar.GetRange_min(),(s32)UISndNoiseBar.GetRange_max());
		UISndNoiseBar.SetProgressPos( (s16)progr );
	}
	

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

	//отрендерить текстуру объектива снайперского прицела или бинокля
	if(m_pActor->HUDview() && m_pWeapon){
		m_pWeapon->OnDrawUI();
	}


	if(m_pActor->HUDview() && m_pWeapon && m_pWeapon->IsZoomed() && m_pWeapon->ZoomHideCrosshair()){
		zoom_mode = true;
		if(m_pWeapon->ZoomTexture() && !m_pWeapon->IsRotatingToZoom()){
//			m_pWeapon->ZoomTexture()->SetPos	(0,0);
//			m_pWeapon->ZoomTexture()->SetRect	(0,0,UI_BASE_WIDTH, UI_BASE_HEIGHT);
//			m_pWeapon->ZoomTexture()->Render	();
			scope_mode = true;
		}

		if(psHUD_Flags.is(HUD_CROSSHAIR|HUD_CROSSHAIR_RT)){
			psHUD_Flags.set(HUD_CROSSHAIR_RT, FALSE);
			m_bShowHudCrosshair = true;
		}

		zoom_mode = true;
	}
	
//	if(!scope_mode)
//	{
		if(m_bShowHudInfo)
		{
			CUIWindow::Draw();
			UIZoneMap->Render();			
		}
		if (m_bShowHudCrosshair && !zoom_mode)
		{
			m_bShowHudCrosshair = false;
			psHUD_Flags.set(HUD_CROSSHAIR_RT, TRUE);
		}
		RenderQuickInfos();
//	}

	// Render claws
	if (!m_ClawsAnimation.Done() && m_ClawsTexture.GetShader())
	{
		m_ClawsTexture.SetPos	(0, 0);
		//m_ClawsTexture.Render	(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);
		//m_ClawsTexture.SetRect	(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

		m_ClawsTexture.Render	(PI_DIV_3);
///.m_ClawsTexture.SetScaleXY	(0.5f, 0.5f);
	}

//	DrawPdaMessages();

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

void CUIMainIngameWnd::SetMPChatLog(CUIWindow* pChat, CUIWindow* pLog){
	m_pMPChatWnd = pChat;
	m_pMPLogWnd  = pLog;
}

void CUIMainIngameWnd::Update()
{
	if (m_pMPChatWnd)
		m_pMPChatWnd->Update();
	if (m_pMPLogWnd)
		m_pMPLogWnd->Update();


	static string256 text_str;

	m_pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if (!m_pActor) 
	{
		m_pItem = NULL;
		m_pWeapon = NULL;
		CUIWindow::Update();
		return;
	}

	if(!m_bShowHudInfo) return;

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
	PIItem	pItem = m_pActor->inventory().ItemFromSlot(OUTFIT_SLOT);
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
		PIItem item =  m_pActor->inventory().ItemFromSlot(m_pActor->inventory().GetActiveSlot());
		CWeapon* pWeapon = smart_cast<CWeapon*>(item); 
		CMissile* pMissile = smart_cast<CMissile*>(item); 
		CWeaponMagazined* pWeaponMagazined = smart_cast<CWeaponMagazined*>(pWeapon);
		
		bool active_item_changed = false;
		// Remember last used ammo types, and if this type doesn't changed 
		// then no need to update info
		static u32			prevAmmoID		= static_cast<u32>(-1);
		static u32			prevState		= static_cast<u32>(-1);
		if((item && m_pItem != item) || 
			(pWeapon && (prevState != pWeapon->State() || prevAmmoID != pWeapon->m_ammoType) ))
		{
			m_pItem = item;
			prevState = 
			active_item_changed = true;
		};		

		if(pMissile && active_item_changed)
		{
			UIWeaponIcon.Show(false);
			UIWeaponSignAmmo.Show(false);

			UIWeaponBack.SetText(m_pItem->NameShort());
			UIWeaponBack.SetTextX(		(UIWeaponBack.GetAbsoluteRect().right - 
										UIWeaponBack.GetAbsoluteRect().left) * 0.5f - 
										UIWeaponIcon.GetFont()->SizeOf(m_pItem->NameShort()) / 2.0f);

		}
		else if(pWeapon)
		{

			if(active_item_changed || !m_pWeapon || prevAmmoID != m_pWeapon->m_ammoType)
			{
				m_pWeapon = pWeapon;
				prevState = pWeapon->State();

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

					UIWeaponIcon.GetUIStaticItem().SetOriginalRect(	float(m_iXPos * INV_GRID_WIDTH),
																	float(m_iYPos * INV_GRID_HEIGHT),
																	float(m_iGridWidth * INV_GRID_WIDTH),
																	float(m_iGridHeight * INV_GRID_HEIGHT));
					UIWeaponIcon.SetStretchTexture(true);

					// now perform only width scale for ammo, which (W)size >2
					// all others ammo (1x1, 1x2) will be not scaled (original picture)
					float w = ((m_iGridWidth>2)?1.6f:m_iGridWidth)*INV_GRID_WIDTH*0.9f;
					float h = INV_GRID_HEIGHT*0.9f;//1 cell
					UIWeaponIcon.SetWidth(w);
					UIWeaponIcon.SetHeight(h);
				}
				else
				{
					UIWeaponIcon.Show(false);
					UIWeaponSignAmmo.Show(false);
				}
			}

//			shared_str sItemName = m_pItem->NameShort();
			string256 sItemName;
			strcpy(sItemName, m_pItem->NameShort());			
			
			if (pWeaponMagazined && pWeaponMagazined->HasFireModes())
			{
				strcat(sItemName, pWeaponMagazined->GetCurrentFireModeStr());
				/*
				int FireMode = pWeaponMagazined->GetCurrentFireMode();
				switch (FireMode)
				{
				case -1:
						sItemName.sprintf("%s (A)", m_pItem->NameShort());
						break;
				default:
					sItemName.sprintf("%s (%d)", m_pItem->NameShort(), FireMode);
					break;
				};	
				*/
			}			

			UIWeaponBack.SetText(sItemName);
			UIWeaponBack.SetTextX(((UIWeaponBack.GetAbsoluteRect().right - 
				UIWeaponBack.GetAbsoluteRect().left) * 0.5f - 
				UIWeaponIcon.GetFont()->SizeOf(m_pItem->NameShort()) / 2.0f));


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


	//сбросить индикаторы
	if(!m_pWeapon)
	{
		UIWeaponSignAmmo.SetText("");
		UIWeaponIcon.Show(false);
	}
    

	//if(m_pActor->HUDview() && m_pWeapon && m_pWeapon->IsZoomed() && m_pWeapon->ZoomTexture()) return;


    // radar
	UIZoneMap->UpdateRadar(Device.vCameraPosition);
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

	FadeUpdate(&UIInfoMessages);//, m_iInfoMessagesFade_mSec);

	UpdateFlashingIcons();
	UIContactsAnimation.Update();
	UIPdaOnline.SetTextColor(subst_alpha(UIPdaOnline.GetTextColor(), color_get_A(UIContactsAnimation.GetColor())));
	m_ClawsAnimation.Update();

	UpdatePickUpItem();

	CUIWindow::Update();
}
#include "UIMessageBox.h"
CUIMessageBox* mb = NULL;

bool CUIMainIngameWnd::OnKeyboardPress(int dik)
{
	if(dik==DIK_K&&strstr(Core.Params,"andy")){
		if(!w){
			w = xr_new<CTestDragDropWnd>	();
			Game().StartStopMenu			(w,true);
		}else{
			Game().StartStopMenu			(w,true);
			xr_delete						(w);
		}
	}

	if(dik==DIK_J&&strstr(Core.Params,"andy")){
//		CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
//		if(pGameSP)pGameSP->ChangeLevel(1,1,Fvector(),Fvector());
		if(!mb){
			mb = xr_new<CUIMessageBox>		();
			mb->Init						("message_box_error");
			HUD().GetUI()->AddDialogToRender(mb);
//			Game().StartStopMenu			(mb,true);
		}else{
			HUD().GetUI()->RemoveDialogToRender(mb);
			xr_delete						(mb);
		}
		
	}

	// поддержка режима adjust hud mode
	bool flag = false;
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

#ifdef DEBUG
		if(CAttachableItem::m_dbgItem){
			static float rot_d = deg2rad(0.5f);
			static float mov_d = 0.01f;
			bool shift = !!pInput->iGetAsyncKeyState(DIK_LSHIFT);
			flag = true;
			switch (dik)
			{
				// Shift +x
			case DIK_A:
				if(shift)	CAttachableItem::rot_dx(rot_d);
				else		CAttachableItem::mov_dx(rot_d);
				break;
				// Shift -x
			case DIK_D:
				if(shift)	CAttachableItem::rot_dx(-rot_d);
				else		CAttachableItem::mov_dx(-rot_d);
				break;
				// Shift +z
			case DIK_Q:
				if(shift)	CAttachableItem::rot_dy(rot_d);
				else		CAttachableItem::mov_dy(rot_d);
				break;
				// Shift -z
			case DIK_E:
				if(shift)	CAttachableItem::rot_dy(-rot_d);
				else		CAttachableItem::mov_dy(-rot_d);
				break;
				// Shift +y
			case DIK_S:
				if(shift)	CAttachableItem::rot_dz(rot_d);
				else		CAttachableItem::mov_dz(rot_d);
				break;
				// Shift -y
			case DIK_W:
				if(shift)	CAttachableItem::rot_dz(-rot_d);
				else		CAttachableItem::mov_dz(-rot_d);
				break;

			case DIK_SUBTRACT:
				if(shift)	rot_d-=deg2rad(0.01f);
				else		mov_d-=0.001f;
				Msg("rotation delta=[%f]; moving delta=[%f]",rot_d,mov_d);
				break;
			case DIK_ADD:
				if(shift)	rot_d+=deg2rad(0.01f);
				else		mov_d+=0.001f;
				Msg("rotation delta=[%f]; moving delta=[%f]",rot_d,mov_d);
				break;

			case DIK_P:
				Msg("LTX section [%s]",*CAttachableItem::m_dbgItem->cNameSect());
				Msg("attach_angle_offset [%f,%f,%f]",VPUSH(CAttachableItem::get_angle_offset()));
				Msg("attach_position_offset [%f,%f,%f]",VPUSH(CAttachableItem::get_pos_offset()));
				break;
			default:
				flag = false;
				break;
			}		
		if(flag)return true;;
		}
#endif		

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
	HUD().GetUI()->m_pMessagesWnd->AddPdaMessage(pSender, msg, info_id);
}

//////////////////////////////////////////////////////////////////////////

bool CUIMainIngameWnd::SetDelayForPdaMessage(int iValue, int iDelay){
	return HUD().GetUI()->m_pMessagesWnd->SetDelayForPdaMessage(iValue, iDelay);
}

void CUIMainIngameWnd::AddGameMessage(LPCSTR message, int iId, int iDelay)
{
	HUD().GetUI()->m_pMessagesWnd->AddPdaMessage(message, iId, iDelay);
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::AddPersonalizedGameMessage(CInventoryOwner* pSender, LPCSTR message, int iId, int iDelay)
{
	HUD().GetUI()->m_pMessagesWnd->AddPersonalPdaMessage(pSender, message, iId, iDelay);
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::AddIconedGameMessage(LPCSTR textureName, Frect originalRect, LPCSTR message, int iId, int iDelay)
{
	HUD().GetUI()->m_pMessagesWnd->AddIconedPdaMessage(textureName, originalRect, message, iId, iDelay);
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
	pItem->UIMsgText.SetWndPos(-(UIInfoMessages.GetFont()->SizeOf(message) / 2.0f),
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
		Frect rect;
		rect.left		= float(news.x1);
		rect.right		= float(news.x2);
		rect.top		= float(news.y1);
		rect.bottom		= float(news.y2);
		AddIconedGameMessage(news.texture_name, rect, news.FullText(), news.news_text, news.show_time);
	}
	else
		AddGameMessage(news.FullText(), news.news_text, news.show_time);
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
	m_pMoneyIndicator->SetMoneyAmount(*newMoneyString);
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::DisplayMoneyChange(shared_str deltaMoney)
{
	m_pMoneyIndicator->SetMoneyChange(*deltaMoney);
}

void CUIMainIngameWnd::DisplayMoneyBonus(shared_str bonus){
	m_pMoneyIndicator->SetMoneyBonus(*bonus);
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

void  CUIMainIngameWnd::AddStaticItem (CUIStaticItem* si, float left, float top, float right, float bottom, int priority)
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
	// Включаем анимацию требуемой иконки
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
	// Пробегаемся по всем нодам и инициализируем из них статики
	for (int i = 0; i < staticsCount; ++i)
	{
		pIcon = xr_new<CUIStatic>();
		xml_init.InitStatic(node, flashingIconNodeName, i, pIcon);
		shared_str iconType = node.ReadAttrib(flashingIconNodeName, i, "type", "none");

		// Теперь запоминаем иконку и ее тип
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
///.			pIcon->GetUIStaticItem().SetScaleXY(0.5f, 0.5f);
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

void CUIMainIngameWnd::PlayClawsAnimation(const shared_str &monsterName)
{
	MonsterClawsTextures_it it = m_ClawsTextures.find(monsterName);
	R_ASSERT2(it != m_ClawsTextures.end(), "Monster claws texture for this monster doesn't exist");
	
	m_ClawsTexture.SetShader	(*it->second);
	m_ClawsAnimation.Reset		();
}

void CUIMainIngameWnd::ShowBattery(bool on)
{
	UIStaticBattery.Show(on);
}

void CUIMainIngameWnd::SetBatteryCharge(float value)
{
	s16 pos = static_cast<s16>(value * 100);
	clamp<s16>(pos, 0, 100);
	UIBatteryBar.SetProgressPos(pos);
}

void CUIMainIngameWnd::SetPickUpItem	(CInventoryItem* PickUpItem)
{
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

	float scale_x = m_iPickUpItemIconWidth/
		float(m_iGridWidth*INV_GRID_WIDTH);

	float scale_y = m_iPickUpItemIconHeight/
		float(m_iGridHeight*INV_GRID_HEIGHT);

	scale_x = (scale_x>1) ? 1.0f : scale_x;
	scale_y = (scale_y>1) ? 1.0f : scale_y;

	float scale = scale_x<scale_y?scale_x:scale_y;

	UIPickUpItemIcon.GetUIStaticItem().SetOriginalRect(
		float(m_iXPos * INV_GRID_WIDTH),
		float(m_iYPos * INV_GRID_HEIGHT),
		float(m_iGridWidth * INV_GRID_WIDTH),
		float(m_iGridHeight * INV_GRID_HEIGHT));

	UIPickUpItemIcon.SetStretchTexture(true);

	UIPickUpItemIcon.SetWidth(m_iGridWidth*INV_GRID_WIDTH*scale);
	UIPickUpItemIcon.SetHeight(m_iGridHeight*INV_GRID_HEIGHT*scale);

	UIPickUpItemIcon.SetWndPos(m_iPickUpItemIconX + 
		(m_iPickUpItemIconWidth - UIPickUpItemIcon.GetWidth())/2,
		m_iPickUpItemIconY + 
		(m_iPickUpItemIconHeight - UIPickUpItemIcon.GetHeight())/2);

	UIPickUpItemIcon.SetColor(color_rgba(255,255,255,192));
	UIPickUpItemIcon.Show(true);
};

//  [7/4/2005]
void	CUIMainIngameWnd::UpdateTeamsScore	(int t1, int t2)
{
	Frect	xT1Rect, xT2Rect;
	xT1Rect = UITeam1Score.GetAbsoluteRect();
	xT2Rect = UITeam2Score.GetAbsoluteRect();

	string16 tmp; 
	sprintf(tmp, "%d", t1);		UITeam1Score.SetText(tmp);
	sprintf(tmp, "%d", t2);		UITeam2Score.SetText(tmp);

	if ((xT1Rect.y1 < xT2Rect.y1 && t1<t2) || (xT1Rect.y1>xT2Rect.y1 && t1>t2))
	{
		int x=0;
		x=x;

		UITeam1Score.SetWndPos(xT1Rect.x1, xT2Rect.y1);
		UITeam2Score.SetWndPos(xT2Rect.x1, xT1Rect.y1);

		xT1Rect = UITeam1Sign.GetAbsoluteRect();
		xT2Rect = UITeam2Sign.GetAbsoluteRect();

		UITeam1Sign.SetWndPos(xT1Rect.x1, xT2Rect.y1);
		UITeam2Sign.SetWndPos(xT2Rect.x1, xT1Rect.y1);

	};
}
//  [7/4/2005]

//  [7/27/2005]
void	CUIMainIngameWnd::SetRank				(int rank)
{
	UIRankIndicator.SetOriginalRect(rank*32.0f, 0.0f, 32.0f, 32.0f);	
};
//  [7/27/2005]