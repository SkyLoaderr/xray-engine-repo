#include "stdafx.h"
#include "torch.h"
#include "entity.h"
#include "actor.h"
#include "../LightAnimLibrary.h"
#include "PhysicsShell.h"
#include "xrserver_objects_alife_items.h"
const float TIME_2_HIDE		= 5.f;

#include "level.h"
#include "../CameraManager.h"
#include "ai/ai_monster_effector.h"

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

	m_pNightVision				= NULL;
}

CTorch::~CTorch(void) 
{
	::Render->light_destroy	(light_render);
	::Render->glow_destroy	(glow_render);
	xr_delete				(collidable.model);
}

void CTorch::Load(LPCSTR section) 
{
	inherited::Load			(section);
	m_pos					= pSettings->r_fvector3(section,"position");
	light_trace_bone		= pSettings->r_string(section,"light_trace_bone");


	m_bNightVisionEnabled = !!pSettings->r_bool(section,"night_vision");
	if(m_bNightVisionEnabled)
	{
		m_sNightVisionTexture = pSettings->r_string(section,"night_vision_texture");
		
		// Load attack postprocess --------------------------------------------------------
		LPCSTR ppi_section = pSettings->r_string(section,"night_vision_effector");
		m_NightVisionEffector.ppi.duality.h		= pSettings->r_float(ppi_section,"duality_h");
		m_NightVisionEffector.ppi.duality.v		= pSettings->r_float(ppi_section,"duality_v");
		m_NightVisionEffector.ppi.gray				= pSettings->r_float(ppi_section,"gray");
		m_NightVisionEffector.ppi.blur				= pSettings->r_float(ppi_section,"blur");
		m_NightVisionEffector.ppi.noise.intensity	= pSettings->r_float(ppi_section,"noise_intensity");
		m_NightVisionEffector.ppi.noise.grain		= pSettings->r_float(ppi_section,"noise_grain");
		m_NightVisionEffector.ppi.noise.fps		= pSettings->r_float(ppi_section,"noise_fps");
		VERIFY(!fis_zero(m_NightVisionEffector.ppi.noise.fps));

		sscanf(pSettings->r_string(ppi_section,"color_base"),	"%f,%f,%f", &m_NightVisionEffector.ppi.color_base.r, &m_NightVisionEffector.ppi.color_base.g, &m_NightVisionEffector.ppi.color_base.b);
		sscanf(pSettings->r_string(ppi_section,"color_gray"),	"%f,%f,%f", &m_NightVisionEffector.ppi.color_gray.r, &m_NightVisionEffector.ppi.color_gray.g, &m_NightVisionEffector.ppi.color_gray.b);
		sscanf(pSettings->r_string(ppi_section,"color_add"),	"%f,%f,%f", &m_NightVisionEffector.ppi.color_add.r,  &m_NightVisionEffector.ppi.color_add.g,  &m_NightVisionEffector.ppi.color_add.b);

		m_NightVisionEffector.time			= pSettings->r_float(ppi_section,"time");
		m_NightVisionEffector.time_attack	= pSettings->r_float(ppi_section,"time_attack");
		m_NightVisionEffector.time_release	= pSettings->r_float(ppi_section,"time_release");

		m_NightVisionEffector.ce_time			= pSettings->r_float(ppi_section,"ce_time");
		m_NightVisionEffector.ce_amplitude		= pSettings->r_float(ppi_section,"ce_amplitude");
		m_NightVisionEffector.ce_period_number	= pSettings->r_float(ppi_section,"ce_period_number");
		m_NightVisionEffector.ce_power			= pSettings->r_float(ppi_section,"ce_power");

		// --------------------------------------------------------------------------------
	}
}

void CTorch::SwitchNightVision()
{
	SwitchNightVision(!m_bNightVisionOn);
}

void CTorch::SwitchNightVision(bool vision_on)
{
	if(!m_bNightVisionEnabled) return;
	m_bNightVisionOn = vision_on;

	if(m_bNightVisionOn)
	{
		CActor *pA = smart_cast<CActor *>(H_Parent());
		if (pA) 
			m_pNightVision = xr_new<CMonsterEffector>(m_NightVisionEffector.ppi, m_NightVisionEffector.time, m_NightVisionEffector.time_attack, m_NightVisionEffector.time_release);
			Level().Cameras.AddEffector(m_pNightVision);
	}
	else
	{
		//m_pNightVision->fLifeTime = m_pNightVision->m_total*m_pNightVision->m_release;
		m_pNightVision = NULL;
		//Level().Cameras.RemoveEffector(xr_new<CMonsterEffector>(get_sd()->m_attack_effector.ppi, get_sd()->m_attack_effector.time, m_NightVisionEffector.time_attack, m_NightVisionEffector.time_release));
	}
}


void CTorch::UpdateSwitchNightVision   ()
{
//	if(m_pNightVision)
//		m_pNightVision->fLifeTime = flt_max;
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
}

void CTorch::UpdateCL	() 
{
	inherited::UpdateCL	();

	// Oles: no need to perform lighting-update for turned-off lights
	// Oles: esp. the "CalculateBones" is slow
	if (!light_render->get_active())	return;	

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

	UpdateSwitchNightVision   ();
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
