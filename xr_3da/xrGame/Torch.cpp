#include "stdafx.h"
#include "torch.h"
#include "entity.h"
#include "actor.h"
#include "../LightAnimLibrary.h"
#include "PhysicsShell.h"
#include "xrserver_objects_alife_items.h"
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

	return					(TRUE);
}

void CTorch::net_Destroy() 
{
	Switch					(false);
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
	Switch							(false);
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
