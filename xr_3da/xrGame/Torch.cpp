#include "stdafx.h"
#include "torch.h"
#include "entity.h"
#include "..\LightAnimLibrary.h"

const float TIME_2_HIDE		= 5.f;

CTorch::CTorch(void) 
{
	m_weight				= .5f;
	m_belt					= true;
	light_render			= ::Render->light_create();
	light_render->set_type	(IRender_Light::SPOT);
	light_render->set_shadow(true);
	lanim					= 0;
	time2hide				= 0;
}

CTorch::~CTorch(void) 
{
	::Render->light_destroy	(light_render);
}

void CTorch::Load(LPCSTR section) 
{
	inherited::Load(section);
	m_pos = pSettings->r_fvector3(section,"position");
}

void CTorch::Switch()
{
	light_render->set_active(!light_render->get_active());
}

BOOL CTorch::net_Spawn(LPVOID DC) 
{
	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeItemTorch		*torch	= dynamic_cast<CSE_ALifeItemTorch*>(e);
	R_ASSERT				(torch);
	cNameVisual_set			(torch->get_visual());
	inherited::net_Spawn	(DC);

	R_ASSERT(!CFORM());
	if (PKinematics(Visual()))	collidable.model	= xr_new<CCF_Skeleton>	(this);
	else						collidable.model	= xr_new<CCF_Rigid>		(this);

	// set bone id
	Fcolor					clr;
	clr.set					(torch->color);
	clr.mul_rgb				(torch->spot_brightness);
	fBrightness				= torch->spot_brightness;
	light_render->set_range	(torch->spot_range);
	light_render->set_color	(clr);
	light_render->set_cone	(torch->spot_cone_angle);
	light_render->set_texture(torch->spot_texture[0]?torch->spot_texture:0);

	R_ASSERT				(Visual());
	lanim					= LALib.FindItem(torch->animator);

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
	setVisible				(false);
	setEnabled				(false);
	H_Root()->Center		(Position());
	XFORM().c.set		(Position());
	m_focus.set				(Position());
	if(m_pPhysicsShell)		m_pPhysicsShell->Deactivate();
}

void CTorch::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
	setVisible(true);
	setEnabled(true);
	CObject* E = dynamic_cast<CObject*>(H_Parent()); R_ASSERT(E);
	XFORM().set(E->XFORM());
	Position().set(XFORM().c);
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
		Position().set(XFORM().c);
	}
	time2hide				= TIME_2_HIDE;
}

void CTorch::UpdateCL() 
{
	inherited::UpdateCL();
	if(getVisible() && m_pPhysicsShell) {
		m_pPhysicsShell->Update	();
		XFORM().set				(m_pPhysicsShell->mXFORM);
		Position().set			(XFORM().c);
		if (light_render->get_active()){
			light_render->set_direction	(XFORM().k);
			light_render->set_position	(XFORM().c);
			time2hide			-= Device.fTimeDelta;
			if (time2hide<0)	light_render->set_active(false);
		}
	} else if(H_Parent()) {
		Collide::ray_query RQ;
		H_Parent()->setEnabled(false);
		Fvector l_p, l_d; dynamic_cast<CEntity*>(H_Parent())->g_fireParams(l_p,l_d);
		//Fmatrix l_cam; Level().Cameras.unaffected_Matrix(l_cam);
		Fvector l_end, l_up; 
		if(Level().ObjectSpace.RayPick(l_p, l_d, 50.f, RQ)) {
			l_end.mad(l_p, l_d, RQ.range); l_up.set(0, 1.f, 0);
			XFORM().k.sub(l_end, l_p); XFORM().k.normalize();
			XFORM().i.crossproduct(l_up, XFORM().k); XFORM().i.normalize();
			XFORM().j.crossproduct(XFORM().k, XFORM().i);
		}
		m_focus.inertion(l_end,1-Device.fTimeDelta*4);

		Fvector _P,_D;
		_P.mad		(l_p,Device.vCameraRight,	m_pos.x);
		_P.mad		(Device.vCameraTop,			m_pos.y);
		_D.sub		(m_focus,_P);
		_D.normalize();
		H_Parent()->setEnabled		(true);
		light_render->set_direction	(_D);	//XFORM().k); // l_d
		light_render->set_position	(_P);	//XFORM().c); // l_p
	}
	// update light source
	if (light_render->get_active()){
		// calc color animator
		if (lanim){
			int frame;
			u32 clr			= lanim->Calculate(Device.fTimeGlobal,frame); // возвращает в формате BGR
			Fcolor			fclr;
			fclr.set		((float)color_get_B(clr),(float)color_get_G(clr),(float)color_get_R(clr),1.f);
			fclr.mul_rgb	(fBrightness/255.f);
			light_render->set_color(fclr);
		}
	}
}

void CTorch::renderable_Render() 
{
	if(getVisible() && !H_Parent()) {
		::Render->set_Transform		(&XFORM());
		::Render->add_Visual		(Visual());
	} else {
		light_render->set_direction	(XFORM().k);
		light_render->set_position	(XFORM().c);
	}
}
