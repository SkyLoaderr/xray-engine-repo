#include "stdafx.h"
#include "mercuryball.h"

CMercuryBall::CMercuryBall(void) {}

CMercuryBall::~CMercuryBall(void) {}

BOOL CMercuryBall::net_Spawn(LPVOID DC) {
	inherited::net_Spawn		(DC);
	setVisible					(true);
	setEnabled					(true);
	//cNameVisual_set				(E->s_Model);

	CKinematics* V				= PKinematics(Visual());
	if(V) V->PlayCycle			("idle");

	return TRUE;
}

void CMercuryBall::OnH_A_Chield() {
	inherited::OnH_A_Chield		();
	setVisible					(false);
	setEnabled					(false);
}

void CMercuryBall::OnH_B_Independent() {
	inherited::OnH_B_Independent();
	setVisible					(true);
	setEnabled					(true);
	CObject*	E		= dynamic_cast<CObject*>(H_Parent());
	R_ASSERT		(E);
	XFORM().set(E->XFORM());
	Position().set(XFORM().c);
}
