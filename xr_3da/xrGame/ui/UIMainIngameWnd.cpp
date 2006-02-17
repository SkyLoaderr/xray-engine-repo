#include "stdafx.h"

#include "UIMainIngameWnd.h"
#include "UIMessagesWindow.h"
#include "../UIZoneMap.h"


#include <dinput.h>
#include "../actor.h"
#include "../HUDManager.h"
#include "../PDA.h"
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
#ifdef DEBUG
#include "../attachable_item.h"
#include "../../xr_input.h"
#endif

#include "UIScrollView.h"
#include "map_hint.h"
#include "UIColorAnimatorWrapper.h"
#include "../game_news.h"



void test_draw	();
void test_key	(int dik);

using namespace InventoryUtilities;

//	hud adjust mode
int			g_bHudAdjustMode			= 0;
float		g_fHudAdjustValue			= 0.0f;
int			g_bNewsDisable				= 0;
bool		g_bShowHudInfo				= true;;

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

#define				MAININGAME_XML				"maingame.xml"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction//////////////////////////////////////////////////////////////////////

CUIMainIngameWnd::CUIMainIngameWnd()
{
	m_pActor					= NULL;
	m_pWeapon					= NULL;
	m_pItem						= NULL;

	g_bShowHudInfo				= true;
	m_bShowHudCrosshair			= false;

	UIZoneMap					= xr_new<CUIZoneMap>();
	m_pPickUpItem				= NULL;
	m_artefactPanel				= xr_new<CUIArtefactPanel>();
	m_pMPChatWnd				= NULL;
	m_pMPLogWnd					= NULL;	
}

//////////////////////////////////////////////////////////////////////////
#include "UIProgressShape.h"
extern CUIProgressShape* g_MissileForceShape;

CUIMainIngameWnd::~CUIMainIngameWnd()
{
	DestroyFlashingIcons		();
	xr_delete					(UIZoneMap);
	xr_delete					(m_artefactPanel);
	HUD_SOUND::DestroySound		(m_contactSnd);
	xr_delete					(g_MissileForceShape);
}

void CUIMainIngameWnd::Init()
{
	CUIXml uiXml;
	uiXml.Init(CONFIG_PATH, UI_PATH, MAININGAME_XML);
	
	CUIXmlInit xml_init;
	CUIWindow::Init(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	//� ��� ���� ����������������� ����� �������������� �� �����
	Enable(false);


	AttachChild(&UIStaticHealth);
	xml_init.InitStatic(uiXml, "static_health", 0, &UIStaticHealth);

	AttachChild(&UIStaticArmor);
	xml_init.InitStatic(uiXml, "static_armor", 0, &UIStaticArmor);

	AttachChild(&UIWeaponBack);
	xml_init.InitStatic(uiXml, "static_weapon", 0, &UIWeaponBack);

	UIWeaponBack.AttachChild(&UIWeaponSignAmmo);
	xml_init.InitStatic(uiXml, "static_ammo", 0, &UIWeaponSignAmmo);
	UIWeaponSignAmmo.SetElipsis(CUIStatic::eepEnd, 2);

	UIWeaponBack.AttachChild(&UIWeaponIcon);
	xml_init.InitStatic(uiXml, "static_wpn_icon", 0, &UIWeaponIcon);
	UIWeaponIcon.SetShader(GetEquipmentIconsShader());
	UIWeaponIcon_rect = UIWeaponIcon.GetWndRect();
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


	UIWeaponIcon.Enable(false);

	//���������� 
	UIZoneMap->Init();
	UIZoneMap->SetScale(DEFAULT_MAP_SCALE);

	if(IsGameTypeSingle())
	{
		xml_init.InitStatic					(uiXml, "static_pda_online", 0, &UIPdaOnline);
		UIZoneMap->Background().AttachChild	(&UIPdaOnline);
	}

	// ��� �������������� ���������
	u32			color;
	CGameFont*	pFont;
	AttachChild(&UIInfoMessages);
	xml_init.InitScrollView(uiXml, "info_list", 0, &UIInfoMessages);
	CUIXmlInit::InitFont(uiXml, "info_list:font", 0, color, pFont);
	UIInfoMessages.SetTextAtrib(pFont, color);

		

	//������ ��������� ��������
	UIStaticHealth.AttachChild(&UIHealthBar);
	xml_init.InitAutoStaticGroup(uiXml,"static_health", &UIStaticHealth);
	xml_init.InitProgressBar2(uiXml, "progress_bar_health", 0, &UIHealthBar);

	//������ ��������� ������
	UIStaticArmor.AttachChild(&UIArmorBar);
	xml_init.InitAutoStaticGroup(uiXml,"static_armor", &UIStaticArmor);
	xml_init.InitProgressBar2(uiXml, "progress_bar_armor", 0, &UIArmorBar);

	

	// ���������, ������� ��������� ��� ��������� ������� �� ������
	AttachChild(&UIStaticQuickHelp);
	xml_init.InitStatic(uiXml, "quick_info", 0, &UIStaticQuickHelp);

	uiXml.SetLocalRoot(uiXml.GetRoot());

	m_UIIcons = xr_new<CUIScrollView>(); m_UIIcons->SetAutoDelete(true);
	xml_init.InitScrollView(uiXml, "icons_scroll_view", 0, m_UIIcons);
	AttachChild(m_UIIcons);

	// ��������� ������ 
	if(IsGameTypeSingle())
	{
		xml_init.InitStatic(uiXml, "starvation_static", 0, &UIStarvationIcon);
		UIStarvationIcon.Show(false);

		xml_init.InitStatic(uiXml, "psy_health_static", 0, &UIPsyHealthIcon);
		UIPsyHealthIcon.Show(false);

		xml_init.InitStatic(uiXml, "can_sleep_static", 0, &UISleepIcon);
		UISleepIcon.Show(false);
	}

	xml_init.InitStatic(uiXml, "weapon_jammed_static", 0, &UIWeaponJammedIcon);
	UIWeaponJammedIcon.Show(false);

	xml_init.InitStatic(uiXml, "radiation_static", 0, &UIRadiaitionIcon);
	UIRadiaitionIcon.Show(false);

	xml_init.InitStatic(uiXml, "wound_static", 0, &UIWoundIcon);
	UIWoundIcon.Show(false);

	xml_init.InitStatic(uiXml, "invincible_static", 0, &UIInvincibleIcon);
	UIInvincibleIcon.Show(false);


	if(GameID()==GAME_ARTEFACTHUNT){
		xml_init.InitStatic(uiXml, "artefact_static", 0, &UIArtefactIcon);
		UIArtefactIcon.Show(false);
	}
	
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
	while (j < ewiInvincible)
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


	// Flashing icons initialize
	uiXml.SetLocalRoot				(uiXml.NavigateToNode("flashing_icons"));
	InitFlashingIcons				(&uiXml);

	uiXml.SetLocalRoot				(uiXml.GetRoot());
	
	AttachChild						(&UICarPanel);
	xml_init.InitWindow				(uiXml, "car_panel", 0, &UICarPanel);

	AttachChild						(&UIMotionIcon);
	UIMotionIcon.Init				();

	if(IsGameTypeSingle()){
		xml_init.InitArtefactPanel(uiXml, "artefact_panel", 0, m_artefactPanel);
		this->AttachChild(m_artefactPanel);	
	}

	AttachChild(&UIStaticDiskIO);
	UIStaticDiskIO.SetWndRect				(1000,750,16,16);
	UIStaticDiskIO.GetUIStaticItem().SetRect(0,0,16,16);
	UIStaticDiskIO.InitTexture				("ui\\ui_disk_io");
	UIStaticDiskIO.SetOriginalRect			(0,0,32,32);
	UIStaticDiskIO.SetStretchTexture		(TRUE);


	HUD_SOUND::LoadSound("maingame_ui", "snd_new_contact"		, m_contactSnd		, TRUE, SOUND_TYPE_IDLE);
}

float UIStaticDiskIO_start_time = 0.0f;
void CUIMainIngameWnd::Draw()
{
	test_draw				();
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

	float		luminocity = smart_cast<CGameObject*>(Level().CurrentEntity())->ROS()->get_luminocity();
	float		power = log(luminocity > .001f ? luminocity : .001f)*(1.f/*luminocity_factor*/);
	luminocity	= exp(power);

	static float cur_lum = luminocity;
	cur_lum = luminocity*0.01f + cur_lum*0.99f;
	UIMotionIcon.SetLuminosity((s16)iFloor(cur_lum*100.0f));

	if(!m_pActor) return;

	UIMotionIcon.SetNoise((s16)(0xffff&iFloor(m_pActor->m_snd_noise*100.0f)));
	
	if(m_pActor->inventory().GetActiveSlot() != NO_ACTIVE_SLOT)
	{
		PIItem item		=  m_pActor->inventory().ItemFromSlot(m_pActor->inventory().GetActiveSlot());

		if(item && m_pActor->HUDview() && smart_cast<CHudItem*>(item))
			(smart_cast<CHudItem*>(item))->OnDrawUI();


		if(m_pActor->HUDview() && m_pWeapon && m_pWeapon->IsZoomed() && m_pWeapon->ZoomHideCrosshair())
		{
			zoom_mode = true;
			if(m_pWeapon->ZoomTexture() && !m_pWeapon->IsRotatingToZoom())
				scope_mode = true;

			if(psHUD_Flags.is(HUD_CROSSHAIR|HUD_CROSSHAIR_RT)){
				psHUD_Flags.set(HUD_CROSSHAIR_RT, FALSE);
				m_bShowHudCrosshair = true;
			}

			zoom_mode = true;
		}
	}
		if(g_bShowHudInfo)
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

#ifdef DEBUG 
	if (g_bHudAdjustMode&&m_pWeapon) //draw firePoint,ShellPoint etc
	{
		CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(!pActor)
			return;

		bool bCamFirstEye = !!m_pWeapon->GetHUDmode();
		string32 hud_view="HUD view";
		string32 _3rd_person_view="3-rd person view";
		CGameFont* F		= UI()->Font()->pFontDI;
		F->SetAligment		(CGameFont::alCenter);
		F->SetSizeI			(0.02f);
		F->OutSetI			(0.f,-0.8f);
		F->SetColor			(0xffffffff);
		F->OutNext			("Hud_adjust_mode=%d",g_bHudAdjustMode);
		if(g_bHudAdjustMode==1)
			F->OutNext			("adjusting zoom offset");
		else if(g_bHudAdjustMode==2)
			F->OutNext			("adjusting fire point for %s",bCamFirstEye?hud_view:_3rd_person_view);
		else if(g_bHudAdjustMode==3)
			F->OutNext			("adjusting missile offset");
		else if(g_bHudAdjustMode==4)
			F->OutNext			("adjusting shell point for %s",bCamFirstEye?hud_view:_3rd_person_view);

		if(bCamFirstEye)
		{
			CWeaponHUD *pWpnHud = NULL;
			pWpnHud = m_pWeapon->GetHUD();

			Fvector FP,SP;//,FP2;

			CKinematics* V			= smart_cast<CKinematics*>(pWpnHud->Visual());
			VERIFY					(V);
			V->CalculateBones		();

			// fire point&direction
			Fmatrix& fire_mat		= V->LL_GetTransform(u16(pWpnHud->FireBone()));
			Fmatrix& parent			= pWpnHud->Transform	();

			const Fvector& fp		= pWpnHud->FirePoint();
			const Fvector& sp		= pWpnHud->ShellPoint();

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
		m_pItem					= NULL;
		m_pWeapon				= NULL;	//Msg("* m_pWeapon = NULL");
		CUIWindow::Update		();
		return;
	}

	if(!g_bShowHudInfo) 
		return;


	if (GameID() == GAME_SINGLE && !(Device.dwFrame%30) )
	{
		if(m_pActor->GetPDA())
		{
			u32 cn = m_pActor->GetPDA()->ActiveContactsNum();
			if(cn>0){
				sprintf(text_str, "%d", cn);
				UIPdaOnline.SetText(text_str);
			}else
				UIPdaOnline.SetText("");
		}
		else
		{
			UIPdaOnline.SetText("");
		}

		if(m_pActor->conditions().AllowSleep())
			SetWarningIconColor	(ewiSleep,0xffffffff);
		else
			SetWarningIconColor	(ewiSleep,0x00ffffff);

	};

	// ewiInvincible:
	bool b_God = (GodMode()||(!Game().local_player)) ? true : Game().local_player->testFlag(GAME_PLAYER_FLAG_INVINCIBLE);
	if(b_God)
		SetWarningIconColor	(ewiInvincible,0xffffffff);
	else
		SetWarningIconColor	(ewiInvincible,0x00ffffff);

	// ewiArtefact
	if(GameID() == GAME_ARTEFACTHUNT){
		bool b_Artefact = !m_pActor->ArtefactsOnBelt().empty();
		if(b_Artefact)
			SetWarningIconColor	(ewiArtefact,0xffffffff);
		else
			SetWarningIconColor	(ewiArtefact,0x00ffffff);
	}

	// Armor indicator stuff
	PIItem	pItem = m_pActor->inventory().ItemFromSlot(OUTFIT_SLOT);
	if (pItem)
	{
		UIArmorBar.Show(true);
		UIStaticArmor.Show(true);
		UIArmorBar.SetProgressPos(static_cast<s16>(pItem->GetCondition() * 100));
	}
	else
	{
		UIArmorBar.Show(false);
		UIStaticArmor.Show(false);
	}

	if(m_pActor->inventory().GetActiveSlot() != NO_ACTIVE_SLOT) 
	{
		PIItem item							=  m_pActor->inventory().ItemFromSlot(m_pActor->inventory().GetActiveSlot());
		CWeapon* pWeapon					= smart_cast<CWeapon*>(item); 
		CMissile* pMissile					= smart_cast<CMissile*>(item); 
		CWeaponMagazined* pWeaponMagazined	= smart_cast<CWeaponMagazined*>(pWeapon);
		
		bool active_item_changed			= false;
		// Remember last used ammo types, and if this type doesn't changed 
		// then no need to update info
		static u32			prevAmmoID		= static_cast<u32>(-1);
		static u32			prevState		= static_cast<u32>(-1);
		static int			prevFireMode	= static_cast<int>(-1);

		if(	((item && m_pItem != item) || 
						 (pWeapon && (prevState != pWeapon->State() || prevAmmoID != pWeapon->m_ammoType) )))
		{
			m_pItem							= item;
			if (pWeapon)
				prevState						= pWeapon->State();
			active_item_changed				= true;
		};		

		if(pMissile && active_item_changed)
		{
			UIWeaponIcon.Show				(false);
			UIWeaponSignAmmo.Show			(false);
			UIWeaponBack.SetText			(m_pItem->NameShort());
		}
//		else 
		if(pWeapon)
		{

			if(active_item_changed || !m_pWeapon || prevAmmoID != m_pWeapon->m_ammoType || 
				(pWeaponMagazined && pWeaponMagazined->HasFireModes() && prevFireMode != pWeaponMagazined->GetCurrentFireMode()))
			{
				m_pWeapon					= pWeapon;//		Msg("- New m_pWeapon - %s[%d][0x%8x]", *pWeapon->cNameSect(), pWeapon->ID(), pWeapon);
				prevState					= pWeapon->State();

				if(m_pWeapon->ShowAmmo())
				{
					UIWeaponIcon.Show		(NULL!=pWeaponMagazined);

					UIWeaponSignAmmo.Show	(true);

					prevAmmoID				= m_pWeapon->m_ammoType;
					shared_str sect_name	= m_pWeapon->m_ammoTypes[m_pWeapon->m_ammoType];

					//properties used by inventory menu
					int iGridWidth			= pSettings->r_u32(sect_name, "inv_grid_width");
					int iGridHeight			= pSettings->r_u32(sect_name, "inv_grid_height");

					int iXPos				= pSettings->r_u32(sect_name, "inv_grid_x");
					int iYPos				= pSettings->r_u32(sect_name, "inv_grid_y");

					UIWeaponIcon.GetUIStaticItem().SetOriginalRect(	float(iXPos * INV_GRID_WIDTH),
						float(iYPos * INV_GRID_HEIGHT),
						float(iGridWidth * INV_GRID_WIDTH),
						float(iGridHeight * INV_GRID_HEIGHT));
					UIWeaponIcon.SetStretchTexture(true);

					// now perform only width scale for ammo, which (W)size >2
					// all others ammo (1x1, 1x2) will be not scaled (original picture)
					float w = ((iGridWidth>2)?1.6f:iGridWidth)*INV_GRID_WIDTH*0.9f;
					float h = INV_GRID_HEIGHT*0.9f;//1 cell
					UIWeaponIcon.SetWidth(w);
					UIWeaponIcon.SetHeight(h);
/*
					float _x = UIWeaponIcon_rect.x1;
					float _y = UIWeaponIcon_rect.y1;

					if(h<UIWeaponIcon_rect.height())
						_y			+= (UIWeaponIcon_rect.height()-h)/2.0f;

					if(w<UIWeaponIcon_rect.width())
						_x			+= (UIWeaponIcon_rect.width()-w)/2.0f;

					UIWeaponIcon.SetWndPos	(_x, _y);
*/

					string256 sItemName;
					strcpy(sItemName, pSettings->r_string(sect_name, "inv_name_short"));

					if (pWeaponMagazined && pWeaponMagazined->HasFireModes())
					{
						prevFireMode = pWeaponMagazined->GetCurrentFireMode();
						strcat(sItemName, pWeaponMagazined->GetCurrentFireModeStr());
					};

					UIWeaponBack.SetText	(sItemName);
				}
				else
				{
					UIWeaponIcon.Show			(false);
					UIWeaponSignAmmo.Show		(false);
				}
			}

			int	AE					= m_pWeapon->GetAmmoElapsed();
			int	AC					= m_pWeapon->GetAmmoCurrent();

			if((AE>=0)&&(AC>=0))
			{
				if (!m_pWeapon->unlimited_ammo())
					sprintf			(text_str, "%d/%d",AE,AC - AE);
				else
					sprintf			(text_str, "%d/--",AE);

				UIWeaponSignAmmo.SetText(text_str);
			}
		}
		else
		{
			m_pWeapon					= NULL; //Msg("* m_pWeapon = NULL");
			UIWeaponBack.SetText		("");
		}
	} 
	else
	{
		m_pWeapon					= NULL; //Msg("* m_pWeapon = NULL");
		UIWeaponBack.SetText		("");
	}


	//�������� ����������
	if(!m_pWeapon)
	{
		UIWeaponSignAmmo.SetText	("");
		UIWeaponIcon.Show			(false);
	}
    

    // radar
	UIZoneMap->UpdateRadar			(Device.vCameraPosition);
	// viewport
	float h,p;
	Device.vCameraDirection.getHP	(h,p);
	UIZoneMap->SetHeading			(-h);
		
	// health&armor
	UIHealthBar.SetProgressPos		(s16(m_pActor->GetfHealth()*100.0f));
	UIMotionIcon.SetPower			(s16(m_pActor->conditions().GetPower()*100.0f));
	
	
	EWarningIcons i					= ewiWeaponJammed;
		
	while (i < ewiInvincible)
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
		case ewiPsyHealth:
			value = 1 - m_pActor->conditions().GetPsyHealth();
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

		if (rit != m_Thresholds[i].rend()){
			float v = *rit;
			SetWarningIconColor(i, RGB_ALPHA(0xFF, clampr<u32>(static_cast<u32>(255 * ((v - min) / (max - min) * 2)), 0, 255), 
												   clampr<u32>(static_cast<u32>(255 * (2.0f - (v - min) / (max - min) * 2)), 0, 255),
												   0));
		}else
			TurnOffWarningIcon(i);

		i = (EWarningIcons)(i + 1);
	}

	UpdatePickUpItem				();
	CUIWindow::Update				();
}

bool CUIMainIngameWnd::OnKeyboardPress(int dik)
{
	test_key(dik);

	// ��������� ������ adjust hud mode
	bool flag = false;
	if (g_bHudAdjustMode)
	{
		CWeaponHUD *pWpnHud = NULL;
		if (m_pWeapon)
		{
			pWpnHud = m_pWeapon->GetHUD();
//			if (!pWpnHud) return false;
		}
		else
			return false;

		Fvector tmpV;

		if (1 == g_bHudAdjustMode) //zoom offset
		{
			if (!pWpnHud) return false;
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
			if(TRUE==m_pWeapon->GetHUDmode())
				tmpV = pWpnHud->FirePoint();
			else
				tmpV = m_pWeapon->vLoadedFirePoint;

		
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

			if(TRUE==m_pWeapon->GetHUDmode())
				Msg("weapon hud section:");
			else
				Msg("weapon section:");

				sprintf(tmpStr, "fire_point\t\t\t= %f,%f,%f",
					tmpV.x,
					tmpV.y,
					tmpV.z);
				Log(tmpStr);
				flag = true;
				break;
			}
#ifdef	DEBUG
			if(TRUE==m_pWeapon->GetHUDmode())
				pWpnHud->dbg_SetFirePoint(tmpV);
			else
				m_pWeapon->vLoadedFirePoint = tmpV;
#endif
		}
		else if (4 == g_bHudAdjustMode) //ShellPoint
		{
			if(TRUE==m_pWeapon->GetHUDmode())
				tmpV = pWpnHud->ShellPoint();
			else
				tmpV = m_pWeapon->vLoadedShellPoint;

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

			if(TRUE==m_pWeapon->GetHUDmode())
				Msg("weapon hud section:");
			else
				Msg("weapon section:");

				sprintf(tmpStr, "shell_point\t\t\t= %f,%f,%f",
					tmpV.x,
					tmpV.y,
					tmpV.z);
				Log(tmpStr);
				flag = true;
				break;
			}
#ifdef DEBUG
			if(TRUE==m_pWeapon->GetHUDmode())
				pWpnHud->dbg_SetShellPoint(tmpV);
			else
				m_pWeapon->vLoadedShellPoint = tmpV;

#endif
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
				Msg("LTX section [%s]",*CAttachableItem::m_dbgItem->item().object().cNameSect());
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
	g_bShowHudInfo = false;
}

void CUIMainIngameWnd::ShowAll()
{
	g_bShowHudInfo = true;
}


void CUIMainIngameWnd::AddInfoMessage(LPCSTR message)
{
	CUIStatic* pItem = UIInfoMessages.AddLogMessage(message);
	pItem->SetLightAnim("ui_main_msgs", false, true, true, true);
	pItem->SetTextAlignment(CGameFont::alCenter);
}

void CUIMainIngameWnd::RenderQuickInfos()
{
	if (!m_pActor)
		return;

	static CGameObject *pObject			= NULL;
	LPCSTR actor_action					= m_pActor->GetDefaultActionForObject();
	UIStaticQuickHelp.Show				(NULL!=actor_action);

	if(NULL!=actor_action){
		if(stricmp(actor_action,UIStaticQuickHelp.GetText()))
			UIStaticQuickHelp.SetText				(actor_action);
	}

	if (pObject!=m_pActor->ObjectWeLookingAt())
	{
		UIStaticQuickHelp.SetText				(actor_action);
		UIStaticQuickHelp.ResetAnimation		();
		pObject	= m_pActor->ObjectWeLookingAt	();
	}
}

void CUIMainIngameWnd::ReceiveNews(GAME_NEWS_DATA* news)
{
	if (g_bNewsDisable) return;

	if(news->texture_name.size())
		HUD().GetUI()->m_pMessagesWnd->AddIconedPdaMessage(*(news->texture_name), news->tex_rect, news->SingleLineText(), news->show_time);
	else
		HUD().GetUI()->m_pMessagesWnd->AddPdaMessage(news->SingleLineText(), news->show_time);
}

void CUIMainIngameWnd::SetWarningIconColor(CUIStatic* s, const u32 cl)
{
	int bOn = (cl>>24);
	bool bIsShown = s->IsShown();

	if(bOn)
		s->SetColor	(cl);

	if(bOn&&!bIsShown){
		m_UIIcons->AddWindow	(s, false);
		s->Show					(true);
	}

	if(!bOn&&bIsShown){
		m_UIIcons->RemoveWindow	(s);
		s->Show					(false);
	}
}

void CUIMainIngameWnd::SetWarningIconColor(EWarningIcons icon, const u32 cl)
{
	bool bMagicFlag = true;

	// ������ ���� ��������� ������
	switch(icon)
	{
	case ewiAll:
		bMagicFlag = false;
	case ewiWeaponJammed:
		SetWarningIconColor		(&UIWeaponJammedIcon, cl);
		if (bMagicFlag) break;
	case ewiRadiation:
		SetWarningIconColor		(&UIRadiaitionIcon, cl);
		if (bMagicFlag) break;
	case ewiWound:
		SetWarningIconColor		(&UIWoundIcon, cl);
		if (bMagicFlag) break;
	case ewiStarvation:
		SetWarningIconColor		(&UIStarvationIcon, cl);
		if (bMagicFlag) break;
	case ewiPsyHealth:
		SetWarningIconColor		(&UIPsyHealthIcon, cl);
		if (bMagicFlag) break;
	case ewiInvincible:
		SetWarningIconColor		(&UIInvincibleIcon, cl);
		if (bMagicFlag) break;
		break;
	case ewiSleep:
		SetWarningIconColor		(&UISleepIcon, cl);
		break;
	case ewiArtefact:
		SetWarningIconColor		(&UIArtefactIcon, cl);
		break;

	default:
		R_ASSERT(!"Unknown warning icon type");
	}
}

void CUIMainIngameWnd::TurnOffWarningIcon(EWarningIcons icon)
{
	SetWarningIconColor(icon, 0x00ffffff);
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::SetFlashIconState_(EFlashingIcons type, bool enable)
{
	// �������� �������� ��������� ������
	FlashingIcons_it icon = m_FlashingIcons.find(type);
	R_ASSERT2(icon != m_FlashingIcons.end(), "Flashing icon with this type not existed");
	icon->second->Show(enable);
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::InitFlashingIcons(CUIXml* node)
{
	const char * const flashingIconNodeName = "flashing_icon";
	int staticsCount = node->GetNodesNum("", 0, flashingIconNodeName);

	CUIXmlInit xml_init;
	CUIStatic *pIcon = NULL;
	// ����������� �� ���� ����� � �������������� �� ��� �������
	for (int i = 0; i < staticsCount; ++i)
	{
		pIcon = xr_new<CUIStatic>();
		xml_init.InitStatic(*node, flashingIconNodeName, i, pIcon);
		shared_str iconType = node->ReadAttrib(flashingIconNodeName, i, "type", "none");

		// ������ ���������� ������ � �� ���
		EFlashingIcons type = efiPdaTask;

		if		(iconType == "pda")		type = efiPdaTask;
		else if (iconType == "mail")	type = efiMail;
		else	R_ASSERT(!"Unknown type of mainingame flashing icon");

		R_ASSERT2(m_FlashingIcons.find(type) == m_FlashingIcons.end(), "Flashing icon with this type already exists");

		CUIStatic* &val	= m_FlashingIcons[type];
		val			= pIcon;

		AttachChild(pIcon);
		pIcon->Show(false);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::DestroyFlashingIcons()
{
	for (FlashingIcons_it it = m_FlashingIcons.begin(); it != m_FlashingIcons.end(); ++it)
	{
		DetachChild(it->second);
		xr_delete(it->second);
	}

	m_FlashingIcons.clear();
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::UpdateFlashingIcons()
{
	for (FlashingIcons_it it = m_FlashingIcons.begin(); it != m_FlashingIcons.end(); ++it)
	{
		it->second->Update();
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIMainIngameWnd::AnimateContacts(bool b_snd)
{
	UIPdaOnline.ResetAnimation	();

	if(b_snd)
		HUD_SOUND::PlaySound	(m_contactSnd, Fvector().set(0,0,0), 0, true );

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


/*
#include "d3dx9core.h"
#include "winuser.h"
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"d3d9.lib")
//dxerr9.lib dxguid.lib d3dx9d.lib d3d9.lib 
ID3DXFont*     g_pTestFont = NULL;
ID3DXFont*     g_pTestFont2 = NULL;
ID3DXSprite*        g_pTextSprite = NULL;   // Sprite for batching draw text calls
ID3DXSprite*        g_pTextSprite2 = NULL;   // Sprite for batching draw text calls
*/
/*
#include "UIGameTutorial.h"
#include "../actor_statistic_mgr.h"
CUIGameTutorial* g_tut = NULL;
*/
//#include "../postprocessanimator.h"
//CPostprocessAnimator* pp = NULL;
//extern void create_force_progress();

#include "UIDragDropListEx.h"
CTestDragDropWnd* w = NULL;
void test_key	(int dik)
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



/*
	if(dik==DIK_K){
		if(g_pTestFont){
			g_pTestFont->Release();
			g_pTestFont = NULL;
			g_pTestFont2->Release();
			g_pTestFont2 = NULL;
			
			g_pTextSprite2->Release();
			g_pTextSprite2->Release();
			return;
		}
	HRESULT hr;
	static int _height	= -12;
	static u32 _width	= 0;
	static u32 _weigth	= FW_BOLD;
	static BOOL _italic = FALSE;

    hr = D3DXCreateFont( HW.pDevice, _height, _width, _weigth, 1, _italic, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         "Times New Roman", &g_pTestFont );

	hr = D3DXCreateFont( HW.pDevice, _height, _width, FW_NORMAL, 1, TRUE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         "Times New Roman", &g_pTestFont2 );

	D3DXCreateSprite( HW.pDevice, &g_pTextSprite );
	D3DXCreateSprite( HW.pDevice, &g_pTextSprite2 );

	g_pTestFont->PreloadText("This is a trivial call to ID3DXFont::DrawText", xr_strlen("This is a trivial call to ID3DXFont::DrawText"));
	g_pTestFont2->PreloadText("This is a trivial call to ID3DXFont::DrawText", xr_strlen("This is a trivial call to ID3DXFont::DrawText"));
	}
*/
}

void test_draw	()
{
//	if(g_MissileForceShape)
//		g_MissileForceShape->Draw();

	/*
	if(g_pTestFont){

	g_pTestFont->PreloadText("This is a trivial call to ID3DXFont::DrawText", xr_strlen("This is a trivial call to ID3DXFont::DrawText"));
	g_pTestFont2->PreloadText("This is a trivial call to ID3DXFont::DrawText", xr_strlen("This is a trivial call to ID3DXFont::DrawText"));

		RECT rc;
        g_pTextSprite->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );
		SetRect( &rc, 50, 150, 0, 0 );        
		g_pTestFont->DrawText( g_pTextSprite, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 50, 200, 0, 0 );        
		g_pTestFont->DrawText( g_pTextSprite, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 50, 250, 0, 0 );        
		g_pTestFont->DrawText( g_pTextSprite, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 50, 300, 0, 0 );        
		g_pTestFont->DrawText( g_pTextSprite, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 50, 350, 0, 0 );        
		g_pTestFont->DrawText( g_pTextSprite, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 50, 400, 0, 0 );        
		g_pTestFont->DrawText( g_pTextSprite, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 50, 450, 0, 0 );        
		g_pTestFont->DrawText( g_pTextSprite, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 50, 500, 0, 0 );        
		g_pTestFont->DrawText( g_pTextSprite, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 50, 550, 0, 0 );        
		g_pTestFont->DrawText( g_pTextSprite, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 50, 600, 0, 0 );        
		g_pTestFont->DrawText( g_pTextSprite, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 50, 650, 0, 0 );        
		g_pTestFont->DrawText( g_pTextSprite, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 50, 700, 0, 0 );        
		g_pTestFont->DrawText( g_pTextSprite, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 50, 750, 0, 0 );        
		g_pTestFont->DrawText( g_pTextSprite, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		g_pTextSprite->End();

		static int aaa = 1;
		if(aaa){		
        g_pTextSprite2->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );
		SetRect( &rc, 450, 150, 0, 0 );        
		g_pTestFont2->DrawText( g_pTextSprite2, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 450, 200, 0, 0 );        
		g_pTestFont2->DrawText( g_pTextSprite2, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 450, 250, 0, 0 );        
		g_pTestFont2->DrawText( g_pTextSprite2, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 450, 300, 0, 0 );        
		g_pTestFont2->DrawText( g_pTextSprite2, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 450, 350, 0, 0 );        
		g_pTestFont2->DrawText( g_pTextSprite2, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 450, 400, 0, 0 );        
		g_pTestFont2->DrawText( g_pTextSprite2, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 450, 450, 0, 0 );        
		g_pTestFont2->DrawText( g_pTextSprite2, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 450, 500, 0, 0 );        
		g_pTestFont2->DrawText( g_pTextSprite2, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 450, 550, 0, 0 );        
		g_pTestFont2->DrawText( g_pTextSprite2, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 450, 600, 0, 0 );        
		g_pTestFont2->DrawText( g_pTextSprite2, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 450, 650, 0, 0 );        
		g_pTestFont2->DrawText( g_pTextSprite2, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 450, 700, 0, 0 );        
		g_pTestFont2->DrawText( g_pTextSprite2, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		SetRect( &rc, 450, 750, 0, 0 );        
		g_pTestFont2->DrawText( g_pTextSprite2, "This is a trivial call to ID3DXFont::DrawText", -1, &rc, 0x00000100, D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));
		g_pTextSprite2->End();
		}
	}
*/
}
