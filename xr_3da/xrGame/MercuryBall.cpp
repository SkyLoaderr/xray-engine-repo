#include "stdafx.h"
#include "mercuryball.h"

CMercuryBall::CMercuryBall(void) {}

CMercuryBall::~CMercuryBall(void) {}

BOOL CMercuryBall::net_Spawn(LPVOID DC) {
	inherited::net_Spawn(DC);
	setVisible(TRUE);
	xrSE_MercuryBall* E = (xrSE_MercuryBall*)DC;
	cNameVisual_set(E->s_Model);
	return TRUE;
}