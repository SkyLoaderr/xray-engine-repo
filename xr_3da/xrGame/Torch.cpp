#include "stdafx.h"
#include "torch.h"
#include "entity.h"

CTorch::CTorch(void) {
	m_weight = .5f;
	m_belt = true;
	light_render			= ::Render->light_create();
	light_render->set_type	(IRender_Light::SPOT);
	light_render->set_shadow(true);
}

CTorch::~CTorch(void) {
	::Render->light_destroy	(light_render);
}

void CTorch::Load(LPCSTR section) {
	inherited::Load(section);
	m_pos = pSettings->r_fvector3(section,"position");
}

BOOL CTorch::net_Spawn(LPVOID DC) {
	BOOL res = inherited::net_Spawn(DC);

	CKinematics* V = PKinematics(Visual());
	if(V) V->PlayCycle("idle");

	light_render->set_range	(300.f);
	light_render->set_color	(1.f,.8f,1.f);
	light_render->set_cone	(PI_DIV_3);
	light_render->set_texture(0);

	if (0==m_pPhysicsShell) {
		// Physics (Box)
		Fobb obb; Visual()->vis.box.get_CD(obb.m_translate,obb.m_halfsize); obb.m_rotate.identity();
		// Physics (Elements)
		CPhysicsElement* E = P_create_Element(); R_ASSERT(E); E->add_Box(obb);
		// Physics (Shell)
		m_pPhysicsShell = P_create_Shell(); R_ASSERT(m_pPhysicsShell);
		m_pPhysicsShell->add_Element(E);
		m_pPhysicsShell->setDensity(2000.f);
		if(!H_Parent())m_pPhysicsShell->Activate(svXFORM(),0,svXFORM());
		m_pPhysicsShell->mDesired.identity();
		m_pPhysicsShell->fDesiredStrength = 0.f;
	}

	setVisible(true);
	setEnabled(true);

	return res;
}

void CTorch::net_Destroy() {
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete(m_pPhysicsShell);
	inherited::net_Destroy();
}

void CTorch::OnH_A_Chield() {
	inherited::OnH_A_Chield		();
	setVisible(false);
	setEnabled(false);
	svTransform.c.set(m_pos);
	vPosition.set(svTransform.c);
	light_render->set_active(true);
}

void CTorch::OnH_B_Independent() {
	inherited::OnH_B_Independent();
	setVisible(true);
	setEnabled(true);
	CObject* E = dynamic_cast<CObject*>(H_Parent()); R_ASSERT(E);
	svTransform.set(E->clXFORM());
	vPosition.set(svTransform.c);
	if(m_pPhysicsShell) {
		Fmatrix trans;
		Level().Cameras.unaffected_Matrix(trans);
		Fvector l_fw; l_fw.set(trans.k);
		Fvector l_up; l_up.set(svTransform.j); l_up.mul(2.f);
		Fmatrix l_p1, l_p2;
		l_p1.set(svTransform); l_p1.c.add(l_up); l_up.mul(1.2f); 
		l_p2.set(svTransform); l_p2.c.add(l_up); l_fw.mul(3.f); l_p2.c.add(l_fw);
		m_pPhysicsShell->Activate(l_p1, 0, l_p2);
		svTransform.set(l_p1);
		vPosition.set(svTransform.c);
	}
	light_render->set_active(false);
}

void CTorch::UpdateCL() {
	inherited::UpdateCL();
	if(getVisible() && m_pPhysicsShell) {
		m_pPhysicsShell->Update	();
		svTransform.set			(m_pPhysicsShell->mXFORM);
		vPosition.set			(svTransform.c);
	} else if(H_Parent()) {
		Collide::ray_query RQ;
		H_Parent()->setEnabled(false);
		Fvector l_p, l_d; dynamic_cast<CEntity*>(H_Parent())->g_fireParams(l_p,l_d);
		//Fmatrix l_cam; Level().Cameras.unaffected_Matrix(l_cam);
		if(Level().ObjectSpace.RayPick(l_p, l_d, 500.f, RQ)) {
			Fvector l_end, l_up; l_end.mad(l_p, l_d, RQ.range); l_up.set(0, 1.f, 0);
			svTransform.k.sub(l_end, l_p); svTransform.k.normalize();
			svTransform.i.crossproduct(l_up, svTransform.k); svTransform.i.normalize();
			svTransform.j.crossproduct(svTransform.k, svTransform.i);
		}
		H_Parent()->setEnabled(true);
	}
}

void CTorch::OnVisible() {
	if(getVisible() && !H_Parent()) {
		::Render->set_Transform		(&clTransform);
		::Render->add_Visual		(Visual());
	} else {
		light_render->set_direction	(clTransform.k);
		light_render->set_position	(clTransform.c);
	}
}
