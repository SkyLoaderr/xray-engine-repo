#include "stdafx.h"
#include "targetcscask.h"
#include "targetcs.h"

CTargetCSCask::CTargetCSCask(void) {}

CTargetCSCask::~CTargetCSCask(void) {}

IC BOOL BE(BOOL A, BOOL B) 
{
	bool a = !!A;
	bool b = !!B;
	return a==b;
}

void CTargetCSCask::OnEvent(NET_Packet& P, u16 type) 
{
	__super::OnEvent(P,type);
	u16 id;
	switch (type) {
		case GE_OWNERSHIP_TAKE : {
			P.r_u16(id);
			CObject *l_pObj = Level().Objects.net_Find(id);

			// Test for CS Target
			CTargetCS* l_pBall = dynamic_cast<CTargetCS*>(l_pObj);
			if(l_pBall) {
				R_ASSERT(BE(Local(),l_pBall->Local()));	// remote can't take local
				l_pBall->H_SetParent(this);
				return;
			}
		} break;
		case GE_OWNERSHIP_REJECT :{
			P.r_u16(id);
			CObject* l_pObj	= Level().Objects.net_Find(id);

			// Test for CS Target
			CTargetCS* l_pBall	= dynamic_cast<CTargetCS*>(l_pObj);
			if(l_pBall) {
				R_ASSERT(BE(Local(),l_pBall->Local()));	// remote can't eject local
				l_pBall->H_SetParent(0);
				return;
			}
		} break;
	}
}
