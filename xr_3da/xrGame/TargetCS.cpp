#include "stdafx.h"
#include "targetcs.h"

CTargetCS::CTargetCS(void)
{
}

CTargetCS::~CTargetCS(void)
{
}

BOOL CTargetCS::net_Spawn(LPVOID DC) {
	inherited::net_Spawn(DC);
	setVisible					(true);
	setEnabled					(true);
	xrSE_Target_CS* E = (xrSE_Target_CS*)DC;
	//cNameVisual_set(E->s_Model);
	return TRUE;
}

void CTargetCS::OnH_A_Chield() {
	inherited::OnH_A_Chield		();
	setVisible					(false);
	setEnabled					(false);
}

void CTargetCS::OnH_B_Independent() {
	inherited::OnH_B_Independent();
	setVisible					(true);
	setEnabled					(true);
	CObject*	E		= dynamic_cast<CObject*>(H_Parent());
	R_ASSERT		(E);
	svTransform.set(E->clXFORM());
	vPosition.set(svTransform.c);
}

void CTargetCS::OnDeviceCreate() {
	inherited::OnDeviceCreate();
	CKinematics* V = PKinematics(Visual());
	if(V) V->PlayCycle("idle");
}

void CTargetCS::OnDeviceDestroy() {
	inherited::OnDeviceDestroy	();
}
