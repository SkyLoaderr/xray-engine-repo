#include "stdafx.h"
#include "torch.h"
#include "entity.h"
#include "actor.h"
#include "../LightAnimLibrary.h"
#include "PhysicsShell.h"
#include "xrserver_objects_alife_items.h"
#include "ai_sounds.h"

#include "HUDManager.h"
#include "level.h"


const float TIME_2_HIDE		= 5.f;


CTorch::CTorch(void) 
{
	m_weight					= .5f;
	m_belt						= true;
	light_render				= ::Render->light_create();
	light_render->set_type		(IRender_Light::SPOT);
	light_render->set_shadow	(true);
	glow_render					= ::Render->glow_create();
	lanim						= 0;
	time2hide					= 0;
	fBrightness					= 1.f;

	m_NightVisionRechargeTime		= 6.f;
	m_NightVisionRechargeTimeMin	= 2.f;
	m_NightVisionDischargeTime		= 10.f;
	m_NightVisionChargeTime			= 0.f;
}

CTorch::~CTorch(void) 
{
	::Render->light_destroy	(light_render);
	::Render->glow_destroy	(glow_render);
	xr_delete				(collidable.model);

	HUD_SOUND::DestroySound(m_NightVisionOnSnd);
	HUD_SOUND::DestroySound(m_NightVisionOffSnd);
	HUD_SOUND::DestroySound(m_NightVisionIdleSnd);
}

void CTorch::Load(LPCSTR section) 
{
	inherited::Load			(section);
	m_pos					= pSettings->r_fvector3(section,"position");
	light_trace_bone		= pSettings->r_string(section,"light_trace_bone");


	m_bNightVisionEnabled = !!pSettings->r_bool(section,"night_vision");
	if(m_bNightVisionEnabled)
	{
		LPCSTR texture = pSettings->r_string(section,"night_vision_texture");
		m_NightVisionTexture.Init(texture, "hud\\default", 0, 0, alNone);

		// Load attack postprocess --------------------------------------------------------
		LPCSTR ppi_section = pSettings->r_string(section,"night_vision_effector");
		m_NightVisionEffector.Load(ppi_section);


		//// Sounds
		if(pSettings->line_exist(section, "snd_night_vision_on"))
			HUD_SOUND::LoadSound(section,"snd_night_vision_on"	, m_NightVisionOnSnd	, TRUE, SOUND_TYPE_ITEM_USING);
		if(pSettings->line_exist(section, "snd_night_vision_off"))
			HUD_SOUND::LoadSound(section,"snd_night_vision_off"	, m_NightVisionOffSnd	, TRUE, SOUND_TYPE_ITEM_USING);
		if(pSettings->line_exist(section, "snd_night_vision_idle"))
			HUD_SOUND::LoadSound(section,"snd_night_vision_idle", m_NightVisionIdleSnd	, TRUE, SOUND_TYPE_ITEM_USING);

		m_NightVisionRechargeTime		= pSettings->r_float(section,"night_vision_recharge_time");
		m_NightVisionRechargeTimeMin	= pSettings->r_float(section,"night_vision_recharge_time_min");
		m_NightVisionDischargeTime		= pSettings->r_float(section,"night_vision_discharge_time");
		m_NightVisionChargeTime			= m_NightVisionRechargeTime;
	}
}

void CTorch::SwitchNightVision()
{
	SwitchNightVision(!m_bNightVisionOn);
}

void CTorch::SwitchNightVision(bool vision_on)
{
	if(!m_bNightVisionEnabled) return;
	
	if(vision_on && m_NightVisionChargeTime > m_NightVisionRechargeTimeMin)
	{
		m_NightVisionChargeTime = m_NightVisionDischargeTime*m_NightVisionChargeTime/m_NightVisionRechargeTime;
		m_bNightVisionOn = true;
	}
	else
		m_bNightVisionOn = false;

	if(m_bNightVisionOn)
	{
		CActor *pA = smart_cast<CActor *>(H_Parent());
		if (pA) 
		{
			bool result = m_NightVisionEffector.Start();
			if(result)
			{
				HUD_SOUND::PlaySound(m_NightVisionOnSnd, pA->Position(), pA, true);
				HUD_SOUND::PlaySound(m_NightVisionIdleSnd, pA->Position(), pA, true, true);
			}
		}
	}
	else
	{
		CActor *pA = smart_cast<CActor *>(H_Parent());
		if(pA)
		{
			bool result = m_NightVisionEffector.Stop();
			if(result)
			{
				HUD_SOUND::PlaySound(m_NightVisionOffSnd, pA->Position(), pA, true);
				HUD_SOUND::StopSound(m_NightVisionIdleSnd);
			}
		}
	}
}


void CTorch::UpdateSwitchNightVision   ()
{
	if(!m_bNightVisionEnabled) return;


	if(m_bNightVisionOn)
	{
		m_NightVisionChargeTime			-= Device.fTimeDelta;

		if(m_NightVisionChargeTime<0.f)
			SwitchNightVision(false);
	}
	else
	{
		m_NightVisionChargeTime			+= Device.fTimeDelta;
		clamp(m_NightVisionChargeTime, 0.f, m_NightVisionRechargeTime);
	}

	if(m_NightVisionEffector.IsActive())
	{
		if(m_NightVisionTexture.GetShader())
			HUD().GetUI()->UIMainIngameWnd.AddStaticItem(&m_NightVisionTexture, 0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);
	}
}

float  CTorch::NightVisionBattery ()
{
	return m_NightVisionChargeTime/m_NightVisionRechargeTime;
}

void CTorch::Switch()
{
	bool bActive			= !light_render->get_active();
	light_render->set_active(bActive);
	glow_render->set_active	(bActive);

	if(*light_trace_bone)
	{
		CKinematics* pVisual = PKinematics(Visual()); VERIFY(pVisual);
		pVisual->LL_SetBoneVisible(pVisual->LL_BoneID(light_trace_bone),bActive,TRUE);
	}
}

void CTorch::Switch	(bool light_on)
{
	light_render->set_active(light_on);
	glow_render->set_active	(light_on);

	if(*light_trace_bone)
	{
		CKinematics* pVisual = PKinematics(Visual()); VERIFY(pVisual);
		pVisual->LL_SetBoneVisible(pVisual->LL_BoneID(light_trace_bone),light_on,TRUE);
	}
}

BOOL CTorch::net_Spawn(LPVOID DC) 
{
	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeItemTorch		*torch	= smart_cast<CSE_ALifeItemTorch*>(e);
	R_ASSERT				(torch);
	cNameVisual_set			(torch->get_visual());

	R_ASSERT				(!CFORM());
	R_ASSERT				(PKinematics(Visual()));
	collidable.model		= xr_new<CCF_Skeleton>	(this);

	if (!inherited::net_Spawn(DC))
		return				(FALSE);
	
	CKinematics* K			= PKinematics(Visual());
	CInifile* pUserData		= K->LL_UserData(); 
	R_ASSERT3				(pUserData,"Empty Torch user data!",torch->get_visual());
	lanim					= LALib.FindItem(pUserData->r_string("torch_definition","color_animator"));
	guid_bone				= K->LL_BoneID	(pUserData->r_string("torch_definition","guide_bone"));	VERIFY(guid_bone!=BI_NONE);
	Fcolor clr				= pUserData->r_fcolor				("torch_definition","color");
	fBrightness				= clr.intensity();
	light_render->set_color	(clr);
	light_render->set_range	(pUserData->r_float					("torch_definition","range"));
	light_render->set_cone	(deg2rad(pUserData->r_float			("torch_definition","spot_angle")));
	light_render->set_texture(pUserData->r_string				("torch_definition","spot_texture"));

	glow_render->set_texture(pUserData->r_string				("torch_definition","glow_texture"));
	glow_render->set_color	(clr);
	glow_render->set_radius	(pUserData->r_float					("torch_definition","glow_radius"));

	//включить/выключить фонарик
	Switch					(torch->m_active);
	VERIFY					(!torch->m_active || (torch->ID_Parent != 0xffff));
	
	SwitchNightVision		(false);

	return					(TRUE);
}

void CTorch::net_Destroy() 
{
	Switch					(false);
	SwitchNightVision		(false);

	inherited::net_Destroy	();
	xr_delete				(collidable.model);
}

void CTorch::OnH_A_Chield() 
{
	inherited::OnH_A_Chield			();
//	H_Root()->Center				(Position());
//	XFORM().c.set					(Position());
	m_focus.set						(Position());
	// commented by Dima
//	Switch							(false);
	// end of comments
}

void CTorch::OnH_B_Independent() 
{
	inherited::OnH_B_Independent	();
	time2hide						= TIME_2_HIDE;

	Switch					(false);
	SwitchNightVision		(false);

	HUD_SOUND::StopSound(m_NightVisionOnSnd);
	HUD_SOUND::StopSound(m_NightVisionOffSnd);
	HUD_SOUND::StopSound(m_NightVisionIdleSnd);

	m_NightVisionChargeTime = m_NightVisionRechargeTime;
}

void CTorch::UpdateCL	() 
{
	inherited::UpdateCL	();

	UpdateSwitchNightVision   ();

	if (light_render->get_active())
	{


		CBoneInstance& BI = PKinematics(Visual())->LL_GetBoneInstance(guid_bone);
		Fmatrix M;

		if (H_Parent()) {
			PKinematics(H_Parent()->Visual())->CalculateBones	();
			M.mul						(XFORM(),BI.mTransform);
			light_render->set_rotation	(M.k,M.i);
			light_render->set_position	(M.c);
			glow_render->set_position	(M.c);
			glow_render->set_direction	(M.k);
		}
		else
		{
			if(getVisible() && m_pPhysicsShell) {
				M.mul					(XFORM(),BI.mTransform);

				if (light_render->get_active()){
					light_render->set_rotation	(M.k,M.i);
					light_render->set_position	(M.c);
					glow_render->set_position	(M.c);
					glow_render->set_direction	(M.k);

					time2hide			-= Device.fTimeDelta;
					if (time2hide<0){
						light_render->set_active(false);
						glow_render->set_active(false);
					}
				}
			} 
		}

		// calc color animator
		if (lanim){
			int frame;
			// возвращает в формате BGR
			u32 clr			= lanim->CalculateBGR(Device.fTimeGlobal,frame); 

			Fcolor			fclr;
			fclr.set		((float)color_get_B(clr),(float)color_get_G(clr),(float)color_get_R(clr),1.f);
			fclr.mul_rgb	(fBrightness/255.f);
			light_render->set_color(fclr);
			glow_render->set_color(fclr);
		}
	}
}

void CTorch::renderable_Render	() 
{
	inherited::renderable_Render();
}

void CTorch::reinit				()
{
	inherited::reinit			();
}

void CTorch::reload				(LPCSTR section)
{
	inherited::reload			(section);
}

void CTorch::create_physic_shell()
{
	CPhysicsShellHolder::create_physic_shell();
}

void CTorch::activate_physic_shell()
{
	CPhysicsShellHolder::activate_physic_shell();
}

void CTorch::setup_physic_shell	()
{
	CPhysicsShellHolder::setup_physic_shell();
}

void CTorch::net_Export			(NET_Packet& P)
{
	inherited::net_Export		(P);
	P.w_u8						(light_render->get_active() ? 1 : 0);
}

void CTorch::net_Import			(NET_Packet& P)
{
	inherited::net_Import		(P);
	light_render->set_active	(!!P.r_u8());
}
