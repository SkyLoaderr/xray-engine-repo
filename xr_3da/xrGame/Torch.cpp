#include "stdafx.h"
#include "torch.h"

CTorch::CTorch(void) {
	m_weight = .5f;
	m_belt = true;
}

CTorch::~CTorch(void) {
}

BOOL CTorch::net_Spawn(LPVOID DC) {
	BOOL res = inherited::net_Spawn(DC);

	CKinematics* V = PKinematics(Visual());
	if(V) V->PlayCycle("idle");

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
	setVisible					(false);
	setEnabled					(false);
}

void CTorch::OnH_B_Independent() {
	inherited::OnH_B_Independent();
	setVisible					(true);
	setEnabled					(true);
}

void CTorch::UpdateCL() {
	inherited::UpdateCL();
	if(getVisible() && m_pPhysicsShell) {
		m_pPhysicsShell->Update	();
		svTransform.set			(m_pPhysicsShell->mXFORM);
		vPosition.set			(svTransform.c);
	}
}

void CTorch::OnVisible() {
	if(getVisible() && !H_Parent()) {
		::Render->set_Transform		(&clTransform);
		::Render->add_Visual		(Visual());
	}
}
