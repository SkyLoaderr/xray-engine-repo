#include "stdafx.h"
#include "targetcs.h"
#include "targetcscask.h"

CTargetCS::CTargetCS(void)
{
	m_pPhysicsShell			= 0;
}

CTargetCS::~CTargetCS(void)
{
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete					(m_pPhysicsShell);
}

BOOL CTargetCS::net_Spawn(LPVOID DC) {
	inherited::net_Spawn(DC);
	setVisible					(true);
	setEnabled					(true);
	Game().targets.push_back	(this);

	CKinematics* V = PKinematics(Visual());
	if(V) V->PlayCycle("idle");

	if (0==m_pPhysicsShell)
	{
		// Physics (Box)
		Fobb								obb;
		Visual()->vis.box.get_CD			(obb.m_translate,obb.m_halfsize);
		obb.m_rotate.identity				();

		// Physics (Elements)
		CPhysicsElement* E					= P_create_Element	();
		R_ASSERT							(E);
		E->add_Box							(obb);

		// Physics (Shell)
		m_pPhysicsShell						= P_create_Shell	();
		R_ASSERT							(m_pPhysicsShell);
		m_pPhysicsShell->add_Element		(E);
		m_pPhysicsShell->setDensity			(10.f);
		m_pPhysicsShell->Activate			(XFORM(),0,XFORM());
		m_pPhysicsShell->mDesired.identity	();
		m_pPhysicsShell->fDesiredStrength	= 0.f;
	}

	return TRUE;
}
void CTargetCS::net_Destroy			()
{
	inherited::net_Destroy();
	Game().targets.erase(std::find(Game().targets.begin(), Game().targets.end(), this));

	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete			(m_pPhysicsShell);
}

void CTargetCS::OnH_A_Chield() {
	//Log("+++++++++++ void CTargetCS::OnH_A_Chield()");
	inherited::OnH_A_Chield		();
	setVisible					(false);
	setEnabled					(false);
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
}

void CTargetCS::OnH_B_Independent() {
	//Log("+++++++++++ void CTargetCS::OnH_B_Independent()");
	inherited::OnH_B_Independent();
	setVisible					(true);
	setEnabled					(true);
	CObject*	E		= dynamic_cast<CObject*>(H_Parent());
	R_ASSERT		(E);
	XFORM().set(E->XFORM());
	Position().set(XFORM().c);
	if(m_pPhysicsShell) {
		Fvector l_fw; l_fw.set(XFORM().k); l_fw.mul(2.f);
		Fvector l_up; l_up.set(XFORM().j); l_up.mul(2.f);
		Fmatrix l_p1, l_p2;
		l_p1.set(XFORM()); l_p1.c.add(l_up); l_p1.c.add(l_fw);
		l_p2.set(XFORM()); l_p2.c.add(l_up); l_fw.mul(3.f); l_p2.c.add(l_fw);
		//Log("aaa",l_p1.c);
		//Log("bbb",l_p2.c);
		m_pPhysicsShell->Activate(l_p1, 0, l_p2);
		XFORM().set(l_p1);
		Position().set(XFORM().c);
	}
}

void CTargetCS::UpdateCL		()
{
	inherited::UpdateCL();
	if(dynamic_cast<CTargetCSCask*>(H_Parent())) {
		//XFORM().set			();
		XFORM().mul			(H_Parent()->XFORM(), m_pos);
		Position().set			(XFORM().c);
		setVisible				(true);
		setEnabled				(true);
	}
	else if (getEnabled() && m_pPhysicsShell)		
	{
		m_pPhysicsShell->Update	();
		XFORM().set			(m_pPhysicsShell->mXFORM);
		Position().set			(XFORM().c);
	}
}

void CTargetCS::SetPos(const Fmatrix& pos) {
	m_pos.set(pos);
	XFORM().set			(m_pos);
	Position().set			(XFORM().c);
}
