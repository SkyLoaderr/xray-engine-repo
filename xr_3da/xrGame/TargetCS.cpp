#include "stdafx.h"
#include "targetcs.h"

CTargetCS::CTargetCS(void)
{
	m_pPhysicsShell			= 0;
}

CTargetCS::~CTargetCS(void)
{
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	_DELETE					(m_pPhysicsShell);
}

BOOL CTargetCS::net_Spawn(LPVOID DC) {
	inherited::net_Spawn(DC);
	setVisible					(true);
	setEnabled					(true);
	//xrSE_Target_CS* E = (xrSE_Target_CS*)DC;
	//cNameVisual_set(E->s_Model);
	return TRUE;
}

void CTargetCS::OnH_A_Chield() {
	Log("+++++++++++ void CTargetCS::OnH_A_Chield()");
	inherited::OnH_A_Chield		();
	setVisible					(false);
	setEnabled					(false);
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
}

void CTargetCS::OnH_B_Independent() {
	Log("+++++++++++ void CTargetCS::OnH_B_Independent()");
	inherited::OnH_B_Independent();
	setVisible					(true);
	setEnabled					(true);
	CObject*	E		= dynamic_cast<CObject*>(H_Parent());
	R_ASSERT		(E);
	svTransform.set(E->clXFORM());
	vPosition.set(svTransform.c);
	if(m_pPhysicsShell) {
		Fvector l_fw; l_fw.set(svTransform.k); l_fw.mul(2.f);
		Fvector l_up; l_up.set(svTransform.j); l_up.mul(2.f);
		Fmatrix l_p1, l_p2;
		l_p1.set(svTransform); l_p1.c.add(l_up); l_p1.c.add(l_fw);
		l_p2.set(svTransform); l_p2.c.add(l_up); l_fw.mul(3.f); l_p2.c.add(l_fw);
		Log("aaa",l_p1.c);
		Log("bbb",l_p2.c);
		m_pPhysicsShell->Activate(l_p1, 0, l_p2);
		svTransform.set(l_p1);
		vPosition.set(svTransform.c);
	}
}

void CTargetCS::OnDeviceCreate() {
	inherited::OnDeviceCreate();
	CKinematics* V = PKinematics(Visual());
	if(V) V->PlayCycle("idle");

	if (0==m_pPhysicsShell)
	{
		// Physics (Box)
		Fobb								obb;
		Visual()->bv_BBox.get_CD			(obb.m_translate,obb.m_halfsize);
		obb.m_rotate.identity				();

		// Physics (Elements)
		CPhysicsElement* E					= P_create_Element	();
		R_ASSERT							(E);
		E->add_Box							(obb);

		// Physics (Shell)
		m_pPhysicsShell						= P_create_Shell	();
		R_ASSERT							(m_pPhysicsShell);
		m_pPhysicsShell->add_Element		(E);
		m_pPhysicsShell->setMass			(10.f);
		m_pPhysicsShell->Activate			(svXFORM(),0,svXFORM());
		m_pPhysicsShell->mDesired.identity	();
		m_pPhysicsShell->fDesiredStrength	= 0.f;
	}
}

void CTargetCS::OnDeviceDestroy() {
	inherited::OnDeviceDestroy	();
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	_DELETE					(m_pPhysicsShell);
}

void CTargetCS::UpdateCL		()
{
	inherited::UpdateCL();
	if (m_pPhysicsShell)		
	{
		m_pPhysicsShell->Update	();
		svTransform.set			(m_pPhysicsShell->mXFORM);
		vPosition.set			(svTransform.c);
	}
}
