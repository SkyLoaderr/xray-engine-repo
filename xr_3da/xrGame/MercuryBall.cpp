#include "stdafx.h"
#include "mercuryball.h"

CMercuryBall::CMercuryBall(void) {}

CMercuryBall::~CMercuryBall(void) {}

BOOL CMercuryBall::net_Spawn(LPVOID DC) {
	inherited::net_Spawn(DC);
	setVisible					(true);
	setEnabled					(true);
	xrSE_MercuryBall* E = (xrSE_MercuryBall*)DC;
	cNameVisual_set(E->s_Model);
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
}

void CMercuryBall::OnDeviceCreate() {
	inherited::OnDeviceCreate();
	CKinematics* V = PKinematics(Visual());
	if(V) V->PlayCycle("idle");
}

void CMercuryBall::OnDeviceDestroy() {
	inherited::OnDeviceDestroy	();
}
