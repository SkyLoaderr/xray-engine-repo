#include "stdafx.h"
#include "torch.h"
#include "entity.h"
#include "actor.h"
#include "../LightAnimLibrary.h"
#include "PhysicsShell.h"
const float TIME_2_HIDE		= 5.f;

CTorch::CTorch(void) 
{
	m_weight				= .5f;
	m_belt					= true;
	light_render			= ::Render->light_create();
	light_render->set_type	(IRender_Light::SPOT);
	light_render->set_shadow(true);
	glow_render				= ::Render->glow_create();
	lanim					= 0;
	time2hide				= 0;
}

CTorch::~CTorch(void) 
{
	::Render->light_destroy	(light_render);
	::Render->glow_destroy	(glow_render);
}

void CTorch::Load(LPCSTR section) 
{
	inherited::Load(section);
	m_pos		= pSettings->r_fvector3(section,"position");
}

void CTorch::Switch()
{
	bool bActive			= !light_render->get_active();
	light_render->set_active(bActive);
	glow_render->set_active	(bActive);
}

void CTorch::Switch	(bool light_on)
{
	light_render->set_active(light_on);
	glow_render->set_active	(light_on);
}

BOOL CTorch::net_Spawn(LPVOID DC) 
{
	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeItemTorch		*torch	= dynamic_cast<CSE_ALifeItemTorch*>(e);
	R_ASSERT				(torch);
	cNameVisual_set			(torch->get_visual());
	inherited::net_Spawn	(DC);

	R_ASSERT(!CFORM());
	R_ASSERT(PKinematics(Visual()));
	collidable.model		= xr_new<CCF_Skeleton>	(this);

	// set bone id
	Fcolor					clr;
	clr.set					(torch->color);
	clr.mul_rgb				(torch->spot_brightness);
	fBrightness				= torch->spot_brightness;
	light_render->set_range	(torch->spot_range);
	light_render->set_color	(clr);
	light_render->set_cone	(torch->spot_cone_angle);
	light_render->set_texture(torch->spot_texture[0]?torch->spot_texture:0);

	glow_render->set_texture(torch->glow_texture[0]?torch->glow_texture:0);
	glow_render->set_color	(clr);
	glow_render->set_radius	(torch->glow_radius);

	R_ASSERT				(Visual());
	lanim					= LALib.FindItem(torch->animator);

	guid_bone				= torch->guid_bone; VERIFY(guid_bone!=BI_NONE);

	VERIFY					(m_pPhysicsShell);
	CSE_Abstract *l_pE = (CSE_Abstract*)DC;
	if(l_pE->ID_Parent==0xffff) m_pPhysicsShell->Activate(XFORM(),0,XFORM());
	/*
	if (0==m_pPhysicsShell) {
		
		// Physics (Box)
		Fobb obb; Visual()->vis.box.get_CD(obb.m_translate,obb.m_halfsize); obb.m_rotate.identity();
		// Physics (Elements)
		CPhysicsElement* E = P_create_Element(); R_ASSERT(E); E->add_Box(obb);
		// Physics (Shell)
		m_pPhysicsShell = P_create_Shell(); R_ASSERT(m_pPhysicsShell);
		m_pPhysicsShell->add_Element(E);
		m_pPhysicsShell->setDensity(2000.f);
		CSE_Abstract *l_pE = (CSE_Abstract*)DC;
		if(l_pE->ID_Parent==0xffff) m_pPhysicsShell->Activate(XFORM(),0,XFORM());
		m_pPhysicsShell->mDesired.identity();
		m_pPhysicsShell->fDesiredStrength = 0.f;

	}
	*/
	setVisible(true);
	setEnabled(true);

	return TRUE;
}

void CTorch::net_Destroy() 
{
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete(m_pPhysicsShell);
	inherited::net_Destroy();
}

void CTorch::OnH_A_Chield() 
{
	inherited::OnH_A_Chield	();
	H_Root()->Center		(Position());
	XFORM().c.set			(Position());
	m_focus.set				(Position());
	if(m_pPhysicsShell)		m_pPhysicsShell->Deactivate();
	const CInventoryOwner	*inventory_owner = dynamic_cast<const CInventoryOwner*>(H_Parent());
	VERIFY					(inventory_owner);
	if (inventory_owner->use_torch())
		setVisible			(true);
}

void CTorch::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
	const CObject* E = dynamic_cast<const CObject*>(H_Parent()); R_ASSERT(E);
	XFORM().set(E->XFORM());
	if(m_pPhysicsShell) {
		Fmatrix trans;
		Level().Cameras.unaffected_Matrix(trans);
		Fvector l_fw; l_fw.set(trans.k);
		Fvector l_up; l_up.set(XFORM().j); l_up.mul(2.f);
		Fmatrix l_p1, l_p2;
		l_p1.set(XFORM()); l_p1.c.add(l_up); l_up.mul(1.2f); 
		l_p2.set(XFORM()); l_p2.c.add(l_up); l_fw.mul(3.f); l_p2.c.add(l_fw);
		m_pPhysicsShell->Activate(l_p1, 0, l_p2);
		XFORM().set(l_p1);
	}
	time2hide				= TIME_2_HIDE;
}

void CTorch::UpdateCL() 
{
	inherited::UpdateCL();
	
	CBoneInstance& BI = PKinematics(Visual())->LL_GetBoneInstance(guid_bone);
	Fmatrix M;

	if (H_Parent()) 
	{
		M.mul(XFORM(),BI.mTransform);
		light_render->set_direction	(M.k);
		light_render->set_position	(M.c);
		glow_render->set_position	(M.c);

		if(light_render->get_active())
		{
			CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());
			if(pActor && pActor->HUDview())
				glow_render->set_active(false);
			else
				glow_render->set_active(true);
		}
	}
	else if(getVisible() && m_pPhysicsShell) 
	{
		m_pPhysicsShell->Update	();
		XFORM().set				(m_pPhysicsShell->mXFORM);
		M.mul(XFORM(),BI.mTransform);

		if (light_render->get_active()){
			light_render->set_direction	(M.k);
			light_render->set_position	(M.c);
			glow_render->set_position	(M.c);
			time2hide			-= Device.fTimeDelta;
			if (time2hide<0){
				light_render->set_active(false);
				glow_render->set_active(false);
			}
		}
	} 
	
	// update light source
	if (light_render->get_active()){
		// calc color animator
		if (lanim){
			int frame;
			// возвращает в формате BGR
			u32 clr			= lanim->Calculate(Device.fTimeGlobal,frame); 
		
			Fcolor			fclr;
			fclr.set		((float)color_get_B(clr),(float)color_get_G(clr),(float)color_get_R(clr),1.f);
			fclr.mul_rgb	(fBrightness/255.f);
			light_render->set_color(fclr);
			glow_render->set_color(fclr);
		}
	}
}

void CTorch::renderable_Render() 
{
	if(getVisible())
	{
		::Render->set_Transform		(&XFORM());
		::Render->add_Visual		(Visual());
/*	 
		if(H_Parent()) 
		{
			light_render->set_direction	(M.k);
			light_render->set_position	(XFORM().c);
			glow_render->set_position	(XFORM().c);
		}
*/
	}
}
